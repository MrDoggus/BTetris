#include "tdraw.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// --- DEFINITIONS --- //

#define TDRAW_PLAYFIELD_XOFFSET (TETRIS_WIDTH*2 + 2 + 1)
#define TDRAW_PLAYFIELD_YOFFSET (TETRIS_HEIGHT + 2 + 1)
#define TDRAW_TITLE_YOFFSET 6
#define TDRAW_PPREVIEW_XOFFSET 20
#define TDRAW_PPREVIEW_YOFFSET 6
#define TDRAW_SCORE_YOFFSET 4
#define TDRAW_GINFO_XOFFSET 32

typedef union tdraw_coord {
    uint32_t isRendered;
    struct {
        uint16_t x;
        uint16_t y;
    } offset;
} tdraw_coord_t;

typedef struct tdraw_winoff {
    int draw_width;
    int draw_height;
    tdraw_coord_t title;
    tdraw_coord_t pfield;
    tdraw_coord_t pprev;
    tdraw_coord_t score;
    tdraw_coord_t debug;
    tdraw_coord_t ginfo;
} tdraw_winoff_t;

typedef struct tdraw_menuitem {
    char* name;
    char* desc;
    int loffset;
} tdraw_menuitem;

/// @brief Draws a tetris block on a given window
/// @param window Window to print block
/// @param color Color to give block
/// @return Error code
int tdraw_block(WINDOW* window, tetris_color_t color);

/// @brief Calculates and returns the window offsets
/// @return Window offset structure.
tdraw_winoff_t tdraw_calc_offsets();

// --- GLOBAL VARS --- //

tdraw_winoff_t win_offsets;

WINDOW* wintitle;
WINDOW* winpfield;
WINDOW* winpprev;
WINDOW* winscore;
WINDOW* windebug;
WINDOW* winginfo;
WINDOW* debug_window;

int hascolors;

// --- Private Functions --- //

int tdraw_block(WINDOW* window, tetris_color_t bcolor)
{
    chtype solid_block;

    if (hascolors)
    {
        if (bcolor) {
            solid_block = ('0'+bcolor) | COLOR_PAIR(bcolor+8);
        }
        else {
            solid_block = ' ' | COLOR_PAIR(8);
        }
        
        waddch(window, solid_block);
        waddch(window, solid_block);
    }
    else
    {
        if (bcolor) {
            solid_block = ('0'+bcolor);
        }
        else {
            solid_block = ' ';
        }
        
        waddch(window, solid_block);
        waddch(window, solid_block);
    }

    return 0;
}

