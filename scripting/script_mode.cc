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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "script_mode.h"

#include "newscriptwizard.h"
#include "python_type.h"
#include "python_scripter.h"

#include "../kig/kig_commands.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/calcpaths.h"
#include "../misc/kigpainter.h"
#include "../modes/dragrectmode.h"
#include "../objects/bogus_imp.h"
#include "../objects/object_imp.h"

#include <qlabel.h>
#include <qpushbutton.h>

#include <kcursor.h>
#include <kiconloader.h>
#include <kmessagebox.h>

void ScriptModeBase::dragRect( const QPoint& p, KigWidget& w )
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

void ScriptModeBase::leftClickedObject( ObjectHolder* o, const QPoint&,
                                        KigWidget& w, bool )
{
  std::list<ObjectHolder*>::iterator dup_o;

  if ( mwawd != SelectingArgs ) return;

  KigPainter pter( w.screenInfo(), &w.stillPix, mdoc.document() );

  if ( (dup_o = std::find( margs.begin(), margs.end(), o )) != margs.end() )
  {
    margs.erase( dup_o );
    pter.drawObject( o, false );
  }
  else
  {
    margs.push_back( o );
    pter.drawObject( o, true );
  };
  w.updateCurPix( pter.overlay() );
  w.updateWidget();
}

void ScriptModeBase::mouseMoved( const std::vector<ObjectHolder*>& os,
                                 const QPoint& pt, KigWidget& w, bool )
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

ScriptModeBase::ScriptModeBase( KigPart& doc )
  : BaseMode( doc ), mwizard( 0 ), mpart( doc ),
    mwawd( SelectingArgs )
{
  mwizard = new NewScriptWizard( doc.widget(), this );

  doc.redrawScreen();
}

ScriptModeBase::~ScriptModeBase()
{
}

void ScriptModeBase::killMode()
{
  mdoc.doneMode( this );
}

bool ScriptCreationMode::queryCancel()
{
  killMode();
  return true;
}

void ScriptModeBase::argsPageEntered()
{
  mwawd = SelectingArgs;
  mdoc.redrawScreen();
}

void ScriptModeBase::enableActions()
{
  KigMode::enableActions();
  // we don't enable any actions..
}

void ScriptModeBase::codePageEntered()
{
  if ( mwizard->text().isEmpty() )
  {
    // insert template code..
    QString tempcode = ScriptType::templateCode( mtype, margs );
    mwizard->setText( tempcode );
  };
  mwizard->setFinishEnabled( mwizard->mpcode, true );
  mwawd = EnteringCode;
  mdoc.redrawScreen();
}

void ScriptModeBase::redrawScreen( KigWidget* w )
{
  std::vector<ObjectHolder*> sel;
  if ( mwawd == SelectingArgs )
    sel = std::vector<ObjectHolder*>( margs.begin(), margs.end() );
  w->redrawScreen( sel );
  w->updateScrollBars();
}

