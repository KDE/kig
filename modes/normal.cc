// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "normal.h"

#include "../kig/kig_view.h"
#include "../kig/kig_part.h"
#include "../kig/kig_document.h"
#include "../kig/kig_commands.h"
#include "../objects/object_factory.h"
#include "../objects/object_imp.h"
#include "../objects/object_drawer.h"
#include "../misc/kigpainter.h"
#include "popup/popup.h"
#include "popup/objectchooserpopup.h"
#include "moving.h"
#include "macro.h"
#include "dragrectmode.h"
#include "historydialog.h"
#include "typesdialog.h"

#include <QUndoStack>
#include <KCursor>
#include <QAction>

#include <algorithm>
#include <functional>
#include <iterator>

using namespace std;

void NormalMode::enableActions()
{
  KigMode::enableActions();
  mdoc.enableConstructActions( true );
  mdoc.aSelectAll->setEnabled( true );
  mdoc.aDeselectAll->setEnabled( true );
  mdoc.aInvertSelection->setEnabled( true );
  mdoc.aDeleteObjects->setEnabled( true );
  mdoc.aShowHidden->setEnabled( true );
  mdoc.aNewMacro->setEnabled( true );
  mdoc.aConfigureTypes->setEnabled( true );
  mdoc.aBrowseHistory->setEnabled( true );
  mdoc.action( "edit_undo" )->setEnabled( mdoc.history()->canUndo() );
  mdoc.action( "edit_redo" )->setEnabled( mdoc.history()->canRedo() );
}

void NormalMode::deleteObjects()
{
  std::vector<ObjectHolder*> sel( sos.begin(), sos.end() );
  mdoc.delObjects( sel );
  sos.clear();
}

void NormalMode::selectObject( ObjectHolder* o )
{
  sos.insert( o );
}

void NormalMode::selectObjects( const std::vector<ObjectHolder*>& os )
{
  // hehe, don't you love this c++ stuff ;)
  std::for_each( os.begin(), os.end(),
                 std::bind1st(
                   std::mem_fun( &NormalMode::selectObject ), this ) );
}

void NormalMode::unselectObject( ObjectHolder* o )
{
  sos.erase( o );
}

void NormalMode::clearSelection()
{
  sos.clear();
}

// KigDocumentPopup* NormalMode::popup( KigDocument* )
// {
//   return 0;
// }

void NormalMode::showHidden()
{
  mdoc.showObjects( mdoc.document().objects() );
}

void NormalMode::newMacro()
{
  DefineMacroMode m( mdoc );
  mdoc.runMode( &m );
}

void NormalMode::redrawScreen( KigWidget* w )
{
  // unselect removed objects..
  std::vector<ObjectHolder*> nsos;
  const std::set<ObjectHolder*> docobjs = mdoc.document().objectsSet();
  std::set_intersection( docobjs.begin(), docobjs.end(), sos.begin(), sos.end(),
                         std::back_inserter( nsos ) );
  sos = std::set<ObjectHolder*>( nsos.begin(), nsos.end() );
  w->redrawScreen( nsos, true );
  w->updateScrollBars();
}

void NormalMode::editTypes()
{
  TypesDialog d( mdoc.widget(), mdoc );
  d.exec();
}

void NormalMode::browseHistory()
{
  KigMode::enableActions();
  HistoryDialog d( mdoc.history(), mdoc.widget() );
  d.exec();
  enableActions();
}

NormalMode::NormalMode( KigPart& d )
  : BaseMode( d )
{
}

NormalMode::~NormalMode()
{
}

void NormalMode::dragRect( const QPoint& p, KigWidget& w )
{
  DragRectMode d( p, mdoc, w );
  mdoc.runMode( &d );

  KigPainter pter( w.screenInfo(), &w.stillPix, mdoc.document() );

  if ( ! d.cancelled() )
  {
    std::vector<ObjectHolder*> sel = d.ret();

    if ( d.needClear() )
    {
      pter.drawObjects( sos.begin(), sos.end(), false );
      clearSelection();
    };

    selectObjects( sel );
    pter.drawObjects( sel, true );
  };

  w.updateCurPix( pter.overlay() );
  w.updateWidget();
}

