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

#include "transform.h"
#include "cubic.h"

//#include "point.h"

#include "../misc/common.h"
#include "../misc/kigpainter.h"
#include "../misc/kigtransform.h"
#include "../misc/coordinate_system.h"
#include "../misc/i18n.h"
#include "../kig/kig_view.h"
#include "../kig/kig_part.h"

#include <kdebug.h>
#include <qpen.h>
#include <math.h>


/*
 * transform a line
 */

void LineTransform::calc()
{
  mvalid = true;
  double transformation[3][3];

  mvalid = getProjectiveTransformation (
     transformationsnum, mtransformations, transformation );

  if ( mvalid )
  {
    mpa = calcTransformedPoint ( mline->p1(), transformation, mvalid );
    mpb = calcTransformedPoint ( mline->p2(), transformation, mvalid );
  }
}

const char* LineTransform::sActionName()
{
  return "objects_new_linetransform";
}

Objects LineTransform::getParents() const
{
  Objects objs;
  objs.push_back( mline );
  for ( int i = 0; i < transformationsnum; i++ )
  {
    objs.push_back( mtransformations[i] );
  }
  return objs;
}

LineTransform::LineTransform(const LineTransform& c)
  : Line( c ), mline( c.mline ), transformationsnum ( c.transformationsnum )
{
  for ( int i = 0; i < c.transformationsnum; i++ )
  {
    mtransformations[i] = c.mtransformations[i];
    mtransformations[i]->addChild(this);
  }
  mline->addChild(this);
}

const QString LineTransform::sDescriptiveName()
{
  return i18n("Transform a Line");
}

const QString LineTransform::sDescription()
{
  return i18n( "Transform a line using an elementary transformation" );
}

Object::WantArgsResult LineTransform::sWantArgs( const Objects& os )
{
  uint size = os.size();
  assert ( size > 0 );
  if ( size > MAXTRANSFORMATIONS + 1 ) return NotGood;
  Objects::const_iterator i = os.begin();

  if ( ! (*i++)->toLine() ) return NotGood;
  if ( i == os.end() ) return NotComplete;
  switch ( WantTransformation (i, os) )
  {
    case tComplete: return Complete;
    case tNotComplete: return NotComplete;
    case tNotGood: return NotGood;
    default: return NotGood;
  }
}

QString LineTransform::sUseText( const Objects& os, const Object* o)
{
  if ( os.size() == 0 ) return i18n( "Transform this line" );
  return getTransformMessage ( os, o );
}

void LineTransform::sDrawPrelim( KigPainter& p, const Objects& os )
{
  Object *transformations[MAXTRANSFORMATIONS];
  bool valid;
  double transformation[3][3];

  int transformationsnum = os.size() - 1;
  assert( transformationsnum <= MAXTRANSFORMATIONS );
  if ( transformationsnum < 1 ) return;  // don't drawprelim if too few points
  Objects::const_iterator i = os.begin();

  assert ((*i)->toLine());
  Line *line = (*i++)->toLine();

  for ( int j = 0; j < transformationsnum; j++ )
  {
    transformations[j] = *i++;
  }

  valid = getProjectiveTransformation ( transformationsnum,
      transformations, transformation );

  if ( ! valid ) return;

  p.setPen(QPen (Qt::red, 1));

  Coordinate a = calcTransformedPoint ( line->p1(), transformation, valid );
  Coordinate b = calcTransformedPoint ( line->p2(), transformation, valid );
  p.drawLine( a, b );

  return;
}

LineTransform::LineTransform( const Objects& os )
  : Line()
{
  assert( os.size() <= MAXTRANSFORMATIONS + 1 );
  Objects::const_iterator i = os.begin();

  assert( (*i)->toLine() );
  mline = (*i++)->toLine();
  mline->addChild( this );

  transformationsnum = os.size() - 1;
  if ( transformationsnum == 0 ) return;

  for ( int j = 0; j < transformationsnum; j++ )
  {
    mtransformations[j] = *i++;
    mtransformations[j]->addChild( this );
  }
}

