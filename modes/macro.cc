// macro.cc
// Copyright (C)  2002  Dominique Devriese <dominique.devriese@student.kuleuven.ac.be>

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

#include "macro.h"

#include "macrowizard.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/kigpainter.h"
#include "../misc/hierarchy.h"
#include "../misc/type.h"

#include <klineedit.h>
#include <kcursor.h>

QString i18n( const char* );

DefineMacroMode::DefineMacroMode( KigDocument* d, KigMode* p )
  : KigMode( d ), mprev( p ), mfinal( 0 )
{
  mwizard = new MacroWizard( d->widget(), this );
  mwizard->show();
  updateNexts();
}

DefineMacroMode::~DefineMacroMode()
{
  delete mwizard;
}

void DefineMacroMode::leftClicked( QMouseEvent* e, KigWidget* )
{
  plc = e->pos();
}

void DefineMacroMode::abandonMacro()
{
  KigMode* m = mprev;
  KigDocument* d = mDoc;
  delete this;
  d->setMode( m );
}

void DefineMacroMode::updateNexts()
{
  mwizard->setNextEnabled( mwizard->mpgiven,
                           !mgiven.empty() );
  mwizard->setNextEnabled( mwizard->mpfinal,
                           mfinal );
  mwizard->setFinishEnabled(
    mwizard->mpname,
    !mwizard->KLineEdit2->text().isEmpty()
    );
}

void DefineMacroMode::enableActions()
{
  KigMode::enableActions();
  // we don't enable any actions...
}

void DefineMacroMode::leftReleased( QMouseEvent* e, KigWidget* v )
{
  if ( (plc - e->pos()).manhattanLength() > 4 ) return;
  Objects os = mDoc->whatAmIOn( v->fromScreen( plc ), 4*v->pixelWidth() );
  if ( os.empty() ) return;
  if( mwizard->currentPage() == mwizard->mpgiven )
  {
    if ( mgiven.contains( os.front() ) )
    {
      mgiven.remove( os.front() );
      os.front()->setSelected( false );
    }
    else
    {
      mgiven.push_back( os.front() );
      os.front()->setSelected( true );
    };
  }
  else if ( mwizard->currentPage() == mwizard->mpfinal )
  {
    if ( mfinal ) mfinal->setSelected( false );
    mfinal = os.front();
    mfinal->setSelected( true );
  }
  else
    return;
  v->redrawScreen();
  updateNexts();
}

void DefineMacroMode::rightClicked( QMouseEvent*, KigWidget* )
{
// TODO
}
void DefineMacroMode::rightReleased( QMouseEvent*, KigWidget* )
{
// TODO
}

void DefineMacroMode::mouseMoved( QMouseEvent* e, KigWidget* v )
{
  if ( mwizard->currentPage() == mwizard->mpname ) return;
  Coordinate c = v->fromScreen( e->pos() );
  Objects os = mDoc->whatAmIOn( c, v->pixelWidth() );
  v->updateCurPix();
  if ( os.empty() )
  {
    v->setCursor( KCursor::arrowCursor() );
    mDoc->emitStatusBarText( 0 );
    v->updateWidget();
  }
  else
  {
    v->setCursor( KCursor::handCursor() );
    QString typeName = os.front()->vTBaseTypeName();
    QString shownText = i18n( "Select this %1" ).arg( typeName );
    mDoc->emitStatusBarText( shownText );
    KigPainter p( v->screenInfo(), &v->curPix );
    p.drawTextStd( e->pos(), typeName );
    v->updateWidget( p.overlay() );
  };
}

void DefineMacroMode::givenPageEntered()
{
  using std::for_each;
  using std::bind2nd;
  using std::mem_fun;
  for_each( mDoc->objects().begin(), mDoc->objects().end(),
            bind2nd( mem_fun( &Object::setSelected ), false ) );
  for_each( mgiven.begin(), mgiven.end(),
            bind2nd( mem_fun( &Object::setSelected ), true ) );
  static_cast<KigView*>( mDoc->widget() )->realWidget()->redrawScreen();

  updateNexts();
};

void DefineMacroMode::finalPageEntered()
{
  using std::for_each;
  using std::bind2nd;
  using std::mem_fun;
  for_each( mDoc->objects().begin(), mDoc->objects().end(),
            bind2nd( mem_fun( &Object::setSelected ), false ) );
  if ( mfinal ) mfinal->setSelected( true );
  static_cast<KigView*>( mDoc->widget() )->realWidget()->redrawScreen();

  updateNexts();
}

void DefineMacroMode::namePageEntered()
{
  using std::for_each;
  using std::bind2nd;
  using std::mem_fun;
  for_each( mDoc->objects().begin(), mDoc->objects().end(),
            bind2nd( mem_fun( &Object::setSelected ), false ) );
  static_cast<KigView*>( mDoc->widget() )->realWidget()->redrawScreen();

  updateNexts();
}

void DefineMacroMode::finishPressed()
{
  ObjectHierarchy* hier = new ObjectHierarchy( mgiven, Objects( mfinal ) );
  MType* type = new MType( hier, mwizard->KLineEdit2->text(),
                           mwizard->KLineEdit1->text() );
  Object::addUserType( type );
  abandonMacro();
}

void DefineMacroMode::cancelPressed()
{
  abandonMacro();
}

void DefineMacroMode::macroNameChanged()
{
  updateNexts();
}
