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

#include "conic.h"
#include "radicallines.h"
#include "../kig/kig_view.h"
#include "../modes/constructing.h"
#include "../modes/popup.h"
#include "../misc/kigpainter.h"
#include "../misc/i18n.h"
#include "../misc/common.h"

#include <kdebug.h>
#include <qpen.h>

#include <cmath>

Objects LineConicRadical::getParents() const
{
  Objects ret;
  ret.push_back( c1 );
  ret.push_back( c2 );
  return ret;
}

LineConicRadical::LineConicRadical(const LineConicRadical& l)
  : Line( l ), c1( l.c1 ), c2( l.c2 ), mwhich( l.mwhich ), mzeroindex( l.mzeroindex )
{
  assert( mwhich == -1 || mwhich == 1 );
  assert( mzeroindex > 0 && mzeroindex < 4 );
  assert( c1 && c2 );
  c1->addChild( this );
  c2->addChild( this );
}

const QString LineConicRadical::sDescriptiveName()
{
  return i18n("Radical Line for Conics");
}

const QString LineConicRadical::sDescription()
{
  return i18n( "The lines constructed through the intersections of two conics.  This is also defined for non-intersecting conics." );
}

const char* LineConicRadical::sActionName()
{
  return "objects_new_lineconicradical";
}

LineConicRadical::LineConicRadical( const Objects& os )
{
  assert( os.size() == 2 );
  c1 = os[0]->toConic();
  c2 = os[1]->toConic();
  assert( c1 && c2 );
  c1->addChild( this );
  c2->addChild( this );
}

Object::prop_map LineConicRadical::getParams()
{
  prop_map map = Object::getParams();
  map["lineconicradical-branch"] = QString::number( mwhich );
  map["lineconicradical-zero"] = QString::number( mzeroindex );
  return map;
}

void LineConicRadical::setParams( const prop_map& map )
{
  Object::setParams( map );
  prop_map::const_iterator p = map.find("lineconicradical-branch");
  bool ok = true;
  if( p == map.end() ) mwhich = 1;   // fixme...
  else
    mwhich = p->second.toInt( &ok );
  assert( ok && ( mwhich == 1 || mwhich == -1 ) );

  p = map.find("lineconicradical-zero");
  if( p == map.end() ) mzeroindex = 1;   // fixme...
  else
  {
    mzeroindex = p->second.toInt( &ok );
    assert( ok && mzeroindex > 0 && mzeroindex < 4 );
  };
}

KigMode*
LineConicRadical::sConstructMode( MultiConstructibleType* ourtype,
                                  KigDocument& theDoc )
{
  return new MultiConstructionMode( ourtype, theDoc );
}

Objects LineConicRadical::sMultiBuild( const Objects& args )
{
  LineConicRadical* a;
  LineConicRadical* b;
  a = new LineConicRadical( args );
  b = new LineConicRadical( args );

  a->mwhich = 1;
  a->mzeroindex = 1;
  b->mwhich = -1;
  b->mzeroindex = 1;

  Objects o;
  o.push_back( a );
  o.push_back( b );
  return o;
}

void LineConicRadical::calc()
{
  assert( mwhich == -1 || mwhich == 1 );

  ConicCartesianEquationData cequation1=c1->cartesianEquationData();
  ConicCartesianEquationData cequation2=c2->cartesianEquationData();

  LineData coords = calcConicRadical( cequation1, cequation2, mwhich,
                                                   mzeroindex, mvalid );
  mpa = coords.a;
  mpb = coords.b;
};

void LineConicRadical::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  Conic* c;
  Conic* d;
  c = os[0]->toConic();
  d = os[1]->toConic();
  assert( c && d );
  ConicCartesianEquationData cda = c->cartesianEquationData();
  ConicCartesianEquationData cdb = d->cartesianEquationData();
  bool valid = true;
  const LineData csa = calcConicRadical( cda, cdb, -1, 1, valid );
  const LineData csb = calcConicRadical( cda, cdb, 1, 1, valid );
  if ( ! valid ) return;
  p.setPen( QPen (Qt::red,1) );
  p.drawLine( csa );
  p.drawLine( csb );
  return;
}

Object::WantArgsResult LineConicRadical::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  // we don't want the same conic twice...
  Conic* c = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! (*i)->toConic() ) return NotGood;
    else if( c == (*i)->toConic() ) return NotGood;
    else c = (*i)->toConic();
  };
  return size == 2 ? Complete : NotComplete;
}

QString LineConicRadical::sUseText( const Objects&, const Object* )
{
  return i18n("Radical Line of this conic");
}

ConicLineIntersectionPoint::ConicLineIntersectionPoint( const Objects& os )
    : Point(), mconic( 0 ), mline( 0 ), mwhich( 0 )
{
  assert( os.size() == 2 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! mconic ) mconic = (*i)->toConic();
    if ( ! mline ) mline = (*i)->toAbstractLine();
  };
  assert( mconic && mline );
  mconic->addChild( this );
  mline->addChild( this );
}

ConicLineIntersectionPoint::ConicLineIntersectionPoint( const ConicLineIntersectionPoint& p )
  : Point( p ), mconic( p.mconic ), mline( p.mline ), mwhich( p.mwhich )
{
  assert( mconic && mline );
  mconic->addChild( this );
  mline->addChild( this );
}

