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

class ObjectImpType;

class ArgsParser
{
public:
  struct spec { const ObjectImpType* type; const char* usetext; };
  enum { Invalid, Valid, Complete };
private:
  // the args spec..
  std::vector<spec> margs;

  spec findSpec( const ObjectImp* o, const Args& parents ) const;
public:
  ArgsParser( const struct spec* args, int n );
  ArgsParser( const std::vector<spec>& args );
  ArgsParser();
  ~ArgsParser();

  void initialize( const std::vector<spec>& args );
  void initialize( const struct spec* args, int n );
  // returns a new ArgsParser that wants the same args, except for the
  // ones of the given type..
  ArgsParser without( const ObjectImpType* type ) const;
  // checks if os matches the argument list this parser should parse..
  int check( const Objects& os ) const;
  int check( const Args& os ) const;
  // returns the usetext for the argument that o would be used for,
  // if sel were used as parents..
  // o should be in sel...
  const char* usetext( const ObjectImp* o, const Args& sel ) const;

  // this reorders the objects or args so that they are in the same
  // order as the requested arguments..
  Objects parse( const Objects& os ) const;
  Args parse( const Args& os ) const;

  // returns the minimal ObjectImp ID that o needs to inherit in order
  // to be useful..  o should be part of parents.
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;

  // Checks the args according to this args specification.  If the
  // objects should never have occurred, then an assertion failure
  // will happen, if one of the args is invalid, then false will be
  // returned, if all is fine, then true is returned..
  // assert that the objects are of the right types, and in the right
  // order as what would be returned by parse( os )..  If minobjects
  // is provided, then not all objects are needed, and it is enough if
  // at least minobjects are available..  Use this for object types
  // that can calc a temporary example object using less than the
  // required args.  These args need to be at the end of argsspec +
  // anyobjsspec.  If minobjects is not provided, then it is assumed
  // that all args are necessary.
  bool checkArgs( const Objects& os ) const;
  bool checkArgs( const Objects& os, uint minobjects ) const;
  bool checkArgs( const Args& os ) const;
  bool checkArgs( const Args& os, uint minobjects ) const;
};

#endif
