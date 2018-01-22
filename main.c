#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "filter.h"

void main(int argc, char *argv[]) {
        //argv[1] name of Input
        //argv[2] name of Output
        //argv[3] size of window
        //argv[4] type of filter
        if (argc != 5) {
                printf("Incorrect number of arguments. Correct usage:\n\tInput file, Output file, Window Size, Filter Type (A/M)\n");
                return;
        }

        int window_size = atoi(argv[3]);
        filter type;
        if (!strcmp(argv[4], "A")) {
                type = MEAN;
        } else if (!strcmp(argv[4], "M")) {
                type = MEDIAN;
        }

        //read file
        printf("Read file %s\n", argv[1]);
        int width, height, max;

        clock_t before = clock();
        RGB *array = readPPM(argv[1], &width, &height, &max);
        clock_t after = clock();
        double diff = ((double)(after - before) / CLOCKS_PER_SEC);
	printf("File read in %.1e seconds\n", diff);

        //denoise file
        printf("Processing file %s\n", argv[1]);
        clock_t before1 = clock();
	RGB *newImage = denoiseImage(width, height, array, window_size, type);
        clock_t after1 = clock();
        double diff1 = ((double)(after1 - before1) / CLOCKS_PER_SEC);
	printf("Image processed in %.1e seconds\n", diff1);

        //write file
	printf("Writing file %s\n", argv[2]);
        clock_t before2 = clock();
        writePPM(argv[2], width, height, max, newImage);
        clock_t after2 = clock();
        double diff2 = ((double)(after2 - before2) / CLOCKS_PER_SEC);
	printf("File written in %.1e seconds\n", argv[2], diff2);
        free(array);
        free(newImage);


}
