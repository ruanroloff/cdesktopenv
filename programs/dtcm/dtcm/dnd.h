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
/*******************************************************************************
**
**  dnd.h
**
**  static char sccsid[] = "@(#)dnd.h 1.9 94/11/07 Copyr 1993 Sun Microsystems, Inc.";
**
**  $XConsortium: dnd.h /main/4 1996/04/15 13:24:16 rswiston $
**
**  RESTRICTED CONFIDENTIAL INFORMATION:
**
**  The information in this document is subject to special
**  restrictions in a confidential disclosure agreement between
**  HP, IBM, Sun, USL, SCO and Univel.  Do not distribute this
**  document outside HP, IBM, Sun, USL, SCO, or Univel without
**  Sun's specific written approval.  This document and all copies
**  and derivative works thereof must be returned or destroyed at
**  Sun's request.
**
**  Copyright 1993 Sun Microsystems, Inc.  All rights reserved.
**
*******************************************************************************/

/*									*
 * (c) Copyright 1993, 1994 Hewlett-Packard Company			*
 * (c) Copyright 1993, 1994 International Business Machines Corp.	*
 * (c) Copyright 1993, 1994 Sun Microsystems, Inc.			*
 * (c) Copyright 1993, 1994 Novell, Inc. 				*
 */

#ifndef _DND_H
#define _DND_H

typedef enum {SingleEditorList, 
	      SingleEditorIcon, 
	      GroupEditorList, 
	      GroupEditorIcon, 
	      TodoEditorList, 
	      TodoEditorIcon, 
	      StandAloneEditor } EditorType;

typedef struct {
	Calendar	*calendar;
	EditorType	editor_type;
	caddr_t		editor;
	char		*data;
} DragContext;

#define DRAG_THRESHOLD 4


extern void cm_register_drop_site(Calendar *c, Widget w);
extern void ApptDragStart(Widget, XEvent *, Calendar *,
					   EditorType);
extern void TranslationDragStart(Widget, XEvent *, String *,
					   Cardinal *);
extern void DtcmProcessPress(Widget, XEvent *, String *,
					   Cardinal *);

#endif
