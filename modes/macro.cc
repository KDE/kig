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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "macro.h"

#include "macrowizard.h"
#include "dragrectmode.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/kigpainter.h"
#include "../misc/object_constructor.h"
#include "../misc/lists.h"
#include "../misc/guiaction.h"
#include "../objects/object_imp.h"

#include <klineedit.h>
#include <kmessagebox.h>
#include <kcursor.h>
#include <klocale.h>

#include <functional>
#include <algorithm>

using namespace std;

DefineMacroMode::DefineMacroMode( KigPart& d )
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
  std::vector<ObjectHolder*> given( mgiven.begin(), mgiven.end() );
  static_cast<KigView*>( mdoc.widget() )->realWidget()->redrawScreen( given );
  updateNexts();
}

void DefineMacroMode::finalPageEntered()
{
  std::vector<ObjectHolder*> final( mfinal.begin(), mfinal.end() );
  static_cast<KigView*>( mdoc.widget() )->realWidget()->redrawScreen( final );

  updateNexts();
}

void DefineMacroMode::namePageEntered()
{
  ObjectCalcer* (ObjectHolder::*memfun)() = &ObjectHolder::calcer;
  std::vector<ObjectCalcer*> given;
  std::transform( mgiven.begin(), mgiven.end(),
                  std::back_inserter( given ),
                  std::mem_fun( memfun ) );
  std::vector<ObjectCalcer*> final;
  std::transform( mfinal.begin(), mfinal.end(),
                  std::back_inserter( final ),
                  std::mem_fun( memfun ) );
  ObjectHierarchy hier( given, final );
  if ( hier.resultDoesNotDependOnGiven() )
  {
    KMessageBox::sorry( mwizard,
                        i18n( "One of the result objects you selected "
                              "cannot be calculated from the given objects. "
                              "Kig cannot calculate this macro because of this. "
                              "Please press Back, and construct the objects "
                              "in the correct order..." ) );
    mwizard->back();
  }
  else if( !hier.allGivenObjectsUsed() )
  {
    KMessageBox::sorry( mwizard,
                        i18n( "One of the given objects is not used in the "
                              "calculation of the resultant objects.  This "
                              "probably means you are expecting Kig to do "
                              "something impossible.  Please check the "
                              "macro and try again." ) );
    mwizard->back();
  }

  static_cast<KigView*>( mdoc.widget() )->realWidget()->redrawScreen( std::vector<ObjectHolder*>() );

  updateNexts();
}

void DefineMacroMode::finishPressed()
{
  ObjectCalcer* (ObjectHolder::*memfun)() = &ObjectHolder::calcer;
  std::vector<ObjectCalcer*> given;
  std::transform( mgiven.begin(), mgiven.end(),
                  std::back_inserter( given ),
                  std::mem_fun( memfun ) );
  std::vector<ObjectCalcer*> final;
  std::transform( mfinal.begin(), mfinal.end(),
                  std::back_inserter( final ),
                  std::mem_fun( memfun ) );
  ObjectHierarchy hier( given, final );
  MacroConstructor* ctor =
    new MacroConstructor( hier,
                          mwizard->KLineEdit2->text(),
                          mwizard->KLineEdit1->text() );
  ConstructibleAction* act = new ConstructibleAction( ctor, 0 );
  MacroList::instance()->add( new Macro( act, ctor ) );

  abandonMacro();
}

void DefineMacroMode::cancelPressed()
{
  abandonMacro();
}

void DefineMacroMode::macroNameChanged()
{
  mwizard->setFinishEnabled(
    mwizard->mpname,
    !mwizard->KLineEdit2->text().isEmpty()
    );
}

void DefineMacroMode::dragRect( const QPoint& p, KigWidget& w )
{
  if ( mwizard->currentPage() == mwizard->mpname ) return;
  std::vector<ObjectHolder*>* objs = mwizard->currentPage() == mwizard->mpgiven ? &mgiven : &mfinal;
  DragRectMode dm( p, mdoc, w );
  mdoc.runMode( &dm );
  KigPainter pter( w.screenInfo(), &w.stillPix, mdoc.document() );
  if ( ! dm.cancelled() )
  {
    std::vector<ObjectHolder*> ret = dm.ret();
    if ( dm.needClear() )
    {
      pter.drawObjects( objs->begin(), objs->end(), false );
      objs->clear();
    }

    std::copy( ret.begin(), ret.end(), std::back_inserter( *objs ) );
    pter.drawObjects( objs->begin(), objs->end(), true );
  };
  w.updateCurPix( pter.overlay() );
  w.updateWidget();

  updateNexts();
}

void DefineMacroMode::leftClickedObject( ObjectHolder* o, const QPoint&,
                                         KigWidget& w, bool )
{
  if ( mwizard->currentPage() == mwizard->mpname ) return;
  std::vector<ObjectHolder*>* objs = mwizard->currentPage() == mwizard->mpgiven ? &mgiven : &mfinal;
  std::vector<ObjectHolder*>::iterator iter = std::find( objs->begin(), objs->end(), o );
  bool isselected = ( iter != objs->end() );
  if ( isselected ) objs->erase( iter );
  else objs->push_back( o );

  KigPainter p( w.screenInfo(), &w.stillPix, mdoc.document() );
  p.drawObject( o, !isselected );
  w.updateCurPix( p.overlay() );
  w.updateWidget();

  updateNexts();
}

void DefineMacroMode::mouseMoved( const std::vector<ObjectHolder*>& os, const QPoint& pt, KigWidget& w, bool )
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
    QString selectstat = os.front()->selectStatement();

    // statusbar text
    mdoc.emitStatusBarText( selectstat );
    KigPainter p( w.screenInfo(), &w.curPix, mdoc.document() );

    // set the text next to the arrow cursor
    QPoint point = pt;
    point.setX(point.x()+15);

    p.drawTextStd( point, selectstat );
    w.updateWidget( p.overlay() );
  }
}

void DefineMacroMode::rightClicked( const std::vector<ObjectHolder*>&, const QPoint&, KigWidget& )
{
}

void DefineMacroMode::midClicked( const QPoint&, KigWidget& )
{
}

