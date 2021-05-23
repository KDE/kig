// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "object_holder.h"

#include "bogus_imp.h"
#include "object_calcer.h"
#include "object_drawer.h"

#include "../misc/coordinate.h"

ObjectHolder::ObjectHolder( ObjectCalcer* calcer )
  : mcalcer( calcer ), mdrawer( new ObjectDrawer ), mnamecalcer( 0 )
{
}

ObjectHolder::ObjectHolder( ObjectCalcer* calcer, ObjectDrawer* drawer,
                            ObjectConstCalcer* namecalcer )
  : mcalcer( calcer ), mdrawer( drawer ), mnamecalcer( namecalcer )
{
  assert( !namecalcer || namecalcer->imp()->inherits( StringImp::stype() ) );
}

ObjectHolder::ObjectHolder( ObjectCalcer* calcer, ObjectDrawer* drawer )
  : mcalcer( calcer ), mdrawer( drawer ), mnamecalcer( 0 )
{
}

ObjectHolder::~ObjectHolder()
{
  delete mdrawer;
}

const ObjectImp* ObjectHolder::imp() const
{
  return mcalcer->imp();
}

const ObjectCalcer* ObjectHolder::calcer() const
{
  return mcalcer.get();
}

const ObjectDrawer* ObjectHolder::drawer() const
{
  return mdrawer;
}

const ObjectConstCalcer* ObjectHolder::nameCalcer() const
{
  return mnamecalcer.get();
}

void ObjectHolder::setDrawer( ObjectDrawer* d )
{
  delete switchDrawer( d );
}

void ObjectHolder::calc( const KigDocument& d )
{
  mcalcer->calc( d );
}

void ObjectHolder::draw( KigPainter& p, bool selected ) const
{
  mdrawer->draw( *imp(), p, selected );
}

bool ObjectHolder::contains( const Coordinate& pt, const KigWidget& w, bool nv ) const
{
  return mdrawer->contains( *imp(), pt, w, nv );
}

bool ObjectHolder::inRect( const Rect& r, const KigWidget& w ) const
{
  return mdrawer->inRect( *imp(), r, w );
}

ObjectCalcer* ObjectHolder::calcer()
{
  return mcalcer.get();
}

ObjectDrawer* ObjectHolder::drawer()
{
  return mdrawer;
}

ObjectConstCalcer* ObjectHolder::nameCalcer()
{
  return mnamecalcer.get();
}

const Coordinate ObjectHolder::moveReferencePoint() const
{
  return mcalcer->moveReferencePoint();
}

void ObjectHolder::move( const Coordinate& to, const KigDocument& doc )
{
  mcalcer->move( to, doc );
}

bool ObjectHolder::canMove() const
{
  return mcalcer->canMove();
}

bool ObjectHolder::isFreelyTranslatable() const
{
  return mcalcer->isFreelyTranslatable();
}

ObjectDrawer* ObjectHolder::switchDrawer( ObjectDrawer* d )
{
  ObjectDrawer* tmp = mdrawer;
  mdrawer = d;
  return tmp;
}

bool ObjectHolder::shown( ) const
{
  return mdrawer->shown( );
}

const QString ObjectHolder::name() const
{
  if ( mnamecalcer )
  {
    assert( mnamecalcer->imp()->inherits( StringImp::stype() ) );
    return static_cast<const StringImp*>( mnamecalcer->imp() )->data();
  }
  else
    return QString();
}

void ObjectHolder::setNameCalcer( ObjectConstCalcer* namecalcer )
{
  assert( !mnamecalcer );
  mnamecalcer = namecalcer;
}

QString ObjectHolder::selectStatement() const
{
  const QString n = name();
  if ( n.isEmpty() )
    return i18n( imp()->type()->selectStatement() );
  else
    return i18n( imp()->type()->selectNameStatement(), n );
}
