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

#include <vector>

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
 * This function calcs a cartesian conic equation such that the
 * given points are on the conic..  There can be at most 5 and at
 * least 1 point.  If there are less than 5, than the coefficients
 * will be chosen to 1.0 if possible
 */
const ConicCartesianEquationData calcCartesian ( const std::vector<Coordinate>& points );

/**
 * This function calcs a polar equation for the conic defined by
 * cartdata..
 */
const ConicPolarEquationData calcPolar ( const ConicCartesianEquationData& cartdata );

/**
 * This function is used by ConicBFFP.  It calcs the polar equation
 * for a hyperbola ( type == -1 ) or ellipse ( type == 1 ) with
 * focuses args[0] and args[1], and with args[2] on the edge of the
 * conic.  args.size() should be two or three.  If it is two, the two
 * points are taken to be the focuses, and a point is taken on the
 * perpendicular through the middle of them for the third point.
 */
const ConicPolarEquationData calcConicBFFP( const std::vector<Coordinate>& args,
                                            int type );

/**
 * This class is a common base class for EllipseBFFP and HyperbolaBFFP
 * meant to reduce code duplication.  It has protected constructors
 * and destructors cause it is not meant to be instantiated...
 * the functions that take a type argument take 1 for an Ellipse and
 * -1 for a Hyperbola...
 */
class ConicBFFP
  : public Conic
{
protected:
  ConicBFFP( const Objects& os );
  ConicBFFP( const ConicBFFP& c );
  ~ConicBFFP();

  void calcCommon( int type );

  // passing arguments
  Objects getParents() const;
  static void sDrawPrelimCommon( KigPainter& p,
                                 const Objects& args,
                                 int type );
public:
  static Object::WantArgsResult sWantArgs( const Objects& os );

protected:
  Point* poc; // point on conic
  Point* focus1;
  Point* focus2;
};

// an ellipse by focuses and a point
class EllipseBFFP
  : public ConicBFFP
{
public:
  EllipseBFFP( const Objects& os );
  EllipseBFFP( const EllipseBFFP& c );
  ~EllipseBFFP();
  EllipseBFFP* copy();

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName();
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
  static QString sUseText( const Objects&, const Object* o );
  static void sDrawPrelim( KigPainter& p, const Objects& args );

  Objects getParents() const;
};

// a hyperbola by focuses and a point
class HyperbolaBFFP
  : public ConicBFFP
{
public:
  HyperbolaBFFP( const Objects& os );
  HyperbolaBFFP( const HyperbolaBFFP& c );
  ~HyperbolaBFFP();
  HyperbolaBFFP* copy();

  void calc();

  const QCString vFullTypeName() const { return sFullTypeName(); };
  static const QCString sFullTypeName() { return "HyperbolaBFFP"; };
  const QString vDescriptiveName() const { return sDescriptiveName(); };
  static const QString sDescriptiveName();
  const QString vDescription() const { return sDescription(); };
  static const QString sDescription();
  const QCString vIconFileName() const { return sIconFileName(); };
  static const QCString sIconFileName() { return "hyperbolabffp"; };
  const int vShortCut() const { return sShortCut(); };
  static const int sShortCut() { return 0; };
  static const char* sActionName();

  // passing arguments
  static QString sUseText( const Objects&, const Object* o );
  static void sDrawPrelim( KigPainter& p, const Objects& args );
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

protected:
  Point* pts[5];

  void calc();
};

#endif
