/*
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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
*/


#ifndef KIG_COMMANDS_H
#define KIG_COMMANDS_H

#include <kcommand.h>
#include <klocale.h>
#include <kdebug.h>

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
  : public QObject, public KNamedCommand
{
  Q_OBJECT
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
  static KigCommand* addCommand( KigPart& doc, ObjectHolder* os );
  /**
   * make sure that when you delete something, you are also deleting
   * its parents.  This class assumes you've done that.
   * \ref KigDocument::delObjects() takes care of this for you.
   */
  static KigCommand* removeCommand( KigPart& doc, const std::vector<ObjectHolder*>& os );
  static KigCommand* removeCommand( KigPart& doc, ObjectHolder* o );

  static KigCommand* changeCoordSystemCommand( KigPart& doc, CoordinateSystem* s );

  void addTask( KigCommandTask* );

  void execute();
  void unexecute();
};

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
  AddObjectsTask( const std::vector<ObjectHolder*>& os);
  ~AddObjectsTask ();
  void execute( KigPart& doc );
  void unexecute( KigPart& doc );
protected:
  bool undone;

  std::vector<ObjectHolder*> mobjs;
};

class RemoveObjectsTask
  : public AddObjectsTask
{
public:
  RemoveObjectsTask( const std::vector<ObjectHolder*>& os );
  void execute( KigPart& );
  void unexecute( KigPart& );
};

class ChangeObjectConstCalcerTask
  : public KigCommandTask
{
public:
  ChangeObjectConstCalcerTask( ObjectConstCalcer* calcer, ObjectImp* newimp );
  ~ChangeObjectConstCalcerTask();

  void execute( KigPart& );
  void unexecute( KigPart& );
protected:
  ObjectConstCalcer::shared_ptr mcalcer;
  ObjectImp* mnewimp;
};

/**
 * this class monitors a set of DataObjects for changes and returns an
 * appropriate ChangeObjectImpsCommand if necessary..
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
   * all the DataObjects in \p objs will be watched..
   */
  MonitorDataObjects( const std::vector<ObjectCalcer*>& objs );
  MonitorDataObjects( ObjectCalcer* c );
  ~MonitorDataObjects();

  /**
   * add \p objs to the list of objs to be watched, and save their
   * current imp's..
   */
  void monitor( const std::vector<ObjectCalcer*>& objs );

  /**
   * add the generated KigCommandTasks to the command \p comm ..
   * monitoring stops after this is called..
   */
  void finish( KigCommand* comm );
};

class ChangeCoordSystemTask
  : public KigCommandTask
{
  CoordinateSystem* mcs;
public:
  /**
   * a command that changes the coordinate-system to \p s ..
   */
  ChangeCoordSystemTask( CoordinateSystem* s );
  ~ChangeCoordSystemTask();

  void execute( KigPart& doc );
  void unexecute( KigPart& doc );
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

  void execute( KigPart& doc );
  void unexecute( KigPart& doc );
};

class KigViewShownRectChangeTask
  : public KigCommandTask
{
  class Private;
  Private* d;
public:
  KigViewShownRectChangeTask( KigWidget& v, const Rect& newrect );
  ~KigViewShownRectChangeTask();

  void execute( KigPart& doc );
  void unexecute( KigPart& doc );
};

class ChangeObjectDrawerTask
  : public KigCommandTask
{
  ObjectHolder* mholder;
  ObjectDrawer* mnewdrawer;
public:
  ChangeObjectDrawerTask( ObjectHolder* holder, ObjectDrawer* newdrawer );
  ~ChangeObjectDrawerTask();

  void execute( KigPart& doc );
  void unexecute( KigPart& doc );
};

#endif