tdraw_winoff_t tdraw_calc_offsets()
{
    int c_height, c_width;
    tdraw_winoff_t new_winoff;

    // Get terminal size
    getmaxyx(stdscr, c_height, c_width);

    // Calculate offsets
    if (c_height >= TDRAW_TITLE_YOFFSET + TDRAW_PLAYFIELD_YOFFSET) 
    {
        new_winoff.draw_height = TDRAW_TITLE_YOFFSET + TDRAW_PLAYFIELD_YOFFSET;

        if (c_width >= TDRAW_PLAYFIELD_XOFFSET + TDRAW_PPREVIEW_XOFFSET + TDRAW_GINFO_XOFFSET) 
        {
            new_winoff.draw_width = TDRAW_PLAYFIELD_XOFFSET + TDRAW_PPREVIEW_XOFFSET + TDRAW_GINFO_XOFFSET;

            new_winoff.title.offset.x = 1;
            new_winoff.title.offset.y = 1;

            new_winoff.pfield.offset.x = 1;
            new_winoff.pfield.offset.y = 1 + TDRAW_TITLE_YOFFSET;

            new_winoff.pprev.offset.x = 1 + TDRAW_PLAYFIELD_XOFFSET;
            new_winoff.pprev.offset.y = 1 + TDRAW_TITLE_YOFFSET;
            
            new_winoff.score.offset.x = 1 + TDRAW_PLAYFIELD_XOFFSET;
            new_winoff.score.offset.y = 1 + TDRAW_TITLE_YOFFSET + TDRAW_PPREVIEW_YOFFSET;

            new_winoff.debug.offset.x = 1 + TDRAW_PLAYFIELD_XOFFSET;
            new_winoff.debug.offset.y = 1 + TDRAW_TITLE_YOFFSET + TDRAW_PPREVIEW_YOFFSET + TDRAW_SCORE_YOFFSET;

            new_winoff.ginfo.offset.x = 1 + TDRAW_PLAYFIELD_XOFFSET + TDRAW_PPREVIEW_XOFFSET;
            new_winoff.ginfo.offset.y = 1 + TDRAW_TITLE_YOFFSET;
        }
        else if (c_width >= TDRAW_PLAYFIELD_XOFFSET + TDRAW_PPREVIEW_XOFFSET) 
        {
            new_winoff.draw_width = TDRAW_PLAYFIELD_XOFFSET + TDRAW_PPREVIEW_XOFFSET;

            new_winoff.title.offset.x = 1;
            new_winoff.title.offset.y = 1;

            new_winoff.pfield.offset.x = 1;
            new_winoff.pfield.offset.y = 1 + TDRAW_TITLE_YOFFSET;

            new_winoff.pprev.offset.x = 1 + TDRAW_PLAYFIELD_XOFFSET;
            new_winoff.pprev.offset.y = 1 + TDRAW_TITLE_YOFFSET;
            
            new_winoff.score.offset.x = 1 + TDRAW_PLAYFIELD_XOFFSET;
            new_winoff.score.offset.y = 1 + TDRAW_TITLE_YOFFSET + TDRAW_PPREVIEW_YOFFSET;

            new_winoff.debug.offset.x = 1 + TDRAW_PLAYFIELD_XOFFSET;
            new_winoff.debug.offset.y = 1 + TDRAW_TITLE_YOFFSET + TDRAW_PPREVIEW_YOFFSET + TDRAW_SCORE_YOFFSET;

            new_winoff.ginfo.isRendered = 0;
        }
        else if (c_width >= TDRAW_PLAYFIELD_XOFFSET) 
        {
            new_winoff.draw_width = TDRAW_PLAYFIELD_XOFFSET;

            new_winoff.title.offset.x = 1;
            new_winoff.title.offset.y = 1;

            new_winoff.pfield.offset.x = 1;
            new_winoff.pfield.offset.y = 1 + TDRAW_TITLE_YOFFSET;

            new_winoff.pprev.isRendered = 0;
            new_winoff.score.isRendered = 0;
            new_winoff.title.isRendered = 0;
            new_winoff.ginfo.isRendered = 0;
        }
        else
        {
            new_winoff.draw_width = 0;

            new_winoff.pfield.isRendered = 0;
            new_winoff.title.isRendered = 0;
            new_winoff.pprev.isRendered = 0;
            new_winoff.score.isRendered = 0;
            new_winoff.debug.isRendered = 0;
            new_winoff.ginfo.isRendered = 0;
        }
    }
    else if (c_height >= TDRAW_PLAYFIELD_YOFFSET) 
    {
        new_winoff.draw_height = TDRAW_PLAYFIELD_YOFFSET;

        if (c_width >= TDRAW_PLAYFIELD_XOFFSET + TDRAW_PPREVIEW_XOFFSET + TDRAW_GINFO_XOFFSET) 
        {
            new_winoff.draw_width = TDRAW_PLAYFIELD_XOFFSET + TDRAW_PPREVIEW_XOFFSET + TDRAW_GINFO_XOFFSET;

            new_winoff.pfield.offset.x = 1;
            new_winoff.pfield.offset.y = 1;

            new_winoff.pprev.offset.x = 1 + TDRAW_PLAYFIELD_XOFFSET;
            new_winoff.pprev.offset.y = 1;

            new_winoff.score.offset.x = 1 + TDRAW_PLAYFIELD_XOFFSET;
            new_winoff.score.offset.y = 1 + TDRAW_PPREVIEW_YOFFSET;

            new_winoff.debug.offset.x = 1 + TDRAW_PLAYFIELD_XOFFSET;
            new_winoff.debug.offset.y = 1 + TDRAW_PPREVIEW_YOFFSET + TDRAW_SCORE_YOFFSET;

            new_winoff.ginfo.offset.x = 1 + TDRAW_PLAYFIELD_XOFFSET + TDRAW_PPREVIEW_XOFFSET;
            new_winoff.ginfo.offset.y = 1;

            new_winoff.title.isRendered = 0;
        }
        else if (c_width >= TDRAW_PLAYFIELD_XOFFSET + TDRAW_PPREVIEW_XOFFSET) 
        {            
            new_winoff.draw_width = TDRAW_PLAYFIELD_XOFFSET + TDRAW_PPREVIEW_XOFFSET;

            new_winoff.pfield.offset.x = 1;
            new_winoff.pfield.offset.y = 1;

            new_winoff.pprev.offset.x = 1 + TDRAW_PLAYFIELD_XOFFSET;
            new_winoff.pprev.offset.y = 1;

            new_winoff.score.offset.x = 1 + TDRAW_PLAYFIELD_XOFFSET;
            new_winoff.score.offset.y = 1 + TDRAW_PPREVIEW_YOFFSET;

            new_winoff.debug.offset.x = 1 + TDRAW_PLAYFIELD_XOFFSET;
            new_winoff.debug.offset.y = 1 + TDRAW_PPREVIEW_YOFFSET + TDRAW_SCORE_YOFFSET;

            new_winoff.title.isRendered = 0;
            new_winoff.ginfo.isRendered = 0;
        }
        else if (c_width >= TDRAW_PLAYFIELD_XOFFSET) 
        {
            new_winoff.draw_width = TDRAW_PLAYFIELD_XOFFSET;

            new_winoff.pfield.offset.x = 1;
            new_winoff.pfield.offset.y = 1;

            new_winoff.title.isRendered = 0;
            new_winoff.pprev.isRendered = 0;
            new_winoff.score.isRendered = 0;
            new_winoff.debug.isRendered = 0;
            new_winoff.ginfo.isRendered = 0;
        }
        else 
        {
            new_winoff.draw_width = 0;

            new_winoff.pfield.isRendered = 0;
            new_winoff.title.isRendered = 0;
            new_winoff.pprev.isRendered = 0;
            new_winoff.score.isRendered = 0;
            new_winoff.debug.isRendered = 0;
            new_winoff.ginfo.isRendered = 0;
        }
    }
    else 
    {
        new_winoff.draw_height = 0;
        new_winoff.draw_width = 0;
        
        new_winoff.pfield.isRendered = 0;
        new_winoff.title.isRendered = 0;
        new_winoff.pprev.isRendered = 0;
        new_winoff.score.isRendered = 0;
        new_winoff.debug.isRendered = 0;
        new_winoff.ginfo.isRendered = 0;
    }

    return new_winoff;
}

