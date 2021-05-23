/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kig_commands.h"

#include "kig_part.h"
#include "kig_document.h"
#include "kig_view.h"

#include "../modes/mode.h"
#include "../objects/object_imp.h"
#include "../objects/object_drawer.h"
#include "../misc/calcpaths.h"
#include "../misc/coordinate_system.h"

#include <vector>
#include <iterator>

using std::vector;
using std::max;
using std::min;

class KigCommand::Private
{
public:
  Private( KigPart& d ) : doc( d ) {}
  KigPart& doc;
  vector<KigCommandTask*> tasks;
};

KigCommand::KigCommand( KigPart& doc, const QString& name )
  : QUndoCommand( name ), d( new Private( doc ) )
{
}

KigCommand::~KigCommand()
{
  for ( uint i = 0; i < d->tasks.size(); ++i )
    delete d->tasks[i];
  delete d;
}

void KigCommand::redo()
{
  for ( uint i = 0; i < d->tasks.size(); ++i )
    d->tasks[i]->execute( d->doc );
  d->doc.redrawScreen();
}

void KigCommand::undo()
{
  for ( uint i = 0; i < d->tasks.size(); ++i )
    d->tasks[i]->unexecute( d->doc );
  d->doc.redrawScreen();
}

void KigCommand::addTask( KigCommandTask* t )
{
  d->tasks.push_back( t );
}

KigCommand* KigCommand::removeCommand( KigPart& doc, ObjectHolder* o )
{
  std::vector<ObjectHolder*> os;
  os.push_back( o );
  return removeCommand( doc, os );
}

KigCommand* KigCommand::addCommand( KigPart& doc, ObjectHolder* o )
{
  std::vector<ObjectHolder*> os;
  os.push_back( o );
  return addCommand( doc, os );
}

KigCommand* KigCommand::removeCommand( KigPart& doc, const std::vector<ObjectHolder*>& os )
{
  assert( os.size() > 0 );
  QString text;
  if ( os.size() == 1 )
    text = os.back()->imp()->type()->removeAStatement();
  else
    text = i18np( "Remove %1 Object", "Remove %1 Objects", os.size() );
  KigCommand* ret = new KigCommand( doc, text );
  ret->addTask( new RemoveObjectsTask( os ) );
  return ret;
}

KigCommand* KigCommand::addCommand( KigPart& doc, const std::vector<ObjectHolder*>& os )
{
  QString text;
  if ( os.size() == 1 )
    text = os.back()->imp()->type()->addAStatement();
  else
    text = i18np( "Add %1 Object", "Add %1 Objects", os.size() );
  KigCommand* ret = new KigCommand( doc, text );
  ret->addTask( new AddObjectsTask( os ) );
  return ret;
}

KigCommand* KigCommand::changeCoordSystemCommand( KigPart& doc, CoordinateSystem* s )
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

AddObjectsTask::AddObjectsTask( const std::vector<ObjectHolder*>& os)
  : KigCommandTask(), undone( true ), mobjs( os )
{
}

void AddObjectsTask::execute( KigPart& doc )
{
  doc._addObjects( mobjs );
  undone = false;
}

void AddObjectsTask::unexecute( KigPart& doc )
{
  doc._delObjects( mobjs );
  undone = true;
}

AddObjectsTask::~AddObjectsTask()
{
  if ( undone )
    for ( std::vector<ObjectHolder*>::iterator i = mobjs.begin();
          i != mobjs.end(); ++i )
      delete *i;
}

RemoveObjectsTask::RemoveObjectsTask( const std::vector<ObjectHolder*>& os )
  : AddObjectsTask( os )
{
  undone = false;
}

void RemoveObjectsTask::execute( KigPart& doc )
{
  AddObjectsTask::unexecute( doc );
}

void RemoveObjectsTask::unexecute( KigPart& doc )
{
  AddObjectsTask::execute( doc );
}

ChangeObjectConstCalcerTask::ChangeObjectConstCalcerTask( ObjectConstCalcer* calcer, ObjectImp* newimp )
  : KigCommandTask(), mcalcer( calcer ), mnewimp( newimp )
{
}

void ChangeObjectConstCalcerTask::execute( KigPart& doc )
{
  mnewimp = mcalcer->switchImp( mnewimp );

  std::set<ObjectCalcer*> allchildren = getAllChildren( mcalcer.get() );
  std::vector<ObjectCalcer*> allchildrenvect( allchildren.begin(), allchildren.end() );
  allchildrenvect = calcPath( allchildrenvect );
  for ( std::vector<ObjectCalcer*>::iterator i = allchildrenvect.begin();
        i != allchildrenvect.end(); ++i )
    ( *i )->calc( doc.document() );
}

void ChangeObjectConstCalcerTask::unexecute( KigPart& doc )
{
  execute( doc );
}

struct MoveDataStruct
{
  ObjectConstCalcer* o;
  ObjectImp* oldimp;
  MoveDataStruct( ObjectConstCalcer* io, ObjectImp* oi )
    : o( io ), oldimp( oi ) { }
};

class MonitorDataObjects::Private
{
public:
  vector<MoveDataStruct> movedata;
};

MonitorDataObjects::MonitorDataObjects( const std::vector<ObjectCalcer*>& objs )
  : d( new Private )
{
  monitor( objs );
}

