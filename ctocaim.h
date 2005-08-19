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

#ifndef _TOC_AIM_H
#define _TOC_AIM_H 1

#include <sys/types.h>

#include <cctype>
#include <string>
#include <vector>
#include <iostream>
#include <ctime>

#include "caimevent.h"
#include "csocket.h"

namespace CTocAim {
  class AIM {
    private:
      std::string _sn;
      std::string _pwd;
      std::string _toc_server;
      std::string _auth_server;
      std::string _agent;
      unsigned int _toc_port;
      unsigned int _auth_port;
      unsigned short _outseq;
      bool _connected;
      bool _debug;
      bool _reconnect;
      double _timeout;
      CSocket _sock;

      void login(void);
      std::string itos(unsigned int i); // int to std::string
      std::string encode_pass(const std::string &pass);
      std::string normalize(const std::string &sn);
      std::string encode(const std::string &data);
      int send_to_aol(const std::string &data);
    public:
      AIM(const std::string &ScreenName,
          const std::string &Password,
          const std::string &Toc_Server = "toc.oscar.aol.com",
          const unsigned int &Toc_Port = 5190,
          const std::string &Auth_Server = "login.oscar.aol.com",
          const unsigned int &Auth_Port = 5159,
          const std::string &Agent = "CTocAim");
      ~AIM(void);
      AIMEvent do_event(void);
      int send_im(const std::string &buddy, const std::string &message);
      int set_idle(const std::string &secs);
      int set_idle(const unsigned int &secs);
      int add_buddy(const std::vector<std::string> &buddies);
      int add_buddy(const std::string &buddy);
      int add_permit(const std::vector<std::string> &buddies);
      int add_permit(const std::string &buddy);
      int add_deny(const std::vector<std::string> &buddies);
      int add_deny(const std::string &buddy);
      int remove_buddy(const std::vector<std::string> &buddies);
      int remove_buddy(const std::string &buddy);
      int set_away(const std::string &message);
      int get_info(const std::string &buddy);
      int set_info(const std::string &info);
      int evil(const std::string &user, const bool &anon=true);
      int chat_invite(const std::string &room,const std::string &message,
                      const std::vector<std::string> &buddies);
      int chat_invite(const std::string &room,const std::string &message,
                      const std::string &buddy);
      int chat_accept(const std::string &id);
      int chat_leave(const std::string &id);
      int chat_whisper(const std::string &id,const std::string &buddy,
                       const std::string &message);
      int chat_send(const std::string &id,const std::string &message);
      int chat_join(const std::string &room);
      bool connected(void);
      bool debug(const bool &flag);
      bool debug(void);
      void disconnect(void);
      void reconnect(void /*const unsigned int &secs_pause = 0*/);
      bool reconnect(const bool &flag);
      void connect(void);
      double timeout(const double &secs);
      double timeout(void);
  };
}
#endif // _TOC_AIM