int tdraw_keybinds();

// --- Public Functions --- //

int tdraw_keybinds()
{
    WINDOW* winkeybinds;

    #define KEYMENULEN 8
    tdraw_menuitem menu[KEYMENULEN] = {
        {.name = "Leftshift",                   .desc = "aA \'KEY_LEFT\'",  .loffset = 1},
        {.name = "Rightshift",                  .desc = "aD \'KEY_RIGHT\'", .loffset = 1},
        {.name = "Clockwise rotate",            .desc = "wWeE \'KEY_UP\'",  .loffset = 1},
        {.name = "Counter-Clockwise rotate",    .desc = "zZqQ",             .loffset = 1},
        {.name = "Soft Drop",                   .desc = "sS \'KEY_DOWN\'",  .loffset = 1},
        {.name = "Hard Drop",                   .desc = "\'SPACE\'",        .loffset = 1},
        {.name = "Pause",                       .desc = "qQ \'ESC\'",       .loffset = 1},
        {.name = "EXIT",                        .desc = "EXIT keybinds",    .loffset = 1}
    };

    const int winheight = KEYMENULEN + 6;
    const int winwidth = 27;

    if (win_offsets.draw_height >= winheight && win_offsets.draw_width >= winwidth)
    {
        winkeybinds = newwin(winheight, winwidth, (win_offsets.draw_height - winheight)/2, (win_offsets.draw_width - winwidth)/2);

        for (int i = 0; i < KEYMENULEN; i++)
        {
            menu[i].loffset = (winwidth - strlen(menu[i].name))/2;
        }
    }
    else
    {
        winkeybinds = newwin(win_offsets.draw_height, win_offsets.draw_width, 0, 0);
    }

    mvwaddstr(winkeybinds, 1, (winwidth - 10) / 2, "KEYBINDS:");
    box(winkeybinds, 0, 0); 
    refresh();

    char doloop = 1;
    int key;
    int highlight = 0;
    while (doloop)
    {
        for (int i = 0; i < KEYMENULEN + 1; i++)
        {
            if (KEYMENULEN == i)
            {
                wmove(winkeybinds, 4 + KEYMENULEN, 1);
                wclrtoeol(winkeybinds);
                mvwaddstr(winkeybinds, 4 + KEYMENULEN, (winwidth - strlen(menu[highlight].desc))/2, menu[highlight].desc);
                continue;
            }
            else if (highlight == i) 
            {
                wattron(winkeybinds, A_STANDOUT);
            }

            mvwaddstr(winkeybinds, 3 + i, menu[i].loffset, menu[i].name);
            wattroff(winkeybinds, A_STANDOUT);
        }
        box(winkeybinds, 0, 0);

        wrefresh(winkeybinds);

        key = getch();
        switch (key)
        {
        case ERR:
            break;

        case 'd':
        case 'D':
        case KEY_RIGHT:
        case 's':
        case 'S':
        case KEY_DOWN:
            highlight = (highlight+1) % KEYMENULEN;
            break;

        case 'a':
        case 'A':
        case KEY_LEFT:
        case 'w':
        case 'W':
        case KEY_UP:
            highlight = (highlight-1) % KEYMENULEN;
            highlight = (highlight<0) ? highlight+KEYMENULEN : highlight;
            break;

        case '\n':
        case KEY_ENTER:
            switch (highlight)
            {
            case KEYMENULEN-1:
                doloop = 0;
                break;
            
            default:
                break;
            }
            break;

        default:
            break;
        }

        usleep(1000);
    }

    werase(winkeybinds);
    wrefresh(winkeybinds);
    delwin(winkeybinds);
    tdraw_touchwin();

    return 0;
}

