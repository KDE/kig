// guiaction.cc
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

#include "guiaction.h"
#include "guiaction.moc"

#include "coordinate_system.h"
#include "coordinate.h"
#include "i18n.h"
#include "object_constructor.h"

#include "../kig/kig_part.h"
#include "../modes/construct_mode.h"
#include "../modes/label.h"
#include "../objects/object_factory.h"
#include "../objects/object.h"
#include "../objects/bogus_imp.h"

#include <kiconloader.h>

int GUIAction::shortcut() const
{
  return 0;
}

GUIAction::~GUIAction()
{
}

ConstructibleAction::~ConstructibleAction()
{
}

ConstructibleAction::ConstructibleAction(
  ObjectConstructor* ctor,
  const QCString& actionname,
  int shortcut )
  : GUIAction(), mctor( ctor ), mactionname( actionname ), mshortcut( shortcut )
{
}

QString ConstructibleAction::description() const
{
  return mctor->description();
}

QCString ConstructibleAction::iconFileName() const
{
  return mctor->iconFileName();
}

QString ConstructibleAction::descriptiveName() const
{
  return mctor->descriptiveName();
}

void ConstructibleAction::act( KigDocument& d )
{
  ConstructMode m( d, mctor );
  d.runMode( &m );
}

KigGUIAction::KigGUIAction( GUIAction* act,
                            KigDocument& doc,
                            QObject* parent )
  : KAction( act->descriptiveName(),
             KGlobal::instance()->iconLoader()->loadIcon(
               act->iconFileName(), KIcon::User ),
             act->shortcut(),
             0, 0,              // no slot connection
             parent, act->actionName() ),
  mact( act ),
  mdoc( doc )
{
  setWhatsThis( act->description() );
  setToolTip( act->descriptiveName() );
}

void KigGUIAction::slotActivated()
{
  mact->act( mdoc );
}

const char* ConstructibleAction::actionName() const
{
  return mactionname;
}

ConstructPointAction::~ConstructPointAction()
{
}

QString ConstructPointAction::description() const
{
  return i18n(
    "A normal point, i.e. one that is either independent or attached "
    "to a line, circle, segment."
    );
}

QCString ConstructPointAction::iconFileName() const
{
  return "point4";
}

QString ConstructPointAction::descriptiveName() const
{
  return i18n("Point");
}

const char* ConstructPointAction::actionName() const
{
  return mactionname;
}

int ConstructPointAction::shortcut() const
{
  return Qt::Key_P;
}

void ConstructPointAction::act( KigDocument& d )
{
  PointConstructMode m( d );
  d.runMode( &m );
}

ConstructPointAction::ConstructPointAction( const char* actionname )
  : mactionname( actionname )
{
}

GUIAction* KigGUIAction::guiAction()
{
  return mact;
}

void KigGUIAction::plug( KigDocument* doc )
{
  mact->plug( doc, this );
}

void ConstructibleAction::plug( KigDocument* doc, KigGUIAction* kact )
{
  mctor->plug( doc, kact );
}

QString ConstructTextLabelAction::description() const
{
  return i18n( "Construct a text label." );
}

QCString ConstructTextLabelAction::iconFileName() const
{
  return "text";
}

QString ConstructTextLabelAction::descriptiveName() const
{
  return i18n( "Text Label" );
}

const char* ConstructTextLabelAction::actionName() const
{
  return mactionname;
}

void ConstructTextLabelAction::act( KigDocument& d )
{
  TextLabelConstructionMode m( d );
  d.runMode( &m );
}

ConstructTextLabelAction::ConstructTextLabelAction( const char* actionname )
  : mactionname( actionname )
{
}

QString AddFixedPointAction::description() const
{
  return i18n( "Construct a Point by its Coordinates" );
}

QCString AddFixedPointAction::iconFileName() const
{
  return "pointxy";
}

QString AddFixedPointAction::descriptiveName() const
{
  return i18n( "Point by Coordinates" );
}

const char* AddFixedPointAction::actionName() const
{
  return mactionname;
}

void AddFixedPointAction::act( KigDocument& doc )
{
  bool ok;
  Coordinate c = doc.coordinateSystem().getCoordFromUser(
    i18n( "Fixed Point" ),
    i18n( "Enter the coordinates for the new point." ) +
    QString::fromLatin1("\n") +
    doc.coordinateSystem().coordinateFormatNotice(),
    doc, doc.widget(), &ok );
  if ( ! ok ) return;
  Object* p = ObjectFactory::instance()->fixedPoint( c );
  p->calc( doc );
  doc.addObject( p );
}

AddFixedPointAction::AddFixedPointAction( const char* actionname )
  : mactionname( actionname )
{
}

AddFixedPointAction::~AddFixedPointAction()
{
}

void GUIAction::plug( KigDocument*, KigGUIAction* )
{
}

int ConstructibleAction::shortcut() const
{
  return mshortcut;
}

int ConstructTextLabelAction::shortcut() const
{
  return Qt::Key_B;
}

int AddFixedPointAction::shortcut() const
{
  return Qt::Key_F;
}

#if 0
TestAction::TestAction( const char* actionname )
  : mactionname( actionname )
{
}

TestAction::~TestAction()
{
}

QString TestAction::description() const
{
  return QString::fromLatin1( "Test stuff !!!" );
}

QCString TestAction::iconFileName() const
{
  return "new";
}

QString TestAction::descriptiveName() const
{
  return QString::fromLatin1( "Test stuff !!!" );
}

const char* TestAction::actionName() const
{
  return mactionname;
}

void TestAction::act( KigDocument& doc )
{
  const char* script =
    "def calc( a ):\n\treturn Point( a.coordinate() + Coordinate( 2, 0 ) )\n";
  Object* constantpoint = ObjectFactory::instance()->fixedPoint( Coordinate( -1, -1 ) );
  constantpoint->calc( doc );

  Object* codeobject = new DataObject( new StringImp( QString::fromLatin1( script ) ) );
  Object* compiledcode = new RealObject( PythonCompileType::instance(), Objects( codeobject ) );
  compiledcode->calc( doc );

  Objects args( compiledcode );
  args.push_back( constantpoint );
  Object* scriptobject = new RealObject( PythonExecuteType::instance(), args );
  scriptobject->calc( doc );

  doc.addObject( constantpoint );
  doc.addObject( scriptobject );
}

#endif // if 0 ( TestAction )

#ifdef KIG_ENABLE_PYTHON_SCRIPTING
#include "../scripting/python_type.h"
#include "../scripting/script_mode.h"

NewScriptAction::NewScriptAction( const char* actionname )
  : GUIAction(), mactionname( actionname )
{
}

NewScriptAction::~NewScriptAction()
{
}

QString NewScriptAction::description() const
{
  return i18n( "Construct a new script object." );
}

QCString NewScriptAction::iconFileName() const
{
  return "";
}

QString NewScriptAction::descriptiveName() const
{
  return i18n( "Script Object" );
}

const char* NewScriptAction::actionName() const
{
  return mactionname;
}

void NewScriptAction::act( KigDocument& doc )
{
  ScriptMode m( doc );
  doc.runMode( &m );
}

int NewScriptAction::shortcut() const
{
  return 0;
}

#endif // if KIG_ENABLE_PYTHON_SCRIPTING ( NewScriptAction )