/*
 * transform a point
 */

void PointTransform::calc()
{
  mvalid = true;
  double transformation[3][3];

  mvalid = getProjectiveTransformation (
     transformationsnum, mtransformations, transformation );

  if ( mvalid )
  {
    mC = calcTransformedPoint ( mpoint->getCoord(), transformation, mvalid );
  }
}

const char* PointTransform::sActionName()
{
  return "objects_new_pointtransform";
}

Objects PointTransform::getParents() const
{
  Objects objs;
  objs.push_back( mpoint );
  for ( int i = 0; i < transformationsnum; i++ )
  {
    objs.push_back( mtransformations[i] );
  }
  return objs;
}

PointTransform::PointTransform(const PointTransform& c)
  : Point( c ), mpoint( c.mpoint ), transformationsnum ( c.transformationsnum )
{
  for ( int i = 0; i < c.transformationsnum; i++ )
  {
    mtransformations[i] = c.mtransformations[i];
    mtransformations[i]->addChild(this);
  }
  mpoint->addChild(this);
}

const QString PointTransform::sDescriptiveName()
{
  return i18n("Transform a Point");
}

const QString PointTransform::sDescription()
{
  return i18n( "Transform a point using an elementary transformation" );
}

Object::WantArgsResult PointTransform::sWantArgs( const Objects& os )
{
  uint size = os.size();
  assert ( size > 0 );
  if ( size > MAXTRANSFORMATIONS + 1 ) return NotGood;
  Objects::const_iterator i = os.begin();

  if ( ! (*i++)->toPoint() ) return NotGood;
  if ( i == os.end() ) return NotComplete;
  switch ( WantTransformation (i, os) )
  {
    case tComplete: return Complete;
    case tNotComplete: return NotComplete;
    case tNotGood: return NotGood;
    default: return NotGood;
  }
}

QString PointTransform::sUseText( const Objects& os, const Object* o)
{
  if ( os.size() == 0 ) return i18n( "Transform this point" );
  return getTransformMessage ( os, o );
}

void PointTransform::sDrawPrelim( KigPainter& p, const Objects& os )
{
  Object *transformations[MAXTRANSFORMATIONS];
  bool valid;
  double transformation[3][3];

  int transformationsnum = os.size() - 1;
  assert( transformationsnum <= MAXTRANSFORMATIONS );
  if ( transformationsnum < 1 ) return;  // don't drawprelim if too few points
  Objects::const_iterator i = os.begin();

  assert ((*i)->toPoint());
  Point *point = (*i++)->toPoint();

  for ( int j = 0; j < transformationsnum; j++ )
  {
    transformations[j] = *i++;
  }

  valid = getProjectiveTransformation ( transformationsnum,
      transformations, transformation );

  if ( ! valid ) return;

  Coordinate pp = calcTransformedPoint ( point->getCoord(), 
                   transformation, valid );
  sDrawPrelimPoint( p, pp );

  return;
}

PointTransform::PointTransform( const Objects& os )
  : Point()
{
  assert( os.size() <= MAXTRANSFORMATIONS + 1 );
  Objects::const_iterator i = os.begin();

  assert( (*i)->toPoint() );
  mpoint = (*i++)->toPoint();
  mpoint->addChild( this );

  transformationsnum = os.size() - 1;
  if ( transformationsnum == 0 ) return;

  for ( int j = 0; j < transformationsnum; j++ )
  {
    mtransformations[j] = *i++;
    mtransformations[j]->addChild( this );
  }
}

/*
 * transform a segment
 */

/*
 * since we cannot inherit directly from the Segment object, we
 * must provide some of the functions ourselves:
 * draw, ...
 */

bool SegmentTransform::contains(const Coordinate& o, 
                                const ScreenInfo& si ) const
{
  return isOnSegment( o, mpa, mpb, si.normalMiss() );
}

void SegmentTransform::draw(KigPainter& p, bool ss) const
{
  p.setPen( QPen( selected && ss ? Qt::red : mColor, 1 ));
  p.drawSegment( mpa, mpb );
}

