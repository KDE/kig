// label.cc
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

#include "label.h"

#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/coordinate_system.h"
#include "../misc/kigpainter.h"
#include "../misc/i18n.h"
#include "property.h"
#include "../modes/label.h"

#include <sstream>
#include <functional>
#include <algorithm>

TextLabel::TextLabel( const Objects& os )
  : Object(), mprops( os.begin(), os.end() )
{
  // STL rocks ! ;)
  using std::for_each;
  using std::bind2nd;
  using std::mem_fun;
  for_each( os.begin(), os.end(),
            bind2nd(
              mem_fun( &Object::addChild ),
              this )
    );
}

TextLabel::TextLabel( const TextLabel& l )
  : Object( l ), mprops( l.mprops ), mtext( l.mtext ), mcoord( l.mcoord )
{
  // damn, this seems unportable...
//   // this one rocks even more :)
//   using std::for_each;
//   using std::bind2nd;
//   using std::mem_fun;
//   using std::compose1;
//   for_each( mprops.begin(), mprops.end(),
//             compose1(
//               bind2nd(
//                 mem_fun( &Object::addChild ),
//                 this ),
//               getObj() )
//     );
  // now wasn't that way cooler than the following.. ;)
  for ( propvect::iterator i = mprops.begin(); i != mprops.end(); ++i )
    i->obj->addChild( this );
}

TextLabel::~TextLabel()
{
}

Object::prop_map TextLabel::getParams()
{
  prop_map ret;
  ret["text"] = mtext;
  ret["coordinate-x"] = QString::number( mcoord.x );
  ret["coordinate-y"] = QString::number( mcoord.y );
  int count = 0;
  for ( propvect::const_iterator i = mprops.begin(); i != mprops.end(); ++i, ++count )
  {
    std::ostringstream os;
    os << "property-for-object-" << count;
    ret[os.str().c_str()] = i->obj->properties()[i->index];
  };
  return ret;
}

void TextLabel::setParams( const prop_map& m )
{
  mtext = m.find("text")->second;
  bool ok = true;
  mcoord = Coordinate(
    m.find("coordinate-x")->second.toDouble( &ok ),
    m.find("coordinate-y")->second.toDouble( &ok ) );
  Q_ASSERT( ok );

  int count = 0;
  for ( propvect::iterator i = mprops.begin(); i != mprops.end(); ++i, ++count )
  {
    std::ostringstream os;
    os << "property-for-object-" << count;
    prop_map::const_iterator pi = m.find( os.str().c_str() );
    assert( pi != m.end() );
    int index = i->obj->properties().findIndex( pi->second.latin1() );
    assert( index != -1 );
    i->index = index;
  };
}

void TextLabel::draw(KigPainter& p, bool ss ) const
{
  p.setColor( selected && ss ? Qt::red : mColor );
  p.setBrush( Qt::NoBrush );
  p.drawSimpleText( mcoord, mcurtext );
  mrwdoi = p.simpleBoundingRect( mcoord, mtext );
}

bool TextLabel::contains( const Coordinate& o, const ScreenInfo& ) const
{
  return mrwdoi.contains( o );
}

KigMode* TextLabel::sConstructMode( Type*, KigDocument& d )
{
  return new TextLabelConstructionMode( d );
}

void TextLabel::startMove( const Coordinate& c, const ScreenInfo& )
{
  mpwwmt = c;
}

void TextLabel::moveTo( const Coordinate& c )
{
  mcoord += ( c - mpwwmt );
  mpwwmt = c;
}

const QCString TextLabel::vBaseTypeName() const
{
  return sBaseTypeName();
}

QCString TextLabel::sBaseTypeName()
{
  return I18N_NOOP("Label");
}

const QCString TextLabel::vFullTypeName() const
{
  return sFullTypeName();
}

const QString TextLabel::vDescriptiveName() const
{
  return sDescriptiveName();
}

const QString TextLabel::sDescriptiveName()
{
  return i18n( "Text Label" );
}

const QCString TextLabel::vIconFileName() const
{
  return sIconFileName();
}

const QCString TextLabel::sIconFileName()
{
  return "text";
}

const char* TextLabel::sActionName()
{
  return "objects_new_textlabel";
}

bool TextLabel::inRect(const Rect& ) const
{
  return false;
}

void TextLabel::calcForWidget( const KigWidget& w )
{
  mcurtext = mtext;
  for ( propvect::iterator i = mprops.begin(); i != mprops.end(); ++i )
  {
    assert( i->index != static_cast<uint>( -1 ) );
    Property prop = i->obj->property( i->index, w );
    switch( prop.type() )
    {
    case Property::Double:
      mcurtext = mcurtext.arg( prop.doubleData() );
      break;
    case Property::String:
      mcurtext = mcurtext.arg( prop.qstringData() );
      break;
    case Property::Coord:
      mcurtext = mcurtext.arg( w.document().coordinateSystem().fromScreen( prop.coordData(), w ) );
      break;
    default:
      assert( false );
    };
  };
}

namespace {
  Object* getObj( const TextLabelProperty& p ) { return p.obj; };
};

Objects TextLabel::getParents() const
{
  Objects ret;
  std::transform( mprops.begin(), mprops.end(), std::back_inserter( ret ), getObj );
  return ret;

}
const QString TextLabel::vDescription() const
{
  return sDescription();
}
const QString TextLabel::sDescription()
{
  return i18n("A piece of text you can add to the document.");
}

TextLabel::TextLabel( const QString text, const Coordinate c, const propvect& props )
  : Object(), mprops( props ), mtext( text ), mcoord( c )
{
  for ( propvect::iterator i = mprops.begin(); i != mprops.end(); ++i )
    i->obj->addChild( this );
}

void TextLabel::calc()
{
}

const uint TextLabel::numberOfProperties() const
{
  return Object::numberOfProperties() + 1;
}

const Property TextLabel::property( uint which, const KigWidget& w ) const
{
  assert( which < TextLabel::numberOfProperties() );
  if ( which < Object::numberOfProperties() ) return Object::property( which, w );
  if ( which == Object::numberOfProperties() )
    return Property( mcurtext );
  else assert( false );
}

const QCStringList TextLabel::properties() const
{
  QCStringList l = Object::properties();
  l << I18N_NOOP( "Text" );
  assert( l.size() == TextLabel::numberOfProperties() );
  return l;
}

bool TextLabel::isa( int type ) const
{
  return type == TextLabelT ? true : Parent::isa( type );
}
