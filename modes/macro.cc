// macro.cc
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

#include "macro.h"

#include "macrowizard.h"
#include "dragrectmode.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/kigpainter.h"
#include "../misc/object_constructor.h"
#include "../misc/object_constructor_list.h"
#include "../misc/i18n.h"
#include "../misc/guiaction_list.h"
#include "../misc/guiaction.h"
#include "../objects/object_imp.h"

#include <klineedit.h>
#include <kcursor.h>

DefineMacroMode::DefineMacroMode( KigDocument& d )
  : BaseMode( d )
{
  mwizard = new MacroWizard( d.widget(), this );
  mwizard->show();
  updateNexts();
}

DefineMacroMode::~DefineMacroMode()
{
  delete mwizard;
}

void DefineMacroMode::abandonMacro()
{
  mdoc.doneMode( this );
}

void DefineMacroMode::updateNexts()
{
  mwizard->setNextEnabled( mwizard->mpgiven,
                           !mgiven.empty() );
  mwizard->setNextEnabled( mwizard->mpfinal,
                           !mfinal.empty() );
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

void DefineMacroMode::givenPageEntered()
{
  using std::for_each;
  using std::bind2nd;
  using std::mem_fun;
  for_each( mdoc.objects().begin(), mdoc.objects().end(),
            bind2nd( mem_fun( &Object::setSelected ), false ) );
  for_each( mgiven.begin(), mgiven.end(),
            bind2nd( mem_fun( &Object::setSelected ), true ) );
  static_cast<KigView*>( mdoc.widget() )->realWidget()->redrawScreen();

  updateNexts();
};

void DefineMacroMode::finalPageEntered()
{
  using std::for_each;
  using std::bind2nd;
  using std::mem_fun;
  mgiven.setSelected( false );
  mfinal.setSelected( true );
  static_cast<KigView*>( mdoc.widget() )->realWidget()->redrawScreen();

  updateNexts();
}

void DefineMacroMode::namePageEntered()
{
  using std::for_each;
  using std::bind2nd;
  using std::mem_fun;
  for_each( mdoc.objects().begin(), mdoc.objects().end(),
            bind2nd( mem_fun( &Object::setSelected ), false ) );
  static_cast<KigView*>( mdoc.widget() )->realWidget()->redrawScreen();

  updateNexts();
}

void DefineMacroMode::finishPressed()
{
  MacroConstructor* ctor =
    new MacroConstructor( mgiven, mfinal,
                          mwizard->KLineEdit2->text(),
                          mwizard->KLineEdit1->text() );

  ObjectConstructorList::instance()->add( ctor );
  GUIActionList::instance()->add( new ConstructibleAction( ctor, 0 ) );

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

void DefineMacroMode::dragRect( const QPoint& p, KigWidget& w )
{
  if ( mwizard->currentPage() == mwizard->mpname ) return;
  Objects* objs = mwizard->currentPage() == mwizard->mpgiven ? &mgiven : &mfinal;
  // the objects that we change..
  Objects cos;
  DragRectMode dm( p, mdoc, w );
  mdoc.runMode( &dm );
  Objects ret = dm.ret();
  if ( dm.needClear() )
  {
    cos = *objs;
    objs->setSelected( false );
    objs->clear();
  };
  cos |= ret;
  ret.setSelected( true );
  objs->upush( ret );

  KigPainter pter( w.screenInfo(), &w.stillPix, mdoc );
  pter.drawObjects( cos );
  w.updateCurPix( pter.overlay() );
  w.updateWidget();

  updateNexts();
}

void DefineMacroMode::leftClickedObject( Object* o, const QPoint&,
                                         KigWidget& w, bool )
{
  if ( mwizard->currentPage() == mwizard->mpname ) return;
  Objects* objs = mwizard->currentPage() == mwizard->mpgiven ? &mgiven : &mfinal;
  if ( objs->contains( o ) )
  {
    objs->remove( o );
    o->setSelected( false );
  }
  else
  {
    objs->push_back( o );
    o->setSelected( true );
  };

  KigPainter p( w.screenInfo(), &w.stillPix, mdoc );
  p.drawObject( o );
  w.updateCurPix( p.overlay() );
  w.updateWidget();

  updateNexts();
}

void DefineMacroMode::mouseMoved( const Objects& os, const QPoint& pt, KigWidget& w )
{
  w.updateCurPix();

  if ( os.empty() )
  {
    w.setCursor( KCursor::arrowCursor() );
    mdoc.emitStatusBarText( 0 );
    w.updateWidget();
  }
  else
  {
    // the cursor is over an object, show object type next to cursor
    // and set statusbar text

    w.setCursor( KCursor::handCursor() );
    QString selectstat = ObjectImp::selectStatement( os.front()->imp()->id() );

    // statusbar text
    mdoc.emitStatusBarText( selectstat );
    KigPainter p( w.screenInfo(), &w.curPix, mdoc );

    // set the text next to the arrow cursor
    QPoint point = pt;
    point.setX(point.x()+15);

    p.drawTextStd( point, selectstat );
    w.updateWidget( p.overlay() );
  }
}

void DefineMacroMode::rightClicked( const Objects&, const QPoint&, KigWidget& )
{
}

void DefineMacroMode::midClicked( const QPoint&, KigWidget& )
{
}

