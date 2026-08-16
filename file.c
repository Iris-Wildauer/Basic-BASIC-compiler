#include <stdio.h>
#include <stdlib.h>

#include "file.h"

int printArray(int size, char* array) {
    for (int i = 0; i < size; i++) {
        printf("%c", array[i]);
    }
    printf("\n");
    return 0;
}

int parseFile(const char *file,char **output,int *len) {
    FILE* fptr = fopen(file, "r");
    if (fptr == NULL) {
        perror("Could not open File");
        return 1;
    }
    int seek = fseek(fptr, 0, SEEK_END);
    if (seek == 1) {
        perror("Seek failed");
        fclose(fptr);
        return -1;
    }

    long size = ftell(fptr);
    char *buffer = malloc(size * sizeof(char));

    fseek(fptr, 0, SEEK_SET);

    size_t newLen = fread(buffer, sizeof(char), size, fptr);
        if ( ferror( fptr ) != 0 ) {
            perror("Error reading file");
        }
    buffer[size] = '\n';
    *output = buffer;
    *len = (int)newLen;
    fclose(fptr);
    return 0;
}
