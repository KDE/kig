// label.cc
// Copyright (C)  2002  Dominique Devriese <fritmebufstek@pandora.be>

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

#include "../modes/constructing.h"

TextLabel::TextLabel( const Objects& os )
  : Object()
{
  assert( os.empty() );
}
TextLabel::TextLabel( const TextLabel& l )
  : Object( l ), mtext( l.mtext ), mcoord( l.mcoord )
{
}
TextLabel::~TextLabel()
{
}
std::map<QCString,QString> TextLabel::getParams()
{
  std::map<QCString, QString> ret;
  ret["text"] = mtext;
  ret["coordinate-x"] = mcoord.x;
  ret["coordinate-y"] = mcoord.y;
  return ret;
}
void TextLabel::setParams( const std::map<QCString,QString>& m )
{
  mtext = m.find("text")->second;
  bool ok = true;
  mcoord = Coordinate(
    m.find("coordinate-x")->second.toDouble( &ok ),
    m.find("coordinate-y")->second.toDouble( &ok ) );
  Q_ASSERT( ok );
}
void TextLabel::draw(KigPainter& p, bool ) const
{
  kdDebug() << k_funcinfo << endl;
  p.drawSimpleText( mcoord, mtext );
  mrwdoi = p.simpleBoundingRect( mcoord, mtext );
}
bool TextLabel::contains( const Coordinate& o, const double ) const
{
  return mrwdoi.contains( o );
}

KigMode* TextLabel::sConstructMode( Type*, KigDocument* d,
                                    NormalMode* p )
{
  return new TextLabelConstructionMode( p, d );
}

void TextLabel::startMove( const Coordinate& c )
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
  // noop
}

Objects TextLabel::getParents() const
{
  return Objects();
}
const QString TextLabel::vDescription() const
{
  return sDescription();
}
const QString TextLabel::sDescription()
{
  return i18n("A piece of text you can add to the document..");
}

TextLabel::TextLabel( const QString text, const Coordinate c )
  : Object(), mtext( text ), mcoord( c )
{
}
