// conic_imp.cc
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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

#include "conic_imp.h"

#include "bogus_imp.h"
#include "point_imp.h"

#include "../misc/kigpainter.h"
#include "../misc/i18n.h"
#include "../misc/coordinate_system.h"

#include "../kig/kig_part.h"

ObjectImp* ConicImp::transform( const Transformation& t ) const
{
  bool valid = true;
  ConicCartesianData d = calcConicTransformation( cartesianData(), t, valid );
  if ( ! valid ) return new InvalidImp;
  else return new ConicImpCart( d );
}

void ConicImp::draw( KigPainter& p ) const
{
  p.drawConic( polarData() );
}

bool ConicImp::valid() const
{
  return true;
}

bool ConicImp::contains( const Coordinate& o, int width, const ScreenInfo& si ) const
{
  const ConicPolarData d = polarData();

  Coordinate focus1 = d.focus1;
  double ecostheta0 = d.ecostheta0;
  double esintheta0 = d.esintheta0;
  double pdimen = d.pdimen;

  Coordinate pos = o - focus1;
  double len = pos.length();
  double costheta = pos.x / len;
  double sintheta = pos.y / len;

  double ecosthetamtheta0 = costheta*ecostheta0 + sintheta*esintheta0;
  double rho = pdimen / (1.0 - ecosthetamtheta0);

  if ( fabs(len - rho) <= si.normalMiss( width ) ) return true;
  rho = - pdimen / ( 1.0 + ecosthetamtheta0 );
  return fabs( len - rho ) <= si.normalMiss( width );
}

bool ConicImp::inRect( const Rect&, int, const ScreenInfo& ) const
{
  // TODO
  return false;
}

const uint ConicImp::numberOfProperties() const
{
  return CurveImp::numberOfProperties() + 5;
}

const QCStringList ConicImp::propertiesInternalNames() const
{
  QCStringList l = CurveImp::propertiesInternalNames();
  l << I18N_NOOP( "type" );
  l << I18N_NOOP( "first-focus" );
  l << I18N_NOOP( "second-focus" );
  l << I18N_NOOP( "cartesian-equation" );
  l << I18N_NOOP( "polar-equation" );
  assert( l.size() == ConicImp::numberOfProperties() );
  return l;
}

const QCStringList ConicImp::properties() const
{
  QCStringList l = CurveImp::properties();
  l << I18N_NOOP( "Type" );
  l << I18N_NOOP( "First Focus" );
  l << I18N_NOOP( "Second Focus" );
  l << I18N_NOOP( "Cartesian Equation" );
  l << I18N_NOOP( "Polar Equation" );
  assert( l.size() == ConicImp::numberOfProperties() );
  return l;
}

ObjectImp* ConicImp::property( uint which, const KigDocument& w ) const
{
  int pnum = 0;

  if ( which < CurveImp::numberOfProperties() )
    return CurveImp::property( which, w );
  if ( which == CurveImp::numberOfProperties() + pnum++ )
    return new StringImp( conicTypeString() );
  else if ( which == CurveImp::numberOfProperties() + pnum++ )
    return new PointImp( focus1() );
  else if ( which == CurveImp::numberOfProperties() + pnum++ )
    return new PointImp( focus2() );
  else if ( which == CurveImp::numberOfProperties() + pnum++ )
    return new StringImp( cartesianEquationString( w ) );
  else if ( which == CurveImp::numberOfProperties() + pnum++ )
    return new StringImp( polarEquationString( w ) );
  else assert( false );
}

double ConicImp::getParam( const Coordinate& p, const KigDocument& ) const
{
  const ConicPolarData d = polarData();
  Coordinate tmp = p - d.focus1;
  double l = tmp.length();
  double theta = atan2(tmp.y, tmp.x);
  double costheta = cos(theta);
  double sintheta = sin(theta);
  double rho1 = d.pdimen / (1 - costheta * d.ecostheta0 - sintheta * d.esintheta0);
  double rho2 = - d.pdimen / (1 + costheta * d.ecostheta0 + sintheta * d.esintheta0);
  if (fabs(rho1 - l) < fabs(rho2 - l))
    return fmod(theta / ( 2 * M_PI ) + 1, 1);
  else
    return fmod(theta / ( 2 * M_PI ) + 0.5, 1);
}

