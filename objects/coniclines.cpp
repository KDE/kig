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

#include "coniclines.h"
#include "conic.h"

#include "../misc/i18n.h"
#include "../misc/kigpainter.h"

#include <qpen.h>

LineDirectrix::~LineDirectrix()
{
}

void LineDirectrix::calc()
{
  const ConicPolarEquationData data=conic->polarEquationData();

  assert ( conic );
  mvalid = true;
  double ec = data.ecostheta0;
  double es = data.esintheta0;
  double eccsq = ec*ec + es*es;

  mpa = data.focus1 - data.pdimen/eccsq*Coordinate(ec,es);
  mpb = mpa + Coordinate(-es,ec);
}

Objects LineDirectrix::getParents() const
{
  Objects objs;
  objs.push_back( conic );
  return objs;
}

LineDirectrix::LineDirectrix(const LineDirectrix& l)
  : Line( l ), conic( l.conic )
{
  conic->addChild(this);
}

const QString LineDirectrix::sDescriptiveName()
{
  return i18n("Directrix of a Conic");
}

QString LineDirectrix::sDescription()
{
  return i18n( "The directrix line of a conic." );
}

const char* LineDirectrix::sActionName()
{
  return "objects_new_linedirectrix";
}

LineDirectrix::LineDirectrix( const Objects& os )
  : conic( 0 )
{
  assert( os.size() == 1 );
  conic = os[0]->toConic();
  assert( conic );
  conic->addChild( this );
}

void LineDirectrix::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 1 ) return;
  assert( os[0]->toConic() );
  const ConicPolarEquationData data=os[0]->toConic()->polarEquationData();

  double ec = data.ecostheta0;
  double es = data.esintheta0;
  double eccsq = ec*ec + es*es;

  Coordinate a = data.focus1 - data.pdimen/eccsq*Coordinate(ec,es);
  Coordinate b = a + Coordinate(-es,ec);
  p.setPen (QPen (Qt::red,1));
  p.drawLine( a, b );
}

Object::WantArgsResult LineDirectrix::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 ) return NotGood;
  if ( os[0]->toConic() ) return Complete;
  return NotGood;
}

QString LineDirectrix::sUseText( const Objects&, const Object* o )
{
  if ( o->toConic() ) return i18n( "Directrix line of this conic" );
  else assert( false );
}
