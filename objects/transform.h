/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Maurizio Paolini <paolini@dmf.unicatt.it>

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


#ifndef KIG_OBJECTS_TRANSFORM_H
#define KIG_OBJECTS_TRANSFORM_H

#include "cubic.h"
#include "segment.h"
#include "ray.h"
#include "circle.h"
#include "conic.h"
#include "curve.h"
#include "../misc/kignumerics.h"

#include <vector>

#include "line.h"
#include "point.h"
#include "../modes/constructing.h"
#include "../misc/common.h"

#define MAXTRANSFORMATIONS 5

class LineTransform
  : public Line
{
public:
  LineTransform( const Objects& os );
  ~LineTransform() {};
  LineTransform(const LineTransform& c);

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "LineTransform"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "objecttransform"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // passing arguments
  static Object::WantArgsResult sWantArgs( const Objects& os );
  static QString sUseText( const Objects& os, const Object* );
  static void sDrawPrelim( KigPainter& p, const Objects& os );

  Objects getParents() const;

protected:
  AbstractLine* mline;
  Object* mtransformations[MAXTRANSFORMATIONS];
  int transformationsnum;

  void calc();
};

class PointTransform
  : public Point
{
public:
  PointTransform( const Objects& os );
  ~PointTransform() {};
  PointTransform(const PointTransform& c);

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "PointTransform"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "objecttransform"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // passing arguments
  static Object::WantArgsResult sWantArgs( const Objects& os );
  static QString sUseText( const Objects& os, const Object* );
  static void sDrawPrelim( KigPainter& p, const Objects& os );

  Objects getParents() const;

protected:
  Point* mpoint;
  Object* mtransformations[MAXTRANSFORMATIONS];
  int transformationsnum;

  void calc();
};

class SegmentTransform
  : public Segment
{
public:
  SegmentTransform( const Objects& os );
  ~SegmentTransform() {};
  SegmentTransform(const SegmentTransform& c);

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "SegmentTransform"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "objecttransform"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // passing arguments
  static Object::WantArgsResult sWantArgs( const Objects& os );
  static QString sUseText( const Objects& os, const Object* );
  static void sDrawPrelim( KigPainter& p, const Objects& os );

  Objects getParents() const;

protected:
  Segment* msegment;
  Object* mtransformations[MAXTRANSFORMATIONS];
  int transformationsnum;

  void calc();
};

class RayTransform
  : public Line
{
public:
  RayTransform( const Objects& os );
  ~RayTransform() {};
  RayTransform(const RayTransform& c);

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "RayTransform"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "objecttransform"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // passing arguments
  static Object::WantArgsResult sWantArgs( const Objects& os );
  static QString sUseText( const Objects& os, const Object* );
  static void sDrawPrelim( KigPainter& p, const Objects& os );

  Objects getParents() const;

protected:
  AbstractLine* mray;
  Object* mtransformations[MAXTRANSFORMATIONS];
  int transformationsnum;

  void calc();
};

class CircleTransform
  : public Circle
{
public:
  CircleTransform( const Objects& os );
  ~CircleTransform() {};
  CircleTransform(const CircleTransform& c);

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "CircleTransform"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "objecttransform"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // passing arguments
  static Object::WantArgsResult sWantArgs( const Objects& os );
  static QString sUseText( const Objects& os, const Object* );
  static void sDrawPrelim( KigPainter& p, const Objects& os );

  Objects getParents() const;

protected:
  Circle* mcircle;
  Object* mtransformations[MAXTRANSFORMATIONS];
  int transformationsnum;

  void calc();
};

class ConicTransform
  : public Conic
{
public:
  ConicTransform( const Objects& os );
  ~ConicTransform() {};
  ConicTransform(const ConicTransform& c);

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "ConicTransform"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "objecttransform"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  virtual const ConicPolarEquationData polarEquationData() const;

  // passing arguments
  static Object::WantArgsResult sWantArgs( const Objects& os );
  static QString sUseText( const Objects& os, const Object* );
  static void sDrawPrelim( KigPainter& p, const Objects& os );

  Objects getParents() const;

protected:
  Conic* mconic;
  Object* mtransformations[MAXTRANSFORMATIONS];
  int transformationsnum;

//  ConicCartesianEquationData cequation;
  ConicPolarEquationData mequation;
  void calc();
};

const ConicCartesianEquationData calcConicTransformation (
    ConicCartesianEquationData data,
    double transformation[3][3], bool& valid );

const CubicCartesianEquationData calcCubicTransformation (
    CubicCartesianEquationData data,
    double transformation[3][3], bool& valid );

class CubicTransform
  : public Cubic
{
public:
  CubicTransform( const Objects& os );
  ~CubicTransform() {};
  CubicTransform(const CubicTransform& c);
  CubicTransform* copy() { return new CubicTransform(*this); };

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "CubicTransform"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "objecttransform"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // passing arguments
  static Object::WantArgsResult sWantArgs( const Objects& os );
  static QString sUseText( const Objects& os, const Object* );
  static void sDrawPrelim( KigPainter& p, const Objects& os );

  Objects getParents() const;

protected:
  Cubic* mcubic;
  Object* mtransformations[MAXTRANSFORMATIONS];
  int transformationsnum;

  Point* pt1;
  Point* pt2;
  void calc();
};

#endif // KIG_OBJECTS_TRANSFORM_H
