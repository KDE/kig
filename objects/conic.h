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


#ifndef CONIC_H
#define CONIC_H

#include "curve.h"
#include "../misc/common.h"

/**
 * This class represents an equation of a conic in the form
 * "ax^2 + by^2 + cxy + dx + ey + f = 0".  The coefficients are stored
 * in the order a - f.
 */
struct ConicCartesianEquationData
{
  double coeffs[6];
public:
  ConicCartesianEquationData( double a, double b, double c,
                              double d, double e, double f )
    {
      coeffs[0] = a;
      coeffs[1] = b;
      coeffs[2] = c;
      coeffs[3] = d;
      coeffs[4] = e;
      coeffs[5] = f;
    };
  ConicCartesianEquationData( const double incoeffs[6] )
    {
      std::copy( incoeffs, incoeffs + 6, coeffs );
    };
};

/**
 * This class represents an equation of a conic in the form
 * "\rho(\theta) = \frac{p}{1 - e \cos\theta}" ( run this through
 * LaTeX to get nice output ;)  focus and the ecostheta stuff
 * represent the coordinate system in which the equation yields the
 * good result..
 */
struct ConicPolarEquationData
{
  Coordinate focus1;
  double pdimen;
  double ecostheta0;
  double esintheta0;
};

class Conic
  : public Curve
{
 public:
  Conic();
  Conic( const Conic& c );
  ~Conic();

  Conic* toConic();
  const Conic* toConic() const;

  // type identification
  virtual const QCString vBaseTypeName() const { return sBaseTypeName();};
  static const QCString sBaseTypeName();

  bool contains (const Coordinate& o, const ScreenInfo& si ) const;
  void draw (KigPainter& p, bool showSelection) const;
  bool inRect (const Rect&) const;
  Coordinate getPoint (double param) const;
  double getParam (const Coordinate&) const;

  Coordinate getFocus1() const { return mequation.focus1; };

protected:
  ConicPolarEquationData mequation;
};

/**
 * This function calcs a cartesian conic equation such that the 5
 * given points are on the conic..
 */
const ConicCartesianEquationData calcCartesian ( const Coordinate points[5] );

/**
 * This function calcs a polar equation for the conic defined by
 * cartdata..
 */
const ConicPolarEquationData calcPolar ( const ConicCartesianEquationData& cartdata );

// an ellipse by focuses and a point
class EllipseBFFP
  : public Conic
{
public:
  EllipseBFFP( const Objects& os );
  ~EllipseBFFP(){};
  EllipseBFFP(const EllipseBFFP& c);
  EllipseBFFP* copy() { return new EllipseBFFP(*this); };

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "EllipseBFFP"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "ellipsebffp"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  void calc();

  // passing arguments
  static Object::WantArgsResult sWantArgs( const Objects& os );
  static QString sUseText( const Objects&, const Object* o );
  static void sDrawPrelim( KigPainter& p, const Objects& args );

  Objects getParents() const;

  //moving is disabled..
  void startMove(const Coordinate&, const ScreenInfo& );
  void moveTo(const Coordinate&);
  void stopMove();
protected:
  Point* poc; // point on ellipse
  Point* focus1;
  Point* focus2;
};

class ConicB5P
  : public Conic
{
public:
  ConicB5P( const Objects& os );
  ~ConicB5P() {};
  ConicB5P(const ConicB5P& c);
  ConicB5P* copy() { return new ConicB5P(*this); };

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "ConicB5P"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "conicb5p"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // passing arguments
  static Object::WantArgsResult sWantArgs( const Objects& os );
  static QString sUseText( const Objects& os, const Object* );
  static void sDrawPrelim( KigPainter& p, const Objects& os );

  Objects getParents() const;

  //moving
  void startMove(const Coordinate&, const ScreenInfo& ) {};
  void moveTo(const Coordinate&) {};
  void stopMove() {};
  void cancelMove() {};

protected:
  Point* pts[5];

  void calc();
};

#endif