bool SegmentTransform::inRect(const Rect&) const
{
  // TODO: implement for real (see segment.cpp)
//  if ( mpa->inRect( p ) || mpb->inRect( p ) ) return true;
  return false;
}

Coordinate SegmentTransform::getPoint(double param) const
{
  Coordinate dir = mpb - mpa;
  return mpa + dir*param;
}

double SegmentTransform::getParam(const Coordinate& point) const
{
  Coordinate pa = point - mpa;
  Coordinate ba = mpb - mpa;
  double balsq = ba.x*ba.x + ba.y*ba.y;
  assert (balsq > 0);

  double t = (pa.x*ba.x + pa.y*ba.y)/balsq;
  if ( t < 0 ) return 0.;
  if ( t > 1 ) return 1.;
  return t;
}

void SegmentTransform::calc()
{
  mvalid = true;
  double transformation[3][3];

  mvalid = getProjectiveTransformation (
     transformationsnum, mtransformations, transformation );

  if ( mvalid )
  {
    mpa = calcTransformedPoint ( msegment->p1(), transformation, mvalid );
    mpb = calcTransformedPoint ( msegment->p2(), transformation, mvalid );
  }
}

const char* SegmentTransform::sActionName()
{
  return "objects_new_segmenttransform";
}

Objects SegmentTransform::getParents() const
{
  Objects objs;
  objs.push_back( msegment );
  for ( int i = 0; i < transformationsnum; i++ )
  {
    objs.push_back( mtransformations[i] );
  }
  return objs;
}

SegmentTransform::SegmentTransform(const SegmentTransform& c)
  : Line( c ), msegment( c.msegment ), 
    transformationsnum ( c.transformationsnum )
{
  for ( int i = 0; i < c.transformationsnum; i++ )
  {
    mtransformations[i] = c.mtransformations[i];
    mtransformations[i]->addChild(this);
  }
  msegment->addChild(this);
}

const QString SegmentTransform::sDescriptiveName()
{
  return i18n("Transform a Segment");
}

const QString SegmentTransform::sDescription()
{
  return i18n( "Transform a segment using an elementary transformation" );
}

Object::WantArgsResult SegmentTransform::sWantArgs( const Objects& os )
{
  uint size = os.size();
  assert ( size > 0 );
  if ( size > MAXTRANSFORMATIONS + 1 ) return NotGood;
  Objects::const_iterator i = os.begin();

  if ( ! (*i++)->toSegment() ) return NotGood;
  if ( i == os.end() ) return NotComplete;
  switch ( WantTransformation (i, os) )
  {
    case tComplete: return Complete;
    case tNotComplete: return NotComplete;
    case tNotGood: return NotGood;
    default: return NotGood;
  }
}

QString SegmentTransform::sUseText( const Objects& os, const Object* o)
{
  if ( os.size() == 0 ) return i18n( "Transform this segment" );
  return getTransformMessage ( os, o );
}

void SegmentTransform::sDrawPrelim( KigPainter& p, const Objects& os )
{
  Object *transformations[MAXTRANSFORMATIONS];
  bool valid;
  double transformation[3][3];

  int transformationsnum = os.size() - 1;
  assert( transformationsnum <= MAXTRANSFORMATIONS );
  if ( transformationsnum < 1 ) return;  // don't drawprelim if too few points
  Objects::const_iterator i = os.begin();

  assert ((*i)->toSegment());
  Segment *segment = (*i++)->toSegment();

  for ( int j = 0; j < transformationsnum; j++ )
  {
    transformations[j] = *i++;
  }

  valid = getProjectiveTransformation ( transformationsnum,
      transformations, transformation );

  if ( ! valid ) return;

  p.setPen(QPen (Qt::red, 1));

  Coordinate a = calcTransformedPoint ( segment->p1(), transformation, valid );
  Coordinate b = calcTransformedPoint ( segment->p2(), transformation, valid );
  p.drawSegment( a, b );

  return;
}

