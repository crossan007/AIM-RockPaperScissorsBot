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

#ifndef _TOC_AIM_CPP
#define _TOC_AIM_CPP 1

#include "ctocaim.h"

namespace CTocAim {

  void AIM::login(void) {
    std::string scrnm = normalize(_sn);
    srand(time(0));
    _outseq = (rand()%100000)+1;
    char signon_data[50];
    sprintf(signon_data,"*\1%c%c%c%c%c%c%c\1%c\1%c%c%s",
            (_outseq>>8)&255,_outseq&255,((8+scrnm.length())>>8)&255,
            (8+scrnm.length())&255,0,0,0,0,0,scrnm.length(),
            scrnm.c_str());
    if (_sock.write(signon_data,14+scrnm.length()) > 0) {
      if (_debug) std::cout << "signon_data sent successfully." << std::endl;
      ++_outseq;
      std::string buff = "toc_signon " + _auth_server + " " + itos(_auth_port) +
                         " " + scrnm + " " + encode_pass(_pwd) + " english " +
                         encode(_agent);
      if (send_to_aol(buff) > 0) {
        if (_debug) std::cout << "toc_signon sent successfully." << std::endl;
      }
      else {
        std::cerr << "ERROR: Failed to send toc_signon." << std::endl;
        disconnect();
        if (_reconnect) connect();
      }
    }
    else {
      std::cerr << "ERROR: Failed to send signon data to server." << std::endl;
      disconnect();
      if (_reconnect) connect();
    }
  }

  std::string AIM::itos(unsigned int i) {
    std::string ret;
    while (i) {
      ret = (char)((i%10) + 48) + ret;
      i/=10;
    }
    return ret;
  }

  std::string AIM::encode_pass(const std::string &pass) {
    char p[35] = {'0','x'}; // max pass len = 16
    char *table = "Tic/Toc";
    char temp[3];
  
    for (int i=0;i<pass.length()&&i<16;++i) {
      sprintf(temp,"%02x",pass[i]^table[i%7]);
      strcat(p,temp);
    }
    std::string ret = p;
    return ret;
  }

  std::string AIM::normalize(const std::string &sn) {
    std::string ret;
    for (int i=0;i<sn.length();++i) {
      if (isalnum(sn[i])) ret += tolower(sn[i]);
    }
    return ret;
  }

  std::string AIM::encode(const std::string &data) {
    std::string ret = "\"";
    for (int i=0;i<data.length();++i) {
      if (data[i] == '\\' || data[i] == '{' || data[i] == '}' ||
          data[i] == '('  || data[i] == ')' || data[i] == '[' ||
          data[i] == ']'  || data[i] == '$' || data[i] == '"') ret += '\\';
      ret += data[i];
    }
    ret += "\"";
    return ret;
  }

  int AIM::send_to_aol(const std::string &data) {
    char buffer[1024];
    std::string trunc = data.substr(0,1017);
    sprintf(buffer,"*\2%c%c%c%c%s",(_outseq>>8)&255,_outseq++&255,
            ((trunc.length()+1)>>8)&255,(trunc.length()+1)&255,trunc.c_str());
    int ret = _sock.write(buffer,trunc.length()+7);
    if (ret < 1) {
      std::cerr << "ERROR: Failed writing to socket." << std::endl;
      disconnect();
      if (_reconnect) reconnect();
    }
    return ret;
  }

  AIM::AIM(const std::string &ScreenName,
           const std::string &Password,
           const std::string &Toc_Server,
           const unsigned int &Toc_Port,
           const std::string &Auth_Server,
           const unsigned int &Auth_Port,
           const std::string &Agent) {
    _sn = ScreenName;
    _pwd = Password;
    _toc_server = Toc_Server;
    _toc_port = Toc_Port;
    _auth_server = Auth_Server;
    _auth_port = Auth_Port;
    _agent = Agent;
    _debug = false;
    _connected = false;
    _reconnect = true;
    _timeout = 0.1;

  }