bool ScriptCreationMode::queryFinish()
{
  std::vector<ObjectCalcer*> args;

  QString script = mwizard->text();
  args.push_back( new ObjectConstCalcer( new StringImp( script ) ) );

  ObjectTypeCalcer* compiledscript =
    new ObjectTypeCalcer( PythonCompileType::instance(), args );
  compiledscript->calc( mdoc.document() );

  args.clear();
  args.push_back( compiledscript );
  for ( std::list<ObjectHolder*>::iterator i = margs.begin();
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

void ScriptModeBase::midClicked( const QPoint&, KigWidget& )
{
}

void ScriptModeBase::rightClicked( const std::vector<ObjectHolder*>&,
                                   const QPoint&, KigWidget& )
{
}

void ScriptModeBase::setScriptType( ScriptType::Type type )
{
  mtype = type;
  mwizard->setType( mtype );
  if ( mtype != ScriptType::Unknown )
  {
    KIconLoader* il = mpart.instance()->iconLoader();
    mwizard->setIcon( il->loadIcon( ScriptType::icon( mtype ), KIcon::Small ) );
  }
}

void ScriptModeBase::addArgs( const std::vector<ObjectHolder*>& obj, KigWidget& w )
{
  KigPainter pter( w.screenInfo(), &w.stillPix, mdoc.document() );

  std::copy( obj.begin(), obj.end(), std::inserter( margs, margs.begin() ) );
  pter.drawObjects( obj, true );

  w.updateCurPix( pter.overlay() );
  w.updateWidget();
}

void ScriptModeBase::goToCodePage()
{
  mwizard->next();
}

ScriptCreationMode::ScriptCreationMode( KigPart& doc )
  : ScriptModeBase( doc )
{
  mwizard->show();
}

ScriptCreationMode::~ScriptCreationMode()
{
}

ScriptEditMode::ScriptEditMode( ObjectTypeCalcer* exec_calc, KigPart& doc )
  : ScriptModeBase( doc ), mexecuted( exec_calc )
{
  mwawd = EnteringCode;

  mexecargs = mexecuted->parents();
  assert( mexecargs.size() >= 1 );

  mcompiledargs = mexecargs[0]->parents();
  assert( mcompiledargs.size() == 1 );

  const ObjectImp* imp = static_cast<ObjectConstCalcer*>( mcompiledargs[0] )->imp();
  assert( dynamic_cast<const StringImp*>( imp ) );
  // save the original script text, in case the user modifies the text
  // in the editor and aborts the editing
  morigscript = static_cast<const StringImp*>( imp )->data();

  mwizard->setCaption( i18n( "'Edit' is a verb", "Edit Script" ) );
  mwizard->setText( morigscript );
  mwizard->show();
  mwizard->next();
  mwizard->backButton()->setEnabled( false );
  mwizard->finishButton()->setEnabled( true );
}

ScriptEditMode::~ScriptEditMode()
{
}

bool ScriptEditMode::queryFinish()
{
  MonitorDataObjects mon( mcompiledargs );

  static_cast<ObjectConstCalcer*>( mcompiledargs[0] )->switchImp( new StringImp( mwizard->text() ) );
  mexecargs[0]->calc( mpart.document() );

  mexecuted->calc( mpart.document() );

  mpart.redrawScreen();

  KigCommand* comm = new KigCommand( mpart, i18n( "Edit Python Script" ) );
  mon.finish( comm );

  if ( mexecuted->imp()->inherits( InvalidImp::stype() ) )
  {
    PythonScripter* inst = PythonScripter::instance();
    QCString errtrace = inst->lastErrorExceptionTraceback().c_str();
    if ( inst->errorOccurred() )
    {
      KMessageBox::detailedSorry(
        mpart.widget(), i18n( "The Python interpreter caught an error during the execution of your "
                              "script. Please fix the script." ),
        i18n( "The Python Interpreter generated the following error output:\n%1").arg( errtrace ) );
    }
    else
    {
      KMessageBox::sorry(
        mpart.widget(), i18n( "There seems to be an error in your script. The Python interpreter "
                              "reported no errors, but the script does not generate "
                              "a valid object. Please fix the script." ) );
    }
    delete comm;
    return false;
  }

  mpart.history()->addCommand( comm );
  mpart.setModified( true );

  killMode();
  return true;
}

bool ScriptEditMode::queryCancel()
{
  // reverting the original script text
  static_cast<ObjectConstCalcer*>( mcompiledargs[0] )->switchImp( new StringImp( morigscript ) );
  mexecargs[0]->calc( mpart.document() );

  mexecuted->calc( mpart.document() );
  // paranoic check
  assert( !mexecuted->imp()->inherits( InvalidImp::stype() ) );

  mpart.redrawScreen();

  // no need to further checks here, as the original script text is ok

  killMode();
  return true;
}

