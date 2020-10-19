#include "core.h"
#include "editor.h"
#include "utilities.h"
#include <ncurses.h>
#include <stdio.h>

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

editor_cfg* mainEditor = NULL;

#define TEXT_DEFAULT_COLOR 1

void core_init(int argc, char** argv){
    initscr();
    keypad(stdscr, TRUE);
    use_default_colors();
    start_color();
    noecho();
    int canAddColors = 0;
    // if the terminal have more then 16 colors, we use the brighter ones
    if(COLORS >= 16 && can_change_color()) canAddColors = 8;
    init_pair(TEXT_DEFAULT_COLOR, COLOR_WHITE+canAddColors, -1);
    init_pair(2, COLOR_CYAN+canAddColors, -1);
    init_pair(3, COLOR_GREEN+canAddColors, -1);
    init_pair(4, COLOR_MAGENTA+canAddColors, -1);
    init_pair(5, COLOR_BLUE+canAddColors, -1);
    init_pair(6, COLOR_YELLOW+canAddColors, -1);
    init_pair(7, COLOR_MAGENTA, -1);
    init_pair(8, COLOR_RED+canAddColors, -1);
    init_pair(9, COLOR_BLACK, COLOR_WHITE);
    attron(COLOR_PAIR(1));
    curs_set(1);
    mainEditor = editor_create();
    if(argc > 1){
        editor_loadFile(mainEditor, argv[1]);
    }
    printw("GCTE Copyright (C) 2020 Gabriel Martins\n");
    printw("This program comes with ABSOLUTELY NO WARRANTY\n");
    printw("This is free software, and you are welcome to redistribute it\n");
    printw("under certain conditions; (read COPYING for details)\n");
    printw("Press any key to continue\n");
    refresh();
    getch();
}

void core_input(){
    int ch = getch();
    switch(ch){
        case 27: // esc key
        mainEditor->mode = !mainEditor->mode;
        break;
        default:
        editor_input(mainEditor, ch);
        break;
    }
}

void core_loop(){
    while(!mainEditor->quit){
        clear();
        editor_draw(mainEditor);
        refresh();
        core_input();
    }
}

void core_quit(){
    editor_destroy(mainEditor);
    endwin();
}

void core_run(int argc, char** argv){
    core_init(argc, argv);
    core_loop();
    core_quit();
}
