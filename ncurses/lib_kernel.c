
/***************************************************************************
*                            COPYRIGHT NOTICE                              *
****************************************************************************
*                ncurses is copyright (C) 1992-1995                        *
*                          Zeyd M. Ben-Halim                               *
*                          zmbenhal@netcom.com                             *
*                          Eric S. Raymond                                 *
*                          esr@snark.thyrsus.com                           *
*                                                                          *
*        Permission is hereby granted to reproduce and distribute ncurses  *
*        by any means and for any fee, whether alone or as part of a       *
*        larger distribution, in source or in binary form, PROVIDED        *
*        this notice is included with any such distribution, and is not    *
*        removed from any of its header files. Mention of ncurses in any   *
*        applications linked with it is highly appreciated.                *
*                                                                          *
*        ncurses comes AS IS with no warranty, implied or expressed.       *
*                                                                          *
***************************************************************************/


/*
 *	lib_kernel.c
 *
 *	Misc. low-level routines:
 *		napms()
 *		reset_prog_mode()
 *		reset_shell_mode()
 *		erasechar()
 *		killchar()
 *		flushinp()
 *		savetty()
 *		resetty()
 *
 * The baudrate() and delay_output() functions could logically live here,
 * but are in other modules to reduce the static-link size of programs
 * that use only these facilities.
 */

#include <curses.priv.h>
#include <term.h>	/* cur_term */

MODULE_ID("$Id: lib_kernel.c,v 1.15 1998/01/10 20:22:58 tom Exp $")

int napms(int ms)
{
	T((T_CALLED("napms(%d)"), ms));

	usleep(1000*(unsigned)ms);
	returnCode(OK);
}

int reset_prog_mode(void)
{
	T((T_CALLED("reset_prog_mode()")));

	if (cur_term != 0) {
		_nc_set_curterm(&cur_term->Nttyb);
		if (SP && stdscr && stdscr->_use_keypad)
			_nc_keypad(TRUE);
		returnCode(OK);
	}
	returnCode(ERR);
}


int reset_shell_mode(void)
{
	T((T_CALLED("reset_shell_mode()")));

	if (cur_term != 0) {
		if (SP)
		{
			fflush(SP->_ofp);
			_nc_keypad(FALSE);
		}
		returnCode(_nc_set_curterm(&cur_term->Ottyb));
	}
	returnCode(ERR);
}

/*
 *	erasechar()
 *
 *	Return erase character as given in cur_term->Ottyb.
 *
 */

char
erasechar(void)
{
	T((T_CALLED("erasechar()")));

	if (cur_term != 0) {
#ifdef TERMIOS
		returnCode(cur_term->Ottyb.c_cc[VERASE]);
#else
		returnCode(cur_term->Ottyb.sg_erase);
#endif
	}
	returnCode(ERR);
}



/*
 *	killchar()
 *
 *	Return kill character as given in cur_term->Ottyb.
 *
 */

char
killchar(void)
{
	T((T_CALLED("killchar()")));

	if (cur_term != 0) {
#ifdef TERMIOS
		returnCode(cur_term->Ottyb.c_cc[VKILL]);
#else
		returnCode(cur_term->Ottyb.sg_kill);
#endif
	}
	returnCode(ERR);
}



/*
 *	flushinp()
 *
 *	Flush any input on cur_term->Filedes
 *
 */

int flushinp(void)
{
	T((T_CALLED("flushinp()")));

	if (cur_term != 0) {
#ifdef TERMIOS
		tcflush(cur_term->Filedes, TCIFLUSH);
#else
		errno = 0;
		do {
		    ioctl(cur_term->Filedes, TIOCFLUSH, 0);
		} while
		    (errno == EINTR);
#endif
		if (SP) {
			SP->_fifohead = -1;
			SP->_fifotail = 0;
			SP->_fifopeek = 0;
		}
		returnCode(OK);
	}
	returnCode(ERR);
}

/*
**	savetty()  and  resetty()
**
*/

static TTY   buf;

int savetty(void)
{
	T((T_CALLED("savetty()")));

	returnCode(_nc_get_curterm(&buf));
}

int resetty(void)
{
	T((T_CALLED("resetty()")));

	returnCode(_nc_set_curterm(&buf));
}