void MonitorDataObjects::monitor( const std::vector<ObjectCalcer*>& objs )
{
  for ( std::vector<ObjectCalcer*>::const_iterator i = objs.begin(); i != objs.end(); ++i )
    if ( dynamic_cast<ObjectConstCalcer*>( *i ) )
    {
      MoveDataStruct n( static_cast<ObjectConstCalcer*>( *i ), (*i)->imp()->copy() );
      d->movedata.push_back( n );
    };
}

void MonitorDataObjects::finish( KigCommand* comm )
{
  for ( uint i = 0; i < d->movedata.size(); ++i )
  {
    ObjectConstCalcer* o = d->movedata[i].o;
    if ( ! d->movedata[i].oldimp->equals( *o->imp() ) )
    {
      ObjectImp* newimp = o->switchImp( d->movedata[i].oldimp );
      comm->addTask( new ChangeObjectConstCalcerTask( o, newimp ) );
    }
    else
      delete d->movedata[i].oldimp;
  };
  d->movedata.clear();
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

void ChangeCoordSystemTask::execute( KigPart& doc )
{
  mcs = doc.document().switchCoordinateSystem( mcs );
  std::vector<ObjectCalcer*> calcpath = calcPath( getAllCalcers( doc.document().objects() ) );
  for ( std::vector<ObjectCalcer*>::iterator i = calcpath.begin(); i != calcpath.end(); ++i )
    ( *i )->calc( doc.document() );
  doc.coordSystemChanged( doc.document().coordinateSystem().id() );
}

void ChangeCoordSystemTask::unexecute( KigPart& doc )
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
  ObjectTypeCalcer* o;
  std::vector<ObjectCalcer::shared_ptr> newparents;
  const ObjectType* newtype;
};

ChangeParentsAndTypeTask::~ChangeParentsAndTypeTask()
{
  delete d;
}

ChangeParentsAndTypeTask::ChangeParentsAndTypeTask(
  ObjectTypeCalcer* o, const std::vector<ObjectCalcer*>& newparents,
  const ObjectType* newtype )
  : KigCommandTask(), d( new Private )
{
  d->o = o;
  std::copy( newparents.begin(), newparents.end(),
             std::back_inserter( d->newparents ) );
  d->newtype = newtype;
}

void ChangeParentsAndTypeTask::execute( KigPart& doc )
{
  const ObjectType* oldtype = d->o->type();
  d->o->setType( d->newtype );
  d->newtype = oldtype;

  std::vector<ObjectCalcer*> oldparentso = d->o->parents();
  std::vector<ObjectCalcer::shared_ptr> oldparents(
    oldparentso.begin(), oldparentso.end() );
  std::vector<ObjectCalcer*> newparents;
  for ( std::vector<ObjectCalcer::shared_ptr>::iterator i = d->newparents.begin();
        i != d->newparents.end(); ++i )
    newparents.push_back( i->get() );
  d->o->setParents( newparents );
  d->newparents = oldparents;

  for ( std::vector<ObjectCalcer*>::iterator i = newparents.begin(); i != newparents.end(); ++i )
    ( *i )->calc( doc.document() );
  d->o->calc( doc.document() );
  std::set<ObjectCalcer*> allchildren = getAllChildren( d->o );
  std::vector<ObjectCalcer*> allchildrenvect( allchildren.begin(), allchildren.end() );
  allchildrenvect = calcPath( allchildrenvect );
  for ( std::vector<ObjectCalcer*>::iterator i = allchildrenvect.begin();
        i != allchildrenvect.end(); ++i )
    ( *i )->calc( doc.document() );
}

void ChangeParentsAndTypeTask::unexecute( KigPart& doc )
{
  execute( doc );
}

class KigViewShownRectChangeTask::Private
{
public:
  Private( KigWidget& view, const Rect& r ) : v( view ), rect( r )  { }
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

void KigViewShownRectChangeTask::execute( KigPart& doc )
{
  Rect oldrect = d->v.showingRect();
  d->v.setShowingRect( d->rect );
  doc.mode()->redrawScreen( &d->v );
  d->v.updateScrollBars();
  d->rect = oldrect;
}

void KigViewShownRectChangeTask::unexecute( KigPart& doc )
{
  execute( doc );
}

ChangeObjectDrawerTask::~ChangeObjectDrawerTask()
{
  delete mnewdrawer;
}

ChangeObjectDrawerTask::ChangeObjectDrawerTask(
  ObjectHolder* holder, ObjectDrawer* newdrawer )
  : KigCommandTask(), mholder( holder ), mnewdrawer( newdrawer )
{
}

void ChangeObjectDrawerTask::execute( KigPart& )
{
  mnewdrawer = mholder->switchDrawer( mnewdrawer );
}

void ChangeObjectDrawerTask::unexecute( KigPart& doc )
{
  execute( doc );
}

MonitorDataObjects::MonitorDataObjects( ObjectCalcer* c )
  : d( new Private )
{
  if ( dynamic_cast<ObjectConstCalcer*>( c ) )
  {
    MoveDataStruct n( static_cast<ObjectConstCalcer*>( c ), c->imp()->copy() );
    d->movedata.push_back( n );
  };
}

ChangeObjectConstCalcerTask::~ChangeObjectConstCalcerTask()
{
  delete mnewimp;
}

