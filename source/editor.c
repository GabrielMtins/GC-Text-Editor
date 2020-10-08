#include "editor.h"
#include "utilities.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ncurses.h>

row* row_create(){
    row* new_row = malloc(sizeof(row));
    new_row->size = 1;
    new_row->characters[0] = '\0';
    return new_row;
}

void row_addCharacter(row* current_row, const char push_character){
    if(current_row == NULL) return;
    current_row->size++;
    current_row->characters[current_row->size-1] = '\0';
    current_row->characters[current_row->size-2] = push_character;
}

void row_insertCharacter(row* current_row, const char push_character, const int position){
    if(position >= current_row->size-1){
        row_addCharacter(current_row, push_character);
        return;
    }
    if(position < 0) return;
    current_row->size++;
    for(int i = current_row->size-1; i >= position+1; i--){
        current_row->characters[i] = current_row->characters[i-1];
    }
    current_row->characters[position] = push_character;
}

void row_remove(row* current_row, const unsigned int char_pos){
    if(current_row == NULL) return;
    if(current_row->size == 1) return;
    for(int i = char_pos-1; i < current_row->size-1; i++){
        current_row->characters[i] = current_row->characters[i+1];
    }
}

void row_pop(row* current_row){
    if(current_row->size <= 1) return;
    current_row->characters[current_row->size-2] = '\0';
    current_row->size--;
}

void row_destroy(row* current_row){
    free(current_row);
}

editor_cfg* editor_create(){
    editor_cfg* cfg = malloc(sizeof(editor_cfg));
    cfg->current_row = 0;
    cfg->mode = TEXT_MODE;
    cfg->command_row = row_create();
    editor_addRow(cfg);
    cfg->cursor_x = 0; cfg->cursor_y = 0;
    return cfg;
}

void editor_addRow(editor_cfg* cfg){
    cfg->current_row++;
    cfg->cursor_y++;
    if(cfg->current_row == 1){ // if size is equal 1, we just push the first row
        cfg->cursor_x = 0;
        cfg->rows_stack[0] = row_create();
    }
    else{
        for(int i = cfg->current_row-1; i >= cfg->cursor_y; i--){
            cfg->rows_stack[i] = cfg->rows_stack[i-1];
        } // we modify the other rolls position
        cfg->rows_stack[cfg->cursor_y] = row_create();
        if(cfg->cursor_x != cfg->rows_stack[cfg->cursor_y-1]->size-1){
            char new_line_str[256] = ""; // this is the next line
            int size_to_copy = cfg->rows_stack[cfg->cursor_y-1]->size-cfg->cursor_x-1; // this is how many bytes we need to copy
            char* row_ptr = cfg->rows_stack[cfg->cursor_y-1]->characters; // this is the pointer to the current row
            strncpy(new_line_str, row_ptr+cfg->cursor_x, size_to_copy);
            /*
                here we just copy the part in front of the cursor to another line:
                test
                result of enter key:
                te
                st
            */
            row_ptr[cfg->cursor_x] = '\0';
            strcpy(cfg->rows_stack[cfg->cursor_y]->characters, new_line_str);
        }
        cfg->cursor_x = 0;
    }
}

void editor_controlCursor(editor_cfg* cfg, const int key){
    switch(key){
        case KEY_LEFT:
        cfg->cursor_x--;
        break;
        case KEY_RIGHT:
        cfg->cursor_x++;
        break;
        case KEY_DOWN:
        cfg->cursor_y++;
        break;
        case KEY_UP:
        cfg->cursor_y--;
        break;
        case KEY_BACKSPACE:
        editor_popLastCharacter(cfg);
        break;
        case 10:
        if(cfg->mode == TEXT_MODE) editor_addRow(cfg);
        if(cfg->mode == CMD_MODE) editor_processCommand(cfg);
        break;
    }
    if(cfg->cursor_y < 0) cfg->cursor_y = 0;
    if(cfg->cursor_x < 0){
        cfg->cursor_x = 0;
    }
    if(cfg->cursor_y >= cfg->current_row-1) cfg->cursor_y = cfg->current_row-1;
    if(cfg->cursor_x >= cfg->rows_stack[cfg->cursor_y]->size) cfg->cursor_x = cfg->rows_stack[cfg->cursor_y]->size-1;
}

