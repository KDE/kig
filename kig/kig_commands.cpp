/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#include "kig_commands.h"
#include "kig_commands.moc"

#include "kig_part.h"
#include "kig_view.h"

#include "../modes/mode.h"
#include "../objects/object_imp.h"
#include "../misc/calcpaths.h"
#include "../misc/coordinate_system.h"

#include <vector>

using std::vector;
using std::max;
using std::min;

class KigCommand::Private
{
public:
  Private( KigDocument& d ) : doc( d ) {};
  KigDocument& doc;
  vector<KigCommandTask*> tasks;
};

KigCommand::KigCommand( KigDocument& doc, const QString& name )
  : KNamedCommand(name), d( new Private( doc ) )
{
}

KigCommand::~KigCommand()
{
  for ( uint i = 0; i < d->tasks.size(); ++i )
    delete d->tasks[i];
  delete d;
}

void KigCommand::execute()
{
  for ( uint i = 0; i < d->tasks.size(); ++i )
    d->tasks[i]->execute( d->doc );
  d->doc.mode()->redrawScreen();
}

void KigCommand::unexecute()
{
  for ( uint i = 0; i < d->tasks.size(); ++i )
    d->tasks[i]->unexecute( d->doc );
  d->doc.mode()->redrawScreen();
}

void KigCommand::addTask( KigCommandTask* t )
{
  d->tasks.push_back( t );
}

KigCommand* KigCommand::removeCommand( KigDocument& doc, Object* o )
{
  Objects os( o );
  return addCommand( doc, os );
}

KigCommand* KigCommand::addCommand( KigDocument& doc, Object* o )
{
  Objects os( o );
  return addCommand( doc, os );
}

KigCommand* KigCommand::removeCommand( KigDocument& doc, const Objects& os )
{
  QString text;
  if ( os.size() == 1 )
    text = ObjectImp::removeAStatement( os.back()->imp()->id() );
  else
    text = i18n( "Remove %1 Objects" ).arg( os.size() );
  KigCommand* ret = new KigCommand( doc, text );
  ret->addTask( new RemoveObjectsTask( os ) );
  return ret;
}

KigCommand* KigCommand::addCommand( KigDocument& doc, const Objects& os )
{
  QString text;
  if ( os.size() == 1 )
    text = ObjectImp::addAStatement( os.back()->imp()->id() );
  else
    text = i18n( "Add %1 Objects" ).arg( os.size() );
  KigCommand* ret = new KigCommand( doc, text );
  ret->addTask( new AddObjectsTask( os ) );
  return ret;
}

KigCommand* KigCommand::changeCoordSystemCommand( KigDocument& doc, CoordinateSystem* s )
{
  QString text = CoordinateSystemFactory::setCoordinateSystemStatement( s->id() );
  KigCommand* ret = new KigCommand( doc, text );
  ret->addTask( new ChangeCoordSystemTask( s ) );
  return ret;
}

KigCommandTask::KigCommandTask()
{
}

KigCommandTask::~KigCommandTask()
{
}

AddObjectsTask::AddObjectsTask( const Objects& os)
  : KigCommandTask(), undone( true ), mobjsref( os )
{
}

void AddObjectsTask::execute( KigDocument& doc )
{
  doc._addObjects( mobjsref.parents() );
  undone = false;
};

void AddObjectsTask::unexecute( KigDocument& doc )
{
  doc._delObjects( mobjsref.parents() );
  undone = true;
};

AddObjectsTask::~AddObjectsTask()
{
}

RemoveObjectsTask::RemoveObjectsTask( const Objects& os )
  : AddObjectsTask( os )
{
  undone = false;
}

void RemoveObjectsTask::execute( KigDocument& doc )
{
  AddObjectsTask::unexecute( doc );
}

void RemoveObjectsTask::unexecute( KigDocument& doc )
{
  AddObjectsTask::execute( doc );
}

struct MoveObjectData
{
  DataObject* o;
  ObjectImp* newimp;
};

class ChangeObjectImpsTask::Private
{
public:
  typedef vector<MoveObjectData> datavect;
  datavect data;
};

ChangeObjectImpsTask::ChangeObjectImpsTask()
  : KigCommandTask(), d( new Private )
{
}

ChangeObjectImpsTask::~ChangeObjectImpsTask()
{
  for ( Private::datavect::iterator i = d->data.begin();
        i != d->data.end(); ++i )
  {
    delete i->newimp;
  };
  delete d;
}

void ChangeObjectImpsTask::execute( KigDocument& doc )
{
  Objects children;
  for ( Private::datavect::iterator i = d->data.begin();
        i != d->data.end(); ++i )
  {
    i->newimp = i->o->switchImp( i->newimp );
    children.upush( i->o->getAllChildren() );
  };
  children = calcPath( children );
  children.calc( doc );
}

