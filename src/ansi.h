#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define GMAP_FIRST	0xb0
#define GMAP_LAST	0xda

//enum { BLACK, BLUE, GREEN, CYAN, RED, MAGENTA, YELLOW, WHITE };
enum { CS_ASCII, CS_GRAPH, CS_CUSTOM };
static char custom_char[] = {"[]"};
#define NUM_CUSTOM	2

void ansi_init(void);
void ansi_recall(void);
void ansi_reset(void);
void ansi_clearscr(void);
void ansi_setcursor(int row, int col);
void ansi_cursor(int show);
void ansi_setcolor(int fg, int bg);
void ansi_ibmchar(unsigned char c, unsigned char attr);
void ansi_putstr(const char *s, unsigned char attr);

