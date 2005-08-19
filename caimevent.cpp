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

#ifndef _CAIM_EVENT_CPP
#define _CAIM_EVENT_CPP 1

#include "caimevent.h"

namespace CTocAim {

  void AIMEvent::parse(const char *data,int len) {
    _data.clear();
    if (!data) return;
    if (len < 1) {
      _type = "NULL_EVENT";
      return;
    }
    tokenize(_data,data,":",len);
  }

  void AIMEvent::tokenize(std::vector<std::string> &a,const char *st,const char *d,int ub) {
    std::string s;
    int i;
    for (i=0;i<ub;++i) s += st[i];
    i=0;
    while (s.find(d) != -1) {
      std::string temp = s.substr(0,s.find(d));
      ++i;
      if (i==1) _type = temp;
      else if (i == 2 && _type == "IM_IN") _from = temp;
      else if (i == 2 && _type == "CHAT_IN") _to = temp;
      else if (i == 2 && _type == "EVILED") _from = temp;
      else if (i == 3 && _type == "CHAT_IN") _from = temp;
      else if (i == 4 && _type == "CHAT_INVITE") _from = temp;
      else a.push_back(temp);
      s = s.substr(temp.length()+strlen(d));
      if (i == 1 && (_type == "SIGN_ON"   || _type == "CONFIG"     || _type == "NICK" ||
                     _type == "CHAT_LEFT" || _type == "DIR_STATUS")) {
        a.push_back(s);
        return;
      }
      if (i == 2 && (_type == "ERROR"     || _type == "EVILED" ||
                     _type == "CHAT_JOIN" || _type == "GOTO_URL")) {
        a.push_back(s);
        return;
      }
      if (i == 3 && (_type == "IM_IN" || _type == "CHAT_IN")) {
        a.push_back(s);
        return;
      }
      if (i == 4 && _type == "CHAT_INVITE") {
        a.push_back(s);
        return;
      }
    }
    if (_type == "") _type = s;
    else a.push_back(s);
  }

  AIMEvent::AIMEvent(void) { }

  AIMEvent::AIMEvent(const std::string &To, const char *Data,
                     const int &Length) {
    _from = "<server>";
    _to = To;
    parse(Data,Length);
  }

  AIMEvent::~AIMEvent(void) { }

  void AIMEvent::dump(void) {
    std::cerr << "TYPE: " << _type << std::endl;
    std::cerr << "FROM: " << _from << std::endl;
    std::cerr << "TO:   " << _to   << std::endl;
    std::cerr << "Args: " << std::endl;
    for (int i=0;i<_data.size();++i) {
      std::cerr << "\targ[" << i << "]: " << _data[i] << std::endl;
    }
    std::cerr << std::endl;
  }

  AIMEvent &AIMEvent::operator=(const AIMEvent &e) {
    _type = e._type;
    _from = e._from;
    _to = e._to;
    _data = e._data;
    return *this;
  }

  std::vector<std::string> AIMEvent::args(void) { return _data; }

  void AIMEvent::args(const std::vector<std::string> &Arguments) { _data = Arguments; }

  std::string AIMEvent::arg(unsigned int index) {
    if (index >= _data.size()) return "NULL";
    return _data[index];
  }

  std::string AIMEvent::from(void) { return _from; }

  void AIMEvent::from(const std::string &From) { _from = From; }

  std::string AIMEvent::to(void) { return _to; }

  void AIMEvent::to(const std::string &To) { _to = To; }

  std::string AIMEvent::type(void) { return _type; }

  void AIMEvent::type(const std::string &Type) { _type = Type; }

  std::string AIMEvent::trans(const std::string &code) {
    return "UNKNOWN";
  }

  void AIMEvent::data(const char *Data, const int &Length) {
    parse(Data,Length>0?Length:strlen(Data));
  }

} // namespace ctocaim

#endif // _CAIM_EVENT_CPP
