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

#include "../modes/mode.h"
#include "../objects/object_imp.h"

static int countRealObjects( const Objects& os )
{
  int ret = 0;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    if ( !(*i)->isInternal() ) ++ret;
  return ret;
};

AddObjectsCommand::AddObjectsCommand(KigDocument* inDoc, const Objects& inOs)
  : KigCommand( inDoc,
		countRealObjects( inOs ) == 1 ?
                ObjectImp::addAStatement( inOs.back()->imp()->id() ) :
                i18n( "Add %1 objects" ).arg( countRealObjects( inOs ) ) ),
    undone(true),
    os (inOs)
{
}

AddObjectsCommand::AddObjectsCommand( KigDocument* inDoc, Object* o )
    : KigCommand ( inDoc, ObjectImp::addAStatement( o->imp()->id() ) ),
      undone( true )
{
  os.push_back( o );
};

void AddObjectsCommand::execute()
{
  for ( Objects::iterator i = os.begin(); i != os.end(); ++i )
  {
    (*i)->calc( *document );
    document->_addObject(*i);
  }
  undone = false;
  document->mode()->objectsAdded();
};

void AddObjectsCommand::unexecute()
{
  for ( Objects::iterator i = os.begin(); i != os.end(); ++i )
  {
    document->_delObject(*i);
  };
  undone=true;
  document->mode()->objectsRemoved();
};

// this function is used by the AddObjectsCommand and
// RemoveObjectsCommand destructors.  They have to delete the objects
// they contain, but what this function adds is that they also delete
// their parents if those are internal and have no more children.
// Same goes for their deleted parents' parents etc.  This to avoid
// KigDocument keeping useless DataObjects around after all their
// children have been deleted..
static void deleteObjectsAndDeadParents( Objects& os, KigDocument& d )
{
  while ( !os.empty() )
  {
    Objects tmp;
    for ( Objects::iterator i = os.begin(); i != os.end(); ++i )
    {
      d._delObject( *i );
      Objects parents = (*i)->parents();
      delete *i;
      for ( Objects::iterator j = parents.begin(); j != parents.end(); ++j )
        if ( (*j)->isInternal() && (*j)->children().empty() && ! os.contains( *j ) )
          tmp.upush( *j );
    };
    os = tmp;
  };
};

AddObjectsCommand::~AddObjectsCommand()
{
  if ( undone )
  {
    deleteObjectsAndDeadParents( os, *document );
  };
}

RemoveObjectsCommand::RemoveObjectsCommand(KigDocument* inDoc, const Objects& inOs)
  : KigCommand(inDoc,
	       countRealObjects( inOs ) == 1 ?
               ObjectImp::removeAStatement( inOs.back()->imp()->id() ) :
               i18n( "Remove %1 objects" ).arg( countRealObjects( inOs ) ) ),
    undone( false ),
    os( inOs )
{
}

RemoveObjectsCommand::RemoveObjectsCommand( KigDocument* inDoc, Object* o)
  : KigCommand( inDoc, ObjectImp::removeAStatement( o->imp()->id() ) ),
    undone( false )
{
  os.push_back( o );
}

RemoveObjectsCommand::~RemoveObjectsCommand()
{
  if (!undone)
  {
    deleteObjectsAndDeadParents( os, *document );
  };
}

void RemoveObjectsCommand::execute()
{
  for ( Objects::iterator i = os.begin(); i != os.end(); ++i )
  {
    document->_delObject(*i);
  };
  undone=false;
  document->mode()->objectsRemoved();
}

void RemoveObjectsCommand::unexecute()
{
  for ( Objects::iterator i = os.begin(); i != os.end(); ++i )
  {
    (*i)->calc( *document );
    document->_addObject(*i);
  };
  undone = true;
  document->mode()->objectsAdded();
}

void MoveCommand::execute()
{
}

void MoveCommand::unexecute()
{
}
