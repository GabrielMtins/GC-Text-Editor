#ifndef ROW_H
#define ROW_H

#include <stddef.h>
#include <stdint.h>

typedef struct{
    char characters[256];
    size_t size;
} row;

row* row_create();

void row_addCharacter(row* current_row, char push_character);

void row_insertCharacter(row* current_row, char push_character, size_t position);

void row_remove(row* current_row, size_t char_pos);

void row_pop(row* current_row);

void row_destroy(row* current_row);

#endif
