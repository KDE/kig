// translatedpoint.cc
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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

#include "translatedpoint.h"

#include "vector.h"

#include "../misc/kigpainter.h"
#include "../misc/i18n.h"

#include <kdebug.h>

Objects TranslatedPoint::getParents() const
{
  Objects tmp;
  tmp.push_back( mp );
  tmp.push_back( mv );
  return tmp;
};

void TranslatedPoint::calc()
{
  mvalid = mp->valid() && mv->valid();
  if ( mvalid )
  {
    Coordinate a = mp->getCoord();
    Coordinate d = mv->getDir();

    mC = a + d;
  };
}

TranslatedPoint::TranslatedPoint( const TranslatedPoint& p )
  : Point( p ), mp( p.mp ), mv( p.mv )
{
  mp->addChild( this );
  mv->addChild( this );
};

TranslatedPoint::~TranslatedPoint()
{
}

TranslatedPoint* TranslatedPoint::copy()
{
  return new TranslatedPoint( *this );
}

const QCString TranslatedPoint::vFullTypeName() const
{
  return sFullTypeName();
}

const QCString TranslatedPoint::sFullTypeName()
{
  return "TranslatedPoint";
}

const QString TranslatedPoint::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString TranslatedPoint::sDescriptiveName()
{
  return i18n("Translated Point");
}

const QString TranslatedPoint::vDescription() const
{
  return sDescription();
}

const QString TranslatedPoint::sDescription()
{
  return i18n( "The translation of a point according to a vector" );
}

const QCString TranslatedPoint::vIconFileName() const
{
  return sIconFileName();
}

const QCString TranslatedPoint::sIconFileName()
{
  return "move";
}

const int TranslatedPoint::vShortCut() const
{
  return sShortCut();
}

const int TranslatedPoint::sShortCut()
{
  return 0;
}

void TranslatedPoint::startMove(const Coordinate&, const ScreenInfo& )
{
}

void TranslatedPoint::moveTo(const Coordinate& )
{
}

void TranslatedPoint::stopMove()
{
}

const char* TranslatedPoint::sActionName()
{
  return "objects_new_translatedpoint";
}

TranslatedPoint::TranslatedPoint( const Objects& os )
  : Point(), mp( 0 ), mv( 0 )
{
  assert( os.size() == 2 );
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! mp ) mp = (*i)->toPoint();
    if ( ! mv ) mv = (*i)->toVector();
  };
  assert( mp && mv );
  mp->addChild( this );
  mv->addChild( this );
}

void TranslatedPoint::sDrawPrelim( KigPainter& p, const Objects& os )
{
  if ( os.size() != 2 ) return;
  Point* q = 0;
  Vector* v = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( ! q ) q = (*i)->toPoint();
    if ( ! v ) v = (*i)->toVector();
  };
  assert( q && v );
  Coordinate c = q->getCoord() + v->getDir();
  p.setPen( QPen( Qt::red, 1 ) );
  p.drawFatPoint( c );
}

Object::WantArgsResult TranslatedPoint::sWantArgs( const Objects& os )
{
  uint size = os.size();
  if ( size != 1 && size != 2 ) return NotGood;
  int p = 0;
  int v = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    if ( (*i)->toPoint() ) ++p;
    else if ( (*i)->toVector() ) ++v;
    else return NotGood;
  };
  if ( p > 1 || v > 1 ) return NotGood;
  return size == 2 ? Complete : NotComplete;
}

QString TranslatedPoint::sUseText( const Objects&, const Object* o )
{
  if ( o->toPoint() ) return i18n( "Translation of this point" );
  if ( o->toVector() ) return i18n( "Translate point by this vector" );
  assert( false );
}
