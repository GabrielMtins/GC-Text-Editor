#include "editor.h"
#include "row.h"
#include "utilities.h"

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
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include <ncurses.h>

editor_cfg* editor_create(){
    editor_cfg* cfg = malloc(sizeof(editor_cfg));
    cfg->current_row = 0;
    cfg->mode = TEXT_MODE;
    for(size_t i = 0; i < MAX_ROWS; i++) cfg->rows_stack[i] = NULL;
    cfg->command_row = row_create();
    editor_addRow(cfg);
    cfg->cursor_x = cfg->cursor_y = cfg->offset_cursor_y = cfg->offset_cursor_x = 0;
    return cfg;
}

void editor_cleanLines(editor_cfg* cfg){
    if(cfg == NULL) return;
    for(int i = 0; i < MAX_ROWS; i++){
        if(cfg->rows_stack[i] == NULL) continue;
        row_destroy(cfg->rows_stack[i]);
        cfg->rows_stack[i] = NULL;
    }
    cfg->current_row = 0;
}

void editor_destroy(editor_cfg* cfg){
    editor_cleanLines(cfg);
    row_destroy(cfg->command_row);
    free(cfg);
}

void editor_addRow(editor_cfg* cfg){
    cfg->current_row++;
    cfg->cursor_y++;
    if(cfg->current_row <= 1){ // if size is equal 1, we just push the first row
        cfg->cursor_x = 0;
        cfg->rows_stack[0] = row_create();
    }
    else{
        for(size_t i = cfg->current_row-1; i >= cfg->cursor_y; i--){
            cfg->rows_stack[i] = cfg->rows_stack[i-1];
        } // we modify the other rolls position
        cfg->rows_stack[cfg->cursor_y] = row_create();
        if(cfg->cursor_x != cfg->rows_stack[cfg->cursor_y-1]->size-1){ // if the cursor isnt on the end of the row
            char new_line_str[256] = ""; // this is the next line
            int size_to_copy = cfg->rows_stack[cfg->cursor_y-1]->size-cfg->cursor_x-1; // this is how many bytes we need to copy
            char* row_ptr = cfg->rows_stack[cfg->cursor_y-1]->characters; // this is the pointer to the current row
            strncat(new_line_str, row_ptr+cfg->cursor_x, size_to_copy);
            row_ptr[cfg->cursor_x] = '\0';
            // here we just copy the part in front of the cursor to another line:
            strcpy(cfg->rows_stack[cfg->cursor_y]->characters, new_line_str);
            cfg->rows_stack[cfg->cursor_y-1]->size -= strlen(new_line_str); // subtract what we copied to the another line
            cfg->rows_stack[cfg->cursor_y]->size = strlen(new_line_str)+1;
        }
        else{
            // basically this codes adds a tab to the next line if there's a tab on the current line
            // just so it makes easier to code
            char* row_ptr = cfg->rows_stack[cfg->cursor_y-1]->characters;
            int howManyTabs = 0;
            while(1){
                int end = 0;
                for(int j = 0; j < TAB_SIZE; j++){
                    if(row_ptr[j+howManyTabs*TAB_SIZE] != ' '){
                        end = 1;
                        break;
                    }
                }
                if(end) break;
                howManyTabs++;
            } // here we count how many tabs there are
            for(int i = 0; i < howManyTabs; i++){
                for(int j = 0; j < TAB_SIZE; j++){
                    row_addCharacter(cfg->rows_stack[cfg->cursor_y], ' '); // here we add the tabs to the next line
                }
            }
        }
        cfg->cursor_x = cfg->rows_stack[cfg->cursor_y]->size+1;
    }
}

void editor_controlCursor(editor_cfg* cfg, int key){
    int x_max = 0, y_max = 0;
    getmaxyx(stdscr, y_max, x_max);
    x_max--; y_max--;
    switch(key){
        case KEY_LEFT:
        if(cfg->cursor_x > 0) cfg->cursor_x--;
        break;
        case KEY_RIGHT:
        cfg->cursor_x++;
        break;
        case KEY_DOWN:
        cfg->cursor_y++;
        break;
        case KEY_UP:
        if(cfg->cursor_y > 0) cfg->cursor_y--;
        break;
        case KEY_BACKSPACE:
        editor_popLastCharacter(cfg);
        break;
        case 10:
        if(cfg->mode == TEXT_MODE) editor_addRow(cfg);
        if(cfg->mode == CMD_MODE) editor_processCommand(cfg);
        break;
    }
    if(cfg->cursor_y >= cfg->current_row-1) cfg->cursor_y = cfg->current_row-1;
    if(cfg->cursor_x >= cfg->rows_stack[cfg->cursor_y]->size) cfg->cursor_x = cfg->rows_stack[cfg->cursor_y]->size-1;
    // move the offset of the editor in the y axis
    if(cfg->cursor_y >= cfg->offset_cursor_y+y_max) cfg->offset_cursor_y = cfg->cursor_y-y_max+1;
    if(cfg->cursor_y < cfg->offset_cursor_y) cfg->offset_cursor_y = cfg->cursor_y;
    // move the offset of the editor in x axis
    if(cfg->cursor_x >= cfg->offset_cursor_x+x_max-4) cfg->offset_cursor_x = cfg->cursor_x-x_max+4;
    if(cfg->cursor_x < cfg->offset_cursor_x) cfg->offset_cursor_x = cfg->cursor_x;
}