int tdraw_start(tetris_game_t* game)
{
    WINDOW* winstart;

    #define SMENULEN 3
    tdraw_menuitem menu[SMENULEN] = {
        {.name = "KEYBINDS",    .loffset = 1},
        {.name = "START",       .loffset = 1},
        {.name = "QUIT",        .loffset = 1}
    };
    
    const int winheight = SMENULEN + 5;
    const int winwidth = 20;

    if (win_offsets.draw_height >= winheight && win_offsets.draw_width >= winwidth)
    {
        winstart = newwin(winheight, winwidth, (win_offsets.draw_height - winheight)/2, (win_offsets.draw_width - winwidth)/2);

        for (int i = 0; i < SMENULEN; i++)
        {
            menu[i].loffset = (winwidth - strlen(menu[i].name))/2;
        }
    }
    else
    {
        winstart = newwin(win_offsets.draw_height, win_offsets.draw_width, 0, 0);
    }

    mvwaddstr(winstart, 1, 1, "TETRIS START MENU");
    mvwaddstr(winstart, 2, 1, "(ENTER to select)");
    box(winstart, 0, 0);
    refresh();

    char doloop = 1;
    int key;
    int highlight = 0;
    while (doloop)
    {
        for (int i = 0; i < SMENULEN; i++)
        {
            if (highlight == i) 
            {
                wattron(winstart, A_STANDOUT);
            }

            mvwaddstr(winstart, 4 + i, menu[i].loffset, menu[i].name);
            wattroff(winstart, A_STANDOUT);
        }

        wrefresh(winstart);

        key = getch();

        if (key != ERR)
        {
            tetris_rand_entropy(game, key);
        }

        switch (key)
        {
        case ERR:
            break;

        case 'd':
        case 'D':
        case KEY_RIGHT:
        case 's':
        case 'S':
        case KEY_DOWN:
            highlight = (highlight+1) % SMENULEN;
            break;

        case 'a':
        case 'A':
        case KEY_LEFT:
        case 'w':
        case 'W':
        case KEY_UP:
            highlight = (highlight-1) % SMENULEN;
            highlight = (highlight<0) ? highlight+SMENULEN : highlight;
            break;

        case '\n':
        case KEY_ENTER:
            switch (highlight)
            {
            case 0:
                tdraw_keybinds();
                touchwin(winstart);
                break;
            case 1:
                doloop = 0;
                break;
            case 2: 
                endwin();
                exit(0);
            default:
                break;
            }
            break;

        default:
            break;
        }

        usleep(1000);

        tetris_tick(game, 1000);
    }

    tetris_start(game);

    werase(winstart);
    wrefresh(winstart);
    delwin(winstart);
    tdraw_touchwin();

    return 0;
}

int tdraw_pause(tetris_game_t* game)
{
    WINDOW* winpause;

    #define PMENULEN 3
    tdraw_menuitem menu[PMENULEN] = {
        {.name = "KEYBINDS",    .loffset = 1},
        {.name = "RESUME",      .loffset = 1},
        {.name = "QUIT",        .loffset = 1}
    };

    tetris_pause(game);

    const int winheight = PMENULEN + 5;
    const int winwidth = 20;

    if (win_offsets.draw_height >= winheight && win_offsets.draw_width >= winwidth)
    {
        winpause = newwin(winheight, winwidth, (win_offsets.draw_height - winheight)/2, (win_offsets.draw_width - winwidth)/2);

        for (int i = 0; i < PMENULEN; i++)
        {
            menu[i].loffset = (winwidth - strlen(menu[i].name))/2;
        }
    }
    else
    {
        winpause = newwin(win_offsets.draw_height, win_offsets.draw_width, 0, 0);
    }

    mvwaddstr(winpause, 1, 1, "TETRIS PAUSE MENU");
    mvwaddstr(winpause, 2, 1, "(ENTER to select)");
    box(winpause, 0, 0);
    refresh();

    char doloop = 1;
    int key;
    int highlight = 0;
    while (doloop)
    {
        for (int i = 0; i < PMENULEN; i++)
        {
            if (highlight == i) 
            {
                wattron(winpause, A_STANDOUT);
            }

            mvwaddstr(winpause, 4 + i, menu[i].loffset, menu[i].name);
            wattroff(winpause, A_STANDOUT);
        }

        wrefresh(winpause);

        key = getch();

        if (key != ERR)
        {
            tetris_rand_entropy(game, key);
        }

        switch (key)
        {
        case ERR:
            break;

        case 'd':
        case 'D':
        case KEY_RIGHT:
        case 's':
        case 'S':
        case KEY_DOWN:
            highlight = (highlight+1) % PMENULEN;
            break;

        case 'a':
        case 'A':
        case KEY_LEFT:
        case 'w':
        case 'W':
        case KEY_UP:
            highlight = (highlight-1) % PMENULEN;
            highlight = (highlight<0) ? highlight+PMENULEN : highlight;
            break;

        case '\n':
        case KEY_ENTER:
            switch (highlight)
            {
            case 0:
                tdraw_keybinds();
                touchwin(winpause);
                break;
            case 1:
                doloop = 0;
                break;
            case 2:
                endwin();
                exit(0);
            default:
                break;
            }
            break;

        default:
            break;
        }

        usleep(1000);
    }

    werase(winpause);
    wrefresh(winpause);
    delwin(winpause);
    tdraw_touchwin();

    tetris_unpause(game);

    return 0;
}

