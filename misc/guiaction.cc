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
  const char* actionname )
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
                            KActionCollection* parent )
  : KAction( act->descriptiveName(),
             KGlobal::instance()->iconLoader()->loadIconSet(
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
