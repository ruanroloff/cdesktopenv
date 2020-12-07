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
/* $XConsortium: ftwalk.c /main/3 1995/11/01 17:55:51 rswiston $ */
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
#include	"dirlib.h"

#include	<ftwalk.h>

#if MAXNAMLEN > 16
#define MINNAME		24
#else
#define MINNAME		16
#endif

/* function to determine type of an object */
#ifdef S_ISLNK
#define TYPE(m)		(S_ISDIR(m) ? FTW_D : S_ISLNK(m) ? FTW_SL : FTW_F)
#else
#define TYPE(m)		(S_ISDIR(m) ? FTW_D : FTW_F)
#endif

/* to set state.status and state.base on calls to user function */
#define STATUS(cdrv)	(cdrv == 0 ? FTW_NAME : FTW_PATH)

/* see if same object */
#define SAME(one,two)	((one).st_ino == (two).st_ino && (one).st_dev == (two).st_dev)

/* path name */
#define PATH(p,l)	((l) > 0 && (p)[0] == '.' && (p)[1] == '/' ? (p)+2 : (p))

/*
	Make/free an object of type FTW.
*/
static Ftw_t	*Free;
#define freeFtw(f)	((f)->link = Free, Free = (f))

static Ftw_t *newFtw __PARAM__((char* name, int namelen), (name, namelen)) __OTORP__(char* name; int namelen;){
	Ftw_t	*f;
	int	amount;

	if(Free && namelen < MINNAME)
		f = Free, Free = f->link;
	else
	{
		amount = namelen < MINNAME ? MINNAME : namelen+1;
		if(!(f = newof(0, Ftw_t, 1, amount-sizeof(int))))
			return 0;
	}
	f->link = 0;
	f->local.number = 0;
	f->local.pointer = 0;
	f->status = FTW_NAME;
	f->namelen = namelen;
	memcpy(f->name,name,namelen+1);

	return f;
}

static int freeAll __PARAM__((Ftw_t* f, int rv), (f, rv)) __OTORP__(Ftw_t* f; int rv;){
	Ftw_t	*next;
	int	freeing;

	for(freeing = 0; freeing < 2; ++freeing)
	{
		if(freeing == 1)
			f = Free, Free = 0;
		while(f)
		{
			next = f->link;
			free(f);
			f = next;
		}
	}
	return rv;
}

/*
	To compare directories by device/inode.
*/
static int statcmp __PARAM__((Ftw_t* f1, Ftw_t* f2), (f1, f2)) __OTORP__(Ftw_t* f1; Ftw_t* f2;){
	int	d;
	if((d = f1->statb.st_ino - f2->statb.st_ino) != 0)
		return d;
	if((d = f1->statb.st_dev - f2->statb.st_dev) != 0)
		return d;
	/* hack for NFS system where (dev,ino) do not uniquely identify objects */
	return (f1->statb.st_mtime - f2->statb.st_mtime);
}

/*
	Search trees with top-down splaying (a la Tarjan and Sleator).
	When used for insertion sort, this implements a stable sort.
*/
#define RROTATE(r)	(t = r->left, r->left = t->right, t->right = r, r = t)
#define LROTATE(r)	(t = r->right, r->right = t->left, t->left = r, r = t)

static Ftw_t *search __PARAM__((Ftw_t* e, Ftw_t* root, int(*comparf)(Ftw_t*, Ftw_t*), int insert), (e, root, comparf, insert)) __OTORP__(Ftw_t* e; Ftw_t* root; int(*comparf)(); int insert;){
	int		cmp;
	Ftw_t		*t, *left, *right, *lroot, *rroot;

	left = right = lroot = rroot = 0;
	while(root)
	{
		if((cmp = (*comparf)(e,root)) == 0 && !insert)
			break;
		if(cmp < 0)
		{	/* this is the left zig-zig case */
			if(root->left && (cmp = (*comparf)(e,root->left)) <= 0)
			{
				RROTATE(root);
				if(cmp == 0 && !insert)
					break;
			}

			/* stick all things > e to the right tree */
			if(right)
				right->left = root;
			else	rroot = root;
			right = root;
			root = root->left;
			right->left = 0;
		}
		else
		{	/* this is the right zig-zig case */
			if(root->right && (cmp = (*comparf)(e,root->right)) >= 0)
			{
				LROTATE(root);
				if(cmp == 0 && !insert)
					break;
			}

			/* stick all things <= e to the left tree */
			if(left)
				left->right = root;
			else	lroot = root;
			left = root;
			root = root->right;
			left->right = 0;
		}
	}

	if(!root)
		root = e;
	else
	{
		if(right)
			right->left = root->right;
		else	rroot = root->right;
		if(left)
			left->right = root->left;
		else	lroot = root->left;
	}

	root->left = lroot;
	root->right = rroot;
	return root;
}

