#include "utilities.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ncurses.h>

void editor_saveAsFile(const editor_cfg* cfg, const char* filename){
    FILE* file = fopen(filename, "w");
    for(size_t i = 0; i < cfg->current_row; i++){
        fprintf(file, "%s\n", cfg->rows_stack[i]->characters);
    }
    fclose(file);
}

void editor_loadFile(editor_cfg* cfg, const char* filename){
    strcpy(cfg->current_file, filename);
    FILE* file = fopen(filename, "r");
    if(file == NULL) return;
    editor_cleanLines(cfg);
    editor_addRow(cfg);
    char current_char = 'x';
    while(fscanf(file, "%c", &current_char) == 1){
        if(current_char != '\n'){
            editor_input(cfg, current_char);
        }
        else{
            editor_addRow(cfg);
            cfg->rows_stack[cfg->current_row-1]->characters[0] = '\0';
            cfg->rows_stack[cfg->current_row-1]->size = 1;
        }
    }
    cfg->cursor_x = 0;
    cfg->cursor_y = 0;
    fclose(file);
}

static size_t util_isTokenOnTheList(const char* token, const char* list[], size_t list_size){
    if(token == NULL || list_size == 0) return 0;
    for(size_t i = 0; i < list_size; i++){
        if(!strcmp(token, list[i])) return i+1;
    }
    return 0;
}

static void util_changeColorOnCells(uint8_t* color_cell, uint8_t color, size_t start_cell, size_t num_bytes){
    for(size_t i = start_cell; i < start_cell+num_bytes; i++){
        color_cell[i] = color;
    }
}

void util_printSyntaxC(const char* row_str, int cursor_x, int cursor_y, int x_max){
    if(cursor_x < 0 || cursor_y < 0) return;
    move(cursor_y, cursor_x);
    char token[256] = "";
    uint8_t color_cell[256];
    for(int i = 0; i < 256; i++){
        color_cell[i] = 1;
    }
    const char* blue_words[] = {
        "int ", "char ", "double ", "float ", "void ",
        "int*", "char*", "double*", "float*", "void*",
    };
    const char* red_words[] = {
        "return", "for", "while", "const", "if", "else"
    };
    const char* cyan_words[] = {
        "printf", "scanf", "main"
    };
    for(size_t i = 0; i < strlen(row_str); i++){
        if(row_str[i] == '#'){
            while(i < strlen(row_str)){
                color_cell[i] = 3;
                i++;
            }
            break;
        }
        strncat(token, &row_str[i], 1);
        if(row_str[i] == '+' || row_str[i] == '-' || row_str[i] == '/' || row_str[i] == '*' ||
        row_str[i] == '>' || row_str[i] == '<' || row_str[i] == '|' || row_str[i] == '&' || row_str[i] == '='){
            color_cell[i] = 4;
        }
        size_t isTokenBlue = util_isTokenOnTheList(token, blue_words, 10);
        if(isTokenBlue != 0){
            size_t size_wrd = strlen(blue_words[isTokenBlue-1]);
            util_changeColorOnCells(color_cell, 5, i+1-size_wrd, size_wrd-1);
            token[0] = '\0';
            continue;
        }
        size_t isTokenCyan = util_isTokenOnTheList(token, cyan_words, 3);
        if(isTokenCyan != 0){
            size_t size_wrd = strlen(cyan_words[isTokenCyan-1]);
            util_changeColorOnCells(color_cell, 2, i+1-size_wrd, size_wrd);
            token[0] = '\0';
            continue;
        }
        size_t isTokenRed = util_isTokenOnTheList(token, red_words, 6);
        if(isTokenRed != 0){
            size_t size_wrd = strlen(red_words[isTokenRed-1]);
            util_changeColorOnCells(color_cell, 4, i+1-size_wrd, size_wrd);
            token[0] = '\0';
            continue;
        }
        if(row_str[i] == '\"'){
            do{
                color_cell[i] = 3;
                i++;
            } while(row_str[i] != '\"' && i < strlen(row_str));
            color_cell[i] = 3;
        }
        if(row_str[i] == ' ' || row_str[i] == ';' || row_str[i] == '(' || row_str[i] == ')' || row_str[i] == '{' || row_str[i] == '}'){
            token[0] = '\0';
        }
        if(row_str[i] >= '0' && row_str[i] <= '9'){
            color_cell[i] = 6;
        }
    }
    int size_of_array = strlen(row_str);
    if(size_of_array > x_max) size_of_array = x_max;
    for(int i = 0; i < size_of_array; i++){
        attron(COLOR_PAIR(color_cell[i]));
        if(row_str[i] == '%'){
            printw("%%");
        }
        else{
            addch(row_str[i]);
        }
    }
    attron(COLOR_PAIR(1));
}
