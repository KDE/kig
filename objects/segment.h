/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/


#ifndef segment_h
#define segment_h

#include "curve.h"

class Segment
  : public Curve
{
public:
  Segment();
  Segment(const Segment& s);
  Segment* copy() { return new Segment(*this); };
  ~Segment();

  // type identification
  Segment* toSegment() { return this; };
  const Segment* toSegment() const { return this; };
  // some type information
  const QCString vBaseTypeName() const { return sBaseTypeName();};
  static QCString sBaseTypeName() { return I18N_NOOP("segment"); };
  const QCString vFullTypeName() const { return sFullTypeName(); };
  static QCString sFullTypeName() { return I18N_NOOP("Segment"); };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName() { return i18n( "Segment" ); };
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription() { return i18n( "A segment constructed from its start and end point" ); };
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "segment"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return CTRL+Key_S; };

  QCString iType() const { return I18N_NOOP("segment"); };
  bool contains (const Coordinate& o, const double fault ) const;
  void draw ( KigPainter& p, bool showSelection ) const;
  void drawPrelim ( KigPainter&, const Coordinate& ) const;

  bool inRect (const Rect&) const;

  // arguments
  QString wantArg ( const Object* ) const;
  QString wantPoint() const;
  bool selectArg (Object* which);
  void unselectArg (Object* which);
    Objects getParents() const;

  // moving
  void startMove(const Coordinate&);
  void moveTo(const Coordinate&);
  void stopMove();
  //     void cancelMove();

  void calc();
  void deleted( Object* o );

  Coordinate getPoint (double param) const;
  double getParam (const Coordinate&) const;

  const Coordinate& getP1() { return p1->getCoord();};
  const Coordinate& getP2() { return p2->getCoord();};
  Point* getPoint1() { return p1; };
  Point* getPoint2() { return p2; };
protected:
  Point* p1, *p2;
  Coordinate pwwsm; // point where we started moving
};

#endif
