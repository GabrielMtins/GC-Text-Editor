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