int tdraw_gameover(tetris_game_t* game)
{
    WINDOW* wingover;

    #define GMENULEN 3
    tdraw_menuitem menu[GMENULEN] = {
        {.name = "KEYBINDS",    .loffset = 1},
        {.name = "PLAY AGAIN",  .loffset = 1},
        {.name = "QUIT",        .loffset = 1}
    };

    const int winheight = GMENULEN + 6;
    const int winwidth = 20;

    if (win_offsets.draw_height >= winheight && win_offsets.draw_width >= winwidth)
    {
        wingover = newwin(winheight, winwidth, (win_offsets.draw_height - winheight)/2, (win_offsets.draw_width - winwidth)/2);

        for (int i = 0; i < GMENULEN; i++)
        {
            menu[i].loffset = (winwidth - strlen(menu[i].name))/2;
        }
    }
    else
    {
        wingover = newwin(win_offsets.draw_height, win_offsets.draw_width, 0, 0);
    }

    char strbuff[18];
    int sprintret;
    mvwaddstr(wingover, 1, (winwidth - 12)/2, "GAME OVER!!");

    sprintret = snprintf(strbuff, 18, "Score: %ld", game->score);
    mvwaddstr(wingover, 2, (winwidth - sprintret)/2, strbuff);

    sprintret = snprintf(strbuff, 18, "Level: %d", game->level);
    mvwaddstr(wingover, 3, (winwidth - sprintret)/2, strbuff);

    box(wingover, 0, 0);
    refresh();

    char doloop = 1;
    int key;
    int highlight = 0;
    while (doloop)
    {
        for (int i = 0; i < GMENULEN; i++)
        {
            if (highlight == i) 
            {
                wattron(wingover, A_STANDOUT);
            }

            mvwaddstr(wingover, 5 + i, menu[i].loffset, menu[i].name);
            wattroff(wingover, A_STANDOUT);
        }

        wrefresh(wingover);

        key = getch();

        if (key != ERR)
        {
            tetris_rand_entropy(game, key);
        }

        switch (key)
        {
        case ERR:
            break;

        case 'd':
        case 'D':
        case KEY_RIGHT:
        case 's':
        case 'S':
        case KEY_DOWN:
            highlight = (highlight+1) % GMENULEN;
            break;

        case 'a':
        case 'A':
        case KEY_LEFT:
        case 'w':
        case 'W':
        case KEY_UP:
            highlight = (highlight-1) % GMENULEN;
            highlight = (highlight<0) ? highlight+GMENULEN : highlight;
            break;

        case '\n':
        case KEY_ENTER:
            switch (highlight)
            {
            case 0:
                tdraw_keybinds();
                touchwin(wingover);
                break;
            case 1:
                doloop = 0;
                break;
            case 2:
                endwin();
                exit(0);
            default:
                break;
            }
            break;

        default:
            break;
        }

        usleep(1000);
    }

    tetris_reset(game);
    tetris_start(game);

    werase(wingover);
    wrefresh(wingover);
    delwin(wingover);
    tdraw_touchwin();

    return 0;
}

int tdraw_pfield(tetris_game_t* game)
{
    char fdrawn;
    int fidx = 0;

    if (!win_offsets.pfield.isRendered) {
        return 1;
    }

    for (int y = TETRIS_HEIGHT-1; y >= 0; y--)
    {
        wmove(winpfield, TETRIS_HEIGHT - y, 1);
        for (int x = 0; x < TETRIS_WIDTH; x++)
        {
            if (game->board->pf[y][x] == TETRIS_BLANK)
            {
                fdrawn = 0;
                for (int i = fidx; i < 4; i++)
                {
                    if (game->board->fpos[i].h == y && game->board->fpos[i].w == x) {
                        tdraw_block(winpfield, game->board->fcol);
                        fidx++;
                        fdrawn = 1;
                        break;
                    }
                }

                if (!fdrawn) {
                    tdraw_block(winpfield, TETRIS_BLANK);
                }
            }
            else {
                tdraw_block(winpfield, game->board->pf[y][x]);
            }
        }
    }

    wmove(winpfield, 1, 1);

    wrefresh(winpfield);

    return 0;
}

