// centerofcurvature_type.cc
// Copyright (C)  2004  Maurizio Paolini <paolini@dmf.unicatt.it>

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

static const char constructcenterofcurvaturepoint[] =
  I18N_NOOP( "Construct the center of curvature corresponding to this point" );

static const ArgsParser::spec argsspecCocConic[] =
{
  { ConicImp::stype(), I18N_NOOP( "Construct the center of curvature wrt. this conic" ),
    I18N_NOOP( "Select the conic..." ), false },
  { PointImp::stype(), constructcenterofcurvaturepoint,
    I18N_NOOP( "Select a point on the conic..." ), false }
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
  { CubicImp::stype(), I18N_NOOP( "Construct the center of curvature wrt. this cubic" ),
    I18N_NOOP( "Select the cubic..." ), false },
  { PointImp::stype(), constructcenterofcurvaturepoint,
    I18N_NOOP( "Select a point on the cubic..." ), false }
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
