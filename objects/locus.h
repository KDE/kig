/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

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


#ifndef LOCUS_H
#define LOCUS_H

#include "curve.h"

#include "../misc/rect.h"

#include <list>

class ObjectHierarchy;

// this object is inspired on KSeg source, credits for all of the
// ideas go to the KSeg author Ilya Baran <ibaran@mit.edu>
// a locus object is a mathematical locus.  It is defined by a
// constrained point, which moves over a curve, and an object,
// which somehow depends on that point.  The locus contains all
// objects the object moves over as the point moves over all of
// its possible positions...
// this is implemented by having a Locus simply contain some 500
// objects (hmm...:)
// drawing is done by simply drawing the points...
class Locus
  : public Curve
{
public:
  // number of points to include, i think this is a nice default...
  static const int numberOfSamples = 1000;
public:
  Locus( const Objects& os );
  Locus( const Locus& loc );
  ~Locus();
  Locus* copy();

  virtual const QCString vBaseTypeName() const;
  static const QCString sBaseTypeName();
  virtual const QCString vFullTypeName() const;
  static const QCString sFullTypeName();
  const QString vDescriptiveName() const;
  static const QString sDescriptiveName();
  const QString vDescription() const;
  static const QString sDescription();
  const QCString vIconFileName() const;
  static const QCString sIconFileName();
  const int vShortCut() const;
  static const int sShortCut();
  static const char* sActionName();

  void draw (KigPainter& p, bool showSelection) const;
  bool contains (const Coordinate& o, const ScreenInfo& si ) const;
  bool inRect (const Rect&) const;

  // arguments
  static Object::WantArgsResult sWantArgs( const Objects& os );
  static QString sUseText( const Objects&, const Object* o );
  static void sDrawPrelim( KigPainter& p, const Objects& args );

  void calc();
  void calcForWidget( const KigWidget& );

  Coordinate getPoint( double param ) const;
  double getParam (const Coordinate&) const;

  Objects getParents() const;

private:
  NormalPoint* cp;
  Point* mp;

  Objects calcpath;

  // objs is a list of the objects we contain..
  Objects objs;

  struct CPt
  {
    CPt(Coordinate inPt, double inPm) : pt(inPt), pm (inPm) {};
    Coordinate pt;
    double pm;
  };

  typedef std::vector<CPt> CPts;

  // for calcPointLocus we need some special magic, so it is a special
  // type...
  CPts pts;

  // some functions used by calcPointLocus...
  inline Coordinate internalGetCoord( double );
};
#endif
