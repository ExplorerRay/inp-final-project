#include "renderer.h"

int preview_cells[4][2];

void set_up_screen() {
    ansi_init();
    // initscr();
    ansi_clearscr();
    ansi_cursor(0);
    //start_color();
    //noecho();
    //curs_set(false);

    // use_default_colors();
    // Set up color schemes for each block
    // init_pair(CYAN, -1, COLOR_CYAN);
    // init_pair(BLUE, -1, COLOR_BLUE);
    // init_pair(WHITE, -1, COLOR_WHITE);
    // init_pair(YELLOW, -1, COLOR_YELLOW);
    // init_pair(GREEN, -1, COLOR_GREEN);
    // init_pair(PURPLE, -1, COLOR_MAGENTA);
    // init_pair(RED, -1, COLOR_RED);
    // init_pair(GHOST, COLOR_WHITE, -1);
}

void display_grid(int grid[GRID_W][GRID_H]) {
    int i, j, block;

    for (int y = OFFSET; y < GRID_H; y++) {
        new_mvprintw(y-OFFSET, 0, "|");
        for (int x = 0; x < GRID_W; x++) {
            if ((block = grid[x][y]) != EMPTY) {
                // attron(COLOR_PAIR(block));

                if (block == GHOST) {
                    printf("@");
                } else {
                    ansi_setcolor(WHITE, block);
                    printf(" \033[0m");
                }

                // attroff(COLOR_PAIR(block));
            } else {
                printf(" ");
            }
        }
        printf("|\n");
    }

    new_mvprintw(GRID_H-OFFSET, 0, "------------");
}

void new_mvprintw(int row, int col, char *s){
    ansi_setcursor(row, col);
    ansi_putstr(s, 0x70);
}

int display_controls(int row) {
    char *tmp = (char*) malloc(sizeof(char) * 100);
    new_mvprintw(row, MENU_COL, "CONTROLS:"); row++;
    sprintf(tmp, "Down ---------------- %c", DOWN_KEY);
    new_mvprintw(row, MENU_COL, tmp); row++;
    sprintf(tmp, "Left ---------------- %c", LEFT_KEY);
    new_mvprintw(row, MENU_COL, tmp); row++;
    sprintf(tmp, "Right --------------- %c", RIGHT_KEY);
    new_mvprintw(row, MENU_COL, tmp); row++;
    sprintf(tmp, "Hold ---------------- %c", HOLD_KEY);
    new_mvprintw(row, MENU_COL, tmp); row++;
    sprintf(tmp, "Rotate -------------- %c", ROTATE_KEY);
    new_mvprintw(row, MENU_COL, tmp); row++;
    sprintf(tmp, "Pause --------------- %c", PAUSE_KEY);
    new_mvprintw(row, MENU_COL, tmp); row++;
    sprintf(tmp, "Quit ---------------- %c", QUIT_KEY);
    new_mvprintw(row, MENU_COL, tmp); row++;
    sprintf(tmp, "Pause --------------- %c", PAUSE_KEY);
    new_mvprintw(row, MENU_COL, tmp); row++;
    // sprintf(tmp, "Boss Mode ----------- %c", BOSS_MODE_KEY);
    // new_mvprintw(row, MENU_COL, tmp); row++;
    sprintf(tmp, "Resume -------------- %c", RESUME_KEY);
    new_mvprintw(row, MENU_COL, tmp); row++;
    new_mvprintw(row, MENU_COL, "Select -------------- ENTER"); row++;
    return row;
}

void display_preview(int row, BlockType next) {
    // must assign string of 4 spaces to index into
    // later on

    copy_cells(next, preview_cells);
    new_mvprintw(row++, MENU_COL, "NEXT:");
    display_block(row, MENU_COL, next);
}

void display_hold(int row, BlockType t) {
    copy_cells(t, preview_cells);
    display_block(row, MENU_COL+15, t);
}

void display_block(int row, int col, BlockType type) {
    // must assign string of 4 spaces to index into
    // later on
    char preview[2][5] = {"    ", "    "};
    int x, y;

    for (int i=0; i<4; i++) {
        x = preview_cells[i][0];
        y = preview_cells[i][1];
        preview[y+1][x+1] = '@';
    }

    for (int i=0; i<5; i++) {
        if (preview[0][i] == '@') {
            //ansi_setcolor(WHITE, CYAN);
            // attron(COLOR_PAIR(type+1));
        }
        new_mvprintw(row, col+i, " ");
        // attroff(COLOR_PAIR(type+1));
        if (preview[1][i] == '@') {
            //ansi_setcolor(WHITE, CYAN);
            // attron(COLOR_PAIR(type+1));
        }
        ansi_setcolor(WHITE, CYAN);
        new_mvprintw(row+1, col+i, " ");
        // attroff(COLOR_PAIR(type+1));
    }
}

void render_menu(const char* title,
        const char** items,
        int num_items,
        int selection) {
    // Render the menu title
    new_mvprintw(MENU_ROW, MENU_COL, title);

    for (int i=0; i<num_items; i++) {
        // Highlight the currently selected menu item
        if (i == selection) {
            // attron(A_STANDOUT);
        }

        // Render each menu item
        new_mvprintw(MENU_ROW+1+i, MENU_COL+2, items[i]);
        // attroff(A_STANDOUT);
    }
}

void render(State* state) {
    int row = 1;

    display_grid(state->grid);
    row = display_controls(row);
    row++; // blank line

    char *tmp = (char*) malloc(sizeof(char) * 100);
    sprintf(tmp, "SCORE: %d", state->score);
    new_mvprintw(row, MENU_COL, tmp); row++;
    sprintf(tmp, "LEVEL: %d", state->level);
    new_mvprintw(row, MENU_COL, tmp); row++;
    row++; // blank line


    new_mvprintw(row, MENU_COL+15, "HOLD:");
    if (state->held_block != NONE) {
        display_hold(row+1, state->held_block);
    }

    if (state->level < 5) {
        display_preview(row, state->next);
    } else {
        new_mvprintw(row++, MENU_COL, "NO PREVIEW");
        new_mvprintw(row++, MENU_COL, "AT LEVELS");
        new_mvprintw(row++, MENU_COL, "5 OR ABOVE");
    }
}