SegmentTransform::SegmentTransform( const Objects& os )
  : Line()
{
  assert( os.size() <= MAXTRANSFORMATIONS + 1 );
  Objects::const_iterator i = os.begin();

  assert( (*i)->toSegment() );
  msegment = (*i++)->toSegment();
  msegment->addChild( this );

  transformationsnum = os.size() - 1;
  if ( transformationsnum == 0 ) return;

  for ( int j = 0; j < transformationsnum; j++ )
  {
    mtransformations[j] = *i++;
    mtransformations[j]->addChild( this );
  }
}

/*
 * transform a ray
 */

void RayTransform::calc()
{
  mvalid = true;
  double transformation[3][3];

  mvalid = getProjectiveTransformation (
     transformationsnum, mtransformations, transformation );

  if ( mvalid )
  {
    mpa = calcTransformedPoint ( mray->p1(), transformation, mvalid );
    mpb = calcTransformedPoint ( mray->p2(), transformation, mvalid );
  }
}

const char* RayTransform::sActionName()
{
  return "objects_new_raytransform";
}

Objects RayTransform::getParents() const
{
  Objects objs;
  objs.push_back( mray );
  for ( int i = 0; i < transformationsnum; i++ )
  {
    objs.push_back( mtransformations[i] );
  }
  return objs;
}

RayTransform::RayTransform(const RayTransform& c)
  : Line( c ), mray( c.mray ), transformationsnum ( c.transformationsnum )
{
  for ( int i = 0; i < c.transformationsnum; i++ )
  {
    mtransformations[i] = c.mtransformations[i];
    mtransformations[i]->addChild(this);
  }
  mray->addChild(this);
}

const QString RayTransform::sDescriptiveName()
{
  return i18n("Transform a Ray");
}

const QString RayTransform::sDescription()
{
  return i18n( "Transform a ray using an elementary transformation" );
}

Object::WantArgsResult RayTransform::sWantArgs( const Objects& os )
{
  uint size = os.size();
  assert ( size > 0 );
  if ( size > MAXTRANSFORMATIONS + 1 ) return NotGood;
  Objects::const_iterator i = os.begin();

  if ( ! (*i++)->toRay() ) return NotGood;
  if ( i == os.end() ) return NotComplete;
  switch ( WantTransformation (i, os) )
  {
    case tComplete: return Complete;
    case tNotComplete: return NotComplete;
    case tNotGood: return NotGood;
    default: return NotGood;
  }
}

QString RayTransform::sUseText( const Objects& os, const Object* o)
{
  if ( os.size() == 0 ) return i18n( "Transform this ray" );
  return getTransformMessage ( os, o );
}

void RayTransform::sDrawPrelim( KigPainter& p, const Objects& os )
{
  Object *transformations[MAXTRANSFORMATIONS];
  bool valid;
  double transformation[3][3];

  int transformationsnum = os.size() - 1;
  assert( transformationsnum <= MAXTRANSFORMATIONS );
  if ( transformationsnum < 1 ) return;  // don't drawprelim if too few points
  Objects::const_iterator i = os.begin();

  assert ((*i)->toRay());
  Ray *ray = (*i++)->toRay();

  for ( int j = 0; j < transformationsnum; j++ )
  {
    transformations[j] = *i++;
  }

  valid = getProjectiveTransformation ( transformationsnum,
      transformations, transformation );

  if ( ! valid ) return;

  p.setPen(QPen (Qt::red, 1));

  Coordinate a = calcTransformedPoint ( ray->p1(), transformation, valid );
  Coordinate b = calcTransformedPoint ( ray->p2(), transformation, valid );
  p.drawRay( a, b );

  return;
}

RayTransform::RayTransform( const Objects& os )
  : Line()
{
  assert( os.size() <= MAXTRANSFORMATIONS + 1 );
  Objects::const_iterator i = os.begin();

  assert( (*i)->toRay() );
  mray = (*i++)->toRay();
  mray->addChild( this );

  transformationsnum = os.size() - 1;
  if ( transformationsnum == 0 ) return;

  for ( int j = 0; j < transformationsnum; j++ )
  {
    mtransformations[j] = *i++;
    mtransformations[j]->addChild( this );
  }
}

