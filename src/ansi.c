#include "ansi.h"

// for building the border of game
unsigned char gmap[] = {
	0x61, 0x61, 0x61,		/* checker */
	0x78,					/* vline */
	0x75, 0x75, 0x75,		/* right T */
	0x6b, 0x6b,				/* upper-right corner */
	0x75,					/* right T */
	0x78,					/* vline */
	0x6b,					/* upper-right corner */
	0x6a, 0x6a, 0x6a,		/* lower-right corner */
	0x6b,					/* upper-right corner */
	0x6d,					/* lower-left corner */
	0x76,					/* bottom T */
	0x77,					/* top T */
	0x74,					/* left T */
	0x71,					/* hline */
	0x6e,					/* cross */
	0x74, 0x74,				/* left T */
	0x6d,					/* lower-left corner */
	0x6c,					/* upper-left corner */
	0x76,					/* bottom T */
	0x77,					/* top T */
	0x74,					/* left T */
	0x71,					/* hline */
	0x6e,					/* cross */
	0x76, 0x76,				/* bottom T */
	0x77, 0x77,				/* top T */
	0x6d, 0x6d,				/* lower-left corner */
	0x6c, 0x6c,				/* upper-left corner */
	0x6e, 0x6e,				/* cross */
	0x6a,					/* lower-right corner */
	0x6c					/* upper-left corner */
};

static unsigned char cmap[] = {0, 4, 2, 6, 1, 5, 3, 7};
static unsigned char cur_attr = 0xff;
static int cur_cs = CS_ASCII;

char *termenv;

void ansi_init(void)
{
	int i, val, vtclass = -1;
	char buf[64];

    termenv = getenv("TERM");

	/* detect the terminal type
	 * if there is a TERM env var with "vtxxx" where xxx >= 200, enable
	 * graphical blocks and set vtclass accordingly.
	 */
	if(termenv && termenv[0] == 'v' && termenv[1] == 't') {
		// if((val = atoi(termenv + 2)) >= 200 && val < 600 && !no_autogfx) {
		// 	use_gfxchar = 1;
		// }
		vtclass = 60 + val / 100;
	}

	/* unknown or unset TERM, try asking for the device attributes string */
	if(vtclass == -1) {
		char *ptr;
		int have_softchar = 0;

		printf("\033[c\n");
		fflush(stdout);
		if(fgets(buf, sizeof buf, stdin) && (memcmp(buf, "\033[?", 3) == 0 ||
				memcmp(buf, "\233?", 2) == 0)) {
			ptr = buf + (buf[0] == '\033' ? 3 : 2);

			fprintf(stderr, "term id: %s\n", ptr);

			for(;;) {
				switch((val = atoi(ptr))) {
				case 7:
					have_softchar = 1;
					break;

				default:
					if(val >= 62 && val < 70) {
						/* assume it's a VT class */
						vtclass = val;
					}
				}
				while(*ptr && *ptr != 'c' && *ptr != ';') ptr++;
				if(*ptr != ';') break;
				ptr++;
			}

			if(vtclass != -1) {
				/* found a vt class, treat the rest as valid */
				fprintf(stderr, "detected VT class %d [DRCS:%d]\n",
						vtclass, have_softchar);

				// if(!no_autogfx && have_softchar) {
				// 	use_gfxchar = 1;
				// }
			}
		}
	}
}

void ansi_recall(void)
{
	fputs("\033c", stdout);
	fflush(stdout);
}

void ansi_reset(void)
{
	fputs("\033[0m", stdout);	/* select graphics rendition (SGR) normal */
	fputs("\033[!p", stdout);	/* soft terminal reset (DECSTR) */
	/* DECSTR disables auto-wrap which is annoying ... */
	fputs("\033[?7h", stdout);	/* set-mode auto-wrap (DECAWN) */
	fflush(stdout);
}

void ansi_clearscr(void)
{
	fputs("\033[H\033[2J", stdout);
}

void ansi_setcursor(int row, int col)
{
	if((row | col) == 0) {
		fputs("\033[H", stdout);
	} else {
		printf("\033[%d;%dH", row + 1, col + 1);
	}
}

void ansi_cursor(int show)
{
	printf("\033[?25%c", show ? 'h' : 'l');
	fflush(stdout);
}

void ansi_setcolor(int fg, int bg)
{
	// if(monochrome) return;

	cur_attr = (fg << 4) | bg;
	fg = cmap[fg];
	bg = cmap[bg];

	printf("\033[;%d;%dm", fg + 30, bg + 40);
}

void ansi_ibmchar(unsigned char c, unsigned char attr)
{
	char cmd[32];
	char *ptr = cmd;

	if(c >= GMAP_FIRST && c <= GMAP_LAST) {
		if(cur_cs != CS_GRAPH) {
			memcpy(ptr, "\033(0", 3);
			ptr += 3;
			cur_cs = CS_GRAPH;
		}

		c = gmap[c - GMAP_FIRST];
	// } else if(use_gfxchar && (c == '[' || c == ']')) {
	// 	if(cur_cs != CS_CUSTOM) {
	// 		memcpy(ptr, "\033( @", 4);
	// 		ptr += 4;
	// 		cur_cs = CS_CUSTOM;
	// 	}
	} else {
		if(cur_cs != CS_ASCII) {
			memcpy(ptr, "\033(B", 3);
			ptr += 3;
			cur_cs = CS_ASCII;
		}
	}

	// if(monochrome) {
	// 	attr &= 0x80;
	// }

	if(attr != cur_attr) {
		int bold = attr & 0x80 ? 1 : 0;

        unsigned char bg = cmap[attr & 7];
        unsigned char fg = cmap[(attr >> 4) & 7];

        ptr += sprintf(ptr, "\033[%d;%d;%dm", bold, fg + 30, bg + 40);
        cur_attr = attr;

		// if(monochrome) {
		// 	ptr += sprintf(ptr, "\033[%dm", bold);
		// } else {
		// 	unsigned char bg = cmap[attr & 7];
		// 	unsigned char fg = cmap[(attr >> 4) & 7];

		// 	ptr += sprintf(ptr, "\033[%d;%d;%dm", bold, fg + 30, bg + 40);
		// }
		// cur_attr = attr;
	}

	*ptr++ = c;
	*ptr = 0;

	fputs(cmd, stdout);
}

void ansi_putstr(const char *s, unsigned char attr)
{
	while(*s) {
		ansi_ibmchar(*s++, attr);
	}
}
