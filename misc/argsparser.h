// argsparser.h
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef KIG_MISC_ARGSPARSER_H
#define KIG_MISC_ARGSPARSER_H

#include <vector>

#include "../objects/common.h"

class ArgsChecker
{
public:
  virtual ~ArgsChecker();
  enum { Invalid = 0, Valid = 1, Complete = 2 };

  virtual int check( const Objects& os ) const = 0;
};

class CheckOneArgs
  : public ArgsChecker
{
public:
  int check( const Objects& os ) const;
};

class ArgParser
  : public ArgsChecker
{
public:
  struct spec { int type; const char* usetext; };
private:
  // the args spec..
  std::vector<spec> margs;
  // sometimes a random object is requested: any type goes.  Those
  // requests require some special treatment.  This vector holds the
  // usetexts for those requests..
  std::vector<const char*> manyobjsspec;
public:
  ArgParser( const struct spec* args, int n );
  ArgParser( const std::vector<spec>& args, const std::vector<const char*> anyobjsspec );
  // returns a new ArgParser that wants the same args, except for the
  // ones of the given type..
  ArgParser without( int type ) const;
  int check( const Objects& os ) const;
  Args parse( const Args& os ) const;
  const char* usetext( const Object& o, const Objects& sel ) const;
  Objects parse( const Objects& os ) const;
};

#endif