/*
 * transform a circle
 */

void CircleTransform::calc()
{
  mvalid = true;
  double transformation[3][3];

  mvalid = getProjectiveTransformation (
     transformationsnum, mtransformations, transformation );

  if ( ! isHomoteticTransformation (transformation) )
  {
    mvalid = false;
    return;
  }
  if ( mvalid )
  {
    qpc = calcTransformedPoint ( mcircle->center(), transformation, mvalid );
    double det = transformation[1][1]*transformation[2][2] -
                 transformation[1][2]*transformation[2][1];
    mradius = sqrt(fabs(det))*mcircle->radius();
  }
}

const char* CircleTransform::sActionName()
{
  return "objects_new_circletransform";
}

Objects CircleTransform::getParents() const
{
  Objects objs;
  objs.push_back( mcircle );
  for ( int i = 0; i < transformationsnum; i++ )
  {
    objs.push_back( mtransformations[i] );
  }
  return objs;
}

CircleTransform::CircleTransform(const CircleTransform& c)
  : Circle( c ), mcircle( c.mcircle ), transformationsnum ( c.transformationsnum )
{
  for ( int i = 0; i < c.transformationsnum; i++ )
  {
    mtransformations[i] = c.mtransformations[i];
    mtransformations[i]->addChild(this);
  }
  mcircle->addChild(this);
}

const QString CircleTransform::sDescriptiveName()
{
  return i18n("Transform a Circle");
}

const QString CircleTransform::sDescription()
{
  return i18n( "Transform a circle using an elementary transformation" );
}

Object::WantArgsResult CircleTransform::sWantArgs( const Objects& os )
{
  uint size = os.size();
  assert ( size > 0 );
  if ( size > MAXTRANSFORMATIONS + 1 ) return NotGood;
  Objects::const_iterator i = os.begin();

  if ( ! (*i++)->toCircle() ) return NotGood;
  if ( i == os.end() ) return NotComplete;
  switch ( WantTransformation (i, os) )
  {
    case tComplete: return Complete;
    case tNotComplete: return NotComplete;
    case tNotGood: return NotGood;
    default: return NotGood;
  }
}

QString CircleTransform::sUseText( const Objects& os, const Object* o)
{
  if ( os.size() == 0 ) return i18n( "Transform this circle" );
  return getTransformMessage ( os, o );
}

void CircleTransform::sDrawPrelim( KigPainter& p, const Objects& os )
{
  Object *transformations[MAXTRANSFORMATIONS];
  bool valid;
  double transformation[3][3];

  int transformationsnum = os.size() - 1;
  assert( transformationsnum <= MAXTRANSFORMATIONS );
  if ( transformationsnum < 1 ) return;  // don't drawprelim if too few points
  Objects::const_iterator i = os.begin();

  assert ((*i)->toCircle());
  Circle *circle = (*i++)->toCircle();

  for ( int j = 0; j < transformationsnum; j++ )
  {
    transformations[j] = *i++;
  }

  valid = getProjectiveTransformation ( transformationsnum,
      transformations, transformation );

  if ( ! valid ) return;

  if ( ! isHomoteticTransformation (transformation)) return;

  p.setPen(QPen (Qt::red, 1));

  Coordinate cc = calcTransformedPoint ( circle->center(), 
    transformation, valid );
  double det = transformation[1][1]*transformation[2][2] -
               transformation[1][2]*transformation[2][1];
  double radius = sqrt(fabs(det))*circle->radius();
  p.drawCircle( cc, radius );

  return;
}

CircleTransform::CircleTransform( const Objects& os )
  : Circle()
{
  assert( os.size() <= MAXTRANSFORMATIONS + 1 );
  Objects::const_iterator i = os.begin();

  assert( (*i)->toCircle() );
  mcircle = (*i++)->toCircle();
  mcircle->addChild( this );

  transformationsnum = os.size() - 1;
  if ( transformationsnum == 0 ) return;

  for ( int j = 0; j < transformationsnum; j++ )
  {
    mtransformations[j] = *i++;
    mtransformations[j]->addChild( this );
  }
}

