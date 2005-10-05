// Copyright (C)  2004  Pino Toscano <toscano.pino@tiscali.it>

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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "tangent_type.h"

#include "bogus_imp.h"
#include "conic_imp.h"
#include "cubic_imp.h"
#include "curve_imp.h"
#include "other_imp.h"
#include "point_imp.h"
#include "line_imp.h"

#include "../misc/common.h"
#include "../misc/conic-common.h"
//#include "../misc/calcpaths.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"

static const char constructlinetangentpoint[] = "SHOULDNOTBESEEN";
static const char selecttangent1[] = 
  I18N_NOOP( "Select the curve..." );
static const char selecttangent2[] = 
  I18N_NOOP( "Select the point for the tangent to go through..." );

static const ArgsParser::spec argsspecTangentConic[] =
{
  { ConicImp::stype(), "SHOULDNOTBESEEN", selecttangent1, false },
  { PointImp::stype(), constructlinetangentpoint, selecttangent2, true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( TangentConicType )

TangentConicType::TangentConicType()
  : ArgsParserObjectType( "TangentConic", argsspecTangentConic, 2 )
{
}

TangentConicType::~TangentConicType()
{
}

const TangentConicType* TangentConicType::instance()
{
  static const TangentConicType t;
  return &t;
}

ObjectImp* TangentConicType::calc( const Args& args, const KigDocument& doc ) const
{
  if ( !margsparser.checkArgs( args ) )
    return new InvalidImp;

  const ConicImp* c = static_cast<const ConicImp*>( args[0] );
  const Coordinate& p = static_cast<const PointImp*>( args[1] )->coordinate();

  if ( !c->containsPoint( p, doc ) )
    return new InvalidImp;

  bool ok;
  const LineData tangent = calcConicPolarLine( c->cartesianData(), p, ok );
  
  if ( !ok )
    return new InvalidImp;
  
  return new LineImp( tangent );
}

const ObjectImpType* TangentConicType::resultId() const
{
  return LineImp::stype();
}

/*** Arc starts here ***/

static const ArgsParser::spec argsspecTangentArc[] =
{
  { ArcImp::stype(), "SHOULDNOTBESEEN", selecttangent1, false },
  { PointImp::stype(), constructlinetangentpoint, selecttangent2, true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( TangentArcType )

TangentArcType::TangentArcType()
  : ArgsParserObjectType( "TangentArc", argsspecTangentArc, 2 )
{
}

TangentArcType::~TangentArcType()
{
}

const TangentArcType* TangentArcType::instance()
{
  static const TangentArcType t;
  return &t;
}

ObjectImp* TangentArcType::calc( const Args& args, const KigDocument& doc ) const
{
  if ( !margsparser.checkArgs( args ) )
    return new InvalidImp;

  const ArcImp* arc = static_cast<const ArcImp*>( args[0] );
  const Coordinate& p = static_cast<const PointImp*>( args[1] )->coordinate();

  if ( !arc->containsPoint( p, doc ) )
    return new InvalidImp;

  Coordinate c = arc->center();
  double sqr = arc->radius();
  sqr *= sqr;
  ConicCartesianData data( 1.0, 1.0, 0.0, -2*c.x, -2*c.y, c.x*c.x + c.y*c.y - sqr );

  bool ok;
  const LineData tangent = calcConicPolarLine( data, p, ok );
  
  if ( !ok )
    return new InvalidImp;

  return new LineImp( tangent );
}

const ObjectImpType* TangentArcType::resultId() const
{
  return LineImp::stype();
}

/**** Cubic starts here ****/

static const ArgsParser::spec argsspecTangentCubic[] =
{
  { CubicImp::stype(), "SHOULDNOTBESEEN", selecttangent1, false },
  { PointImp::stype(), constructlinetangentpoint, selecttangent2, true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( TangentCubicType )

TangentCubicType::TangentCubicType()
  : ArgsParserObjectType( "TangentCubic", argsspecTangentCubic, 2 )
{
}

TangentCubicType::~TangentCubicType()
{
}

const TangentCubicType* TangentCubicType::instance()
{
  static const TangentCubicType t;
  return &t;
}

ObjectImp* TangentCubicType::calc( const Args& args, const KigDocument& doc ) const
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

  /* mp: the tangent vector (-gy,gx) is orthogonal to the gradient (gx,gy)
   * which is easy to compute from the CartesianData
   *
   * note: same thing could be done for conics, which would be
   * much more efficient...
   */
     
  Coordinate tangvec = Coordinate (
      - axxy*x*x - 2*axyy*x*y - 3*ayyy*y*y - axy*x - 2*ayy*y - ay,
      3*axxx*x*x + 2*axxy*x*y + axyy*y*y + 2*axx*x + axy*y + ax
                                   );
  const LineData tangent = LineData( p, p + tangvec );
 
  return new LineImp( tangent );
}

const ObjectImpType* TangentCubicType::resultId() const
{
  return LineImp::stype();
}

/**** Curve (locus) starts here ****/

static const ArgsParser::spec argsspecTangentCurve[] =
{
  { CurveImp::stype(), "SHOULDNOTBESEEN", selecttangent1, false },
  { PointImp::stype(), constructlinetangentpoint, selecttangent2, true }
};

KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( TangentCurveType )

TangentCurveType::TangentCurveType()
  : ArgsParserObjectType( "TangentCurve", argsspecTangentCurve, 2 )
{
}

TangentCurveType::~TangentCurveType()
{
}

const TangentCurveType* TangentCurveType::instance()
{
  static const TangentCurveType t;
  return &t;
}

ObjectImp* TangentCurveType::calc( const Args& args, const KigDocument& doc ) const
{
  if ( !margsparser.checkArgs( args ) )
    return new InvalidImp;

  const CurveImp* curve = static_cast<const CurveImp*>( args[0] );
  const Coordinate& p = static_cast<const PointImp*>( args[1] )->coordinate();
  if ( !curve->containsPoint( p, doc ) )
    return new InvalidImp;

  const double t = curve->getParam( p, doc );
  const double tau0 = 1e-3;
  const double sigma = 1e-5;
  const int maxiter = 20;

  double tau = tau0;
  Coordinate tang, err;
  double tplus = t + tau;
  double tminus = t - tau;
  if ( tplus > 1 ) {tplus = 1; tminus = 1 - 2*tau;}
  if ( tminus < 0 ) {tminus = 0; tplus = 2*tau;}
  Coordinate tangold = (curve->getPoint( tplus, doc ) - curve->getPoint( tminus, doc ))/(2*tau);

  for (int i = 0; i < maxiter; i++)
  {
    tau = tau/2;
    tplus = t + tau;
    tminus = t - tau;
    if ( tplus > 1 ) {tplus = 1; tminus = 1 - 2*tau;}
    if ( tminus < 0 ) {tminus = 0; tplus = 2*tau;}
    tang = (curve->getPoint( tplus, doc ) - curve->getPoint( tminus, doc ))/(2*tau);
    err = (tangold - tang)/3;
    if (err.length() < sigma)
    {
      tang = (4*tang - tangold)/3;
      const LineData tangent = LineData( p, p + tang );
      return new LineImp( tangent );
    }
    tangold = tang;
  }
  return new InvalidImp;
}

const ObjectImpType* TangentCurveType::resultId() const
{
  return LineImp::stype();
}
