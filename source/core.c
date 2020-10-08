#include "core.h"
#include "editor.h"
#include "utilities.h"
#include <ncurses.h>
#include <stdio.h>

editor_cfg* mainEditor = NULL;

#define TEXT_DEFAULT_COLOR 1

void core_init(int argc, char** argv){
    initscr();
    keypad(stdscr, TRUE);
    start_color();
    use_default_colors();
    init_pair(TEXT_DEFAULT_COLOR, COLOR_WHITE+8, COLOR_BLACK);
    init_pair(2, COLOR_CYAN+8, COLOR_BLACK);
    init_pair(3, COLOR_GREEN+8, COLOR_BLACK);
    init_pair(4, COLOR_MAGENTA+8, COLOR_BLACK);
    init_pair(5, COLOR_BLUE+8, COLOR_BLACK);
    init_pair(6, COLOR_YELLOW+8, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    curs_set(1);
    mainEditor = editor_create();
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
        break;
        default:
        editor_input(mainEditor, ch);
        break;
    }
}

void core_loop(){
    while(1){
        clear();
        editor_draw(mainEditor);
        refresh();
        core_input();
    }
}

void core_quit(){
    endwin();
}

void core_run(int argc, char** argv){
    core_init(argc, argv);
    core_loop();
    core_quit();
}
