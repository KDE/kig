// bogus_imp.h
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

#ifndef BOGUS_IMP_H
#define BOGUS_IMP_H

#include "object_imp.h"

#include <qstring.h>

class BogusImp
  : public ObjectImp
{
  typedef ObjectImp Parent;
public:
  void draw( KigPainter& p ) const;
  bool contains( const Coordinate& p, const ScreenInfo& si ) const;
  bool inRect( const Rect& r ) const;
  bool valid() const;

  ObjectImp* transform( const Transformation& ) const;

  const uint numberOfProperties() const;
  const QCStringList properties() const;
  const Property property( uint which, const KigWidget& w ) const;
};

class InvalidImp
  : public BogusImp
{
  typedef BogusImp Parent;
public:
  InvalidImp();
  bool valid() const;
  bool inherits( int t ) const;
  InvalidImp* copy() const;
};

class DoubleImp
  : public BogusImp
{
  typedef BogusImp Parent;
  double mdata;
public:
  DoubleImp( const double d );

  double data() const { return mdata; };

  bool inherits( int typeID ) const;

  DoubleImp* copy() const;
};

class IntImp
  : public BogusImp
{
  typedef BogusImp Parent;
  int mdata;
public:
  IntImp( const int d );

  int data() const { return mdata; };

  bool inherits( int typeID ) const;
  IntImp* copy() const;
};

class StringImp
  : public BogusImp
{
  typedef BogusImp Parent;
  QString mdata;
public:
  StringImp( const QString& d );

  const QString& data() const { return mdata; };

  bool inherits( int typeID ) const;
  StringImp* copy() const;
};

#endif
