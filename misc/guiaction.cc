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

#include "../kig/kig_part.h"
#include "../modes/construct_mode.h"
#include "../modes/label.h"

#include "i18n.h"
#include "object_constructor.h"

#include <kiconloader.h>

QString CircleByCenterAndRadiusAction::description() const
{
  return i18n( "Circle by center and radius" );
}

QCString CircleByCenterAndRadiusAction::iconFileName() const
{
  return "baseCircle";
}

void CircleByCenterAndRadiusAction::act( KigDocument& )
{
//   const struct ArgParser::spec myspec[] = { { Object::PointT, 1 } };
//   ArgParser parser( myspec, 1 );
//   StandAloneSelectionMode mode( parser, &d );
//   if ( mode.run( d.mode() ) )
//   {
//     Objects os = mode.selection();
//     assert( os.size() == 1 );
//     Object* o = os.front();
//     assert( o->isa( PointT ) );
//     Point* p = static_cast<Point*>( o );
//     bool ok = true;
//     QString ret = QInputDialog::getDouble( i18n( "Circle with fixed radius" ), i18n( "Please enter the radius" ),
//                                            5, 0, 2147483647, 5, &ok );
//     double radius;
//     if ( ok ) radius = ret.toDouble( &ok );
//     if ( ok )
//     {
//       Circle* c = new CircleBPR( p, radius );
//       d.addObject( c );
//       d.mainWidget()->realWidget()->redrawScreen();
//     };
//   };
}

int GUIAction::shortcut() const
{
  return 0;
}

QString CircleByCenterAndRadiusAction::descriptiveName() const
{
  return i18n( "Circle defined by its center and a fixed radius" );
}

GUIAction::~GUIAction()
{
}

ConstructibleAction::~ConstructibleAction()
{
}

ConstructibleAction::ConstructibleAction(
  ObjectConstructor* ctor,
  const QCString& actionname )
  : GUIAction(), mctor( ctor ), mactionname( actionname )
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

const char* CircleByCenterAndRadiusAction::actionName() const
{
  return "objects_new_circlebcr";
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
  return 0;
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

void ConstructPointAction::plug( KigDocument*, KigGUIAction* )
{
}

void CircleByCenterAndRadiusAction::plug( KigDocument*, KigGUIAction* )
{
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

void ConstructTextLabelAction::plug( KigDocument*, KigGUIAction* )
{
}

