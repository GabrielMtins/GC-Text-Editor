#ifndef UTILITIES_H
#define UTILITIES_H

/* some utilities of the editor, like excluding tabs and putting spaces in place */

#include "editor.h"

void editor_saveAsFile(const editor_cfg* cfg, const char* filename);

void editor_loadFile(editor_cfg* cfg, const char* filename);

void util_printSyntaxC(const char* row_str, const int cursor_x, const int cursor_y, const int x_max);

#endif