  AIM::~AIM(void) { }

  AIMEvent AIM::do_event(void) {
    AIMEvent ret(_sn,"NULL",0);
    if (!_connected) return ret;
    fd_set rfds;
    struct timeval tv;
    tv.tv_sec = (int)_timeout;
    tv.tv_usec = (_timeout - tv.tv_sec) * 1000;
    FD_ZERO(&rfds);
    FD_SET(_sock.sock(), &rfds);
    int sel = select(1, &rfds, 0, 0, &tv);
    if (sel > 0) {
      unsigned char header[7];
      memset(header,0,7);
      if (_sock.read(header,6) > 0) {
        if (header[0] != '*') {
          std::cerr << "ERROR: MALFORMED PACKET HAS HEADER: ";
          for (int i=0;i<6;++i) std::cerr << (int)header[i] << " ";
          std::cerr << std::endl;
        }
        else {
          int len = 256*header[4] + header[5];
          char *data = new char[len + 1];
          if (!data) {
            std::cerr << "ERROR: Unable to allocate " << len+1 <<
                         " byte(s) for incoming event." << std::endl;
            ret.data("EVENT_FAILED");
            return ret;
          }
          int i=0;
          _sock.read(data,len);
          ret.data(data,len);
          if (ret.type() == "ERROR") ret.dump();
          else if (ret.type() == "SIGN_ON") {
            std::string info = "<BODY BGCOLOR=\"#FFFFFF\" TEXT=\"#000000\">";
                       info += "<FONT FACE=\"Verdana\" LANG = \"0\" SIZE=2>";
                       info += "CTocAim v0.1 by ";
                       info += "<A HREF=\"mailto:honey010@bama.ua.edu\">";
                       info += "Jon Honeycutt</A><BR><BR><HR>";
                       info += "http://bama.ua.edu/~honey010/ctocaim/</FONT>";
            add_buddy("ctocaim");
            set_info(info);
            send_to_aol("toc_init_done");
          }
          delete data;
          return ret;
        }
      }
      else {
        std::cerr << "ERROR: Failed reading 6-byte header (disconnected)."
                  << std::endl;
        disconnect();
        ret.data("DISCONNECT");
        return ret;
      }
    }
    ret.data("NULL");
    return ret;
  }

  int AIM::send_im(const std::string &buddy,const std::string &message) {
    std::string buff = "toc_send_im " + normalize(buddy) + " " +
                       encode(message);
    return send_to_aol(buff);
  }

  int AIM::set_idle(const std::string &secs) {
    std::string buff = "toc_set_idle " + secs;
    return send_to_aol(buff);
  }

  int AIM::set_idle(const unsigned int &secs) {
    std::string buff = "toc_set_idle " + itos(secs);
    return send_to_aol(buff);
  }

  int AIM::add_buddy(const std::vector<std::string> &buddies) {
    std::string buff = "toc_add_buddy";
    for (int i=0;i<buddies.size();++i) {
      buff += " " + normalize(buddies[i]);
    }
    return send_to_aol(buff);
  }

  int AIM::add_buddy(const std::string &buddy) {
    std::string buff = "toc_add_buddy " + normalize(buddy);
    return send_to_aol(buff);
  }

  int AIM::add_permit(const std::vector<std::string> &buddies) {
    std::string buff = "toc_add_permit";
    for (int i=0;i<buddies.size();++i) {
      buff += " " + normalize(buddies[i]);
    }
    return send_to_aol(buff);
  }

  int AIM::add_permit(const std::string &buddy) {
    std::string buff = "toc_add_permit " + normalize(buddy);
    return send_to_aol(buff);
  }

  int AIM::add_deny(const std::vector<std::string> &buddies) {
    std::string buff = "toc_add_deny";
    for (int i=0;i<buddies.size();++i) {
      buff += " " + normalize(buddies[i]);
    }
    return send_to_aol(buff);
  }

  int AIM::add_deny(const std::string &buddy) {
    std::string buff = "toc_add_deny " + normalize(buddy);
    return send_to_aol(buff);
  }

