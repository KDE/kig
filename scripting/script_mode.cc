// script_mode.cc
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

#include "../modes/dragrectmode.h"
#include "../objects/object_imp.h"
#include "../objects/bogus_imp.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/kigpainter.h"

#include <kcursor.h>
#include <kmessagebox.h>
#include <ktextedit.h>

void ScriptMode::dragRect( const QPoint& p, KigWidget& w )
{
  if ( mwawd != SelectingArgs ) return;

  DragRectMode dm( p, mdoc, w );
  mdoc.runMode( &dm );
  Objects ret = dm.ret();
  Objects cos;
  if ( dm.needClear() )
  {
    cos = margs;
    margs.setSelected( false );
    margs.clear();
  };

  cos.upush( ret );
  margs.upush( ret );
  ret.setSelected( true );

  KigPainter pter( w.screenInfo(), &w.stillPix, mdoc );
  pter.drawObjects( cos );
  w.updateCurPix( pter.overlay() );
  w.updateWidget();
}

void ScriptMode::leftClickedObject( Object* o, const QPoint&,
                                    KigWidget& w, bool )
{
  if ( mwawd != SelectingArgs ) return;

  if ( margs.contains( o ) )
  {
    margs.remove( o );
    o->setSelected( false );
  }
  else
  {
    margs.push_back( o );
    o->setSelected( true );
  };
  w.redrawScreen();
}

void ScriptMode::mouseMoved( const Objects& os, const QPoint& pt, KigWidget& w, bool )
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
    QString selectstat = os.front()->imp()->type()->selectStatement();

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

ScriptMode::ScriptMode( KigDocument& doc )
  : BaseMode( doc ), mwizard( 0 ),
    mwawd( SelectingArgs )
{
  mwizard = new NewScriptWizard( doc.widget(), this );
  mwizard->show();

  Objects docos = mdoc.objects();
  docos.setSelected( false );
  redrawScreen();
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
  margs.setSelected( true );
  redrawScreen();
}

void ScriptMode::enableActions()
{
  KigMode::enableActions();
  // we don't enable any actions..
}

void ScriptMode::codePageEntered()
{
  if ( mwizard->codeeditor->text().isEmpty() )
  {
    // insert template code..
    QString tempcode = QString::fromLatin1( "def calc(" );
    bool firstarg = true;

    for ( uint i = 0; i < margs.size(); ++i )
    {
      if ( !firstarg ) tempcode += ',';
      firstarg = false;
      tempcode += ' ';
      tempcode += i18n( "Note to translators: this should be a default "
                        "name for an argument in a python "
                        "function.  The default is \"arg%1\" which would "
                        "become arg1, arg2 etc.  Give something which "
                        "seems appropriate for your language.", "arg%1" )
                  .arg( i + 1 );
    };
    if ( !firstarg ) tempcode += ' ';
    tempcode +=
      "):\n"
      "\t# calculate whatever you want to show here, and return it..\n"
      "\t# for example, to implement a mid point, you would put this \n"
      "\t# code here:\n"
      "\t#\treturn Point( ( arg1.coordinate() + arg2.coordinate() ) / 2 )\n"
      "\t# Please refer to the manual for more information..\n"
      "\t\n";
    mwizard->codeeditor->setText( tempcode );
  };
  mwizard->setFinishEnabled( mwizard->mpcode, true );
  mwawd = EnteringCode;
  Objects docos = mdoc.objects();
  docos.setSelected( false );
  redrawScreen();
}

void ScriptMode::redrawScreen()
{
  const std::vector<KigWidget*>& widgets = mdoc.widgets();
  for ( uint i = 0; i < widgets.size(); ++i )
  {
    KigWidget* w = widgets[i];
    w->redrawScreen();
    w->updateScrollBars();
  };
}

bool ScriptMode::queryFinish()
{
  Objects ret;

  QString script = mwizard->codeeditor->text();
  Object* scripto = new DataObject( new StringImp( script ) );
  ret.push_back( scripto );

  Object* compiledscript =
    new RealObject( PythonCompileType::instance(), Objects( scripto ) );
  compiledscript->calc( mdoc );

  Objects args;
  args.push_back( compiledscript );
  copy( margs.begin(), margs.end(), back_inserter( args ) );

  Object* reto = new RealObject( PythonExecuteType::instance(), args );
  ReferenceObject ref( reto );
  reto->calc( mdoc );

  if ( reto->imp()->inherits( InvalidImp::stype() ) )
  {
    // TODO: show the error from the python interpreter to the user (
    // how ?? :) )
    KMessageBox::sorry(
      mwizard, i18n( "There seems to be an error in your script; it will not generate "
                     "a valid object.  Please fix the script, and press the Finish button "
                     "again." ) );
    return false;
  }
  else
  {
    mdoc.addObject( reto );
    killMode();
    return true;
  }
}

void ScriptMode::midClicked( const QPoint&, KigWidget& )
{
}

void ScriptMode::rightClicked( const Objects&, const QPoint&, KigWidget& )
{
}

