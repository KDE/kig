// text_imp.h
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

#ifndef KIG_OBJECTS_TEXT_IMP_H
#define KIG_OBJECTS_TEXT_IMP_H

#include "object_imp.h"

#include "../misc/coordinate.h"
#include "../misc/rect.h"

class TextImp
  : public ObjectImp
{
  typedef ObjectImp Parent;
  QString mtext;
  Coordinate mloc;
  // with this var, we keep track of the place we drew in, for use in
  // the contains() function..
  mutable Rect mboundrect;
public:
  TextImp( const QString& text, const Coordinate& loc );
  TextImp* copy() const;
  ~TextImp();

  ObjectImp* transform( const Transformation& ) const;

  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, int width, const ScreenInfo& si ) const;
  bool inRect( const Rect& r ) const;
  bool valid() const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const QCStringList propertiesInternalNames() const;
  ObjectImp* property( uint which, const KigDocument& w ) const;

  bool inherits( int typeID ) const;

  int id() const;

  const char* baseName() const;
};

#endif