void editor_input(editor_cfg* cfg, int character_push){
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
            for(int i = 0; i < TAB_SIZE; i++){ // we insert tab as TAB_SIZE spaces
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
    if(edit_row == NULL) return;
    if(edit_row->size == 1){ // if the size is equal one, we just destroy the current line
        if(cfg->cursor_y > 0){ // we cant destroy the first line
            row_destroy(edit_row);
            cfg->rows_stack[cfg->cursor_y] = NULL;
            if(cfg->current_row != cfg->cursor_y+1){
                for(size_t i = cfg->cursor_y; i < cfg->current_row-1; i++){
                    // we copy the next roll to the current
                    cfg->rows_stack[i] = cfg->rows_stack[i+1];
                }
                cfg->rows_stack[cfg->current_row-1] = NULL;
            }
            if(cfg->cursor_y == cfg->current_row-1) cfg->cursor_x = cfg->rows_stack[cfg->cursor_y-1]->size;
            // just change the cursor to the end of the current row
            cfg->current_row--;
            cfg->cursor_y--;
        }
    }
    else{ // else we just pop the character of cursor on line
        int deleted_char_wasSpace = isspace(edit_row->characters[cfg->cursor_x-1]);
        row_remove(edit_row, cfg->cursor_x);
        if(cfg->cursor_x > 0){
            cfg->cursor_x--;
            if(cfg->cursor_x > 0){
                int new_char_wasSpace = isspace(edit_row->characters[cfg->cursor_x-1]);
                // if both characters are space, we just deleted it until there is no space left
                // i use this so I can delete tabs faster
                if(new_char_wasSpace && deleted_char_wasSpace) editor_popLastCharacter(cfg);
            }
        }
    }
}

void editor_processCommand(editor_cfg* cfg){
    if(!strncmp(cfg->command_row->characters, "w", 1)){ // cmp to first 1 bytes
        if(cfg->command_row->size == 2){ // save with the default filename
            editor_saveAsFile(cfg, cfg->current_file); // save file
        }
        else{
            char save_file[256] = "";
            strncpy(save_file, cfg->command_row->characters+2, cfg->command_row->size-2); // cpy the final bytes
            editor_saveAsFile(cfg, save_file); // save file
        }
    }
    if(!strncmp(cfg->command_row->characters, "r", 1)){ // cmp to first 1 bytes
        char load_file[256] = "";
        strncpy(load_file, cfg->command_row->characters+2, cfg->command_row->size-2); // cpy the final bytes
        cfg->mode = TEXT_MODE;
        editor_loadFile(cfg, load_file); // save file
    }
    if(!strncmp(cfg->command_row->characters, "q", 1)){ // cmp to first 1 bytes
        cfg->quit = 1;
    }
    if(!strncmp(cfg->command_row->characters, "j", 1)){ // cmp to first 1 bytes
        char line_to_jump_str[4] = "";
        strncpy(line_to_jump_str, cfg->command_row->characters+2, cfg->command_row->size-2);
        size_t line_to_jump = 0;
        sscanf(line_to_jump_str, "%lu", &line_to_jump);
        cfg->cursor_y = line_to_jump-1;
    }
    cfg->command_row->characters[0] = '\0';
    cfg->command_row->size = 1;
    cfg->mode = TEXT_MODE;
}

void editor_draw(const editor_cfg* cfg){
    attron(COLOR_PAIR(1));
    int y_max = 0, x_max = 0;
    getmaxyx(stdscr, y_max, x_max); // get size of the window
    y_max--;
    x_max--;
    int min_lines = cfg->current_row;
    if(min_lines > y_max) min_lines = y_max;// the minimum number of lines to print
    for(int i = 0; i < min_lines; i++){
        int line_number = i+1+cfg->offset_cursor_y;
        if(cfg->rows_stack[line_number-1] == NULL) continue;
        int move_by = 0; // i need this for the numbers be aligned
        if(line_number < 10) move_by = 2;
        else if(line_number < 100) move_by = 1;
        attron(COLOR_PAIR(7)); // set color to magenta
        move(i, move_by);
        printw("%i", line_number); // print the number of the line
        util_printSyntaxC(cfg->rows_stack[line_number-1]->characters, 4, i, x_max-4, cfg->offset_cursor_x);
    }
    { // draw commands
        attron(COLOR_PAIR(9));
        move(y_max, 0);
        for(int i = 0; i < x_max+1; i++){
            addch(' ');
        }
        move(y_max, 0);
        addch('?');
        move(y_max, 2);
        printw(cfg->command_row->characters);
    }
    { // draw cursor
        if(cfg->mode != CMD_MODE) move(cfg->cursor_y-cfg->offset_cursor_y, cfg->cursor_x+4-cfg->offset_cursor_x);
        else move(y_max, cfg->command_row->size+1);
    }
}