/*
**	Delete the root element from the tree
*/
static Ftw_t *deleteroot __PARAM__((Ftw_t* root), (root)) __OTORP__(Ftw_t* root;){
	Ftw_t *t, *left, *right;

	left = root->left;
	right = root->right;
	if(!left)
		root = right;
	else
	{
		while(left->right)
			LROTATE(left);
		left->right = right;
		root = left;
	}
	return root;
}

/*
	Convert a binary search tree into a sorted todo (link) list
*/
static void getlist __PARAM__((Ftw_t** top, Ftw_t** bot, Ftw_t* root), (top, bot, root)) __OTORP__(Ftw_t** top; Ftw_t** bot; Ftw_t* root;){
	if(root->left)
		getlist(top,bot,root->left);
	if (*top) (*bot)->link = root, *bot = root;
	else *top = *bot = root;
	if(root->right)
		getlist(top,bot,root->right);
}

/*
	Set directory when curdir is lost in space
*/
static int setdir __PARAM__((char* home, char* path), (home, path)) __OTORP__(char* home; char* path;){
	int	cdrv;

	if(path[0] == '/')
		cdrv = pathcd(path,NiL);
	else
	{	/* Note that path and home are in the same buffer */
		path[-1] = '/';
		cdrv = pathcd(home,NiL);
		path[-1] = '\0';
	}
	if(cdrv < 0)
		(void) pathcd(home,NiL);
	return cdrv;
}

/*
	Set to parent dir
*/
static int setpdir __PARAM__((char* home, char* path, char* base), (home, path, base)) __OTORP__(char* home; char* path; char* base;){
	int	cdrv, c;

	if(base > path)
	{
		c = base[0];
		base[0] = '\0';
		cdrv = setdir(home,path);
		base[0] = c;
	}
	else	cdrv = pathcd(home,NiL);
	return cdrv;
}

/*
	Pop a set of directories
*/
static int popdirs __PARAM__((int n_dir, Ftw_t* ftw), (n_dir, ftw)) __OTORP__(int n_dir; Ftw_t* ftw;){
	struct stat	sb;
	char	*s, *endbuf;
	char		buf[PATH_MAX];

	if(!ftw || ftw->level < 0)
		return -1;

	endbuf = buf + (PATH_MAX-4);
	while(n_dir > 0)
	{
		for(s = buf; s < endbuf && n_dir > 0; --n_dir)
			*s++ = '.', *s++ = '.', *s++ = '/';
		*s = '\0';
		if(chdir(buf) < 0)
			return -1;
	}
	if(stat(".",&sb) != 0 || !SAME(sb,ftw->statb))
		return -1;
	return 0;
}

/*
	Get top list of elt to process
*/
static Ftw_t *toplist __PARAM__((char** paths, int(*statf)(const char*, struct stat*),int(*comparf)(Ftw_t*, Ftw_t*), int metaphysical), (paths, statf, comparf, metaphysical)) __OTORP__(char** paths; int(*statf)();int(*comparf)(); int metaphysical;){
	char		*path;
	Ftw_t		*f, *root;
	Ftw_t			*top, *bot;
	struct stat	*sb;
	struct stat		st;

	top = bot = root = 0;
	for(; *paths; ++paths)
	{
		path = *paths;
		if(!path[0])
			path = ".";

		/* make elements */
		if(!(f = newFtw(path,strlen(path))))
			break;
		f->level = 0;
		sb = &(f->statb);
		f->info = (*statf)(path,sb) < 0 ? FTW_NS : TYPE(sb->st_mode);
#ifdef S_ISLNK
		/*
		 * don't let any standards committee member
		 * get away with calling your idea a hack
		 */

		if (metaphysical && f->info == FTW_SL && stat(path,&st) == 0)
		{
			*sb = st;
			f->info = TYPE(sb->st_mode);
		}
#endif

		if(comparf)
			root = search(f,root,comparf,1);
		else if(bot)
			bot->link = f, bot = f;
		else	top = bot = f;
	}
	if(comparf)
		getlist(&top,&bot,root);
	return top;
}