int tdraw_pprev(tetris_game_t* game)
{
    if (!win_offsets.pprev.isRendered) {
        return 1;
    }

    const tetris_coord_t PPOFFSET = (tetris_coord_t){
        .h = (TETRIS_HEIGHT),
        .w = ((TETRIS_WIDTH/2)) - 2
    };

    char isOddWidth;
    tetris_coord_t tmp_coord;

    werase(winpprev);

    for (int i = 1; i < 5; i++)
    {
        mvwaddch(winpprev, i, 9, '|');
    }
    

    // Loop through pieces in piece preview 
    for (int ppi = 0; ppi < TETRIS_PP_SIZE; ppi++)
    {
        if (game->ppreview[ppi] == TETRIS_BLANK) {
            continue;
        }

        if (game->ppreview[ppi] > 2) {
            isOddWidth = 1;
        }
        else {
            isOddWidth = 0;
        }

        // Loop through blocks for a piece in piece preview
        for (int ppj = 0; ppj < 4; ppj++)
        {
            tmp_coord = tetris_subCoord(TETRIS_TETROMINO_START[game->ppreview[ppi]][ppj], PPOFFSET);
            tmp_coord.h = 3 - tmp_coord.h;

            wmove(winpprev, tmp_coord.h + 1, tmp_coord.w*2 + ppi * 9 + isOddWidth + 1);
            tdraw_block(winpprev, game->ppreview[ppi]);
        }
    }

    box(winpprev, 0, 0);

    wrefresh(winpprev);

    return 0;
}

int tdraw_score(tetris_game_t* game)
{
    if (!win_offsets.score.isRendered) {
        return 1;
    }

    mvwprintw(winscore, 1, 1, "Score: %ld", game->score);
    wclrtoeol(winscore);

    mvwprintw(winscore, 2, 1, "Level: %d", game->level);
    wclrtoeol(winscore);

    box(winscore, 0, 0);

    wrefresh(winscore);

    return 0;
}

int tdraw_ginfo(tetris_game_t* game)
{
    if (!win_offsets.ginfo.isRendered) {
        return 1;
    }

    werase(winginfo);

    box(winginfo, 0, 0);

    mvwprintw(winginfo, 1, 1, "tetris_game: state");
    mvwprintw(winginfo, 2, 3, "Started: %s", game->isStarted ? "true" : "false");
    mvwprintw(winginfo, 3, 3, "Running: %s", game->isRunning ? "true" : "false");
    mvwprintw(winginfo, 4, 3, "Gameover: %s", game->isGameover ? "true" : "false");

    mvwprintw(winginfo, 5, 1, "tetris_game: score");
    mvwprintw(winginfo, 6, 3, "combo: %d", game->combo);
    mvwprintw(winginfo, 7, 3, "lines: %d", game->lines);

    mvwprintw(winginfo, 8, 1, "tetris_game: tetromino queue");
    mvwprintw(winginfo, 9, 3, "queue idx: %d", game->qidx);
    mvwprintw(winginfo, 10, 3, "pqueue: ");
    for (int i = 0; i < 7; i++)
    {
        tdraw_block(winginfo, game->queue[i]);
    }
    mvwprintw(winginfo, 11, 3, "squeue: ");
    for (int i = 0; i < 7; i++)
    {
        tdraw_block(winginfo, game->shuffle_queue[i]);
    }
    mvwprintw(winginfo, 12, 3, "randx: %d", game->randx);

    mvwprintw(winginfo, 13, 1, "tetris_game: time");
    mvwprintw(winginfo, 14, 3, "tmicro: %ld", game->tmicro);
    mvwprintw(winginfo, 15, 3, "tdrop: %ld", game->tdrop);

    mvwprintw(winginfo, 17, 1, "tetris_board:");
    mvwprintw(winginfo, 18, 3, "falling color: ");
    tdraw_block(winginfo, game->board->fcol);
    mvwprintw(winginfo, 19, 3, "falling rotation: %d", game->board->frot);
    mvwprintw(winginfo, 20, 3, "gc_valid: %s", game->board->gc_valid ? "true" : "false");

    wrefresh(winginfo);

    return 0;
}