ConicLineIntersectionPoint::~ConicLineIntersectionPoint()
{
}

const QCString ConicLineIntersectionPoint::vFullTypeName() const
{
  return sFullTypeName();
}

const QCString ConicLineIntersectionPoint::sFullTypeName()
{
  return "ConicLineIntersectionPoint";
}

const QString ConicLineIntersectionPoint::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString ConicLineIntersectionPoint::sDescriptiveName()
{
  return i18n("Intersection of a Line and a Conic");
}

const QString ConicLineIntersectionPoint::vDescription() const
{
  return sDescription();
}

const QCString ConicLineIntersectionPoint::vIconFileName() const
{
  return sIconFileName();
}

const QCString ConicLineIntersectionPoint::sIconFileName()
{
  return "coniclineintersection";
}

const int ConicLineIntersectionPoint::vShortCut() const
{
  return sShortCut();
}

const int ConicLineIntersectionPoint::sShortCut()
{
  return 0;
}

const char* ConicLineIntersectionPoint::sActionName()
{
  return "objects_new_coniclineintersect";
}

void ConicLineIntersectionPoint::sDrawPrelim( KigPainter& p, const Objects& os )
{
  assert( os.size() <= 2 );
  if ( os.size() != 2 ) return;
  Conic* c = 0;
  AbstractLine* l = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! c ) c = (*i)->toConic();
    if ( ! l ) l = (*i)->toAbstractLine();
  };
  assert( c && l );

  ConicCartesianEquationData ccd = c->cartesianEquationData();
  const LineData ld = l->lineData();

  bool valid = true;

  Coordinate cc = calcConicLineIntersect( ccd.coeffs, ld, -1, valid );
  Coordinate cd = calcConicLineIntersect( ccd.coeffs, ld, 1, valid );

  if ( valid )
  {
    sDrawPrelimPoint( p, cc );
    sDrawPrelimPoint( p, cd );
  };

  return;
}

Object::WantArgsResult ConicLineIntersectionPoint::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  bool gotc = false;
  bool gotl = false;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! gotc ) gotc = (*i)->toConic();
    if ( ! gotl ) gotl = (*i)->toAbstractLine();
  };
  if ( size == 1 ) return ( gotc || gotl ) ? NotComplete : NotGood;
  if ( size == 2 ) return ( gotc && gotl ) ? Complete : NotGood;
  assert( false );
}

QString ConicLineIntersectionPoint::sUseText( const Objects&, const Object* o )
{
  if ( o->toConic() ) return i18n( "Intersection of a line and this conic" );
  if ( o->toLine() ) return i18n( "Intersection of a conic and this line" );
  if ( o->toSegment() ) return i18n( "Intersection of a conic and this segment" );
  if ( o->toRay() ) return i18n( "Intersection of a conic and this ray" );
  assert( false );
}

Objects ConicLineIntersectionPoint::getParents() const
{
  Objects o;
  o.push_back( mconic );
  o.push_back( mline );
  return o;
}

void ConicLineIntersectionPoint::calc()
{
  ConicCartesianEquationData cequation(0,0,0,0,0,0);
  assert( mconic && mline );
  Coordinate t;
  cequation = mconic->cartesianEquationData();
  t = calcConicLineIntersect( cequation.coeffs,
                              mline->lineData(),
                              mwhich, mvalid );
  if ( mvalid ) mC = t;
}

Object::prop_map ConicLineIntersectionPoint::getParams()
{
  prop_map map = Point::getParams();
  map["coniclineintersect-side"] = QString::number( mwhich );
  return map;
}

void ConicLineIntersectionPoint::setParams( const prop_map& map )
{
  Point::setParams( map );
  prop_map::const_iterator p = map.find("coniclineintersect-side");
  if( p == map.end() ) mwhich = 1; // this is an error in the kig
                                   // file, but we ignore it..
  else
  {
    bool ok = true;
    mwhich = p->second.toInt( &ok );
    assert( ok && ( mwhich == 1 || mwhich == -1 ) );
  };
}

Objects ConicLineIntersectionPoint::sMultiBuild( const Objects& args )
{
  ConicLineIntersectionPoint* a;
  ConicLineIntersectionPoint* b;
  a = new ConicLineIntersectionPoint( args );
  b = new ConicLineIntersectionPoint( args );
  a->mwhich = 1;
  b->mwhich = -1;
  Objects o;
  o.push_back( a );
  o.push_back( b );
  return o;
}

KigMode*
ConicLineIntersectionPoint::sConstructMode( MultiConstructibleType* ourtype,
                                             KigDocument& theDoc )
{
  return new MultiConstructionMode( ourtype, theDoc );
}

const QString ConicLineIntersectionPoint::sDescription()
{
  return i18n("Construct the intersection of a line and a conic.");
}

void LineConicRadical::addActions( NormalModePopupObjects& p )
{
  p.addNormalAction( 58731, i18n( "Switch" ) );
}

void LineConicRadical::doNormalAction( int which, KigDocument* d, KigWidget* v, NormalMode* m, const Coordinate& cp )
{
  if ( which != 58731 ) Line::doNormalAction( which, d, v, m, cp );
  else
  {
    mzeroindex = mzeroindex % 3 + 1;
    calcForWidget( *v );
    v->redrawScreen();
  };
}
