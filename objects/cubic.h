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


#ifndef KIG_OBJECTS_CUBIC_H
#define KIG_OBJECTS_CUBIC_H

#include "curve.h"
#include "../misc/kignumerics.h"

#include <vector>

#include "line.h"
#include "point.h"
#include "../modes/constructing.h"
#include "../misc/common.h"

/**
 * This class represents an equation of a cubic in the form
 * "a_{ijk} x_i x_j x_k = 0" (in homogeneous coordinates, i,j,k = 0,1,2),
 * i <= j <= k.
 * The coefficients are stored in lessicografic order.
 */
class CubicCartesianEquationData
{
public:
  double coeffs[10];
  explicit CubicCartesianEquationData();
  CubicCartesianEquationData( double a000, double a001, double a002,
                              double a011, double a012, double a022,
                              double a111, double a112, double a122,
                              double a222 )
    {
      coeffs[0] = a000;
      coeffs[1] = a001;
      coeffs[2] = a002;
      coeffs[3] = a011;
      coeffs[4] = a012;
      coeffs[5] = a022;
      coeffs[6] = a111;
      coeffs[7] = a112;
      coeffs[8] = a122;
      coeffs[9] = a222;
    };
  CubicCartesianEquationData( const double incoeffs[10] );
};

/**
 * This function calcs a cartesian cubic equation such that the
 * given points are on the cubic.  There can be at most 9 and at
 * least 2 point.  If there are less than 9, than the coefficients
 * will be chosen to 1.0 if possible
 */

const CubicCartesianEquationData calcCubicThroughPoints (
    const std::vector<Coordinate>& points );

const CubicCartesianEquationData calcCubicCuspThroughPoints (
    const std::vector<Coordinate>& points );

const CubicCartesianEquationData calcCubicNodeThroughPoints (
    const std::vector<Coordinate>& points );

double calcCubicYvalue ( double x, double ymin, double ymax,
                         int root, CubicCartesianEquationData data,
                         bool& valid, int& numroots );

const Coordinate calcCubicLineIntersect( const CubicCartesianEquationData& c,
                                         const LineData& l,
                                         int root, bool& valid );

void calcCubicLineRestriction ( CubicCartesianEquationData data,
         Coordinate p1, Coordinate dir,
         double& a, double& b, double& c, double& d );

/**
 * This is the abstract base class for cubics.
 * (CubicB9P, CubicNodeB6P)
 */
class Cubic
  : public Curve
{
  typedef Curve Parent;
 public:
  Cubic();
  Cubic( const Cubic& c );
  ~Cubic();

  virtual bool isa( int type ) const;

  // type identification
  virtual const QCString vBaseTypeName() const { return sBaseTypeName();};
  static const QCString sBaseTypeName();

  virtual bool contains (const Coordinate& o, const ScreenInfo& si ) const;
  virtual void draw (KigPainter& p, bool showSelection) const;
  virtual bool inRect (const Rect&) const;
  virtual Coordinate getPoint (double param) const;
  virtual double getParam (const Coordinate&) const;
  const CubicCartesianEquationData cartesianEquationData() const;
protected:
  CubicCartesianEquationData cequation;
};

class CubicB9P
  : public Cubic
{
public:
  CubicB9P( const Objects& os );
  ~CubicB9P() {};
  CubicB9P(const CubicB9P& c);

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "CubicB9P"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "cubicb9p"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // passing arguments
  static Object::WantArgsResult sWantArgs( const Objects& os );
  static QString sUseText( const Objects& os, const Object* );
  static void sDrawPrelim( KigPainter& p, const Objects& os );

  Objects getParents() const;

protected:
  Point* pts[9];

  void calc();
};

/*
 * cubic with a cusp (with horizontal tangent) at the origin and 
 * constrained through other 4 points
 */

class CubicCuspB4P
  : public Cubic
{
public:
  CubicCuspB4P( const Objects& os );
  ~CubicCuspB4P() {};
  CubicCuspB4P(const CubicCuspB4P& c);

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "CubicCuspB4P"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "cubiccuspb4p"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // passing arguments
  static Object::WantArgsResult sWantArgs( const Objects& os );
  static QString sUseText( const Objects& os, const Object* );
  static void sDrawPrelim( KigPainter& p, const Objects& os );

  Objects getParents() const;

protected:
  Point* pts[4];

  void calc();
};

/*
 * cubic with a nodal point at the origin and constrained through other
 * 6 points
 */

class CubicNodeB6P
  : public Cubic
{
public:
  CubicNodeB6P( const Objects& os );
  ~CubicNodeB6P() {};
  CubicNodeB6P(const CubicNodeB6P& c);

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "CubicNodeB6P"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "cubicnodeb6p"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // passing arguments
  static Object::WantArgsResult sWantArgs( const Objects& os );
  static QString sUseText( const Objects& os, const Object* );
  static void sDrawPrelim( KigPainter& p, const Objects& os );

  Objects getParents() const;

protected:
  Point* pts[6];

  void calc();
};

class MultiConstructibleType;

/*
 * intersection of a line and a cubic
 */

class CubicLineIntersectionPoint
  : public Point
{
  Cubic* mcubic;
  AbstractLine* mline;
  int mroot;            // which of the three intersections
public:
  CubicLineIntersectionPoint( const Objects& os );
  CubicLineIntersectionPoint( const CubicLineIntersectionPoint& p );
  ~CubicLineIntersectionPoint();

  const QCString vFullTypeName() const;
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

  static void sDrawPrelim ( KigPainter& p, const Objects& os );
  static Object::WantArgsResult sWantArgs ( const Objects& os );
  static QString sUseText( const Objects& os, const Object* o );
  Objects getParents() const;

  void calc();

  virtual prop_map getParams ();
  virtual void setParams ( const Object::prop_map& );

  static Objects sMultiBuild( const Objects& args );

  static KigMode* sConstructMode( MultiConstructibleType* ourtype,
                                  KigDocument* theDoc,
                                  NormalMode* previousMode );
};

#endif // KIG_OBJECTS_CUBIC_H
