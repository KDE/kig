// property.h
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


#ifndef PROPERTY_H
#define PROPERTY_H

class KigDocument;
class KigWidget;
class QString;
class Object;
class Coordinate;
class CoordinateSystem;

class Property
{
  typedef union
  {
    QString* qs;
    double d;
    Coordinate* coord;
  } data_t;

  int mtype;
  data_t mdata;

public:
  Property( const QString& s );
  Property( const double d );
  Property( const Coordinate& c );
  ~Property();

  static const int Double = 0;
  static const int String = 1;
  static const int Coord = 2;

  QString toString( const KigDocument& d, const KigWidget& w );

  // what type of property is this ?
  const int type() const;
  const double doubleData() const;
  const QString qstringData() const;
  const Coordinate coordData() const;
};

// this struct holds information about how to reach a given
// property, i.e. the object it belongs to, and its index...
struct TextLabelProperty
{
  TextLabelProperty( Object* o, unsigned int i )
    : obj( o ), index( i ) {};

  TextLabelProperty( Object* o )
    : obj( o ), index( static_cast<unsigned int>( -1 ) ) {};

  TextLabelProperty()
    : obj( 0 ), index( static_cast<unsigned int>( -1 ) ) {};

  Property value();

  bool valid();

  Object* obj;
  unsigned int index;
};

#endif
