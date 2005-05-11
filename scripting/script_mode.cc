// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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

#include "script_mode.h"

#include "newscriptwizard.h"
#include "python_type.h"
#include "python_scripter.h"

#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/kigpainter.h"
#include "../modes/dragrectmode.h"
#include "../objects/bogus_imp.h"
#include "../objects/object_imp.h"

#include <qlabel.h>

#include <kcursor.h>
#include <kiconloader.h>
#include <kmessagebox.h>

void ScriptMode::dragRect( const QPoint& p, KigWidget& w )
{
  if ( mwawd != SelectingArgs ) return;

  DragRectMode dm( p, mdoc, w );
  mdoc.runMode( &dm );
  std::vector<ObjectHolder*> ret = dm.ret();

  KigPainter pter( w.screenInfo(), &w.stillPix, mdoc.document() );
  if ( dm.needClear() )
  {
    std::vector<ObjectHolder*> tmp( margs.begin(), margs.begin() );
    pter.drawObjects( tmp, false );
    margs.clear();
  }

  std::copy( ret.begin(), ret.end(), std::inserter( margs, margs.begin() ) );
  pter.drawObjects( ret, true );

  w.updateCurPix( pter.overlay() );
  w.updateWidget();
}

void ScriptMode::leftClickedObject( ObjectHolder* o, const QPoint&,
                                    KigWidget& w, bool )
{
  if ( mwawd != SelectingArgs ) return;

  KigPainter pter( w.screenInfo(), &w.stillPix, mdoc.document() );

  if ( margs.find( o ) != margs.end() )
  {
    margs.erase( o );
    pter.drawObject( o, false );
  }
  else
  {
    margs.insert( o );
    pter.drawObject( o, true );
  };
  w.updateCurPix( pter.overlay() );
  w.updateWidget();
}

void ScriptMode::mouseMoved( const std::vector<ObjectHolder*>& os, const QPoint& pt, KigWidget& w, bool )
{
  if ( mwawd != SelectingArgs ) return;

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

ScriptMode::ScriptMode( KigPart& doc )
  : BaseMode( doc ), mwizard( 0 ), mpart( doc ),
    mwawd( SelectingArgs )
{
  mwizard = new NewScriptWizard( doc.widget(), this );
  mwizard->show();

  doc.redrawScreen();
}

ScriptMode::~ScriptMode()
{
}

void ScriptMode::killMode()
{
  mdoc.doneMode( this );
}

bool ScriptMode::queryCancel()
{
  killMode();
  return true;
}

void ScriptMode::argsPageEntered()
{
  mwawd = SelectingArgs;
  mdoc.redrawScreen();
}

void ScriptMode::enableActions()
{
  KigMode::enableActions();
  // we don't enable any actions..
}

void ScriptMode::codePageEntered()
{
  if ( mwizard->text().isEmpty() )
  {
    // insert template code..
    QString tempcode = ScriptType::templateCode( mtype, margs.size() );
    mwizard->setText( tempcode );
  };
  mwizard->setFinishEnabled( mwizard->mpcode, true );
  mwawd = EnteringCode;
  mdoc.redrawScreen();
}

void ScriptMode::redrawScreen( KigWidget* w )
{
  std::vector<ObjectHolder*> sel;
  if ( mwawd == SelectingArgs )
    sel = std::vector<ObjectHolder*>( margs.begin(), margs.end() );
  w->redrawScreen( sel );
  w->updateScrollBars();
}

bool ScriptMode::queryFinish()
{
  std::vector<ObjectCalcer*> args;

  QString script = mwizard->text();
  args.push_back( new ObjectConstCalcer( new StringImp( script ) ) );

  ObjectTypeCalcer* compiledscript =
    new ObjectTypeCalcer( PythonCompileType::instance(), args );
  compiledscript->calc( mdoc.document() );

  args.clear();
  args.push_back( compiledscript );
  for ( std::set<ObjectHolder*>::iterator i = margs.begin();
        i != margs.end(); ++i )
    args.push_back( ( *i )->calcer() );

  ObjectTypeCalcer::shared_ptr reto =
    new ObjectTypeCalcer( PythonExecuteType::instance(), args );
  reto->calc( mdoc.document() );

  if ( reto->imp()->inherits( InvalidImp::stype() ) )
  {
    PythonScripter* inst = PythonScripter::instance();
    QCString errtrace = inst->lastErrorExceptionTraceback().c_str();
    if ( inst->errorOccurred() )
    {
      KMessageBox::detailedSorry(
        mwizard, i18n( "The Python interpreter caught an error during the execution of your "
                       "script. Please fix the script and click the Finish button again." ),
        i18n( "The Python Interpreter generated the following error output:\n%1").arg( errtrace ) );
    }
    else
    {
      KMessageBox::sorry(
        mwizard, i18n( "There seems to be an error in your script. The Python interpreter "
                       "reported no errors, but the script does not generate "
                       "a valid object. Please fix the script, and click the Finish button "
                       "again." ) );
    }
    return false;
  }
  else
  {
    mdoc.addObject( new ObjectHolder( reto.get() ) );
    killMode();
    return true;
  }
}

void ScriptMode::midClicked( const QPoint&, KigWidget& )
{
}

void ScriptMode::rightClicked( const std::vector<ObjectHolder*>&, const QPoint&, KigWidget& )
{
}

void ScriptMode::setScriptType( ScriptType::Type type )
{
  mtype = type;
  mwizard->setType( mtype );
  if ( mtype != ScriptType::Unknown )
  {
    KIconLoader* il = mpart.instance()->iconLoader();
    mwizard->setIcon( il->loadIcon( ScriptType::icon( mtype ), KIcon::Small ) );
  }
}

void ScriptMode::addArgs( const std::vector<ObjectHolder*>& obj, KigWidget& w )
{
  KigPainter pter( w.screenInfo(), &w.stillPix, mdoc.document() );

  std::copy( obj.begin(), obj.end(), std::inserter( margs, margs.begin() ) );
  pter.drawObjects( obj, true );

  w.updateCurPix( pter.overlay() );
  w.updateWidget();
}

void ScriptMode::goToCodePage()
{
  mwizard->next();
}