int tdraw_initcolor()
{
    hascolors = has_colors();
    if (hascolors)
    {
        start_color();
        // use_default_colors();

        int TCOLOR_CYAN = COLORS-7;
        int TCOLOR_YELLOW = COLORS-6;
        int TCOLOR_BLUE = COLORS-5;
        int TCOLOR_ORANGE = COLORS-4;
        int TCOLOR_GREEN = COLORS-3;
        int TCOLOR_PURPLE = COLORS-2;
        int TCOLOR_RED = COLORS-1;

        // init_color(COLOR_BLACK, 0, 0, 0);
        init_color(TCOLOR_CYAN, 0, 900, 900);
        init_color(TCOLOR_YELLOW, 900, 900, 0);
        init_color(TCOLOR_BLUE, 100, 100, 900);
        init_color(TCOLOR_ORANGE, 900, 500, 0);
        init_color(TCOLOR_GREEN, 0, 900, 0);
        init_color(TCOLOR_PURPLE, 500, 0, 900);
        init_color(TCOLOR_RED, 900, 0, 0);
        
        
        init_pair(TETRIS_CYAN+8, TCOLOR_CYAN, TCOLOR_CYAN);
        init_pair(TETRIS_YELLOW+8, TCOLOR_YELLOW, TCOLOR_YELLOW);
        init_pair(TETRIS_BLUE+8, TCOLOR_BLUE, TCOLOR_BLUE);
        init_pair(TETRIS_ORANGE+8, TCOLOR_ORANGE, TCOLOR_ORANGE);
        init_pair(TETRIS_GREEN+8, TCOLOR_GREEN, TCOLOR_GREEN);
        init_pair(TETRIS_PURPLE+8, TCOLOR_PURPLE, TCOLOR_PURPLE);
        init_pair(TETRIS_RED+8, TCOLOR_RED, TCOLOR_RED);
        init_pair(TETRIS_BLANK+8, COLOR_BLACK, COLOR_BLACK);

        if (win_offsets.debug.isRendered)
        {
            wprintw(debug_window, "Color :)\n");
            wprintw(debug_window, "COLOR_PAIRS: %d, COLORS: %d\n", COLOR_PAIRS, COLORS);
        }
    }
    else 
    {
        if (win_offsets.debug.isRendered)
        {
            wprintw(debug_window, "No color :(\n");
        }
        return 1;
    }

    return 0;
}

int tdraw_wininit()
{
    win_offsets = tdraw_calc_offsets();

    if (win_offsets.title.isRendered)
    {
        wintitle = newwin(6, 42, win_offsets.title.offset.y, win_offsets.title.offset.x);
        box(wintitle, 0, 0);
        wrefresh(wintitle);
    }

    if (win_offsets.pfield.isRendered)
    {
        winpfield = newwin(TETRIS_HEIGHT + 2, TETRIS_WIDTH*2 + 2, win_offsets.pfield.offset.y, win_offsets.pfield.offset.x);
        box(winpfield, 0, 0);
        wrefresh(winpfield);
    }

    if (win_offsets.pprev.isRendered)
    {
        winpprev = newwin(6, 19, win_offsets.pprev.offset.y, win_offsets.pprev.offset.x);
        box(winpprev, 0, 0);
        wrefresh(winpprev);
    }

    if (win_offsets.score.isRendered)
    {
        winscore = newwin(4, 19, win_offsets.score.offset.y, win_offsets.score.offset.x);
        box(winscore, 0, 0);
        wrefresh(winscore);
    }

    if (win_offsets.debug.isRendered)
    {
        windebug = newwin(12, 19, win_offsets.debug.offset.y, win_offsets.debug.offset.x);
        box(windebug, 0, 0);
        wrefresh(windebug);

        // Create subwindow inside of the box so the box isnt overwritten
        // debug_window = derwin(windebug, 8, 17, 1, 1);
        debug_window = newwin(10, 17, getbegy(windebug)+1, getbegx(windebug)+1);
        scrollok(debug_window, true);
        wrefresh(debug_window);
        
        // touchwin(windebug);
    }

    if (win_offsets.ginfo.isRendered)
    {
        winginfo = newwin(22, 31, win_offsets.ginfo.offset.y, win_offsets.ginfo.offset.x);
        box(winginfo, 0, 0);
        wrefresh(winginfo);
    }

    return 0;
}

