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

#ifndef _AIM_EVENT_H
#define _AIM_EVENT_H 1

#include <string>
#include <vector>
#include <iostream>

namespace CTocAim {

  class AIMEvent {
    private:
      std::string _type;
      std::string _from;
      std::string _to;
      std::vector<std::string> _data;
      void parse(const char *data,int len);
      void tokenize(std::vector<std::string> &a,const char *st,
                    const char *d,int ub);
    public:
      AIMEvent(void);
      AIMEvent(const std::string &To, const char *Data,
               const int &Length);
      ~AIMEvent(void);

      void dump(void);

      AIMEvent &operator=(const AIMEvent &e);

      std::vector<std::string> args(void);
      void args(const std::vector<std::string> &Arguments);

      std::string arg(unsigned int index);

      std::string from(void);
      void from(const std::string &From);

      std::string to(void);
      void to(const std::string &To);

      std::string type(void);
      void type(const std::string &Type);

      std::string trans(const std::string &code);
      void data(const char *Data, const int &Length = -1);
  };

} // namespace ctocaim

#endif // _CAIM_EVENT_H
