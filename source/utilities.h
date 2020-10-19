#ifndef UTILITIES_H
#define UTILITIES_H

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

/* some utilities of the editor, like excluding tabs and putting spaces in place */

#include "editor.h"

void editor_saveAsFile(editor_cfg* cfg, const char* filename);

void editor_loadFile(editor_cfg* cfg, const char* filename);

void util_printSyntaxC(const char* row_str, int cursor_x, int cursor_y, int x_max, int offset_x);

#endif
