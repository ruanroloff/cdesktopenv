/*
 * CDE - Common Desktop Environment
 *
 * Copyright (c) 1993-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these libraries and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
 */
/* $XConsortium: argnod.h /main/3 1995/11/01 16:38:43 rswiston $ */
/***************************************************************
*                                                              *
*                      AT&T - PROPRIETARY                      *
*                                                              *
*        THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF        *
*                    AT&T BELL LABORATORIES                    *
*         AND IS NOT TO BE DISCLOSED OR USED EXCEPT IN         *
*            ACCORDANCE WITH APPLICABLE AGREEMENTS             *
*                                                              *
*                Copyright (c) 1995 AT&T Corp.                 *
*              Unpublished & Not for Publication               *
*                     All Rights Reserved                      *
*                                                              *
*       The copyright notice above does not evidence any       *
*      actual or intended publication of such source code      *
*                                                              *
*               This software was created by the               *
*           Advanced Software Technology Department            *
*                    AT&T Bell Laboratories                    *
*                                                              *
*               For further information contact                *
*                    {research,attmail}!dgk                    *
*                                                              *
***************************************************************/

/* : : generated by proto : : */
                  
#ifndef ARG_RAW
#if !defined(__PROTO__)
#if defined(__STDC__) || defined(__cplusplus) || defined(_proto) || defined(c_plusplus)
#if defined(__cplusplus)
#define __MANGLE__	"C"
#else
#define __MANGLE__
#endif
#define __STDARG__
#define __PROTO__(x)	x
#define __OTORP__(x)
#define __PARAM__(n,o)	n
#if !defined(__STDC__) && !defined(__cplusplus)
#if !defined(c_plusplus)
#define const
#endif
#define signed
#define void		int
#define volatile
#define __V_		char
#else
#define __V_		void
#endif
#else
#define __PROTO__(x)	()
#define __OTORP__(x)	x
#define __PARAM__(n,o)	o
#define __MANGLE__
#define __V_		char
#define const
#define signed
#define void		int
#define volatile
#endif
#if defined(__cplusplus) || defined(c_plusplus)
#define __VARARG__	...
#else
#define __VARARG__
#endif
#if defined(__STDARG__)
#define __VA_START__(p,a)	va_start(p,a)
#else
#define __VA_START__(p,a)	va_start(p)
#endif
#endif

/*
 *	struct to hold a word argument
 *	Written by David Korn
 *
 */

#include	<stak.h>

struct ionod
{
	unsigned	iofile;
	char		*ioname;
	struct ionod	*ionxt;
	struct ionod	*iolst;
	char		*iodelim;
	off_t		iooffset;
	long		iosize;
};

struct comnod
{
	int		comtyp;
	struct ionod	*comio;
	struct argnod	*comarg;
	struct argnod	*comset;
	__V_		*comnamp;
	int		comline;
};

#define COMBITS		4
#define COMMSK		((1<<COMBITS)-1)
#define COMSCAN		(01<<COMBITS)

struct slnod 	/* struct for link list of stacks */
{
	struct slnod	*slnext;
	struct slnod	*slchild;
	Stak_t		*slptr;
};

/*
 * This struct is use to hold $* lists and arrays
 */

struct dolnod
{
	short		dolrefcnt;	/* reference count */
	short		dolmax;		/* size of dolval array */
	short		dolnum;		/* number of elements */
	short		dolbot;		/* current first element */
	struct dolnod	*dolnxt;	/* used when list are chained */
	char		*dolval[1];	/* array of value pointers */
};

/*
 * This struct is used to hold word arguments of variable size during
 * parsing and during expansion.  The flags indicate what processing
 * is required on the argument.
 */

struct argnod
{
	union
	{
		struct argnod	*ap;
		char		*cp;
	}		argnxt;
	union
	{
		struct argnod	*ap;
		int		len;
	}		argchn;
	unsigned char	argflag;
	char		argval[4];
};



/* The following should evaluate to the offset of argval in argnod */
extern __MANGLE__ int errno;	/* could be any l-value */
#define ARGVAL	((unsigned)(((struct argnod*)(&errno))->argval-(char*)(&errno)))
#define sh_argstr(ap)	((ap)->argflag&ARG_RAW?sh_fmtq((ap)->argval):(ap)->argval)
#ifdef SHOPT_VPIX
#   define ARG_SPARE 2
#else
#   define ARG_SPARE 1
#endif /* SHOPT_VPIX */


/* legal argument flags */
#define ARG_RAW		0x1	/* string needs no processing */
#define ARG_MAKE	0x2	/* bit set during argument expansion */
#define ARG_MAC		0x4	/* string needs macro expansion */
#define	ARG_EXP		0x8	/* string needs file expansion */
#define ARG_ASSIGN	0x10	/* argument is an assignment */
#define ARG_QUOTED	0x20	/* word contained quote characters */
#define ARG_MESSAGE	0x40	/* contains international string */
#define ARG_APPEND	0x80	/* for += assignment */

extern __MANGLE__ char 		**sh_argbuild __PROTO__((int*,const struct comnod*));
extern __MANGLE__ struct dolnod	*sh_argcreate __PROTO__((char*[]));
extern __MANGLE__ char 		*sh_argdolminus __PROTO__((void));
extern __MANGLE__ struct dolnod	*sh_argfree __PROTO__((struct dolnod*,int));
extern __MANGLE__ struct dolnod	*sh_argnew __PROTO__((char*[],struct dolnod**));
extern __MANGLE__ int		sh_argopts __PROTO__((int,char*[]));
extern __MANGLE__ void 		sh_argreset __PROTO__((struct dolnod*,struct dolnod*));
extern __MANGLE__ void 		sh_argset __PROTO__((char*[]));
extern __MANGLE__ struct dolnod	*sh_arguse __PROTO__((void));

extern __MANGLE__ const char	e_heading[];
extern __MANGLE__ const char	e_off[];
extern __MANGLE__ const char	e_on[];
extern __MANGLE__ const char	e_sptbnl[];
extern __MANGLE__ const char	e_subst[];
extern __MANGLE__ const char	e_option[];
extern __MANGLE__ const char	e_argexp[];
extern __MANGLE__ const char	e_exec[];
extern __MANGLE__ const char	e_devfdNN[];

#endif /* ARG_RAW */
