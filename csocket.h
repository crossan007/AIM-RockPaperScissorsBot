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

#ifndef _CSOCKET_H
#define _CSOCKET_H 1

#include <winsock2.h>
#include <ctime>
#include <stdio.h>

namespace CTocAim {

  class CSocket {
    private:
      SOCKET _socket;
      WSADATA _wsadata;
      SOCKADDR_IN _sockaddr;

      void reset(unsigned int port);

    public:
      CSocket(void);
      ~CSocket(void);

      void conn(char *remaddr, int port);
      void close(void);
      int read(void *data, unsigned int len);
      int write(void *data, int len = -1);
      SOCKET sock(void) { return _socket; }
  };

  class CSocketException {
    private:
      char m_szText[128];

    public:
      CSocketException(char *err) { strcpy(m_szText, err); }
      ~CSocketException(void) { }

      char *what(void) { return m_szText; }
  };

} // namespace ctocaim

#endif // _CSOCKET_H
