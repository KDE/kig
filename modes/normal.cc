// normal.cc
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

#include "normal.h"

#include "../kig/kig_view.h"
#include "../kig/kig_part.h"
#include "../objects/object_factory.h"
#include "../objects/object_imp.h"
#include "../misc/kigpainter.h"
#include "../misc/i18n.h"
#include "popup.h"
#include "moving.h"
#include "macro.h"
#include "dragrectmode.h"
#include "typesdialog.h"

#include <kcursor.h>
#include <kaction.h>
#include <kcommand.h>

void NormalMode::enableActions()
{
  KigMode::enableActions();
  mdoc.enableConstructActions( true );
  mdoc.aDeleteObjects->setEnabled( true );
  mdoc.aShowHidden->setEnabled( true );
  mdoc.aNewMacro->setEnabled( true );
  mdoc.aConfigureTypes->setEnabled( true );
  mdoc.history()->updateActions();
}

void NormalMode::deleteObjects()
{
  mdoc.delObjects( sos );
  sos.clear();
}

void NormalMode::selectObject( Object* o )
{
  assert( o->inherits( Object::ID_RealObject ) );
  sos.push_back( o );
  static_cast<RealObject*>( o )->setSelected( true );
}

void NormalMode::selectObjects( Objects& os )
{
  // hehe, don't you love this c++ stuff ;)
  std::for_each( os.begin(), os.end(),
                 std::bind1st(
                   std::mem_fun( &NormalMode::selectObject ),
                   this ) );
}

void NormalMode::unselectObject( Object* o )
{
  assert( o->inherits( Object::ID_RealObject ) );
  static_cast<RealObject*>( o )->setSelected( false );
  sos.remove( o );
}

void NormalMode::clearSelection()
{
  for ( Objects::iterator i = sos.begin(); i != sos.end(); ++i )
  {
    assert( (*i)->inherits( Object::ID_RealObject ) );
    static_cast<RealObject*>(*i)->setSelected( false );
  };
  sos.clear();
}

// KigObjectsPopup* NormalMode::popup( const Objects& )
// {
//   return 0;
// }

// KigDocumentPopup* NormalMode::popup( KigDocument* )
// {
//   return 0;
// }

void NormalMode::showHidden()
{
  const Objects& os = mdoc.objects();
  for (Objects::const_iterator i = os.begin(); i != os.end(); ++i )
  {
    assert( (*i)->inherits( Object::ID_RealObject ) );
    static_cast<RealObject*>(*i)->setShown( true );
  };
  objectsAdded();
}

void NormalMode::newMacro()
{
  DefineMacroMode m( mdoc );
  mdoc.runMode( &m );
}

void NormalMode::objectsAdded()
{
  KigWidget* w = mdoc.mainWidget()->realWidget();
  w->redrawScreen();
  w->updateScrollBars();
}

void NormalMode::objectsRemoved()
{
  objectsAdded();
}

void NormalMode::editTypes()
{
//   TypesDialog* d = new TypesDialog( mdoc.widget() );
//   d->exec();
//   delete d;
}

NormalMode::NormalMode( KigDocument& d )
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

  Objects sel = d.ret();
  Objects cos = sel;

  if ( d.needClear() )
  {
    cos |= sos;
    clearSelection();
  };

  selectObjects( sel );

  KigPainter pter( w.screenInfo(), &w.stillPix );
  pter.drawObjects( cos );
  w.updateCurPix( pter.overlay() );
  w.updateWidget();
}

void NormalMode::dragObject( const Objects& oco, const QPoint& pco,
                             KigWidget& w, bool ctrlOrShiftDown )
{
  // first determine what to move...
  if( ( oco & sos ).empty() )
  {
    // the user clicked on something that is currently not
    // selected... --> we select it, taking the Ctrl- and
    // Shift-buttons into account...
    if ( !ctrlOrShiftDown ) clearSelection();
    selectObject(oco.front());
  }

  MovingMode m( sos, w.fromScreen( pco ), w, mdoc );
  mdoc.runMode( &m );
}

void NormalMode::leftClickedObject( Object* o, const QPoint&,
                                    KigWidget& w, bool ctrlOrShiftDown )
{
  Objects cos; // objects whose selection changed..

  if ( ! o )
  {
    cos = sos;
    clearSelection();
  }
  else if( !sos.contains( o ) )
  {
    // clicked on an object that wasn't selected....
    if (!ctrlOrShiftDown)
    {
      cos = sos;
      clearSelection();
    };
    selectObject( o );
    cos.push_back( o );
  }
  else
  {
    // clicked on an object that was selected....
    unselectObject( o );
    cos.push_back( o );
  };
  KigPainter pter( w.screenInfo(), &w.stillPix );
  pter.drawObjects( cos );
  w.updateCurPix( pter.overlay() );
  w.updateWidget();
}

void NormalMode::midClicked( const QPoint& p, KigWidget& w )
{
  Object* pt = ObjectFactory::instance()->sensiblePoint( w.fromScreen( p ), mdoc, w );
  pt->calc();
  mdoc.addObject( pt );

  // refresh the screen...
  w.redrawScreen();
  w.updateScrollBars();
}

void NormalMode::rightClicked( const Objects& os,
                               const QPoint&,
                               KigWidget& )
{
  // TODO
  if( !os.empty() )
  {
    if( !sos.contains( os.front() ) )
    {
      clearSelection();
      selectObject( oco.front() );
    };
//     // show a popup menu...
//     NormalModePopupObjects* p = new NormalModePopupObjects( mdoc, v, this, sos );
//     p->exec( QCursor::pos() );
//     delete p;
  }
  else
  {
//     KigDocumentPopup* m = popup( mdoc );
//     if( m ) m->exec( w.mapToGlobal( plc ) );
  };
}

void NormalMode::mouseMoved( const Objects& os,
                             const QPoint& plc,
                             KigWidget& w )
{
  w.updateCurPix();
  if( os.empty() )
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

    QString stat = i18n( ObjectImp::selectStatement( os.front()->imp()->id() ) );

    // statusbar text
    mdoc.emitStatusBarText( stat );
    KigPainter p( w.screenInfo(), &w.curPix );

    // set the text next to the arrow cursor
    QPoint point = plc;
    point.setX(point.x()+15);

    p.drawTextStd( point, stat );
    w.updateWidget( p.overlay() );
  };
}