void NormalMode::dragObject( const std::vector<ObjectHolder*>& oco, const QPoint& pco,
                             KigWidget& w, bool ctrlOrShiftDown )
{
  // first determine what to move...
  if( sos.find( oco.front() ) == sos.end() )
  {
    // the user clicked on something that is currently not
    // selected... --> we select it, taking the Ctrl- and
    // Shift-buttons into account...
    if ( !ctrlOrShiftDown ) clearSelection();
    selectObject(oco.front());
  }

  std::vector<ObjectHolder*> sosv( sos.begin(), sos.end() );
  MovingMode m( sosv, w.fromScreen( pco ), w, mdoc );
  mdoc.runMode( &m );
}

void NormalMode::leftClickedObject( ObjectHolder* o, const QPoint&,
                                    KigWidget& w, bool ctrlOrShiftDown )
{
  KigPainter pter( w.screenInfo(), &w.stillPix, mdoc.document() );

  if ( ! o )
  {
    pter.drawObjects( sos.begin(), sos.end(), false );
    clearSelection();
  }
  else if( sos.find( o ) == sos.end() )
  {
    // clicked on an object that wasn't selected....
    if (!ctrlOrShiftDown)
    {
      pter.drawObjects( sos.begin(), sos.end(), false );
      clearSelection();
    };
    pter.drawObject( o, true );
    selectObject( o );
  }
  else
  {
    // clicked on an object that was selected....
    pter.drawObject( o, false );
    unselectObject( o );
  };
  w.updateCurPix( pter.overlay() );
  w.updateWidget();
}

void NormalMode::midClicked( const QPoint& p, KigWidget& w )
{
  ObjectHolder* pto = ObjectFactory::instance()->sensiblePoint( w.fromScreen( p ), mdoc.document(), w );
  pto->calc( mdoc.document() );
  mdoc.addObject( pto );

  // refresh the screen...
  // not necessary, done by addObjects, which calls NormalMode::redrawScreen..
//  w.redrawScreen();
//   w.updateScrollBars();
}

void NormalMode::rightClicked( const std::vector<ObjectHolder*>& os,
                               const QPoint& plc,
                               KigWidget& w )
{
  // saving the current cursor position
  QPoint pt = QCursor::pos();
  if( !os.empty() )
  {
    ObjectHolder* o = 0;
    int id = ObjectChooserPopup::getObjectFromList( pt, &w, os );
    if ( id >= 0 )
      o = os[id];
    else
      return;
    if( sos.find( o ) == sos.end() )
    {
      clearSelection();
      selectObject( o );
    };
    // show a popup menu...
    std::vector<ObjectHolder*> sosv( sos.begin(), sos.end() );
    NormalModePopupObjects p( mdoc, w, *this, sosv, plc );
    p.exec( pt );
  }
  else
  {
    NormalModePopupObjects p( mdoc, w, *this, std::vector<ObjectHolder*>(), plc );
    p.exec( pt );
  };
}

void NormalMode::mouseMoved( const std::vector<ObjectHolder*>& os,
                             const QPoint& plc,
                             KigWidget& w,
                             bool )
{
  w.updateCurPix();
  if( os.empty() )
  {
    w.setCursor( Qt::ArrowCursor );
    mdoc.emitStatusBarText( 0 );
    w.updateWidget();
  }
  else
  {
    // the cursor is over an object, show object type next to cursor
    // and set statusbar text

    w.setCursor( Qt::PointingHandCursor );

    int id = ObjectChooserPopup::getObjectFromList( plc, &w, os, false );
    QString stat = id == 0 ? os.front()->selectStatement() : i18n( "Which object?" );

    // statusbar text
    mdoc.emitStatusBarText( stat );
    KigPainter p( w.screenInfo(), &w.curPix, mdoc.document() );

    // set the text next to the arrow cursor
    QPoint point = plc;
    point.setX(point.x()+15);

    p.drawTextStd( point, stat );
    w.updateWidget( p.overlay() );
  };
}

void NormalMode::selectAll()
{
  const std::vector<ObjectHolder*> os = mdoc.document().objects();
  selectObjects( os );
  mdoc.redrawScreen();
}

void NormalMode::deselectAll()
{
  clearSelection();
  mdoc.redrawScreen();
}

void NormalMode::invertSelection()
{
  std::vector<ObjectHolder*> os = mdoc.document().objects();
  std::set<ObjectHolder*> oldsel = sos;
  clearSelection();
  for ( std::vector<ObjectHolder*>::const_iterator i = os.begin();
        i != os.end(); ++i )
    if ( oldsel.find( *i ) == oldsel.end() )
      sos.insert( *i );
  mdoc.redrawScreen();
}
