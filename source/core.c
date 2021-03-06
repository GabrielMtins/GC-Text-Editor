#include "core.h"
#include "editor.h"
#include "utilities.h"
#include <ncurses.h>
#include <stdio.h>
#include <string.h>

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
    contact: gabrielmartinsthe@gmail.com
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
    init_pair(TEXT_DEFAULT_COLOR, COLOR_WHITE+canAddColors, COLOR_BLACK);
    init_pair(2, COLOR_CYAN+canAddColors, COLOR_BLACK);
    init_pair(3, COLOR_GREEN+canAddColors, COLOR_BLACK);
    init_pair(4, COLOR_MAGENTA+canAddColors, COLOR_BLACK);
    init_pair(5, COLOR_BLUE+canAddColors, COLOR_BLACK);
    init_pair(6, COLOR_YELLOW+canAddColors, COLOR_BLACK);
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(8, COLOR_RED+canAddColors, COLOR_BLACK);
    init_pair(9, COLOR_BLACK, COLOR_WHITE);
    attron(COLOR_PAIR(1));
    bkgd(COLOR_PAIR(1));
    curs_set(1);
    mainEditor = editor_create();
    { // start screen
        int y_max, x_max;
        getmaxyx(stdscr, y_max, x_max);
        const char* text[] = {
            "GCTE Copyright (C) 2020 Gabriel Martins\n",
            "This program comes with ABSOLUTELY NO WARRANTY\n",
            "This is free software, and you are welcome to redistribute it\n",
            "under certain conditions; (read COPYING for details)\n",
            "Press any key to continue\n",
        };
        for(int i = 0; i < 5; i++){
            move(y_max/2-3+i, x_max/2-strlen(text[i])/2);
            printw(text[i]);
        }
        refresh();
        getch();
    }
    if(argc > 1){
        editor_loadFile(mainEditor, argv[1]);
    }
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
