// special_constructors.h
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_MISC_SPECIAL_CONSTRUCTORS_H
#define KIG_MISC_SPECIAL_CONSTRUCTORS_H

#include "object_constructor.h"

class ConicRadicalConstructor
  : public StandardConstructorBase
{
  const ArgparserObjectType* mtype;
  const ArgParser mparser;
public:
  ConicRadicalConstructor();
  ~ConicRadicalConstructor();
  void drawprelim( KigPainter& p, const Objects& parents, const KigDocument& ) const;
  Objects build( const Objects& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigDocument* doc, KigGUIAction* kact );
};

class LocusConstructor
  : public StandardConstructorBase
{
  ArgParser margsparser;
public:
  LocusConstructor();
  ~LocusConstructor();
  // we override the wantArgs() function, since we need to see
  // something about the objects that an ArgParser can't know about,
  // namely, whether the first point is a constrained point...
  const int wantArgs(
    const Objects& os, const KigDocument& d,
    const KigWidget& v
    ) const;
  QString useText( const Object& o, const Objects& sel ) const;
  void drawprelim( KigPainter& p, const Objects& parents, const KigDocument& ) const;
  Objects build( const Objects& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigDocument* doc, KigGUIAction* kact );
};

#endif
