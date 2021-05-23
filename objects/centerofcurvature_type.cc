// SPDX-FileCopyrightText: 2004 Maurizio Paolini <paolini@dmf.unicatt.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "centerofcurvature_type.h"

#include "bogus_imp.h"
#include "conic_imp.h"
#include "cubic_imp.h"
//#include "other_imp.h"
#include "point_imp.h"
//#include "line_imp.h"

#include "../misc/common.h"
#include "../misc/conic-common.h"
//#include "../misc/calcpaths.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"

static const char constructcenterofcurvaturepoint[] = "SHOULDNOTBESEEN";
//  I18N_NOOP( "Construct the center of curvature corresponding to this point" );
static const char selectcoc1[] = I18N_NOOP( "Select the curve..." );
static const char selectcoc2[] = I18N_NOOP( "Select a point on the curve..." );

static const ArgsParser::spec argsspecCocConic[] =
{
  { ConicImp::stype(), "SHOULDNOTBESEEN", selectcoc1, false },
  { PointImp::stype(), constructcenterofcurvaturepoint, selectcoc2, false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CocConicType )

CocConicType::CocConicType()
  : ArgsParserObjectType( "CocConic", argsspecCocConic, 2 )
{
}

CocConicType::~CocConicType()
{
}

const CocConicType* CocConicType::instance()
{
  static const CocConicType t;
  return &t;
}

ObjectImp* CocConicType::calc( const Args& args, const KigDocument& doc ) const
{
  if ( !margsparser.checkArgs( args ) )
    return new InvalidImp;

  const ConicImp* conic = static_cast<const ConicImp*>( args[0] );
  const Coordinate& p = static_cast<const PointImp*>( args[1] )->coordinate();

  if ( !conic->containsPoint( p, doc ) )
    return new InvalidImp;

  double x = p.x;
  double y = p.y;
  ConicCartesianData data = conic->cartesianData();
//  double aconst = data.coeffs[5];
  double ax = data.coeffs[3];
  double ay = data.coeffs[4];
  double axx = data.coeffs[0];
  double axy = data.coeffs[2];
  double ayy = data.coeffs[1];

/*
 * mp: we need to compute the normal vector and the curvature
 * of the curve.  The curve (conic) is given in implicit form
 * f(x,y) = 0;  the gradient of f gives the direction of the
 * normal; for the curvature we can use the following formula:
 * k = div(grad f/|grad f|)
 *
 * the hessian matrix has elements [hfxx, hfxy]
 *                                 [hfxy, hfyy]
 *
 * kgf is the curvature multiplied by the norm of grad f
 */

  double gradfx = 2*axx*x + axy*y + ax;
  double gradfy = axy*x + 2*ayy*y + ay;
  Coordinate gradf = Coordinate ( gradfx, gradfy );

  double hfxx = 2*axx;
  double hfyy = 2*ayy;
  double hfxy = axy;

  double kgf = hfxx + hfyy 
     - (hfxx*gradfx*gradfx + hfyy*gradfy*gradfy + 2*hfxy*gradfx*gradfy)
       /(gradfx*gradfx + gradfy*gradfy);

  bool ok = true;

  const Coordinate coc = p - 1/kgf*gradf;
  
  if ( !ok )
    return new InvalidImp;
  
  return new PointImp( coc );
}

const ObjectImpType* CocConicType::resultId() const
{
  return PointImp::stype();
}

/**** Cubic starts here ****/

static const ArgsParser::spec argsspecCocCubic[] =
{
  { CubicImp::stype(), "SHOULDNOTBESEEN", selectcoc1, false },
  { PointImp::stype(), constructcenterofcurvaturepoint, selectcoc2, false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CocCubicType )

CocCubicType::CocCubicType()
  : ArgsParserObjectType( "CocCubic", argsspecCocCubic, 2 )
{
}

CocCubicType::~CocCubicType()
{
}

const CocCubicType* CocCubicType::instance()
{
  static const CocCubicType t;
  return &t;
}

ObjectImp* CocCubicType::calc( const Args& args, const KigDocument& doc ) const
{
  if ( !margsparser.checkArgs( args ) )
    return new InvalidImp;

  const CubicImp* cubic = static_cast<const CubicImp*>( args[0] );
  const Coordinate& p = static_cast<const PointImp*>( args[1] )->coordinate();

  if ( !cubic->containsPoint( p, doc ) )
    return new InvalidImp;

  double x = p.x;
  double y = p.y;
  CubicCartesianData data = cubic->data();
//  double aconst = data.coeffs[0];
  double ax = data.coeffs[1];
  double ay = data.coeffs[2];
  double axx = data.coeffs[3];
  double axy = data.coeffs[4];
  double ayy = data.coeffs[5];
  double axxx = data.coeffs[6];
  double axxy = data.coeffs[7];
  double axyy = data.coeffs[8];
  double ayyy = data.coeffs[9];

  /*
   * we use here the same mechanism as for the
   * conics, see above
   */

  double gradfx = 3*axxx*x*x + 2*axxy*x*y + axyy*y*y + 2*axx*x + axy*y + ax;
  double gradfy = axxy*x*x + 2*axyy*x*y + 3*ayyy*y*y + axy*x + 2*ayy*y + ay;
  Coordinate gradf = Coordinate ( gradfx, gradfy );

  double hfxx = 6*axxx*x + 2*axxy*y + 2*axx;
  double hfyy = 6*ayyy*y + 2*axyy*x + 2*ayy;
  double hfxy = 2*axxy*x + 2*axyy*y + axy;

  double kgf = hfxx + hfyy 
     - (hfxx*gradfx*gradfx + hfyy*gradfy*gradfy + 2*hfxy*gradfx*gradfy)
       /(gradfx*gradfx + gradfy*gradfy);

  bool ok = true;

  const Coordinate coc = p - 1/kgf*gradf;

  if ( !ok )
    return new InvalidImp;

  return new PointImp( coc );  
}

const ObjectImpType* CocCubicType::resultId() const
{
  return PointImp::stype();
}

/**** Curve starts here ****/

static const ArgsParser::spec argsspecCocCurve[] =
{
  { CurveImp::stype(), "SHOULDNOTBESEEN", selectcoc1, false },
  { PointImp::stype(), constructcenterofcurvaturepoint, selectcoc2, false }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( CocCurveType )

CocCurveType::CocCurveType()
  : ArgsParserObjectType( "CocCurve", argsspecCocCurve, 2 )
{
}

CocCurveType::~CocCurveType()
{
}

const CocCurveType* CocCurveType::instance()
{
  static const CocCurveType t;
  return &t;
}

ObjectImp* CocCurveType::calc( const Args& args, const KigDocument& doc ) const
{
  if ( !margsparser.checkArgs( args ) )
    return new InvalidImp;

  const CurveImp* curve = static_cast<const CurveImp*>( args[0] );
  const Coordinate& p = static_cast<const PointImp*>( args[1] )->coordinate();

  if ( !curve->containsPoint( p, doc ) )
    return new InvalidImp;


  const double t = curve->getParam( p, doc );
  const double tau0 = 5e-4;
  const double sigmasq = 1e-12;
  const int maxiter = 20;

  double tau = tau0;
  Coordinate gminus, g, gplus, tang, acc, curv, err;
  double velsq, curvsq;
  double tplus = t + tau;
  double tminus = t - tau;
  double t0 = t;
  if ( tplus > 1 ) {tplus = 1; t0 = 1 - tau; tminus = 1 - 2*tau;}
  if ( tminus < 0 ) {tminus = 0; t0 = tau; tplus = 2*tau;}
  gminus = curve->getPoint( tminus, doc );
  g = curve->getPoint( t0, doc );
  gplus = curve->getPoint( tplus, doc );
  tang = (gplus - gminus)/(2*tau);
  acc = (gminus + gplus - 2*g)/(tau*tau);
  velsq = tang.x*tang.x + tang.y*tang.y;
  tang = tang/velsq;
  Coordinate curvold = acc/velsq - (acc.x*tang.x + acc.y*tang.y)*tang;
  curvsq = curvold.x*curvold.x + curvold.y*curvold.y;
  curvold = curvold/curvsq;

  for (int i = 0; i < maxiter; i++)
  {
    tau = tau/2;
    tplus = t + tau;
    tminus = t - tau;
    t0 = t;
    if ( tplus > 1 ) {tplus = 1; t0 = 1 - tau; tminus = 1 - 2*tau;}
    if ( tminus < 0 ) {tminus = 0; t0 = tau; tplus = 2*tau;}

    gminus = curve->getPoint( tminus, doc );
    g = curve->getPoint( t0, doc );
    gplus = curve->getPoint( tplus, doc );
    tang = (gplus - gminus)/(2*tau);
    acc = (gminus + gplus - 2*g)/(tau*tau);
    velsq = tang.x*tang.x + tang.y*tang.y;
    tang = tang/velsq;
    curv = acc/velsq - (acc.x*tang.x + acc.y*tang.y)*tang;
    curvsq = curv.x*curv.x + curv.y*curv.y;
    curv = curv/curvsq;

    err = (curvold - curv)/3;
    /*
     * curvsq is the inverse squared of the norm of curvsq
     * so this is actually a relative test
     * in the end we return an extrapolated value
     */
    if (err.squareLength() < sigmasq/curvsq)
    {
      curv = (4*curv - curvold)/3;
      return new PointImp( p + curv );
    }
    curvold = curv;
  }
  return new InvalidImp;
}

const ObjectImpType* CocCurveType::resultId() const
{
  return PointImp::stype();
}
