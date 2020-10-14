#include "row.h"
#include <stdlib.h>

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
