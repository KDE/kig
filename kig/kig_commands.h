#ifndef KIG_COMMANDS_H
#define KIG_COMMANDS_H

#include <kcommand.h>
#include <klocale.h>
#include <kdebug.h>

#include "../objects/object.h"

class KigDocument;

class KigCommand
  : public QObject, public KNamedCommand
{
  Q_OBJECT
public:
  KigCommand (KigDocument* inDoc, const QString& name) : KNamedCommand(name), document( inDoc ) {};
public:
    KigDocument* document;
};

class AddObjectsCommand
  : public KigCommand
{
  Q_OBJECT
public:
  AddObjectsCommand (KigDocument* inDoc, const Objects& os);
  AddObjectsCommand( KigDocument* inDoc, Object* o );
  ~AddObjectsCommand ();
  void execute();
  void unexecute();
signals:
  void executed();
  void unexecuted();
protected:
  bool undone;
  Objects os;
};

class RemoveObjectsCommand
  : public KigCommand
{
  Q_OBJECT
public:
  RemoveObjectsCommand(KigDocument* inDoc, const Objects& o);
    RemoveObjectsCommand( KigDocument* inDoc, Object* o );
  ~RemoveObjectsCommand ();
  void execute();
  void unexecute();
signals:
    void executed();
    void unexecuted();
protected:
  bool undone;
  Objects os;
};

// still needs (quite some) work
class MoveCommand
  : public KigCommand
{
public:
  // sbm: sos before moving: contains copies of the sos from before the move
  // sos: contains pointers to the objects as they are currently being used
  MoveCommand(KigDocument* inDoc, const Objects& inSbm, const Objects& inSos)
    : KigCommand (inDoc, i18n("Move %1 objects").arg(sbm.count())),
      sbm (inSbm), sos(inSos) {};
  ~MoveCommand();
  void execute();
  void unexecute();
protected:
  Objects sbm;
  Objects sos;
};

#endif