  int AIM::remove_buddy(const std::vector<std::string> &buddies) {
    std::string buff = "toc_remove_buddy";
    for (int i=0;i<buddies.size();++i) {
      buff += " " + normalize(buddies[i]);
    }
    return send_to_aol(buff);
  }

  int AIM::remove_buddy(const std::string &buddy) {
    std::string buff = "toc_remove_buddy " + normalize(buddy);
    return send_to_aol(buff);
  }

  int AIM::set_away(const std::string &message) {
    if (message == "") return send_to_aol("toc_set_away");
    std::string buff = "toc_set_away " + encode(message);
    return send_to_aol(buff);
  }

  int AIM::get_info(const std::string &buddy) {
    std::string buff = "toc_get_info " + normalize(buddy);
    return send_to_aol(buff);
  }

  int AIM::set_info(const std::string &message) {
    std::string buff = "toc_set_info " + encode(message);
    return send_to_aol(buff);
  }

  int AIM::evil(const std::string &buddy, const bool &anon) {
    std::string buff = "toc_evil " + normalize(buddy) + " " +
                       (anon?"anon":"norm");
    return send_to_aol(buff);
  }

  int AIM::chat_invite(const std::string &room,const std::string &message,
                       const std::vector<std::string> &buddies) {
    std::string buff = "toc_chat_invite " + room + " " + message;
    for (int i=0;i<buddies.size();++i) {
      buff += " " + normalize(buddies[i]);
    }
    return send_to_aol(buff);
  }

  int AIM::chat_accept(const std::string &id) {
    std::string buff = "toc_chat_accept " + id;
    return send_to_aol(buff);
  }

  int AIM::chat_leave(const std::string &id) {
    std::string buff = "toc_chat_leave " + id;
    return send_to_aol(buff);
  }

  int AIM::chat_whisper(const std::string &id,const std::string &user,
                        const std::string &message) {
    std::string buff = "toc_chat_whisper " + id + " " + normalize(user) +
                       " " + encode(message);
    return send_to_aol(buff);
  }

  int AIM::chat_send(const std::string &id,const std::string &message) {
    std::string buff = "toc_chat_send " + id + " " + encode(message);
    return send_to_aol(buff);
  }

  int AIM::chat_join(const std::string &room) {
    std::string buff = "toc_chat_join 4 " + room;
    return send_to_aol(buff);
  }

  bool AIM::connected(void) {
    return (_connected && _sock.sock() != INVALID_SOCKET);
  }

  bool AIM::debug(const bool &d) { return (_debug = d); }

  bool AIM::debug(void) { return _debug; }

  void AIM::disconnect(void) {
    _sock.close();
    _connected = false;
  }

  void AIM::reconnect(void) {
    if (_debug) std::cout << "Reconnecting..." << std::endl;
    disconnect();
    this->connect();
  }

  bool AIM::reconnect(const bool &flag) { return (_reconnect = flag); }

  double AIM::timeout(const double &secs) {
    if (secs < 0) return (_timeout = 0.1);
    return (_timeout = secs);
  }

  double AIM::timeout(void) { return _timeout; }

  void AIM::connect(void) {
    if (_debug) std::cout << "Attempting connect to " << _toc_server << ":" << _toc_port << std::endl;
    try { _sock.conn((char *)_toc_server.c_str(),_toc_port); }
    catch (CSocketException e) {
      std::cerr << "ERROR: Socket Exception: " << e.what() << std::endl;
      disconnect();
      return;
    }
    if (_sock.write("FLAPON\r\n\r\n") < 1) {
      std::cerr << "ERROR: Failed sending FLAPON introduction." << std::endl;
      disconnect();
      if (_reconnect) reconnect();
    }
    else {
      if (_debug) std::cout << "Connected! Logging in.." << std::endl;
      _connected = true;
      login();
    }
  }
} // namespace ctocaim

#endif // _TOC_AIM_CPP
