#include "kig_commands.h"
#include "kig_commands.moc"

#include "kig_part.h"

AddObjectsCommand::AddObjectsCommand(KigDocument* inDoc, const Objects& inOs)
  : KigCommand( inDoc,
		inOs.count() == 1 ?
		  i18n("Add a %1").arg(inOs.getFirst()->vTBaseTypeName()) :
		  i18n( "Add %1 objects" ).arg( os.count() ) ),
    undone(true),
    os (inOs)
{
}

AddObjectsCommand::AddObjectsCommand( KigDocument* inDoc, Object* o )
    : KigCommand ( inDoc, i18n( "Add a %1" ).arg( o->vTBaseTypeName() ) ),
      undone( true )
{
  os.append( o );
};

void AddObjectsCommand::execute()
{
  for ( Object* i = os.first(); i; i = os.next() )
    {
      document->_addObject(i);
    };
  undone = false;
  emit executed();
};

void AddObjectsCommand::unexecute()
{
  for ( Object* i = os.first(); i; i = os.next() )
  {
    document->_delObject(i);
  };
  undone=true;
  emit unexecuted();
};

AddObjectsCommand::~AddObjectsCommand()
{
  if (undone) os.deleteAll();
}

RemoveObjectsCommand::RemoveObjectsCommand(KigDocument* inDoc, const Objects& inOs)
  : KigCommand(inDoc,
	       inOs.count() == 1 ?
   	         i18n("Remove a %1").arg(inOs.getFirst()->vTBaseTypeName()) :
	         i18n( "Remove %1 objects" ).arg( inOs.count()) ),
    undone( false ),
    os( inOs )
{
  // we delete the children too
  for (Object* i = os.first(); i; i = os.next()) {
    Objects tmp = i->getChildren();
    os |= tmp;
  };
}

RemoveObjectsCommand::RemoveObjectsCommand( KigDocument* inDoc, Object* o)
  : KigCommand( inDoc, i18n("Remove a %1").arg(o->vTBaseTypeName()) ),
    undone( false )
{
    os.append( o );
}

RemoveObjectsCommand::~RemoveObjectsCommand()
{
  if (!undone) os.deleteAll();
}

void RemoveObjectsCommand::execute()
{
  for ( Object* i = os.first(); i; i = os.next() )
    {
      document->_delObject(i);
      // the parents should let go of their children (quite a dramatic scene we have here :)
      Objects appel = i->getParents();
      for (Object* j = appel.first(); j; j = appel.next()) {
	j->delChild(i);
      };
    };
  undone=false;
  emit executed();
}

void RemoveObjectsCommand::unexecute()
{
  for ( Object* i = os.first(); i; i = os.next() )
    {
      document->_addObject(i);
      // drama again: parents finding their lost children...
      Objects appel = i->getParents();
      for (Object* j = appel.first(); j; j = appel.next()) {
	j->addChild(i);
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
