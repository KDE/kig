// label.h
// Copyright (C)  2002  Dominique Devriese <fritmebufstek@pandora.be>

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

#ifndef LABEL_H
#define LABEL_H

#include "object.h"

#include "../misc/coordinate.h"
#include "../misc/rect.h"

class TextLabel
  : public Object
{
public:
  TextLabel();
  TextLabel( const TextLabel& l );
  TextLabel( const QString text, const Coordinate c );
  ~TextLabel();

  TextLabel* copy();

  std::map<QCString,QString> getParams ();
  void setParams ( const std::map<QCString,QString>& );

  const TextLabel* toTextLabel() const;
  TextLabel* toTextLabel();

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
  bool contains ( const Coordinate& o, const double allowed_miss ) const;
  bool inRect (const Rect& r) const;
  QString wantArg ( const Object* ) const;
  bool selectArg (Object* which);
  void drawPrelim (KigPainter& p, const Object* prelimArg ) const;

  void startMove(const Coordinate&);
  void moveTo(const Coordinate&);
  void stopMove();

  void calc( const ScreenInfo& showingRect );

  static KigMode* sConstructMode( Type* t, KigDocument* d,
                                  NormalMode* p );

  Objects getParents() const;

private:
  QString mtext;
  Coordinate mcoord;
  // rect we drew ourselves in...
  // mutable hack, cause i can only find this Rect during the draw
  // function
  mutable Rect mrwdoi;
};

#endif
