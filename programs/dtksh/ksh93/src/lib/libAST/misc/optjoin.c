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
/* $XConsortium: optjoin.c /main/3 1995/11/01 17:59:27 rswiston $ */
/***************************************************************
*                                                              *
*                      AT&T - PROPRIETARY                      *
*                                                              *
*         THIS IS PROPRIETARY SOURCE CODE LICENSED BY          *
*                          AT&T CORP.                          *
*                                                              *
*                Copyright (c) 1995 AT&T Corp.                 *
*                     All Rights Reserved                      *
*                                                              *
*           This software is licensed by AT&T Corp.            *
*       under the terms and conditions of the license in       *
*       http://www.research.att.com/orgs/ssr/book/reuse        *
*                                                              *
*               This software was created by the               *
*           Software Engineering Research Department           *
*                    AT&T Bell Laboratories                    *
*                                                              *
*               For further information contact                *
*                     gsf@research.att.com                     *
*                                                              *
***************************************************************/

/* : : generated by proto : : */

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
#include <ast.h>
#include <option.h>

typedef int (*OPTFUN) __PROTO__((char**, int));

int
optjoin __PARAM__((char** argv, ...), (va_alist)) __OTORP__(va_dcl)
{ __OTORP__(char** argv; )
	va_list			ap;
	OPTFUN		fun;
	OPTFUN		rep;
	OPTFUN			err;
	int			more;
	int			user;
	int			last_index;
	int			last_offset;
	int			err_index;
	int			err_offset;
#if !OBSOLETE_IN_96

#undef	opt_again
	extern __MANGLE__ int		opt_again;
	static int		opt_again_old;
#undef	opt_char
	extern __MANGLE__ int		opt_char;
	static int		opt_char_old;
#undef	opt_index
	extern __MANGLE__ int		opt_index;
	static int		opt_index_old;

	if (opt_again_old != opt_again)
		opt_info.again = opt_again_old = opt_again;
	if (opt_char_old != opt_char)
		opt_info.offset = opt_char_old = opt_char;
	if (opt_index_old != opt_index)
		opt_info.index = opt_index_old = opt_index;
#endif

	err = rep = 0;
	for (;;)
	{
		__VA_START__(ap, argv); __OTORP__(argv = va_arg(ap, char** );)
		while (fun = va_arg(ap, OPTFUN))
		{
			last_index = opt_info.index;
			last_offset = opt_info.offset;
#if !OBSOLETE_IN_96
			opt_again = opt_again_old = opt_info.offset;
			opt_char = opt_char_old = opt_info.offset;
			opt_index = opt_index_old = opt_info.index;
#endif
			user = (*fun)(argv, 0);
#if !OBSOLETE_IN_96
			if (opt_again_old != opt_again)
				opt_info.again = opt_again_old = opt_again;
			if (opt_char_old != opt_char)
				opt_info.offset = opt_char_old = opt_char;
			if (opt_index_old != opt_index)
				opt_info.index = opt_index_old = opt_index;
#endif
			more = argv[opt_info.index] != 0;
			if (!opt_info.again)
			{
				if (!more) return(0);
				if (!user)
				{
					if (*argv[opt_info.index] != '+') return(1);
					opt_info.again = -1;
				}
				else err = 0;
			}
			if (opt_info.again)
			{
				if (opt_info.again > 0 && (!err || err_index < opt_info.index || err_index == opt_info.index && err_offset < opt_info.offset))
				{
					err = fun;
					err_index = opt_info.index;
					err_offset = opt_info.offset;
				}
				opt_info.again = 0;
				if (opt_info.pindex)
					opt_info.index = opt_info.pindex;
				opt_info.offset = opt_info.poffset;
			}
			if (!rep || opt_info.index != last_index || opt_info.offset != last_offset)
				rep = fun;
			else if (fun == rep)
			{
				if (!err) return(1);
				(*err)(argv, 1);
				opt_info.offset = 0;
			}
		}
		va_end(ap);
	}
}
