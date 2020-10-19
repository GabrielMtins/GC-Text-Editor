#ifndef ROW_H
#define ROW_H

/*
    Copyright(C) 2020 Gabriel Martins
    This file is part of GCTE.

    GCTE is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    GCTE is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with GCTE.  If not, see <https://www.gnu.org/licenses/>
    contact: gabrielmartinsthe@gmail.com
*/

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
