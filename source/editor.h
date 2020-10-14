#ifndef EDITOR_H
#define EDITOR_H

#include <stddef.h>
#include <stdint.h>

#include "row.h"

#define MAX_ROWS 1024
#define TEXT_MODE 0
#define CMD_MODE 1

#define TAB_SIZE 4

typedef struct{
    size_t cursor_x, cursor_y;
    row* rows_stack[MAX_ROWS];
    size_t current_row;
    row* command_row;
    char current_file[256];
    int mode;
    int quit;
} editor_cfg;

editor_cfg* editor_create();

void editor_cleanLines(editor_cfg* cfg);

void editor_destroy(editor_cfg* cfg);

void editor_addRow(editor_cfg* cfg);

void editor_controlCursor(editor_cfg* cfg, int key);

void editor_input(editor_cfg* cfg, int character_push);

void editor_popLastCharacter(editor_cfg* cfg);

void editor_processCommand(editor_cfg* cfg);

void editor_draw(const editor_cfg* cfg);

#endif
