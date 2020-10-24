/*
 * neyes: an xeyes like program for your terminal.
 * (c) 2020 Trammell Hudson <hudson@trmm.net>
 *
 * Derived from https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/windows.html#WINDOWBASICS
 */
#include <curses.h>
#include <stdio.h>
 
WINDOW *
create_newwin(
	const int height,
	const int width,
	const int starty,
	const int startx
)
{
	WINDOW * const local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0); // default boarders
	wrefresh(local_win);

	return local_win;
}

void
destroy_win(
	WINDOW * local_win
)
{	
	// erase the existing borders before deletion
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
	wrefresh(local_win);
	delwin(local_win);
}

void
update_track(
	WINDOW ** track,
	const int h,
	const int w,
	const int cy,
	const int cx,
	const int ey,
	const int ex
)
{
	if (*track)
		destroy_win(*track);

	int dx = ex - cx;
	int dy = ey - cy;

	if (dx > w/2-4)
		dx = w/2-4;
	if (dx < -(w/2-3))
		dx = -(w/2-3);

	if (dy > h/2-2)
		dy = h/2-2;
	if (dy < -(h/2-3))
		dy = -(h/2-3);

	*track = create_newwin(3, 5, cy - 1 + dy, cx - 2 + dx);
}

int main(void)
{
	initscr();
	cbreak();
	noecho();

	// Enables keypad mode, which also allows mouse events as KEY_MOUSE
	keypad(stdscr, TRUE);

	// Report all mouse events, not only clicks
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);

	// see http://www.xfree86.org/current/ctlseqs.html#Mouse
	printf("\033[?1003h\n");
	refresh();

	// create two windows, one for each "eye"
	const int h = LINES;
	const int w = COLS;
	const int eye_h = h - 3;
	const int eye_w = w/2 - 4;
	WINDOW * eye1 = create_newwin(eye_h, eye_w, 2, 2);
	WINDOW * eye2 = create_newwin(eye_h, eye_w, 2, w/2 + 2);

	const int eye1_cx = 2 + eye_w/2;
	const int eye2_cx = w/2 + 2 + eye_w/2;
	const int eye1_cy = h/2;
	const int eye2_cy = h/2;
	WINDOW * track1 = NULL; //create_newwin(5, 5, eye1_cx - 2, eye1_cy - 2);
	WINDOW * track2 = NULL; //create_newwin(5, 5, eye2_cx - 2, eye2_cy - 2);

	while(1)
	{
		const int c = wgetch(stdscr);
 
		// Exit on space or newline
		if (c == ' ' || c == '\n')
			break;
 
		if (c == ERR)
			break;

		if (c != KEY_MOUSE)
			continue;

		MEVENT event;
		if (getmouse(&event) != OK)
			break;

		update_track(&track1, eye_h, eye_w, eye1_cy, eye1_cx, event.y, event.x);
		update_track(&track2, eye_h, eye_w, eye2_cy, eye2_cx, event.y, event.x);

		move(h-1, w/2 - 7);
		//      1234567890123
		addstr("-=+ neyes +=-");
	}
 
	// Disable mouse movement events, as l = low
	printf("\033[?1003l\n");

	delwin(eye1);
	delwin(eye2);
	endwin();
 
	return 0;
}
