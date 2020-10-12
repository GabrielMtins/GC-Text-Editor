#ifndef ROW_H
#define ROW_H

#include <stddef.h>
#include <stdint.h>

typedef struct{
    char characters[256];
    size_t size;
} row;

row* row_create();

void row_addCharacter(row* current_row, const char push_character);

void row_insertCharacter(row* current_row, const char push_character, const size_t position);

void row_remove(row* current_row, const size_t char_pos);

void row_pop(row* current_row);

void row_destroy(row* current_row);

#define MAX_ROWS 1024
#define TEXT_MODE 0
#define CMD_MODE 1

#define TAB_SIZE 4

typedef struct{
    size_t cursor_x, cursor_y;
    row* rows_stack[MAX_ROWS];
    size_t current_row;
    row* command_row;
    int mode;
    int quit;
} editor_cfg;

editor_cfg* editor_create();

void editor_destroy(editor_cfg* cfg);

void editor_addRow(editor_cfg* cfg);

void editor_controlCursor(editor_cfg* cfg, const int key);

void editor_input(editor_cfg* cfg, const int character_push);

void editor_popLastCharacter(editor_cfg* cfg);

void editor_processCommand(editor_cfg* cfg);

void editor_draw(const editor_cfg* cfg);

#endif
