// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "guiaction.h"

#include "coordinate_system.h"
#include "coordinate.h"
#include "object_constructor.h"

#include "../kig/kig_part.h"
#include "../kig/kig_document.h"
#include "../misc/kiginputdialog.h"
#include "../modes/construct_mode.h"
#include "../modes/label.h"
#include "../objects/object_holder.h"
#include "../objects/object_factory.h"
#include "../objects/bogus_imp.h"

#include <KActionCollection>
#include <KIconEngine>

#include <QIcon>
#include <QRegExp>

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
  const QByteArray& actionname,
  int shortcut )
  : GUIAction(), mctor( ctor ), mactionname( actionname ), mshortcut( shortcut )
{
}

QString ConstructibleAction::description() const
{
  return mctor->description();
}

QByteArray ConstructibleAction::iconFileName( const bool canBeNull ) const
{
  return mctor->iconFileName( canBeNull );
}

QString ConstructibleAction::descriptiveName() const
{
  return mctor->descriptiveName();
}

void ConstructibleAction::act( KigPart& d )
{
  BaseConstructMode* m = mctor->constructMode( d );
  d.runMode( m );
  delete m;
  d.rememberConstruction( this );
}

KigGUIAction::KigGUIAction( GUIAction* act,
                            KigPart& doc )
  : QAction( act->descriptiveName(), doc.actionCollection() ),
  mact( act ),
  mdoc( doc )
{
  QByteArray icon = act->iconFileName( true );
  if ( !icon.isEmpty() )
    setIcon( QIcon( new KIconEngine( icon, doc.iconLoader() ) ) );
  setWhatsThis( act->description() );
  QString tooltip = act->descriptiveName();
  tooltip.replace( QRegExp( "&&" ), QStringLiteral("&") );
  setToolTip( tooltip );
  connect( this, &QAction::triggered, this, &KigGUIAction::slotActivated );

  doc.actionCollection()->addAction(act->actionName(), this);
  doc.actionCollection()->setDefaultShortcut( this, QKeySequence( act->shortcut() ) );
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

QByteArray ConstructPointAction::iconFileName( const bool ) const
{
  return "point";
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

void ConstructPointAction::act( KigPart& d )
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

void KigGUIAction::plug( KigPart* doc )
{
  mact->plug( doc, this );
}

void ConstructibleAction::plug( KigPart* doc, KigGUIAction* kact )
{
  mctor->plug( doc, kact );
}

QString ConstructTextLabelAction::description() const
{
  return i18n( "Construct a text label." );
}

QByteArray ConstructTextLabelAction::iconFileName( const bool ) const
{
  return "kig_text";
}

QString ConstructTextLabelAction::descriptiveName() const
{
  return i18n( "Text Label" );
}

const char* ConstructTextLabelAction::actionName() const
{
  return mactionname;
}

void ConstructTextLabelAction::act( KigPart& d )
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

QByteArray AddFixedPointAction::iconFileName( const bool ) const
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

void AddFixedPointAction::act( KigPart& doc )
{
  bool ok;
  Coordinate c = Coordinate::invalidCoord();
  KigInputDialog::getCoordinate(
    i18n( "Fixed Point" ),
    i18n( "Enter the coordinates for the new point." ) +
    QLatin1String( "<br>" ) +
    doc.document().coordinateSystem().coordinateFormatNoticeMarkup(),
    doc.widget(), &ok, doc.document(), &c );
  if ( ! ok ) return;
  ObjectHolder* p = ObjectFactory::instance()->fixedPoint( c );
  p->calc( doc.document() );
  doc.addObject( p );
}

AddFixedPointAction::AddFixedPointAction( const char* actionname )
  : mactionname( actionname )
{
}

AddFixedPointAction::~AddFixedPointAction()
{
}

QString ConstructNumericLabelAction::description() const
{
  return i18n( "Construct a Numeric Value" );
}

QByteArray ConstructNumericLabelAction::iconFileName( const bool ) const
{
  return "kig_numericvalue";
}

QString ConstructNumericLabelAction::descriptiveName() const
{
  return i18n( "Numeric Value" );
}

const char* ConstructNumericLabelAction::actionName() const
{
  return mactionname;
}

void ConstructNumericLabelAction::act( KigPart& doc )
{
  NumericLabelMode m( doc );
  doc.runMode( &m );
}

ConstructNumericLabelAction::ConstructNumericLabelAction( const char* actionname )
  : mactionname( actionname )
{
}

ConstructNumericLabelAction::~ConstructNumericLabelAction()
{
}

int ConstructNumericLabelAction::shortcut() const
{
  return Qt::Key_N;
}

void GUIAction::plug( KigPart*, KigGUIAction* )
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

QByteArray TestAction::iconFileName( const bool ) const
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

void TestAction::act( KigPart& doc )
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

NewScriptAction::NewScriptAction( const char* descname, const char* description,
                                  const char* actionname, const ScriptType::Type type,
                                  const char* icon )
  : GUIAction(), mactionname( actionname ), mdescname( descname ),
    mdescription( description ), micon( icon ), mtype( type )
{
  if ( QString( micon ).isEmpty() )
  {
    micon = ScriptType::icon( type );
  }
}

NewScriptAction::~NewScriptAction()
{
}

QString NewScriptAction::description() const
{
  return i18n( mdescription );
}

QByteArray NewScriptAction::iconFileName( const bool ) const
{
  return micon;
}

QString NewScriptAction::descriptiveName() const
{
  return i18n( mdescname );
}

const char* NewScriptAction::actionName() const
{
  return mactionname;
}

void NewScriptAction::act( KigPart& doc )
{
  ScriptCreationMode m( doc );
  m.setScriptType( mtype );
  doc.runMode( &m );
}

int NewScriptAction::shortcut() const
{
  return 0;
}

#endif // if KIG_ENABLE_PYTHON_SCRIPTING ( NewScriptAction )
