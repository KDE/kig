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


#ifndef LOCUS_H
#define LOCUS_H

#include "curve.h"

#include "../misc/rect.h"

#include <list>

class ObjectHierarchy;

// this object is inspired on KSeg source, credits for all of the
// ideas go to Ilya Baran <ibaran@mit.edu>
// a locus object is a mathematical locus.  It is defined by a
// constrained point, which moves over a curve, and an object,
// which somehow depends on that point.  The locus contains all
// objects the object moves over as the point moves over all of
// its possible positions...
// this is implemented by having a Locus simply contain some 150
// objects (hmm...:)
// drawing is done by simply drawing the points...
class Locus
  : public Curve
{
public:
  // number of points to include, i think this is a nice default...
  static const int numberOfSamples = 1000;
public:
  Locus();
  ~Locus();
  Locus(const Locus& loc);
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
  bool contains (const Coordinate& o, const double fault ) const;
  bool inRect (const Rect&) const;

  // arguments
  QString wantArg ( const Object* ) const;
  bool selectArg (Object* which);
//   void unselectArg (Object* which);
  void drawPrelim ( KigPainter&, const Object* ) const;

  // moving
  void startMove(const Coordinate&);
  void moveTo(const Coordinate&);
  void stopMove();

  void calc( const ScreenInfo& );

public:
  Coordinate getPoint( double param ) const;
  double getParam (const Coordinate&) const;

  Objects getParents() const;

protected:
  NormalPoint* cp;
  Object* obj;

  bool isPointLocus() const;
  bool _pointLocus;

  Objects calcpath;

  // objs is just a list of pointers to objects
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

  // this is used if the obj is a point; it selects the best points
  // from the possible ones...
  void calcPointLocus( const ScreenInfo& );
  // some functions used by calcPointLocus...
  CPts::iterator addPoint( double param, const ScreenInfo& );
  void recurse( CPts::iterator, CPts::iterator, int&, const ScreenInfo& );

  // this is used when the obj is not a point; it just takes the first
  // numberOfSamples objects it can find...
  void calcObjectLocus( const ScreenInfo& );
};
#endif
