// circle_imp.cc
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

#include "circle_imp.h"

#include "bogus_imp.h"
#include "point_imp.h"

#include "../misc/kigtransform.h"
#include "../misc/kigpainter.h"
#include "../misc/i18n.h"
#include "../misc/coordinate_system.h"

#include "../kig/kig_part.h"

#include <math.h>

CircleImp::CircleImp( const Coordinate& center, double radius )
  : mcenter( center ), mradius( radius )
{
}

CircleImp::~CircleImp()
{
}

ObjectImp* CircleImp::transform( const Transformation& t ) const
{
  if ( t.isHomothetic() )
  {
    bool valid = true;
    Coordinate nc = t.apply( mcenter, valid );
    double nr = t.apply( mradius );
    return new CircleImp( nc, nr );
  }
  else
  {
    // domi: i should return a ConicImp here, but i don't know how to
    // calculate it..
    return Parent::transform( t );
  };
}

void CircleImp::draw( KigPainter& p ) const
{
  p.drawCircle( mcenter, mradius );
}

bool CircleImp::contains( const Coordinate& p, int width, const ScreenInfo& si ) const
{
  return fabs((mcenter - p).length() - mradius) <= si.normalMiss( width );
}

bool CircleImp::inRect( const Rect& ) const
{
  // TODO..
  return false;
}

bool CircleImp::valid() const
{
  return true;
}

const uint CircleImp::numberOfProperties() const
{
  // We _intentionally_ do not use the Conic properties..
  return CurveImp::numberOfProperties() + 6;
}

const QCStringList CircleImp::properties() const
{
  QCStringList l = CurveImp::properties();
  l << I18N_NOOP( "Surface" );
  l << I18N_NOOP( "Circumference" );
  l << I18N_NOOP( "Radius" );
  l << I18N_NOOP( "Center" );
  l << I18N_NOOP( "Cartesian Equation" );
  l << I18N_NOOP( "Polar Equation" );
  assert( l.size() == CircleImp::numberOfProperties() );
  return l;
}

ObjectImp* CircleImp::property( uint which, const KigDocument& w ) const
{
  assert( which < CircleImp::numberOfProperties() );
  if ( which < CurveImp::numberOfProperties() )
    return CurveImp::property( which, w );
  if ( which == CurveImp::numberOfProperties() )
    return new DoubleImp( surface() );
  else if ( which == CurveImp::numberOfProperties() + 1 )
    return new DoubleImp( circumference() );
  else if ( which == CurveImp::numberOfProperties() + 2 )
    return new DoubleImp( radius() );
  else if ( which == CurveImp::numberOfProperties() + 3 )
    return new PointImp( center() );
  else if ( which == CurveImp::numberOfProperties() + 4 )
    return new StringImp( cartesianEquationString( w ) );
  else if ( which == CurveImp::numberOfProperties() + 5 )
    return new StringImp( polarEquationString( w ) );
  else assert( false );
}

const Coordinate CircleImp::center() const
{
  return mcenter;
}

double CircleImp::radius() const
{
  return mradius;
}

double CircleImp::surface() const
{
  return M_PI * squareRadius();
}

double CircleImp::squareRadius() const
{
  return mradius * mradius;
}

double CircleImp::circumference() const
{
  return 2 * M_PI * radius();
}

QString CircleImp::polarEquationString( const KigDocument& w ) const
{
  QString ret = i18n( "rho = %1   [centered at %2]" );
  ConicPolarData data = polarData();
  ret = ret.arg( data.pdimen, 0, 'g', 3 );
  ret = ret.arg( w.coordinateSystem().fromScreen( data.focus1, w ) );
  return ret;
}

QString CircleImp::cartesianEquationString( const KigDocument& ) const
{
  QString ret = i18n( "x^2 + y^2 + %1 x + %2 y + %3 = 0" );
  ConicCartesianData data = cartesianData();
  ret = ret.arg( data.coeffs[3], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[4], 0, 'g', 3 );
  ret = ret.arg( data.coeffs[5], 0, 'g', 3 );
  return ret;
}

bool CircleImp::inherits( int typeID ) const
{
  return typeID == ID_CircleImp ? true : Parent::inherits( typeID );
}

Coordinate CircleImp::focus1() const
{
  return center();
}

Coordinate CircleImp::focus2() const
{
  return center();
}

int CircleImp::conicType() const
{
  return 1;
}

const ConicCartesianData CircleImp::cartesianData() const
{
  Coordinate c = center();
  double sqr = squareRadius();
  ConicCartesianData data(
    1.0, 1.0, 0.0, -2*c.x, -2*c.y,
    c.x*c.x + c.y*c.y - sqr );
  return data;
}

const ConicPolarData CircleImp::polarData() const
{
  return ConicPolarData( center(), radius(), 0, 0 );
}

CircleImp* CircleImp::copy() const
{
  return new CircleImp( mcenter, mradius );
}

const char* CircleImp::baseName() const
{
  return I18N_NOOP( "circle" );
}
