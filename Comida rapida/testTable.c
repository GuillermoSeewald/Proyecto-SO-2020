#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define COLUMN_SIZE 30
#define MAX_COLUMNS 4

void tab(int columnNumber, char* text) {
    char* tab = (char*) malloc(sizeof(char)*(150));
    int i, j;
    int size1 = (COLUMN_SIZE - strlen(text))/2;
    int size2 = (COLUMN_SIZE - strlen(text))/2 + ((COLUMN_SIZE - strlen(text))%2);
    for (i=0; i<columnNumber; i++) {
        for (j=0; j<COLUMN_SIZE; j++)
            tab = strcat(tab, " ");
        strcat(tab, "|");
    }
    for (j=0; j<size1; j++)
        tab = strcat(tab, " ");
    tab = strcat(tab, text);
    for (j=0; j<size2; j++)
        tab = strcat(tab, " ");
    tab = strcat(tab, "|");
    for (i=columnNumber+1; i<MAX_COLUMNS; i++) {
        for (j=0; j<COLUMN_SIZE; j++)
            tab = strcat(tab, " ");
        strcat(tab, "|");
    }
    printf("%s\n", tab);
    free(tab);
}

void tab2(int columnNumber, char* text) {
    char* tab = (char*) malloc(sizeof(char)*(170));
    char defaultText[] = "\e[93m-----\e[0m\0";
    int defaultTextSize = 5;
    int i, j, size1, size2;


    for (i=0; i<columnNumber; i++) {
        size1 = (COLUMN_SIZE - defaultTextSize)/2;
        size2 = size1 + ((COLUMN_SIZE - defaultTextSize)%2);
        for (j=0; j<size1; j++)
            tab = strcat(tab, " ");
        tab = strcat(tab, defaultText);
        for (j=0; j<size2; j++)
            tab = strcat(tab, " ");
        strcat(tab, "|");
    }
    size1 = (COLUMN_SIZE - strlen(text))/2;
    size2 = size1 + ((COLUMN_SIZE - strlen(text))%2);
    for (j=0; j<size1; j++)
        tab = strcat(tab, " ");
    tab = strcat(tab, text);
    for (j=0; j<size2; j++)
        tab = strcat(tab, " ");
    tab = strcat(tab, "|");
    for (i=columnNumber+1; i<MAX_COLUMNS; i++) {
        size1 = (COLUMN_SIZE - defaultTextSize)/2;
        size2 = size1 + ((COLUMN_SIZE - defaultTextSize)%2);
        for (j=0; j<size1; j++)
            tab = strcat(tab, " ");
        tab = strcat(tab, defaultText);
        for (j=0; j<size2; j++)
            tab = strcat(tab, " ");
        strcat(tab, "|");
    }
    printf("%s\n", tab);
    free(tab);
}

int main() {
    char text1[] = "Header 1";
    char text2[] = "Header 21";
    char text3[] = "Header 3";
    tab(0, text1);
    tab(1, text2);
    tab(2, text3);
}
