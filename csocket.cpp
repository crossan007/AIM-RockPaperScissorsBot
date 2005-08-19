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

#ifndef _CSOCKET_CPP
#define _CSOCKET_CPP 1

#include "csocket.h"

namespace CTocAim {

  CSocket::CSocket(void) {
    _socket = INVALID_SOCKET;
  }

  void CSocket::conn(char *remaddr, int port) {
    if (!remaddr) {
      throw CSocketException("NULL pointer passed as remote address.");
    }

    if (_socket > 0) close();

    reset(port);

    _sockaddr.sin_addr.s_addr = inet_addr(remaddr);
    if (_sockaddr.sin_addr.s_addr == INADDR_NONE) {
      LPHOSTENT host = gethostbyname(remaddr);
      if (!host) throw CSocketException("Unable to resolve address.");
      _sockaddr.sin_addr.s_addr = **(int**)(host->h_addr_list);
    }

    if (connect(_socket, (SOCKADDR*)&_sockaddr, sizeof(sockaddr))) {
      throw CSocketException("connect() failed");
    }
  }

  CSocket::~CSocket() { close(); WSACleanup(); }

  void CSocket::close() {
    if (_socket != INVALID_SOCKET) closesocket(_socket);
  }

  int CSocket::read(void *data, unsigned int len) {
    if (!data || !len) return -1;
    return recv(_socket, (char *)data, len, 0);
  }

  void CSocket::reset(unsigned int port) {
    if (WSAStartup(MAKEWORD(2,0), &_wsadata)) {
      throw CSocketException("WSAStartup() failed");
    }

    _socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_socket == INVALID_SOCKET) throw CSocketException("socket() failed");

    memset(&_sockaddr, 0, sizeof(sockaddr));

    _sockaddr.sin_family = AF_INET;
    _sockaddr.sin_port = htons(port);
    _sockaddr.sin_addr.s_addr = INADDR_ANY;
  }
  int CSocket::write(void *data, int len) {
    if (!data) return -1;
    if (len == -1) len = strlen((char *)data);
    if (len < 1) return -1;
    return send(_socket, (char *)data, len, 0);
  }

} // namespace ctocaim

#endif // _CSOCKET_CPP
