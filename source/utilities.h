#ifndef UTILITIES_H
#define UTILITIES_H

/* some utilities of the editor, like excluding tabs and putting spaces in place */

#include "editor.h"

void editor_saveAsFile(editor_cfg* cfg, const char* filename);

void editor_loadFile(editor_cfg* cfg, const char* filename);

void util_printSyntaxC(const char* row_str, int cursor_x, int cursor_y, int x_max, int offset_x);

#endif