/*
 * transform a conic
 */

void ConicTransform::calc()
{
  mvalid = true;
  double transformation[3][3];

  mvalid = getProjectiveTransformation (
     transformationsnum, mtransformations, transformation );

  if ( mvalid )
  {
    mequation = ConicPolarEquationData ( 
      calcConicTransformation ( mconic->cartesianEquationData(),
                                transformation, mvalid )
      );
  }
}

const ConicPolarEquationData ConicTransform::polarEquationData() const
{
  return mequation;
}

const char* ConicTransform::sActionName()
{
  return "objects_new_conictransform";
}

Objects ConicTransform::getParents() const
{
  Objects objs;
  objs.push_back( mconic );
  for ( int i = 0; i < transformationsnum; i++ )
  {
    objs.push_back( mtransformations[i] );
  }
  return objs;
}

ConicTransform::ConicTransform(const ConicTransform& c)
  : Conic( c ), mconic( c.mconic ), transformationsnum ( c.transformationsnum )
{
  for ( int i = 0; i < c.transformationsnum; i++ )
  {
    mtransformations[i] = c.mtransformations[i];
    mtransformations[i]->addChild(this);
  }
  mconic->addChild(this);
}

const QString ConicTransform::sDescriptiveName()
{
  return i18n("Transform a Conic");
}

const QString ConicTransform::sDescription()
{
  return i18n( "Transform a conic using an elementary transformation" );
}

Object::WantArgsResult ConicTransform::sWantArgs( const Objects& os )
{
  uint size = os.size();
  assert ( size > 0 );
  if ( size > MAXTRANSFORMATIONS + 1 ) return NotGood;
  Objects::const_iterator i = os.begin();

  if ( ! (*i++)->toConic() ) return NotGood;
  if ( i == os.end() ) return NotComplete;
  switch ( WantTransformation (i, os) )
  {
    case tComplete: return Complete;
    case tNotComplete: return NotComplete;
    case tNotGood: return NotGood;
    default: return NotGood;
  }
}

QString ConicTransform::sUseText( const Objects& os, const Object* o)
{
  if ( os.size() == 0 ) return i18n( "Transform this conic" );
  return getTransformMessage ( os, o );
}

void ConicTransform::sDrawPrelim( KigPainter& p, const Objects& os )
{
  Object *transformations[MAXTRANSFORMATIONS];
  bool valid;
  double transformation[3][3];

  int transformationsnum = os.size() - 1;
  assert( transformationsnum <= MAXTRANSFORMATIONS );
  if ( transformationsnum < 1 ) return;  // don't drawprelim if too few points
  Objects::const_iterator i = os.begin();

  assert ((*i)->toConic());
  Conic *conic = (*i++)->toConic();

  for ( int j = 0; j < transformationsnum; j++ )
  {
    transformations[j] = *i++;
  }

  valid = getProjectiveTransformation ( transformationsnum,
      transformations, transformation );

  if ( ! valid ) return;

  p.setPen(QPen (Qt::red, 1));
  p.drawConic( ConicPolarEquationData (
    calcConicTransformation ( conic->cartesianEquationData(),
                              transformation, valid )
    ) );

  return;
}

ConicTransform::ConicTransform( const Objects& os )
  : Conic()
{
  assert( os.size() <= MAXTRANSFORMATIONS + 1 );
  Objects::const_iterator i = os.begin();

  assert( (*i)->toConic() );
  mconic = (*i++)->toConic();
  mconic->addChild( this );

  transformationsnum = os.size() - 1;
  if ( transformationsnum == 0 ) return;

  for ( int j = 0; j < transformationsnum; j++ )
  {
    mtransformations[j] = *i++;
    mtransformations[j]->addChild( this );
  }
}

/*
 * transform a cubic using a sequence of elementary transformations
 */

void CubicTransform::calc()
{
  mvalid = true;
  double transformation[3][3];

  mvalid = getProjectiveTransformation ( 
     transformationsnum, mtransformations, transformation );

  if ( mvalid )
  {
    cequation = calcCubicTransformation ( mcubic->cartesianEquationData(), 
                                          transformation, mvalid );
  }
}

