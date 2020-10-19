#include "row.h"
#include <stdlib.h>

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

row* row_create(){
    row* new_row = malloc(sizeof(row));
    new_row->size = 1;
    new_row->characters[0] = '\0';
    return new_row;
}

void row_addCharacter(row* current_row, char push_character){
    if(current_row == NULL) return;
    current_row->size++;
    current_row->characters[current_row->size-1] = '\0';
    current_row->characters[current_row->size-2] = push_character;
}

void row_insertCharacter(row* current_row, char push_character, size_t position){
    if(position >= current_row->size-1){
        row_addCharacter(current_row, push_character);
        return;
    }
    current_row->size++;
    for(size_t i = current_row->size-1; i >= position+1; i--){
        current_row->characters[i] = current_row->characters[i-1];
    }
    current_row->characters[position] = push_character;
}

void row_remove(row* current_row, size_t char_pos){
    if(current_row == NULL) return;
    if(current_row->size == 1) return;
    size_t new_pos = char_pos;
    if(char_pos == 0) new_pos = 0;
    else new_pos = char_pos-1;
    for(size_t i = new_pos; i < current_row->size-1; i++){
        current_row->characters[i] = current_row->characters[i+1];
    }
    current_row->size--;
}

void row_pop(row* current_row){
    if(current_row->size == 1) return;
    current_row->characters[current_row->size-2] = '\0';
    current_row->size--;
}

void row_destroy(row* current_row){
    free(current_row);
}