const Coordinate ConicImp::getPoint( double p, const KigDocument& ) const
{
  const ConicPolarData d = polarData();

  double costheta = cos(p * 2 * M_PI);
  double sintheta = sin(p * 2 * M_PI);
  double rho = d.pdimen / (1 - costheta* d.ecostheta0 - sintheta* d.esintheta0);
  return d.focus1 + Coordinate (costheta, sintheta) * rho;
}

bool ConicImp::inherits( int typeID ) const
{
  return typeID == ID_ConicImp ? true : Parent::inherits( typeID );
}

int ConicImp::conicType() const
{
  const ConicPolarData d = polarData();
  double ec = d.ecostheta0;
  double es = d.esintheta0;
  double esquare = ec*ec + es*es;
  const double parabolamiss = 1e-3;  // don't know what a good value could be

  if (esquare < 1.0 - parabolamiss) return 1;
  if (esquare > 1.0 + parabolamiss) return -1;

  return 0;
}

QString ConicImp::conicTypeString() const
{
  switch (conicType())
  {
  case 1:
    return I18N_NOOP("Ellipse");
  case -1:
    return I18N_NOOP("Hyperbola");
  case 0:
    return I18N_NOOP("Parabola");
  default:
    assert( false );
  }
}

QString ConicImp::cartesianEquationString( const KigDocument& ) const
{
  QString ret = i18n( "%1 x^2 + %2 y^2 + %3 xy + %4 x + %5 y + %6 = 0" );
  ConicCartesianData data = cartesianData();
  ret = ret.arg( data.coeffs[0], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[1], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[2], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[3], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[4], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[5], 0, 'g', 3 );
  return ret;
}

QString ConicImp::polarEquationString( const KigDocument& w ) const
{
  QString ret = i18n( "rho = %1/(1 + %2 cos theta + %3 sin theta)\n    [centered at %4]" );
  const ConicPolarData data = polarData();

  ret = ret.arg( data.pdimen, 0, 'g', 3 );
  ret = ret.arg( -data.ecostheta0, 0, 'g', 3 );
  ret = ret.arg( -data.esintheta0, 0, 'g', 3 );

  ret = ret.arg( w.coordinateSystem().fromScreen( data.focus1, w ) );
  return ret;
}

const ConicCartesianData ConicImp::cartesianData() const
{
  return ConicCartesianData( polarData() );
}

Coordinate ConicImp::focus1() const
{
  return polarData().focus1;
}

Coordinate ConicImp::focus2() const
{
  const ConicPolarData d = polarData();
  double ec = d.ecostheta0;
  double es = d.esintheta0;

  double fact = 2*d.pdimen/(1 - ec*ec - es*es);

  return d.focus1 + fact*Coordinate(ec, es);
}

const ConicPolarData ConicImpCart::polarData() const
{
  return mpolardata;
}

const ConicCartesianData ConicImpCart::cartesianData() const
{
  return mcartdata;
}

ConicImpCart::ConicImpCart( const ConicCartesianData& data )
  : ConicImp(), mcartdata( data ), mpolardata( data )
{
}

ConicImpPolar::ConicImpPolar( const ConicPolarData& data )
  : ConicImp(), mdata( data )
{
}

ConicImpPolar::~ConicImpPolar()
{
}

const ConicPolarData ConicImpPolar::polarData() const
{
  return mdata;
}

ConicImpCart* ConicImpCart::copy() const
{
  return new ConicImpCart( mcartdata );
}

ConicImpPolar* ConicImpPolar::copy() const
{
  return new ConicImpPolar( mdata );
}

ConicImp::ConicImp()
{
}

ConicImp::~ConicImp()
{
}

ConicImpCart::~ConicImpCart()
{
}

const char* ConicImp::baseName() const
{
  return I18N_NOOP( "conic" );
}

int ConicImp::id() const
{
  return ID_ConicImp;
}

