/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

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

#include "object.h"

#include "coordproppoint.h"
#include "label.h"
#include "segment.h"
#include "vector.h"
#include "ray.h"
#include "angle.h"
#include "line.h"
#include "circle.h"
#include "conic.h"
#include "cubic.h"
#include "normalpoint.h"
#include "property.h"

#include "../misc/types.h"
#include "../misc/type.h"
#include "../modes/constructing.h"
#include "../modes/popup.h"

#include "../kig/kig_part.h"
#include "../kig/kig_view.h"

#include <klocale.h>
#include <qpopupmenu.h>
#include <qcursor.h>

myvector<Type*> Object::sbuiltintypes;
myvector<Type*> Object::susertypes;
Types Object::stypes;

Object::Object()
  : mColor( Qt::blue ), selected(false), mshown (true), mvalid(true)
{
};

void Object::setSelected(bool in)
{
  selected = in;
}

Objects Object::getAllChildren() const
{
  // what we are going to return...
  Objects tmp;
  // objects to iterate over...
  Objects objs = children;
  // contains the objects to iterate over the next time around...
  Objects objs2;
  while( !objs.empty() )
  {
    for( Objects::iterator i = objs.begin();
         i != objs.end(); ++i )
    {
      tmp.upush( *i );
      objs2 |= (*i)->getChildren();
    };
    objs = objs2;
    objs2.clear();
  };
  return tmp;
}

Object::prop_map Object::getParams()
{
  prop_map m;
  m["color"] = mColor.name();
  m["shown"] = QString::fromLatin1( mshown ? "true" : "false" );
  return m;
}

void Object::setParams( const prop_map& m )
{
  {
    prop_map::const_iterator p = m.find("color");
    if( p == m.end() ) mColor = Qt::blue;
    else mColor = QColor( p->second );
    assert( mColor.isValid() );
  };
  {
    prop_map::const_iterator p = m.find("shown");
    if( p != m.end() && p->second == QString::fromUtf8( "false" ) )
      mshown = false;
    else mshown = true;
  };
}

const Types& Object::types()
{
  return stypes;
}

const myvector<Type*>& Object::builtinTypes()
{
  return sbuiltintypes;
}

const myvector<Type*>& Object::userTypes()
{
  return susertypes;
}

Object* Object::newObject( const QCString& type, const Objects& parents,
                           const prop_map& params )
{
  return types().buildObject( type, parents, params );
}

KigMode* Object::sConstructMode( StdConstructibleType* ourtype,
                                 KigDocument* theDoc,
                                 NormalMode* previousMode )
{
  return new StdConstructionMode( ourtype, previousMode, theDoc );
}

const QString Object::vTBaseTypeName() const
{
  return i18n(vBaseTypeName());
}

void Object::addChild(Object* o)
{
  children.push_back(o);
}

void Object::delChild(Object* o)
{
  children.remove( o );
}

Object::Object( const Object& o )
  : mColor( o.mColor ), selected( false ), mshown( true ),
    mvalid( o.mvalid )
{

}

void Object::drawWrap(KigPainter& p, bool ss) const
{
  if ( mshown && mvalid ) draw(p,ss);
}

void Object::addBuiltinType( Type* t )
{
  stypes.addType( t );
  sbuiltintypes.push_back( t );
}

void Object::addUserType( Type* t, bool notify )
{
  Types ts;
  ts.addType( t );
  addUserTypes( ts, notify );
}

void Object::addUserTypes( Types& t, bool notify )
{
  for ( Types::const_iterator i = t.begin(); i != t.end(); ++i )
  {
    stypes.addType( i->second );
    susertypes.push_back( i->second );
  };
  if ( notify )
  {
    typedef myvector<KigDocument*> mv;
    mv& v = KigDocument::documents();
    for ( mv::const_iterator i = v.begin(); i != v.end(); ++i )
    {
      (*i)->unplugActionLists();
      for ( Types::const_iterator j = t.begin(); j != t.end(); ++j )
      {
        (*i)->addType( j->second );
      };
      (*i)->plugActionLists();
    };
  };
  t.clear();
}

void Object::removeUserType( Type* t )
{
  susertypes.remove( t );
  stypes.removeType( t );
  typedef myvector<KigDocument*> mv;
  mv& v = KigDocument::documents();
  for ( mv::const_iterator i = v.begin(); i != v.end(); ++i )
  {
    (*i)->unplugActionLists();
  };
  t->deleteActions();
  for ( mv::const_iterator i = v.begin(); i != v.end(); ++i )
  {
    (*i)->plugActionLists();
  };
}

// void Object::removeUserTypes( Types& t, bool notifyParts )
// {

// }
bool Object::valid() const
{
  return mvalid;
}

bool Object::shown() const
{
  return mshown;
}
void Object::setShown( bool in )
{
  mshown = in;
}

void Object::addActions( NormalModePopupObjects& m )
{
  QPopupMenu* pm = new QPopupMenu( &m, "construct_coord_props_menu" );

  bool content = false;         // true if we have content..

  QCStringList s = properties();
  for ( uint i = 0; i < numberOfProperties(); ++i )
  {
    Property p = property( i, m.widget() );
    if ( p.type() == Property::Coord )
    {
      content = true;
      QString str = i18n( s[i] );
      uint id = pm->insertItem( str, i );
      assert( id == i );
    };
  };

  m.addPopupAction( 3849, i18n( "Construct The" ), pm );

  pm = new QPopupMenu( &m, "construct_textlabels_menu" );
  for ( uint i = 0; i < numberOfProperties(); ++i )
  {
    QString str = i18n( s[i] );
    uint id = pm->insertItem( str, i );
    assert( i == id );
  };
  m.addPopupAction( 3850, i18n( "Show The" ), pm );

  return;
}

