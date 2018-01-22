#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filter.h"

RGB *readPPM(const char *file, int *width, int *height, int *max) {
	FILE *fp = fopen(file, "r");
	if (fp == NULL){
		printf("Unable to open the file\n");
		exit(1);
	}
	fscanf(fp, "P3 %d %d %d\n", width, height, max);
	int window = (*width) * (*height);
	RGB *array = malloc(window * sizeof(RGB));
	if (array == NULL){
		printf("Unable to allocate memory\n");
		exit(1);
	}
	for (int i = 0; i < window; i++){
		fscanf(fp, "%hhu %hhu %hhu", &(array[i].r), &(array[i].g), &(array[i].b));
	}
	fclose(fp);

	return array;
}

void writePPM(const char *file, int width, int height, int max, const RGB *image) {
        FILE *fp;
        fp = fopen(file, "w");
        fprintf(fp, "P3\n%d %d\n%d\n", width, height, max);
        for (int h = 0; h < height; h++) {
                for (int w = 0; w < width; w++) {
                        int pos = h*width+w;
                        fprintf(fp, "%hhu %hhu %hhu ", image[pos].r, image[pos].g, image[pos].b);
                }
                fprintf(fp, "\n");
        }
        return;
}

int partition(int* input, int p, int r) {
        int pivot = input[r];

        while (p < r) {
                while (input[p] < pivot) p++;
                while (input[r] > pivot) r--;

                if (input[p] == input[r]) p++;
                else if (p < r) {
                        int temp = input[p];
                        input[p] = input[r];
                        input[r] = temp;
                }
        }
        return r;
}

int quick_select(int* input, int p, int r, int k) {
        if (p == r) return input[p];
        int j = partition(input, p, r);
        int length = j - p + 1;
        if (length == k) {
                return input[j];
        } else if (k < length) {
                return quick_select(input, p, j - 1, k);
        } else {
                return quick_select(input, j + 1, r, k - length);
        }
}

RGB *denoiseImage(int width, int height, const RGB *image, int n, filter f){
        int ndiv = n / 2, ntimes = n * n;


	//checks if the input was mean or median filter
        if (f == MEAN) {
                RGB *newImage = malloc(width * height * sizeof(RGB));
                if (newImage == NULL) {
                        printf("Unable to allocate\n");
                        exit(1);
                }
		/* This algorithm traverses each pixel in a supplied ppm image. At each pixel,
		the surrounding pixels (of width and height supplied by the user) are also
		traversed. If the surrounding pixel is valid, it will increase the number of
		pixels and the sum of each colour to the total that it has to divide by to find
		the mean.
		*/
                for (int h = 0; h < height; h++) {
                        for (int w = 0; w < width; w++) {
                                //get neighbors
                                int sumR = 0, sumG = 0, sumB = 0;
                                int size = 0;
                                for (int dy = -ndiv; dy <= ndiv; dy++) {
                                        if ((h + dy) >= 0 && (h + dy) < height) {

                                                for (int dx = -ndiv; dx <= ndiv; dx++) {
                                                        if ((w + dx) >= 0 && (w + dx) < width) {
                                                                int pos = (w+dx)+(h+dy)*width;
                                                                sumR += image[pos].r;
                                                                sumG += image[pos].g;
                                                                sumB += image[pos].b;
                                                                size++;
                                                        }
                                                }
                                        }
                                }
				//finding the mean of each colour for the new pixel
                                sumR /= size;
                                sumG /= size;
                                sumB /= size;

                                //set new pixel
                                int pos = h*width+w;
                                newImage[pos].r = (unsigned char)sumR;
                                newImage[pos].g = (unsigned char)sumG;
                                newImage[pos].b = (unsigned char)sumB;
                        }
                }
                return newImage;

        } else if (f == MEDIAN) {
                RGB *newImage = malloc(width * height * sizeof(RGB));

		//This is the same algorithm as above, but with changes so it applies to a median filter instead of a mean filter
                for (int h = 0; h < height; h++) {
                        for (int w = 0; w < width; w++) {
                                // get neighbors
                                int neighborsR[ntimes], neighborsG[ntimes], neighborsB[ntimes];
                                int size = 0;
                                for (int dy = -ndiv; dy <= ndiv; dy++) {
                                        if ((h + dy) >= 0 && (h + dy) < height) {

                                                for (int dx = -ndiv; dx <= ndiv; dx++) {
                                                        if ((w + dx) >= 0 && (w + dx) < width) {
                                                                int pos = dx+(ndiv)*n+(dy+(ndiv)), pos2 = (h+dy)*width+(w+dx);
                                                                neighborsR[pos] = image[pos2].r;
                                                                neighborsG[pos] = image[pos2].g;
                                                                neighborsB[pos] = image[pos2].b;
                                                                size++;
                                                        }
                                                }
                                        }
                                }
                                // find the median of the neighbors
                                // quick median
                                int pos = h*width+w;
                                newImage[pos].r = quick_select(neighborsR, 0, size-1, size/2);
                                newImage[pos].g = quick_select(neighborsG, 0, size-1, size/2);
                                newImage[pos].b = quick_select(neighborsB, 0, size-1, size/2);
                        }
                }
                return newImage;
        }

}
