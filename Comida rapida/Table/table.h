#ifndef TABLE_H
#define TABLE_H

#define COLUMN_SIZE 26
#define MAX_COLUMNS 4

char* centerColumn(char* text);
void tableHeader();
void tableRow(int col, char* text);
void tableFooter();

#endif