void Object::doNormalAction( int, KigDocument*, KigWidget*, NormalMode*, const Coordinate& )
{
  return;
}

void Object::doPopupAction( int popupid, int actionid, KigDocument* doc, KigWidget* w, NormalMode*, const Coordinate& cp )
{
  if( popupid == 3849 )
  {
    uint pid = static_cast<uint>( actionid );
    assert( property( pid, *w ).type() == Property::Coord );
    Object* o = new CoordinatePropertyPoint( this, pid );
    o->calcForWidget( *w );
    doc->addObject( o );
    w->redrawScreen();
  }
  else if ( popupid == 3850 )
  {
    uint pid = static_cast<uint>( actionid );
    TextLabel::propvect props;
    props.push_back( TextLabelProperty( this, pid ) );
    Object* o = new TextLabel( QString::fromLatin1( "%1" ), cp, props );
    o->calcForWidget( *w );
    doc->addObject( o );
    w->redrawScreen();
  }
  else assert( false );
}

const QCStringList Object::properties() const
{
  QCStringList s;
  s << I18N_NOOP( "Base Object Type" );
  assert( s.size() == Object::numberOfProperties() );
  return s;
}

const Property Object::property( uint which, const KigWidget& ) const
{
  assert( which == 0 );
  return Property( vTBaseTypeName() );
}

const uint Object::numberOfProperties() const
{
  return 1;
}

void Object::calcForWidget( const KigWidget& )
{
  calc();
}

void Object::startMove( const Coordinate&, const ScreenInfo& )
{
  // default is that we don't move..
}

void Object::moveTo(const Coordinate&)
{
  // default is that we don't move..
}

void Object::stopMove()
{
  // default is that we don't move..
}

bool Object::isa( int ) const
{
  return false;
}

Point* Object::toPoint()
{
  return isa( PointT ) ? static_cast<Point*>( this ) : 0;
}

Segment* Object::toSegment()
{
  return isa( SegmentT ) ? static_cast<Segment*>( this ) : 0;
};

Vector* Object::toVector()
{
  return isa( VectorT ) ? static_cast<Vector*>( this ) : 0;
};

Ray* Object::toRay()
{
  return isa( RayT ) ? static_cast<Ray*>( this ) : 0;
};

Line* Object::toLine()
{
  return isa( LineT ) ? static_cast<Line*>( this ) : 0;
};

Circle* Object::toCircle()
{
  return isa( CircleT ) ? static_cast<Circle*>( this ) : 0;
};

Conic* Object::toConic()
{
  return isa( ConicT ) ? static_cast<Conic*>( this ) : 0;
};

Cubic* Object::toCubic()
{
  return isa( CubicT ) ? static_cast<Cubic*>( this ) : 0;
};

Curve* Object::toCurve()
{
  return isa( CurveT ) ? static_cast<Curve*>( this ) : 0;
};

NormalPoint* Object::toNormalPoint()
{
  return isa( NormalPointT ) ? static_cast<NormalPoint*>( this ) : 0;
};

TextLabel* Object::toTextLabel()
{
  return isa( TextLabelT ) ? static_cast<TextLabel*>( this ) : 0;
};

AbstractLine* Object::toAbstractLine()
{
  return isa( AbstractLineT ) ? static_cast<AbstractLine*>( this ) : 0;
};

Angle* Object::toAngle()
{
  return isa( AngleT ) ? static_cast<Angle*>( this ) : 0;
};

const Point* Object::toPoint() const
{
  return isa( PointT ) ? static_cast<const Point*>( this ) : 0;
};

const Segment* Object::toSegment() const
{
  return isa( SegmentT ) ? static_cast<const Segment*>( this ) : 0;
};

const Vector* Object::toVector() const
{
  return isa( VectorT ) ? static_cast<const Vector*>( this ) : 0;
};

const Ray* Object::toRay() const
{
  return isa( RayT ) ? static_cast<const Ray*>( this ) : 0;
};

const Line* Object::toLine() const
{
  return isa( LineT ) ? static_cast<const Line*>( this ) : 0;
};

const Circle* Object::toCircle() const
{
  return isa( CircleT ) ? static_cast<const Circle*>( this ) : 0;
};

const Conic* Object::toConic() const
{
  return isa( ConicT ) ? static_cast<const Conic*>( this ) : 0;
};

const Cubic* Object::toCubic() const
{
  return isa( CubicT ) ? static_cast<const Cubic*>( this ) : 0;
};

const Curve* Object::toCurve() const
{
  return isa( CurveT ) ? static_cast<const Curve*>( this ) : 0;
};

const NormalPoint* Object::toNormalPoint() const
{
  return isa( NormalPointT ) ? static_cast<const NormalPoint*>( this ) : 0;
};

const TextLabel* Object::toTextLabel() const
{
  return isa( TextLabelT ) ? static_cast<const TextLabel*>( this ) : 0;
};

const AbstractLine* Object::toAbstractLine() const
{
  return isa( AbstractLineT ) ? static_cast<const AbstractLine*>( this ) : 0;
};

const Angle* Object::toAngle() const
{
  return isa( AngleT ) ? static_cast<const Angle*>( this ) : 0;
};

