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
/*
 * $XConsortium: ServiceMgrX.hh /main/3 1996/06/11 16:28:05 cde-hal $
 *
 * Copyright (c) 1992 HaL Computer Systems, Inc.  All rights reserved.
 * UNPUBLISHED -- rights reserved under the Copyright Laws of the United
 * States.  Use of a copyright notice is precautionary only and does not
 * imply publication or disclosure.
 * 
 * This software contains confidential information and trade secrets of HaL
 * Computer Systems, Inc.  Use, disclosure, or reproduction is prohibited
 * without the prior express written permission of HaL Computer Systems, Inc.
 * 
 *                         RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the Government is subject to
 * restrictions as set forth in subparagraph (c)(l)(ii) of the Rights in
 * Technical Data and Computer Software clause at DFARS 252.227-7013.
 *                        HaL Computer Systems, Inc.
 *                  1315 Dell Avenue, Campbell, CA  95008
 * 
 */

#include <X11/Intrinsic.h>

extern "C" {
#include "external-api/olias.h"
}

class ServiceMgr : public ClientEventHandler
{
public: // functions
  ServiceMgr();
  ~ServiceMgr();

  void establish_server();

protected: // functions

  void create_atoms();

  void reply_to_client (Window, OliasStatus);

//  static void handle_olias_event (Widget, XtPointer, XEvent *, Boolean *);

  void handle_event(Widget, XEvent*, Boolean *);

  void get_olias_event (XEvent *);

  static void receive_olias_event (Widget, XtPointer, Atom *, Atom *,
				   XtPointer, unsigned long *, int *);
  void process_olias_event (Window client,
			    unsigned char *stream, unsigned long length);

private:
  LONG_LIVED_HH(ServiceMgr,service_manager);
};

LONG_LIVED_HH2(ServiceMgr,service_manager);


