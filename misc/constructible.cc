// constructible.cc
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

#include "constructible.h"

#include "i18n.h"

const QString StandardConstructorBase::descriptiveName() const
{
  return i18n( mdescname );
}

const QString StandardConstructorBase::description() const
{
  return i18n( mdesc );
}

const QCString StandardConstructorBase::iconFileName() const
{
  return miconfile;
}

StandardConstructorBase::StandardConstructorBase( const char[] descname,
                                                  const char[] desc,
                                                  const char[] iconfile,
                                                  const ArgsParser::spec specs[] )
  : mdescname( descname ),
    mdesc( desc ),
    miconfile( iconfile ),
    margsparser( specs )
{
}

const int StandardConstructorBase::wantArgs( const Objects& os,
                                             const KigDocument&,
                                             const KigView&,
                                             const KigMode& ) const
{
  return margsparser.check( os );
}

void StandardConstructorBase::handleArgs(
  const Object& os, const KigDocument& d,
  const KigWidget& v ) const
{
  Objects args = margsparser.parse( os );
  Objects bos = build( args, d, v );
  bos.calcForWidget( v );
  d.addObjects( bos );
  v.redrawScreen();
}

void StandardConstructorBase::handlePrelim(
  const Objects& os, const Objects& sel,
  const KigDocument& d, const KigWidget& v,
  const KigMode& curmode
  ) const
{
  v->updateCurPix();
  KigPainter p( v.screenInfo(), &v.curPix );

  if ( !os.empty() && margsparser.check( sel.with( os.front() ) ) & ArgsParser::Valid )
  {
    drawprelim( p, sel.with( os.front() ) );

    QString s = usetext( sel, os.front() );

    d.emitStatusBarText( s );

    QPoint point = e->pos();
    point.setX(point.x()+15);
    p.drawTextStd( point, o );

    v.setCursor( KCursor::handCursor() );
  }
  else
  {
    // nothing useful under the cursor..
    // we just tell the user what he's constructing...
    QString o = i18n( "Constructing a %1" ).arg( i18n( basetypename() ) );
    d.emitStatusBarText( o );
    p.drawTextStd( point , o );
    v.setCursor( KCursor::arrowCursor() );
  };
  v.updateWidget( p.overlay() );
}
