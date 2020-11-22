#ifndef TABLE_H
#define TABLE_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "table.h"
#define COLUMN_SIZE 26
#define MAX_COLUMNS 4

char defaultText[] = "-----";
#define defaultCenterText centerColumn(defaultText)


char* centerColumn(char* text) {
    char column[COLUMN_SIZE];
    int sizeLeft, sizeRight;
    size_t sizeText;
    sizeText = strlen(text);
    sizeLeft = (COLUMN_SIZE - sizeText) / 2;
    sizeRight = sizeLeft + ((COLUMN_SIZE - sizeText) % 2);
    sprintf(column, "%*s%s%-*s", sizeLeft, "", text, sizeRight, "");
    return strdup(column);
}

void tableHeader() {
    char lineSep[COLUMN_SIZE];
    int i;
    for (i=0; i<COLUMN_SIZE; i++)
        lineSep[i] = '-';
    lineSep[COLUMN_SIZE] = '\0';

    char cl[] = "Cliente", co[] = "Cocinero", ca[] = "Camarero", li[] = "Limpiador";
    char* centerCl = centerColumn(cl); char* centerCo = centerColumn(co);
    char* centerCa = centerColumn(ca); char* centerLi = centerColumn(li);

    printf("\n|%s|%s|%s|%s|\n", lineSep, lineSep, lineSep, lineSep);
    printf("|%s|%s|%s|%s|\n", centerCl, centerCo, centerCa, centerLi);
    printf("|%s|%s|%s|%s|\n", lineSep, lineSep, lineSep, lineSep);
    free(centerCl); free(centerCo); free(centerCa); free(centerLi);
}

void tableRow(int col, char* text) {
    char format[] = "|%s|%s|%s|%s|";
    char output[(COLUMN_SIZE+2)*MAX_COLUMNS];
    char* toFormat[4] = {
        defaultCenterText,
        defaultCenterText,
        defaultCenterText,
        defaultCenterText
    };
    toFormat[col] = centerColumn(text);
    sprintf(output, format, toFormat[0], toFormat[1], toFormat[2], toFormat[3]);
    printf("%s\n", output);
    free(toFormat[col]);
}

void tableFooter() {
    char lineSep[COLUMN_SIZE];
    int i;
    for (i=0; i<COLUMN_SIZE; i++)
        lineSep[i] = '-';
    lineSep[COLUMN_SIZE] = '\0';
    printf("|%s|%s|%s|%s|\n\n", lineSep, lineSep, lineSep, lineSep);
}

#endif