const char* CubicTransform::sActionName()
{
  return "objects_new_cubictransform";
}

Objects CubicTransform::getParents() const
{
  Objects objs;
  objs.push_back( mcubic );
  for ( int i = 0; i < transformationsnum; i++ )
  {
    objs.push_back( mtransformations[i] );
  }
  return objs;
}

CubicTransform::CubicTransform(const CubicTransform& c)
  : Cubic( c ), mcubic( c.mcubic ), transformationsnum ( c.transformationsnum )
{
  for ( int i = 0; i < c.transformationsnum; i++ )
  {
    mtransformations[i] = c.mtransformations[i];
    mtransformations[i]->addChild(this);
  }
  mcubic->addChild(this);
}

const QString CubicTransform::sDescriptiveName()
{
  return i18n("Transform a Cubic");
}

const QString CubicTransform::sDescription()
{
  return i18n( "Transform a cubic using an elementary transformation" );
}

Object::WantArgsResult CubicTransform::sWantArgs( const Objects& os )
{
  uint size = os.size();
  assert ( size > 0 );
  if ( size > MAXTRANSFORMATIONS + 1 ) return NotGood;
  Objects::const_iterator i = os.begin();

  if ( ! (*i++)->toCubic() ) return NotGood;
//  while ( i != os.end() )
//  {
//    if ( ! isTransformation (i, os) ) return NotGood;
//  }
  if ( i == os.end() ) return NotComplete;
  switch ( WantTransformation (i, os) )
  {
    case tComplete: return Complete;
    case tNotComplete: return NotComplete;
    case tNotGood: return NotGood;
    default: return NotGood;
  }

//  if ( size == MAXTRANSFORMATIONS + 1 ) return Complete;
//
//  if ( size <= 2 ) return NotComplete;
//  if ( *(os.end()-1) == *(os.end()-2) ) return Complete;
//  return NotComplete;
}

QString CubicTransform::sUseText( const Objects& os, const Object* o)
{
  if ( os.size() == 0 ) return i18n( "Transform this cubic" );
  if ( os.size() >= 2 && o == *(os.end()-1) ) 
    return i18n("Finish construction");
  return getTransformMessage ( os, o );
}

void CubicTransform::sDrawPrelim( KigPainter& p, const Objects& os )
{
  Object *transformations[MAXTRANSFORMATIONS];
  bool valid;
  double transformation[3][3];

  int transformationsnum = os.size() - 1;
  assert( transformationsnum <= MAXTRANSFORMATIONS );
  if ( transformationsnum < 1 ) return;  // don't drawprelim if too few points
  Objects::const_iterator i = os.begin();

  assert ((*i)->toCubic());
  Cubic *cubic = (*i++)->toCubic();
 
  if ( transformationsnum > 1 && *(os.end()-1) == *(os.end()-2)) 
        transformationsnum--;
  for ( int j = 0; j < transformationsnum; j++ )
  {
//    assert( isTransformation (*i) );
    transformations[j] = *i++;
  }

  valid = getProjectiveTransformation ( transformationsnum, 
      transformations, transformation );

  if ( ! valid ) return;

  p.setPen(QPen (Qt::red, 1));
  p.drawCubic(
    calcCubicTransformation ( cubic->cartesianEquationData(),
                              transformation, valid )
    );

  return;
}

CubicTransform::CubicTransform( const Objects& os )
  : Cubic()
{
  assert( os.size() <= MAXTRANSFORMATIONS + 1 );
  Objects::const_iterator i = os.begin();

  assert( (*i)->toCubic() );
  mcubic = (*i++)->toCubic();
  mcubic->addChild( this );

  transformationsnum = os.size() - 1;
  if ( transformationsnum == 0 ) return;

  if ( transformationsnum >= 2 && *(os.end()-1) == *(os.end()-2) )
    transformationsnum--;

  for ( int j = 0; j < transformationsnum; j++ )
  {
//    assert ( isTransformation(*i) );
    mtransformations[j] = *i++;
    mtransformations[j]->addChild( this );
  }
}

