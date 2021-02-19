#include <stdio.h>
#include <math.h>    // Need this for sqrt()
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "knn.h"

/* Print the image to standard output in the pgmformat.  
 * (Use diff -w to compare the printed output to the original image)
 */
void print_image(unsigned char *image) {
    printf("P2\n %d %d\n 255\n", WIDTH, HEIGHT);
    for (int i = 0; i < NUM_PIXELS; i++) {
        printf("%d ", image[i]);
        if ((i + 1) % WIDTH == 0) {
            printf("\n");
        }
    }
}

/* Return the label from an image filename.
 * The image filenames all have the same format:
 *    <image number>-<label>.pgm
 * The label in the file name is the digit that the image represents
 */
unsigned char get_label(char *filename) {
    // Find the dash in the filename
    char *dash_char = strstr(filename, "-");
    // Convert the number after the dash into a integer
    return (char) atoi(dash_char + 1);
}

/* ******************************************************************
 * Complete the remaining functions below
 * ******************************************************************/


/* Read a pgm image from filename, storing its pixels
 * in the array img.
 * It is recommended to use fscanf to read the values from the file. First, 
 * consume the header information.  You should declare two temporary variables
 * to hold the width and height fields. This allows you to use the format string
 * "P2 %d %d 255 "
 * 
 * To read in each pixel value, use the format string "%hhu " which will store
 * the number it reads in an an unsigned character.
 * (Note that the img array is a 1D array of length WIDTH*HEIGHT.)
 */
void load_image(char *filename, unsigned char *img) {
    // Open corresponding image file, read in header (which we will discard)
    FILE *f2 = fopen(filename, "r");

    if (f2 == NULL) {
        perror("fopen");
        exit(1);
    }
	//TODO
    int column;
    int row;

    fscanf(f2, "P2 %d %d 255", &column, &row);

    int num_pixel = column*row;
    for(int i = 0; i < num_pixel; i++){
        fscanf(f2, "%hhu", &img[i]);
    }

    fclose(f2);
}


/**
 * Load a full dataset into a 2D array called dataset.
 *
 * The image files to load are given in filename where
 * each image file name is on a separate line.
 * 
 * For each image i:
 *  - read the pixels into row i (using load_image)
 *  - set the image label in labels[i] (using get_label)
 * 
 * Return number of images read.
 */
int load_dataset(char *filename, 
                 unsigned char dataset[MAX_SIZE][NUM_PIXELS],
                 unsigned char *labels) {
    // We expect the file to hold up to MAX_SIZE number of file names
    FILE *f1 = fopen(filename, "r");
    if (f1 == NULL) {
        perror("fopen");
        exit(1);
    }

    //TODO
    char file[MAX_NAME];

    int i = 0;
    while(fscanf(f1, "%s", file) != EOF){
        load_image(file, dataset[i]);
        // print_image(dataset[i]);
        // printf("%s\n", file);
        labels[i] = get_label(file);
        i++;
    }

    fclose(f1);
    return i;
}

/** 
 * Return the euclidean distance between the image pixels in the image
 * a and b.  (See handout for the euclidean distance function)
 */
double distance(unsigned char *a, unsigned char *b) {
    int distance = 0;
    for(int i = 0; i<NUM_PIXELS; i++){
        int dif = a[i]-b[i];
        distance += dif*dif;
    }

    return sqrt(distance);
}

/**
 * Return the most frequent label of the K most similar images to "input"
 * in the dataset
 * Parameters:
 *  - input - an array of NUM_PIXELS unsigned chars containing the image to test
 *  - K - an int that determines how many training images are in the 
 *        K-most-similar set
 *  - dataset - a 2D array containing the set of training images.
 *  - labels - the array of labels that correspond to the training dataset
 *  - training_size - the number of images that are actually stored in dataset
 * 
 * Steps
 *   (1) Find the K images in dataset that have the smallest distance to input
 *         When evaluating an image to decide whether it belongs in the set of 
 *         K closest images, it will only replace an image in the set if its
 *         distance to the test image is strictly less than all of the images in 
 *         the current K closest images.
 *   (2) Count the frequencies of the labels in the K images
 *   (3) Return the most frequent label of these K images
 *         In the case of a tie, return the smallest value digit.
 */ 

int knn_predict(unsigned char *input, int K,
                unsigned char dataset[MAX_SIZE][NUM_PIXELS],
                unsigned char *labels,
                int training_size) {

    //------make kscore_list which contains k images with the least scores------//
    float kscore_list[K]; //kscore_list[k] -> [score, label]
    int klabel_list[K];
    bool check = true;
    int max_index = 0; // index of element in kscore_list with max score
    int max_score = 0;

    for(int i = 0; i<training_size; i++){
        float score = distance(input, dataset[i]);
        // no training image tested yet
        if(i < K){
            kscore_list[i] = score;
            klabel_list[i] = labels[i];
            continue;
        }

        if(check){
            max_score = 0;
            // find max in k_score_list to replace
            for(int j=0; j<K; j++){
                if(max_score< kscore_list[j]){
                    max_score = kscore_list[j];
                    max_index = j;
                }
            }
        }

        // replace if current score is smaller than max score of kscore_list
        if(score < max_score){
            kscore_list[max_index]=score;
            klabel_list[max_index]=labels[i];
            check = true;
        }else{
            check = false; // no need to check for max score - it will be the same as this loop
        }
    }

    //-----find out what label occurred the most in kscore_list-----//
    int num_counts[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // ex: num_counts[0] -> number of 0s that occur in kscore_list
    for(int i = 0; i<K; i++){
        num_counts[klabel_list[i]] += 1;
    }
    int max_count = 0;
    int max_num = 0;
    for(int i = 0; i<10; i++){
        if(num_counts[i]>max_count){
            max_count = num_counts[i];
            max_num = i;
        }
    }

    return max_num;
}
