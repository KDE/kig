/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/


#ifndef KIG_COMMANDS_H
#define KIG_COMMANDS_H

#include <QUndoStack>

#include <QDebug>

#include "../objects/object_holder.h"

class KigDocument;
class KigPart;
class CoordinateSystem;

class KigCommandTask;
class KigWidget;
class Rect;

/**
 * a KigCommand represents almost every action performed in Kig.
 * Used mainly in the Undo/Redo stuff...
 */
class KigCommand
  : public QUndoCommand
{
  class Private;
  Private* d;
public:
  KigCommand( KigPart& inDoc, const QString& name );
  ~KigCommand();

  /**
   * To avoid confusion, this doesn't add a command to anything, this
   * creates an AddCommand ;)
   */
  static KigCommand* addCommand( KigPart& doc, const std::vector<ObjectHolder*>& os );
  /**
   * \overload
   */
  static KigCommand* addCommand( KigPart& doc, ObjectHolder* os );
  /**
   * make sure that when you delete something, you are also deleting
   * its parents.  This class assumes you've done that.
   * \ref KigDocument::delObjects() takes care of this for you.
   */
  static KigCommand* removeCommand( KigPart& doc, const std::vector<ObjectHolder*>& os );
  /**
   * \overload
   */
  static KigCommand* removeCommand( KigPart& doc, ObjectHolder* o );

  /**
   * This creates directly a command to change the coordinate system
   * to \p s .
   * \sa ChangeCoordSystemTask
   */
  static KigCommand* changeCoordSystemCommand( KigPart& doc, CoordinateSystem* s );

  void addTask( KigCommandTask* );

  void redo() Q_DECL_OVERRIDE;
  void undo() Q_DECL_OVERRIDE;
private:
  Q_DISABLE_COPY( KigCommand )
};

/**
 * This represents a single task to be executed in a KigCommand.
 */
class KigCommandTask
{
public:
  KigCommandTask();
  virtual ~KigCommandTask();

  virtual void execute( KigPart& doc ) = 0;
  virtual void unexecute( KigPart& doc ) = 0;
};

class AddObjectsTask
  : public KigCommandTask
{
public:
  explicit AddObjectsTask( const std::vector<ObjectHolder*>& os);
  ~AddObjectsTask ();
  void execute( KigPart& doc ) Q_DECL_OVERRIDE;
  void unexecute( KigPart& doc ) Q_DECL_OVERRIDE;
protected:
  bool undone;

  std::vector<ObjectHolder*> mobjs;
};

class RemoveObjectsTask
  : public AddObjectsTask
{
public:
  explicit RemoveObjectsTask( const std::vector<ObjectHolder*>& os );
  void execute( KigPart& ) Q_DECL_OVERRIDE;
  void unexecute( KigPart& ) Q_DECL_OVERRIDE;
};

class ChangeObjectConstCalcerTask
  : public KigCommandTask
{
public:
  ChangeObjectConstCalcerTask( ObjectConstCalcer* calcer, ObjectImp* newimp );
  ~ChangeObjectConstCalcerTask();

  void execute( KigPart& ) Q_DECL_OVERRIDE;
  void unexecute( KigPart& ) Q_DECL_OVERRIDE;
protected:
  ObjectConstCalcer::shared_ptr mcalcer;
  ObjectImp* mnewimp;
};

/**
 * this class monitors a set of DataObjects for changes and returns an
 * appropriate ChangeObjectImpsCommand if necessary.
 * E.g.  MovingMode wants to move certain objects, so it monitors all
 * the parents of the explicitly moving objects:
 * \code
 *   MonitorDataObjects mon( getAllParents( emo ) );
 * \endcode
 * It then moves them around, and when it is finished, it asks to add
 * the KigCommandTasks to a KigCommand, and applies that..
 * \code
 *   KigCommand* comm = new KigCommand( doc, i18n( "Move Stuff" ) );
 *   mon.finish( comm );
 * \endcode
 */
class MonitorDataObjects
{
  class Private;
  Private* d;
public:
  /**
   * all the DataObjects in \p objs will be watched.
   */
  explicit MonitorDataObjects( const std::vector<ObjectCalcer*>& objs );
  explicit MonitorDataObjects( ObjectCalcer* c );
  ~MonitorDataObjects();

  /**
   * add \p objs to the list of objs to be watched, and save their
   * current imp's.
   */
  void monitor( const std::vector<ObjectCalcer*>& objs );

  /**
   * add the generated KigCommandTasks to the command \p comm
   * monitoring stops after this is called.
   */
  void finish( KigCommand* comm );
};

/**
 * A task to change the coordinate system.
 */
class ChangeCoordSystemTask
  : public KigCommandTask
{
  CoordinateSystem* mcs;
public:
  /**
   * a command that changes the coordinate-system to \p s .
   */
  explicit ChangeCoordSystemTask( CoordinateSystem* s );
  ~ChangeCoordSystemTask();

  void execute( KigPart& doc ) Q_DECL_OVERRIDE;
  void unexecute( KigPart& doc ) Q_DECL_OVERRIDE;
};

class ChangeParentsAndTypeTask
  : public KigCommandTask
{
  class Private;
  Private* d;
public:
  ChangeParentsAndTypeTask( ObjectTypeCalcer* o, const std::vector<ObjectCalcer*>& newparents,
                            const ObjectType* newtype );
  ~ChangeParentsAndTypeTask();

  void execute( KigPart& doc ) Q_DECL_OVERRIDE;
  void unexecute( KigPart& doc ) Q_DECL_OVERRIDE;
};

class KigViewShownRectChangeTask
  : public KigCommandTask
{
  class Private;
  Private* d;
public:
  KigViewShownRectChangeTask( KigWidget& v, const Rect& newrect );
  ~KigViewShownRectChangeTask();

  void execute( KigPart& doc ) Q_DECL_OVERRIDE;
  void unexecute( KigPart& doc ) Q_DECL_OVERRIDE;
};

class ChangeObjectDrawerTask
  : public KigCommandTask
{
  ObjectHolder* mholder;
  ObjectDrawer* mnewdrawer;
public:
  ChangeObjectDrawerTask( ObjectHolder* holder, ObjectDrawer* newdrawer );
  ~ChangeObjectDrawerTask();

  void execute( KigPart& doc ) Q_DECL_OVERRIDE;
  void unexecute( KigPart& doc ) Q_DECL_OVERRIDE;
};

#endif