int tdraw_winupdate()
{
    tdraw_winoff_t new_winoff;

    new_winoff = tdraw_calc_offsets();

    if (win_offsets.title.isRendered != new_winoff.title.isRendered)
    {
        // Not enough room for title window, delete window
        if (win_offsets.title.isRendered && !new_winoff.title.isRendered) {
            delwin(wintitle);
            wintitle = NULL;
        }
        // There is room to create title window
        else if (!win_offsets.title.isRendered && new_winoff.title.isRendered) {
            wintitle = newwin(6, 42, new_winoff.title.offset.y, new_winoff.title.offset.x);
            box(winpfield, 0, 0);
            wrefresh(wintitle);
        }
        else {
            mvwin(wintitle, new_winoff.title.offset.y, new_winoff.title.offset.x);
            wrefresh(wintitle);
        }
    }

    if (win_offsets.pfield.isRendered != new_winoff.pfield.isRendered)
    {
        // Not enough room for playfield window, delete window
        if (win_offsets.pfield.isRendered && !new_winoff.pfield.isRendered) {
            delwin(winpfield);
            winpfield = NULL;
        }
        // There is room to create playfield window
        else if (!win_offsets.pfield.isRendered && new_winoff.pfield.isRendered) {
            winpfield = newwin(TETRIS_HEIGHT + 2, TETRIS_WIDTH*2 + 2, new_winoff.pfield.offset.y, new_winoff.pfield.offset.x);
            box(winpfield, 0, 0);
            wrefresh(winpfield);
        }
        // Playfield window needs to be moved
        else {
            mvwin(winpfield, new_winoff.pfield.offset.y, new_winoff.pfield.offset.x);
            wrefresh(winpfield);
        }
    }

    if (win_offsets.pprev.isRendered != new_winoff.pprev.isRendered)
    {
        // Not enough room for piece preview window, delete window
        if (win_offsets.pprev.isRendered && !new_winoff.pprev.isRendered) {
            delwin(winpprev);
            winpprev = NULL;
        }
        // There is room to create piece preview window
        else if (!win_offsets.pprev.isRendered && new_winoff.pprev.isRendered) {
            winpprev = newwin(6, 19, new_winoff.pprev.offset.y, new_winoff.pprev.offset.x);
            box(winpprev, 0, 0);
            wrefresh(winpprev);
        }
        // Piece preview window needs to be moved
        else {
            mvwin(winpprev, new_winoff.pprev.offset.y, new_winoff.pprev.offset.x);
            wrefresh(winpprev);
        }
    }

    if (win_offsets.score.isRendered != new_winoff.score.isRendered)
    {
        // Not enough room for score window, delete window
        if (win_offsets.score.isRendered && !new_winoff.score.isRendered) {
            delwin(winscore);
            winscore = NULL;
        }
        // There is room to create score window
        else if (!win_offsets.score.isRendered && new_winoff.score.isRendered) {
            winscore = newwin(4, 19, new_winoff.score.offset.y, new_winoff.score.offset.x);
            box(winscore, 0, 0);
            wrefresh(winscore);
        }
        // Score window needs to be moved
        else {
            mvwin(winscore, new_winoff.score.offset.y, new_winoff.score.offset.x);
            wrefresh(winscore);
        }
    }

    if (win_offsets.debug.isRendered != new_winoff.debug.isRendered)
    {
        // Not enough room for debug window, delete window
        if (win_offsets.debug.isRendered && !new_winoff.debug.isRendered) {
            delwin(debug_window);
            delwin(windebug);
            debug_window = NULL;
            windebug = NULL;
        }
        // There is room to create debug window
        else if (!win_offsets.debug.isRendered && new_winoff.debug.isRendered) {
            windebug = newwin(12, 19, new_winoff.debug.offset.y, new_winoff.debug.offset.x);
            box(windebug, 0, 0);
            wrefresh(windebug);

            // Create subwindow inside of the box so the box isnt overwritten
            debug_window = newwin(10, 17, getbegy(windebug)+1, getbegx(windebug)+1);
            scrollok(debug_window, true);
            wrefresh(debug_window);

            // touchwin(windebug);
        }
        // debug window needs to be moved
        else {
            mvwin(windebug, new_winoff.debug.offset.y, new_winoff.debug.offset.x);
            mvwin(debug_window, getbegy(windebug)+1, getbegx(windebug)+1);
            wrefresh(windebug);
            wrefresh(debug_window);
        }
    }

    if (win_offsets.ginfo.isRendered != new_winoff.ginfo.isRendered)
    {
        // Not enough room for score window, delete window
        if (win_offsets.ginfo.isRendered && !new_winoff.ginfo.isRendered)
        {
            delwin(winginfo);
            winginfo = NULL;
        }

        // There is room to create score window
        else if (!win_offsets.ginfo.isRendered && new_winoff.ginfo.isRendered) {
            winginfo = newwin(22, 31, new_winoff.ginfo.offset.y, new_winoff.ginfo.offset.x);
            box(winginfo, 0, 0);
            wrefresh(winginfo);
        }

        // Score window needs to be moved
        else {
            mvwin(winginfo, new_winoff.ginfo.offset.y, new_winoff.ginfo.offset.x);
            wrefresh(winginfo);
        }
    }

    win_offsets = new_winoff;

    refresh();

    return 0;
}

int tdraw_touchwin()
{
    if (wintitle != NULL) {
        touchwin(wintitle);
        wrefresh(wintitle);
    }
    if (winpfield != NULL) {
        touchwin(winpfield);
        wrefresh(winpfield);
    }
    if (winpprev != NULL) {
        touchwin(winpprev);
        wrefresh(winpprev);
    }
    if (winscore != NULL) {
        touchwin(winscore);
        wrefresh(winscore);
    }
    if (windebug != NULL) {
        touchwin(windebug);
        wrefresh(windebug);
    }
    if (debug_window != NULL) {
        touchwin(debug_window);
        wrefresh(debug_window);
    }
    if (winginfo != NULL) {
        touchwin(winginfo);
        wrefresh(winginfo);
    }

    return 0;
}