void editor_input(editor_cfg* cfg, const int character_push){
    editor_controlCursor(cfg, character_push);
    switch(cfg->mode){
        case CMD_MODE:
        if(character_push != KEY_BACKSPACE) row_addCharacter(cfg->command_row, (char)character_push);
        break;
        case TEXT_MODE:
        if(character_push > 127) return;
        if(character_push != '\t'){
            if(character_push != '\n'){
                row_insertCharacter(cfg->rows_stack[cfg->cursor_y], (char)character_push, cfg->cursor_x);
                cfg->cursor_x++;
            }
        }
        else{
            for(int i = 0; i < TAB_SIZE; i++){
                row_insertCharacter(cfg->rows_stack[cfg->cursor_y], ' ', cfg->cursor_x);
                cfg->cursor_x++;
            }
        }
        break;
    }
}

void editor_popLastCharacter(editor_cfg* cfg){
    if(cfg == NULL) return;
    if(cfg->mode == CMD_MODE){
        row_pop(cfg->command_row);
        return;
    }
    row* edit_row = cfg->rows_stack[cfg->cursor_y];
    if(edit_row->size <= 1){ // if the size is equal one, we just destroy the current line
        if(cfg->current_row > 1){ // we cant destroy the first line
            row_destroy(edit_row);
            for(int i = cfg->cursor_y; i < cfg->current_row; i++){
                cfg->rows_stack[i] = cfg->rows_stack[i+1];
            }
            if(cfg->cursor_y == cfg->current_row-1) cfg->cursor_x = cfg->rows_stack[cfg->cursor_y-1]->size;
            cfg->current_row--;
            cfg->cursor_y--;
        }
    }
    else{ // else we just pop the character of cursor on line
        row_remove(edit_row, cfg->cursor_x);
        cfg->cursor_x--;
        if(cfg->cursor_x < 0) cfg->cursor_x = 0;
    }
}

void editor_processCommand(editor_cfg* cfg){
    if(!strncmp(cfg->command_row->characters, "sv", 2)){ // cmp to first 2 bytes
        char save_file[256] = "";
        strncpy(save_file, cfg->command_row->characters+3, cfg->command_row->size-3); // cpy the final bytes
        editor_saveAsFile(cfg, save_file); // save file
    }
    if(!strncmp(cfg->command_row->characters, "ld", 2)){ // cmp to first 2 bytes
        char load_file[256] = "";
        strncpy(load_file, cfg->command_row->characters+3, cfg->command_row->size-3); // cpy the final bytes
        cfg->mode = TEXT_MODE;
        editor_loadFile(cfg, load_file); // save file
    }
    cfg->mode = TEXT_MODE;
}

void editor_draw(const editor_cfg* cfg){
    attron(COLOR_PAIR(1));
    int y_max = 0;
    int x_max = 0;
    getmaxyx(stdscr, y_max, x_max);
    y_max--;
    x_max--;
    int offset_lines = cfg->cursor_y+1-y_max;
    if(offset_lines<0) offset_lines = 0;
    int min_lines = cfg->current_row;
    if(min_lines > y_max) min_lines = y_max;// the minimum number of lines to print
    for(int i = 0; i < min_lines; i++){
        move(i, 0);
        char str_num[256] = "";
        sprintf(str_num, "%i", i+1+offset_lines); // convert the number to a string
        printw(str_num); // print the number of the line
        util_printSyntaxC(cfg->rows_stack[i+offset_lines]->characters, strlen(str_num)+1, i, x_max);
    }
    { // draw commands
        attron(COLOR_PAIR(1));
        move(y_max, 0);
        addch('?');
        move(y_max, 2);
        printw(cfg->command_row->characters);
    }
    { // draw cursor
        int offset_cursor_x = 0;
        if(cfg->cursor_y+1 > 9) offset_cursor_x++;
        move(cfg->cursor_y-offset_lines, cfg->cursor_x+2+offset_cursor_x);
    }
}
