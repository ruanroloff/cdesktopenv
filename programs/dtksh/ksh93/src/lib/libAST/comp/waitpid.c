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
/* $XConsortium: waitpid.c /main/3 1995/11/01 17:21:28 rswiston $ */
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
#include <wait.h>

#if _lib_waitpid

NoN(waitpid)

#else

#if _lib_wait4

struct rusage;

extern __MANGLE__ int	wait4 __PROTO__((int, int*, int, struct rusage*));

pid_t
waitpid __PARAM__((pid_t pid, int* status, int flags), (pid, status, flags)) __OTORP__(pid_t pid; int* status; int flags;){
	return(wait4(pid, status, flags, NiL));
}

#else

#undef	SIGCLD

#if _lib_wait3

extern __MANGLE__ int		wait3 __PROTO__((int*, int, struct rusage*));

#else

#if _lib_wait2

#define wait3(s,f,u)	wait2(s,f)

extern __MANGLE__ int		wait2 __PROTO__((int*, int));

#else

#include <sig.h>

#define wait3(s,f,u)	wait(s)

static int	caught;

static void
catch(sig)
int	sig;
{
	NoP(sig);
	caught = 1;
}

#endif

#endif

#include <error.h>

struct zombie
{
	struct zombie*	next;
	int		status;
	pid_t		pid;
};

pid_t
waitpid __PARAM__((pid_t pid, int* status, int flags), (pid, status, flags)) __OTORP__(pid_t pid; int* status; int flags;){
	struct zombie*	zp;
	struct zombie*	pp;
	int		p;
	int			s;
#if !_lib_wait2 && !_lib_wait3
#if !defined(SIGCLD)
	int			n;
	int			oerrno;
#endif
	Handler_t		handler;
#endif

	static struct zombie*	zombies;

	pp = 0;
	zp = zombies;
	while (zp)
	{
		if (zp->pid >= 0 && (zp->pid == pid || pid <= 0))
		{
			if (pp) pp->next = zp->next;
			else zombies = zp->next;
			if (status) *status = zp->status;
			pid = zp->pid;
			free(zp);
			return(pid);
		}
	}
	if (pid > 0 && kill(pid, 0) < 0) return(-1);
	for (;;)
	{
#if !_lib_wait2 && !_lib_wait3
#if !defined(SIGCLD)
		oerrno = errno;
#endif
		if (flags & WNOHANG)
		{
			caught = 0;
#if defined(SIGCLD)
			handler = signal(SIGCLD, catch);
			if (!caught)
			{
				signal(SIGCLD, handler);
				return(0);
			}
#else
#if defined(SIGALRM)
			handler = signal(SIGALRM, catch);
			n = alarm(1);
#endif
#endif
		}
#endif
		p = wait3(&s, flags, NiL);
#if !_lib_wait3
#if !_lib_wait2
#if defined(SIGCLD)
		if (flags & WNOHANG) signal(SIGCLD, handler);
#else
#if defined(SIGALRM)
		if (flags & WNOHANG)
		{
			if (n == 0 && !caught || n == 1) alarm(n);
			else if (n > 1) alarm(n - caught);
			signal(SIGALRM, handler);
		}
		if (p == -1 && errno == EINTR)
		{
			errno = oerrno;
			p = 0;
			s = 0;
		}
#endif
#endif
#else
		if (p == -1 && errno == EINVAL && (flags & ~WNOHANG))
			p = wait3(&s, flags & WNOHANG, NiL);
#endif
#endif
		if (p <= 0)
		{
			if (p == 0 && status) *status = s;
			return(p);
		}
		if (pid <= 0 || p == pid)
		{
			if (status) *status = s;
			return(p);
		}
		if (!(zp = newof(0, struct zombie, 1, 0))) return(-1);
		zp->pid = p;
		zp->status = s;
		zp->next = zombies;
		zombies = zp;
	}
	/*NOTREACHED*/
}

#endif

#endif
