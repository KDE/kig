// label.h
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

#ifndef KIG_OBJECTS_LABEL_H
#define KIG_OBJECTS_LABEL_H

#include "object.h"

#include "../misc/coordinate.h"
#include "../misc/rect.h"

#include <vector>

class TextLabel
  : public Object
{
  typedef Object Parent;
public:
  typedef std::vector<TextLabelProperty> propvect;

  TextLabel( const Objects& os );
  TextLabel( const TextLabel& l );
  TextLabel( const QString text, const Coordinate c, const propvect& props );
  ~TextLabel();

  virtual bool isa( int type ) const;

  prop_map getParams ();
  void setParams ( const prop_map& );

  const QCString vBaseTypeName() const;
  static QCString sBaseTypeName();

  const QCString vFullTypeName() const;
  static const QCString sFullTypeName();

  const QString vDescriptiveName() const;
  static const QString sDescriptiveName();

  const QString vDescription() const;
  static const QString sDescription();

  const QCString vIconFileName() const;
  static const QCString sIconFileName();

  static const char* sActionName();

  void draw (KigPainter& p, bool showSelection) const;
  bool contains ( const Coordinate& o, const ScreenInfo& si ) const;
  bool inRect (const Rect& r) const;

  void startMove(const Coordinate&, const ScreenInfo&);
  void moveTo(const Coordinate&);

  void calc();
  void calcForWidget( const KigWidget& w );

  static KigMode* sConstructMode( Type* t, KigDocument& d );

  Objects getParents() const;

  const uint numberOfProperties() const;
  const Property property( uint which, const KigWidget& w ) const;
  const QCStringList properties() const;

private:
  propvect mprops;
  QString mtext;
  QString mcurtext;
  Coordinate mcoord;
  Coordinate mpwwmt;
  /**
   * rect we drew ourselves in...
   * mutable hack, cause i can only find this Rect during the draw
   * function
   */
  mutable Rect mrwdoi;
};

#endif