const ConicCartesianEquationData calcConicTransformation (
    ConicCartesianEquationData data,
    double t[3][3], bool& valid )
{
  double a[3][3];
  double b[3][3];
  double ti[3][3];
  ConicCartesianEquationData dataout;

  a[1][1] = data.coeffs[0];
  a[2][2] = data.coeffs[1];
  a[1][2] = a[2][1] = data.coeffs[2]/2;
  a[0][1] = a[1][0] = data.coeffs[3]/2;
  a[0][2] = a[2][0] = data.coeffs[4]/2;
  a[0][0] = data.coeffs[5];

  valid = Invert3by3matrix ( t, ti );
  if ( ! valid ) return dataout;

  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      b[i][j] = 0.;
      for (int ii = 0; ii < 3; ii++)
      {
        for (int jj = 0; jj < 3; jj++)
        {
	  b[i][j] += a[ii][jj]*ti[ii][i]*ti[jj][j];
	}
      }
    }
  }

  assert (fabs(b[0][1] - b[1][0]) < 1e-8);  // test a couple of cases
  assert (fabs(b[0][2] - b[2][0]) < 1e-8);

  return ConicCartesianEquationData ( b[1][1],   b[2][2], 2*b[1][2],
                                    2*b[0][1], 2*b[0][2],   b[0][0] );
}

const CubicCartesianEquationData calcCubicTransformation (
    CubicCartesianEquationData data,
    double t[3][3], bool& valid )
{
  double a[3][3][3];
  double b[3][3][3];
  double ti[3][3];
  CubicCartesianEquationData dataout;

  int icount = 0;
  for (int i=0; i < 3; i++)
  {
    for (int j=i; j < 3; j++)
    {
      for (int k=j; k < 3; k++)
      {
	a[i][j][k] = data.coeffs[icount++];
	if ( i < k )
	{
	  if ( i == j )    // case aiik
	  {
	    a[i][i][k] /= 3.;
	    a[i][k][i] = a[k][i][i] = a[i][i][k];
	  }
	  else if ( j == k )  // case aijj
	  {
	    a[i][j][j] /= 3.;
	    a[j][i][j] = a[j][j][i] = a[i][j][j];
	  }
	  else             // case aijk  (i<j<k)
	  {
	    a[i][j][k] /= 6.;
	    a[i][k][j] = a[j][i][k] = a[j][k][i] = 
                         a[k][i][j] = a[k][j][i] = a[i][j][k];
	  }
	}
      }
    }
  }

  valid = Invert3by3matrix ( t, ti );
  if ( ! valid ) return dataout;

  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      for (int k = 0; k < 3; k++)
      {
	b[i][j][k] = 0.;
        for (int ii = 0; ii < 3; ii++)
        {
          for (int jj = 0; jj < 3; jj++)
          {
            for (int kk = 0; kk < 3; kk++)
            {
	      b[i][j][k] += a[ii][jj][kk]*ti[ii][i]*ti[jj][j]*ti[kk][k];
	    }
	  }
	}
      }
    }
  }

  assert (fabs(b[0][1][2] - b[1][2][0]) < 1e-8);  // test a couple of cases
  assert (fabs(b[0][1][1] - b[1][1][0]) < 1e-8);

  icount = 0;
  for (int i=0; i < 3; i++)
  {
    for (int j=i; j < 3; j++)
    {
      for (int k=j; k < 3; k++)
      {
	dataout.coeffs[icount] = b[i][j][k];
	if ( i < k )
	{
	  if ( i == j )    // case biik
	  {
	    dataout.coeffs[icount] *= 3.;
	  }
	  else if ( j == k )  // case bijj
	  {
	    dataout.coeffs[icount] *= 3.;
	  }
	  else             // case bijk  (i<j<k)
	  {
	    dataout.coeffs[icount] *= 6.;
	  }
	}
	icount++;
      }
    }
  }

  return dataout;
}