void ChangeObjectImpsTask::unexecute( KigDocument& doc )
{
  execute( doc );
}

void ChangeObjectImpsTask::addObject( DataObject* o, ObjectImp* newimp )
{
  MoveObjectData n;
  n.o = o;
  n.newimp = newimp;
  d->data.push_back( n );
}

struct MoveDataStruct
{
  DataObject* o;
  ObjectImp* oldimp;
  MoveDataStruct( DataObject* io, ObjectImp* oi )
    : o( io ), oldimp( oi ) { }
};

class MonitorDataObjects::Private
{
public:
  vector<MoveDataStruct> movedata;
};

MonitorDataObjects::MonitorDataObjects( const Objects& objs )
  : d( new Private )
{
  monitor( objs );
}

void MonitorDataObjects::monitor( const Objects& objs )
{
  for ( Objects::const_iterator i = objs.begin(); i != objs.end(); ++i )
    if ( (*i)->inherits( Object::ID_DataObject ) )
    {
      MoveDataStruct n( static_cast<DataObject*>( *i ), (*i)->imp()->copy() );
      d->movedata.push_back( n );
    };
}

ChangeObjectImpsTask* MonitorDataObjects::finish()
{
  ChangeObjectImpsTask* ret = new ChangeObjectImpsTask();
  for ( uint i = 0; i < d->movedata.size(); ++i )
  {
    DataObject* o = d->movedata[i].o;
    if ( ! d->movedata[i].oldimp->equals( *o->imp() ) )
    {
      ObjectImp* newimp = o->switchImp( d->movedata[i].oldimp );
      ret->addObject( o, newimp );
    }
    else
      delete d->movedata[i].oldimp;
  };
  d->movedata.clear();
  return ret;
}

MonitorDataObjects::~MonitorDataObjects()
{
  assert( d->movedata.empty() );
  delete d;
}

ChangeCoordSystemTask::ChangeCoordSystemTask( CoordinateSystem* s )
  : KigCommandTask(), mcs( s )
{
}

void ChangeCoordSystemTask::execute( KigDocument& doc )
{
  mcs = doc.switchCoordinateSystem( mcs );
  calcPath( doc.objects() ).calc( doc );
}

void ChangeCoordSystemTask::unexecute( KigDocument& doc )
{
  execute( doc );
}

ChangeCoordSystemTask::~ChangeCoordSystemTask()
{
  delete mcs;
}

class ChangeParentsAndTypeTask::Private
{
public:
  RealObject* o;
  ReferenceObject newparentsref;
  const ObjectType* newtype;
};

ChangeParentsAndTypeTask::~ChangeParentsAndTypeTask()
{
  delete d;
}

ChangeParentsAndTypeTask::ChangeParentsAndTypeTask(
  RealObject* o, const Objects& newparents,
  const ObjectType* newtype )
  : KigCommandTask(), d( new Private )
{
  d->o = o;
  d->newparentsref.setParents( newparents );
  d->newtype = newtype;
}

void ChangeParentsAndTypeTask::execute( KigDocument& doc )
{
  Objects tmp( d->o );

  ReferenceObject newparentsref( d->newparentsref.parents() );
  d->newparentsref.setParents( d->o->parents() );
  d->o->setParents( newparentsref.parents() );

  const ObjectType* oldtype = d->o->type();
  d->o->setType( d->newtype );
  d->newtype = oldtype;

  d->o->parents().calc( doc );
  d->o->calc( doc );
  d->o->getAllChildren().calc( doc );
}

void ChangeParentsAndTypeTask::unexecute( KigDocument& doc )
{
  execute( doc );
}

class KigViewShownRectChangeTask::Private
{
public:
  Private( KigWidget& view, const Rect& r ) : v( view ), rect( r )  { };
  KigWidget& v;
  Rect rect;
};

KigViewShownRectChangeTask::KigViewShownRectChangeTask(
  KigWidget& v, const Rect& newrect )
  : KigCommandTask()
{
  d = new Private( v, newrect );
}

KigViewShownRectChangeTask::~KigViewShownRectChangeTask()
{
  delete d;
}

void KigViewShownRectChangeTask::execute( KigDocument& )
{
  Rect oldrect = d->v.showingRect();
  d->v.setShowingRect( d->rect );
  d->v.redrawScreen();
  d->v.updateScrollBars();
  d->rect = oldrect;
}

void KigViewShownRectChangeTask::unexecute( KigDocument& doc )
{
  execute( doc );
}

