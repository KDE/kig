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

#include "../misc/kigpainter.h"
#include "../misc/i18n.h"
#include "property.h"

#include "../modes/label.h"

#include <sstream>
#include <functional>
#include <algorithm>

namespace {
  class getObj
    : public std::unary_function<TextLabelProperty, Object*>
  {
  public:
    inline Object* operator()( const TextLabelProperty& p ) const { return p.obj; };
  };
};

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

std::map<QCString,QString> TextLabel::getParams()
{
  std::map<QCString, QString> ret;
  ret["text"] = mtext;
  ret["coordinate-x"] = QString::number( mcoord.x );
  ret["coordinate-y"] = QString::number( mcoord.y );
  int count = 0;
  for ( propvect::const_iterator i = mprops.begin(); i != mprops.end(); ++i, ++count )
  {
    std::ostringstream os;
    os << "property-index-for-object-" << count;
    ret[os.str().c_str()] = i->index;
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
    os << "property-index-for-object-" << count;
    prop_map::const_iterator pi = m.find( os.str().c_str() );
    assert( pi != m.end() );
    bool ok = true;
    int index = pi->second.toInt( &ok );
    assert( ok );
    i->index = index;
  };
}

void TextLabel::draw(KigPainter& p, bool ) const
{
  p.setColor( mColor );
  p.drawSimpleText( mcoord, mcurtext );
  mrwdoi = p.simpleBoundingRect( mcoord, mtext );
}

bool TextLabel::contains( const Coordinate& o, const ScreenInfo& ) const
{
  return mrwdoi.contains( o );
}

KigMode* TextLabel::sConstructMode( Type*, KigDocument* d,
                                    NormalMode* p )
{
  return new TextLabelConstructionMode( p, d );
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

void TextLabel::stopMove()
{
  // noop
}

TextLabel* TextLabel::copy()
{
  return new TextLabel( *this );
}

const TextLabel* TextLabel::toTextLabel() const
{
  return this;
}
TextLabel* TextLabel::toTextLabel()
{
  return this;
}
const QCString TextLabel::vBaseTypeName() const
{
  return sBaseTypeName();
}
QCString TextLabel::sBaseTypeName()
{
  return I18N_NOOP("label");
}
const QCString TextLabel::vFullTypeName() const
{
  return sFullTypeName();
}
const QCString TextLabel::sFullTypeName()
{
  return "TextLabel";
}
const QString TextLabel::vDescriptiveName() const
{
  return sDescriptiveName();
}
const QString TextLabel::sDescriptiveName()
{
  return i18n( "A text label" );
}
const QCString TextLabel::vIconFileName() const
{
  return sIconFileName();
}
const QCString TextLabel::sIconFileName()
{
  return 0;
}
const char* TextLabel::sActionName()
{
  return "objects_new_textlabel";
}
bool TextLabel::inRect(const Rect& ) const
{
  return false;
}
QString TextLabel::wantArg( const Object* ) const
{
  return 0;
}
bool TextLabel::selectArg(Object*)
{
  // noop
  return true;
}
void TextLabel::drawPrelim(KigPainter&, const Object* ) const
{
  // noop
}

void TextLabel::calc( const ScreenInfo& )
{
  mcurtext = mtext;
  for ( propvect::iterator i = mprops.begin(); i != mprops.end(); ++i )
  {
    assert( i->index != static_cast<uint>( -1 ) );
    Property prop = i->obj->property( i->index );
    switch( prop.type() )
    {
    case Property::Double:
      mcurtext = mcurtext.arg( prop.doubleData() );
      break;
    case Property::String:
      mcurtext = mcurtext.arg( prop.qstringData() );
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
  return i18n("A piece of text you can add to the document..");
}

TextLabel::TextLabel( const QString text, const Coordinate c, const propvect& props )
  : Object(), mprops( props ), mtext( text ), mcoord( c )
{
  for ( propvect::iterator i = mprops.begin(); i != mprops.end(); ++i )
    i->obj->addChild( this );
}
