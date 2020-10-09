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
    FILE* file = fopen(filename, "r");
    if(file == NULL) return;
    while(!feof(file)){
        char current_char;
        fscanf(file, "%c", &current_char);
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

void util_printSyntaxC(const char* row_str, const int cursor_x, const int cursor_y, const int x_max){
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
    const char* green_words[] = {
        "#include ", "<stdio.h>", "<stdlib.h>", "<string.h>"
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
        for(int j = 0; j < 10; j++){
            if(!strcmp(token, blue_words[j])){
                for(size_t k = i-strlen(blue_words[j])+1; k < i; k++){
                    color_cell[k] = 5;
                }
                token[0] = '\0';
            }
        }
        for(int j = 0; j < 3; j++){
            if(!strcmp(token, cyan_words[j])){
                for(size_t k = i-strlen(cyan_words[j])+1; k <= i; k++){
                    color_cell[k] = 2;
                }
                token[0] = '\0';
            }
        }
        for(int j = 0; j < 4; j++){
            if(!strcmp(token, green_words[j])){
                for(size_t k = i-strlen(green_words[j])+1; k <= i; k++){
                    color_cell[k] = 3;
                }
                token[0] = '\0';
            }
        }
        for(int j = 0; j < 6; j++){
            if(!strcmp(token, red_words[j])){
                for(size_t k = i-strlen(red_words[j])+1; k <= i; k++){
                    color_cell[k] = 4;
                }
                token[0] = '\0';
            }
        }
        if(row_str[i] == 34){
            do{
                color_cell[i] = 3;
                i++;
            } while(row_str[i] != 34 && i < strlen(row_str));
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
