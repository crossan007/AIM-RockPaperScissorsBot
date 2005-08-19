/* Copyright (C) 2003 Jon Honeycutt, honey010@bama.ua.edu
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <iostream>
#include "ctocaim.h"

int main() {
  CTocAim::AIM a("CTocAim","password"); // username, password
  a.connect(); // connect to server
  CTocAim::AIMEvent e; // event object
  while (a.connected()) {
    e = a.do_event(); // pull one event from server
    if (e.type() != "NULL_EVENT") { // make sure it's a valid event
      e.dump(); // print to STDOUT
    }
  }
  return 0;
}
