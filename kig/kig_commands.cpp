#include "kig_commands.h"

#include "kig_part.h"

AddObjectsCommand::AddObjectsCommand(KigDocument* inDoc, const Objects& inOs)
  : KigCommand( inDoc,
                 inOs.size() == 1 ?
                   i18n("Add a %1").arg(inOs.front()->vTBaseTypeName()) :
                   i18n( "Add %1 objects" ).arg( os.size() ) ),
    undone(true),
    os (inOs)
{
}

AddObjectsCommand::AddObjectsCommand( KigDocument* inDoc, Object* o )
    : KigCommand ( inDoc, i18n( "Add a %1" ).arg( o->vTBaseTypeName() ) ),
      undone( true )
{
  os.push( o );
};

void AddObjectsCommand::execute()
{
    for ( Objects::iterator i = os.begin(); i != os.end(); i++ )
    {
        document->_addObject(*i);
    };
    undone = false;
    emit executed();
};

void AddObjectsCommand::unexecute()
{
  for ( Objects::iterator i = os.begin(); i != os.end(); i++ )
  {
    document->_delObject(*i);
  };
  undone=true;
  emit unexecuted();
};

AddObjectsCommand::~AddObjectsCommand()
{
  if (undone) os.deleteAll();
}

RemoveObjectsCommand::RemoveObjectsCommand(KigDocument* inDoc, const Objects& inOs)
  : KigCommand(inDoc, inOs.size() == 1 ?
             i18n("Remove a %1").arg(inOs.front()->vTBaseTypeName()) :
             i18n( "Remove %1 objects" ).arg( inOs.size()) ),
    undone( false ),
    os( inOs )
{
  // we delete the children too
  for (Objects::iterator i = os.begin(); i != os.end(); ++i) {
    os |= (*i)->getChildren();
  };
}

RemoveObjectsCommand::RemoveObjectsCommand( KigDocument* inDoc, Object* o)
  : KigCommand( inDoc, i18n("Remove a %1").arg(o->vTBaseTypeName()) ),
    undone( false )
{
    os.push( o );
}

RemoveObjectsCommand::~RemoveObjectsCommand()
{
  if (!undone) os.deleteAll();
}

void RemoveObjectsCommand::execute()
{
  for ( Objects::iterator i = os.begin(); i != os.end(); i++ )
    {
      document->_delObject(*i);
      // the parents should let go of their children (quite a dramatic scene we have here :)
      Objects appel = (*i)->getParents();
      for (Objects::iterator j = appel.begin(); j != appel.end(); ++j) {
	(*j)->delChild(*i);
      };
    };
  undone=false;
  emit executed();
}

void RemoveObjectsCommand::unexecute()
{
  for ( Objects::iterator i = os.begin(); i != os.end(); i++ )
    {
      document->_addObject(*i);
      // and here, the parents are united once again with their beloved offspring...
      Objects appel = (*i)->getParents();
      for (Objects::iterator j = appel.begin(); j != appel.end(); ++j) {
	(*j)->addChild(*i);
      };
    };
    undone = true;
    emit unexecuted();
}

void MoveCommand::execute()
{

}

void MoveCommand::unexecute()
{

}