/*
	Resize path buffer.
	Note that free() is not used because we may need to chdir(home)
	if there isn't enough space to continue
*/
static int resize __PARAM__((char** home, char** endbuf, char** path, char** base, int n_buf, int incre), (home, endbuf, path, base, n_buf, incre)) __OTORP__(char** home; char** endbuf; char** path; char** base; int n_buf; int incre;){
	char	*old, *newp;
	int		n_old;

	/* add space for "/." used in testing FTW_DNX */
	n_old = n_buf;
	n_buf = ((n_buf+incre+4)/PATH_MAX + 1)*PATH_MAX;
	if(!(newp = newof(0, char, n_buf, 0)))
		return -1;

	old = *home;
	*home = newp;
	memcpy(newp,old,n_old);
	if(endbuf)
		*endbuf = newp + n_buf - 4;
	if(path)
		*path = newp + (*path - old);
	if(base)
		*base = newp + (*base - old);

	free(old);
	return n_buf;
}

/*
	The real thing.
*/
int
ftwalk __PARAM__((const char *cpath, int (*userf)(Ftw_t*), int flags, int (*comparf)(Ftw_t*, Ftw_t*)), (cpath, userf, flags, comparf)) __OTORP__(const char *cpath; int (*userf)(); int flags; int (*comparf)();){
	char		*path = (char*)cpath;
	int	cdrv;		/* chdir value */
	int		fnrv;		/* return value from user function */
	Ftw_t		topf,		/* the parent of top elt */
			*todo, *top, *bot;
	DIR		*dirfp;
	int		(*statf) __PROTO__((const char*, struct stat*));
	int		preorder, children, postorder;
	char		*endbuf; /* space to build paths */
	static char	*Home;
	static int	N_buf;

	/* decode the flags */
	children = (flags&FTW_CHILDREN) ? 1 : 0;
	children = (children && (flags&FTW_DELAY)) ? 2 : children;
	preorder = ((flags&FTW_POST) == 0 && !children);
	postorder = (flags&(FTW_POST|FTW_TWICE));
	cdrv = (flags&FTW_DOT) ? 1 : -1;
#ifdef S_ISLNK
	statf = (flags&FTW_PHYSICAL) ? lstat : pathstat;
#else
	statf = stat;
#endif
	/* space for home directory and paths */
	if(!Home)
		N_buf = 2*PATH_MAX;
	while(1)
	{
		if(!Home && !(Home = newof(0, char, N_buf, 0)))
			return -1;
		Home[0] = 0;
		if(cdrv > 0 || getcwd(Home,N_buf))
			break;
		else if(errno == ERANGE)
		{	/* need larger buffer */
			free(Home);
			N_buf += PATH_MAX;
			Home = 0;
		}
		else	cdrv = 1;
	}
	endbuf = Home + N_buf - 4;

	fnrv = -1;

	/* make the list of top elements */
	todo = top = bot = 0;
	if((flags&FTW_MULTIPLE) && path)
		todo = toplist((char**)path,statf,comparf,(flags&(FTW_META|FTW_PHYSICAL))==(FTW_META|FTW_PHYSICAL));
	else
	{
		char	*p[2];
		p[0] = path ? path : ".";
		p[1] = 0;
		todo = toplist(p,statf,comparf,(flags&(FTW_META|FTW_PHYSICAL))==(FTW_META|FTW_PHYSICAL));
	}

	path = Home + strlen(Home) + 1;
	dirfp = 0;
	while(todo)
	{
		int		i, nd;
		Ftw_t		*ftw, *f;
		struct stat	*sb;
		char		*name, *endbase;
		Ftw_t			*link, *root, *curdir, *diroot, *dotdot;
		struct dirent		*dir;
		char			*base;
		int		level, n_base, nostat, cpname;

		/* process the top object on the stack */
		ftw = todo;
		link = ftw->link;
		sb = &(ftw->statb);
		name = ftw->name;
		level = ftw->level;
		fnrv = -1;
		top = bot = root = 0;

		/* initialize for level 0 */
		if(level == 0)
		{
			/* initialize parent */
			memcpy(&topf,ftw,sizeof(topf));
			topf.level = -1;
			topf.name[0] = '\0';
			topf.path = 0;
			topf.pathlen = topf.namelen = 0;
			topf.parent = 0;
			ftw->parent = &topf;
			
			diroot = 0;
			if(cdrv == 0)
				(void) pathcd(Home,NiL);
			else if(cdrv < 0)
				cdrv = 0;
			curdir = cdrv ? 0 : ftw->parent;
			base = path;
			*base = '\0';
		}

		/* chdir to parent dir if asked for */
		if(cdrv < 0)
		{
			cdrv = setdir(Home,path);
			curdir = cdrv ? 0 : ftw->parent;
		}

		/* add object's name to the path */
		if((n_base = ftw->namelen) >= endbuf-base &&
	   	   (N_buf = resize(&Home,&endbuf,&path,&base,N_buf,n_base)) < 0)
			goto done;
		memcpy(base,name,n_base+1);
		name = cdrv ? path : base;

		/* check for cycle and open dir */
		if(ftw->info == FTW_D)
		{
			if((diroot = search(ftw,diroot,statcmp,0)) != ftw || level > 0 && statcmp(ftw,ftw->parent) == 0)
			{
				ftw->info = FTW_DC;
				ftw->link = diroot;
			}
			else
			{	/* buffer is known to be large enough here! */
				if(base[n_base-1] != '/')
					memcpy(base+n_base,"/.",3);
				if(!(dirfp = opendir(name)))
					ftw->info = FTW_DNX;
				base[n_base] = '\0';
				if(!dirfp && !(dirfp = opendir(name)))
					ftw->info = FTW_DNR;
			}
		}

		/* call user function in preorder */
		nd = ftw->info & ~FTW_DNX;
		if(nd || preorder)
		{
			ftw->status = STATUS(cdrv);
			ftw->link = 0;
			ftw->path = PATH(path,level);
			ftw->pathlen = (base - ftw->path) + n_base;
			fnrv = (*userf)(ftw);
			ftw->link = link;
			if(fnrv)
				goto done;

			/* follow symlink if asked to */
			if(ftw->info == FTW_SL && ftw->status == FTW_FOLLOW)
			{
				ftw->info = stat(name,sb) ? FTW_NS : TYPE(sb->st_mode);
				if(ftw->info != FTW_SL)
					continue;
			}
			/* about to prune this ftw and already at home */
			if(cdrv == 0 && level == 0 && nd)
				cdrv = -1;
		}

		/* pruning the search tree */
		if(!dirfp || nd || ftw->status == FTW_SKIP)
		{
			if(dirfp)
				closedir(dirfp), dirfp = 0;
			goto popstack;
		}

		/* FTW_D or FTW_DNX, about to read children */
		if(cdrv == 0)
		{
			if((cdrv = chdir(name)) < 0)
				(void) pathcd(Home,NiL);
			curdir = cdrv < 0 ? 0 : ftw;
		}
		nostat = (children > 1 || ftw->info == FTW_DNX);
		cpname = ((cdrv && !nostat) || (!children && !comparf));
		dotdot = 0;
		endbase = base+n_base;
		if(endbase[-1] != '/')
			*endbase++ = '/';

		while(dir = readdir(dirfp))
		{
			name = dir->d_name;
			nd = 0;
			if(name[0] == '.')
			{
				if(name[1] == '\0')
					nd = 1;
				else if(name[1] == '.' && name[2] == '\0')
					nd = 2;
			}
			if(!children && nd > 0)
				continue;

			/* make a new entry */
			fnrv = -1;
#if _mem_d_namlen_dirent
			i = dir->d_namlen;
#else
			i = strlen(dir->d_name);
#endif
			if(!(f = newFtw(name,i)))
				goto done;
			f->parent = ftw;
			f->level = level+1;
			sb = &(f->statb);

			/* check for space */
			if(i >= endbuf-endbase)
			{
	   	   		N_buf = resize(&Home,&endbuf,&path,&base,N_buf,i);
				if(N_buf < 0)
					goto done;
				endbase = base+n_base;
				if(endbase[-1] != '/')
					++endbase;
			}
			if(cpname)
			{
				memcpy(endbase,name,i+1);
				if(cdrv)
					name = path;
			}

			if(nd == 1)
			{
				f->info = FTW_D;
				memcpy(sb,&(ftw->statb),sizeof(struct stat));
			}
			else if(nostat || (*statf)(name,sb))
			{
				f->info = FTW_NS;
#if _mem_d_fileno_dirent || _mem_d_ino_dirent
#if !_mem_d_fileno_dirent
#define d_fileno	d_ino
#endif
				sb->st_ino = dir->d_fileno;
#else
				sb->st_ino = 0;
#endif
			}
			else	f->info = TYPE(sb->st_mode);

			if(nd)
			{	/* don't recurse on . and .. */
				f->status = FTW_SKIP;
				if(nd == 2 && f->info != FTW_NS)
					dotdot = f;
			}

			if(comparf) /* object ordering */
				root = search(f,root,comparf,1);
			else if(children || f->info == FTW_D || f->info == FTW_SL)
				top ? (bot->link = f, bot = f) : (top = bot = f);
			else
			{	/* terminal node */
				f->status = STATUS(cdrv);
				f->path = PATH(path,1);
				f->pathlen = endbase - f->path + f->namelen;
				fnrv = (*userf)(f);
				freeFtw(f);
				if(fnrv)
					goto done;
			}
		}

		/* done with directory reading */
		closedir(dirfp), dirfp = 0;

		if(root)
			getlist(&top,&bot,root);

		/* delay preorder with the children list */
		if(children)
		{	/* try moving back to parent dir */
			base[n_base] = '\0';
			if(cdrv <= 0)
			{
				f = ftw->parent;
				if(cdrv < 0 || curdir != ftw || !dotdot ||
			   	   !SAME(f->statb,dotdot->statb) ||
				   (cdrv = chdir("..")) < 0)
					cdrv = setpdir(Home,path,base);
				curdir = cdrv ? 0 : f;
			}

			ftw->link = top;
			ftw->path = PATH(path,level);
			ftw->pathlen = (base - ftw->path) + ftw->namelen;
			ftw->status = STATUS(cdrv);
			fnrv = (*userf)(ftw);
			ftw->link = link;
			if(fnrv)
				goto done;

			/* chdir down again */
			nd = (ftw->status == FTW_SKIP);
			if(!nd && cdrv == 0)
			{
				if((cdrv = chdir(base)) < 0)
					(void) pathcd(Home,NiL);
				curdir = cdrv ? 0 : ftw;
			}

			/* prune */
			if(base[n_base-1] != '/')
				base[n_base] = '/';
			for(bot = 0, f = top; f; )
			{
				if(nd || f->status == FTW_SKIP)
				{
					if(bot)
						bot->link = f->link;
					else	top = f->link;
					freeFtw(f);
					f = bot ? bot->link : top;
					continue;
				}

				if(children > 1 && ftw->info != FTW_DNX)
				{	/* now read stat buffer */
					sb = &(f->statb);
					if(f->status == FTW_STAT)
						f->info = TYPE(sb->st_mode);
					else
					{
						name = f->name;
						if(cdrv)
						{
							memcpy(endbase,
								name,f->namelen+1);
							name = path;
						}
						if((*statf)(name,sb) == 0)
							f->info = TYPE(sb->st_mode);
					}
				}

				/* normal continue */
				bot = f, f = f->link;
			}
		}

		base[n_base] = '\0';
		if(top)
			bot->link = todo, todo = top, top = 0;

		/* pop objects completely processed */
	popstack:
		nd = 0;	/* count number of ".." */
		while(todo && ftw == todo)
		{
			f = ftw->parent;
			if(ftw->info & FTW_DNX)
			{
				if(curdir == ftw)	/* ASSERT(cdrv == 0) */
				{
					nd += 1;
					curdir = f;
				}

				/* perform post-order processing */
				if(postorder &&
				   ftw->status != FTW_SKIP && ftw->status != FTW_NOPOST)
				{	/* move to parent dir */
					if(nd > 0)
					{
						cdrv = popdirs(nd,curdir);
						nd = 0;
					}
					if(cdrv < 0)
						cdrv = setpdir(Home,path,base);
					curdir = cdrv ? 0 : f;
	
					ftw->info = FTW_DP;
					ftw->path = PATH(path,ftw->level);
					ftw->pathlen = (base - ftw->path) + ftw->namelen;
					ftw->status = STATUS(cdrv);
					link = ftw->link;
					ftw->link = 0;
					fnrv = (*userf)(ftw);
					ftw->link = link;
					if(fnrv)
						goto done;
					if(ftw->status == FTW_AGAIN)
						ftw->info = FTW_D;
				}

				/* delete from dev/ino tree */
				if(diroot != ftw)
					diroot = search(ftw,diroot,statcmp,0);
				diroot = deleteroot(diroot);
			}

			/* reset base */
			if(base > path+f->namelen)
				--base;
			*base = '\0';
			base -= f->namelen;

			/* delete from top of stack */
			if(ftw->status != FTW_AGAIN)
			{
				todo = todo->link;
				freeFtw(ftw);
			}
			ftw = f;
		}

		/* reset current directory */
		if(nd > 0 && popdirs(nd,curdir) < 0)
		{
			(void) pathcd(Home,NiL);
			curdir = 0;
			cdrv = -1;
		}

		if(todo)
		{
			if(*base)
				base += ftw->namelen;
			if(*(base-1) != '/')
				*base++ = '/';
			*base = '\0';
		}
	}

	/* normal ending */
	fnrv = 0;

done:
	if(dirfp)
		closedir(dirfp);
	if(cdrv == 0)
		(void) pathcd(Home,NiL);
	if(top)
		bot->link = todo, todo = top;
	return freeAll(todo,fnrv);
}
