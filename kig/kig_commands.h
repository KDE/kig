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


#ifndef KIG_COMMANDS_H
#define KIG_COMMANDS_H

#include <kcommand.h>
#include <klocale.h>
#include <kdebug.h>

#include "../objects/object.h"

class KigDocument;
class CoordinateSystem;

class KigCommandTask;

class KigCommand
  : public QObject, public KNamedCommand
{
  Q_OBJECT
  class Private;
  Private* d;
public:
  KigCommand( KigDocument& inDoc, const QString& name );
  ~KigCommand();

  static KigCommand* addCommand( KigDocument& doc, const Objects& os );
  static KigCommand* addCommand( KigDocument& doc, Object* os );
  /**
   * make sure that when you delete something, you are also deleting
   * its parents.  This class assumes you've done that.
   * KigDocument::delObjects takes care of this for you.
   */
  static KigCommand* removeCommand( KigDocument& doc, const Objects& os );
  static KigCommand* removeCommand( KigDocument& doc, Object* o );

  static KigCommand* changeCoordSystemCommand( KigDocument& doc, CoordinateSystem* s );

  void addTask( KigCommandTask* );

  void execute();
  void unexecute();
};

class KigCommandTask
{
public:
  KigCommandTask();
  virtual ~KigCommandTask();

  virtual void execute( KigDocument& doc ) = 0;
  virtual void unexecute( KigDocument& doc ) = 0;
};

class AddObjectsTask
  : public KigCommandTask
{
public:
  AddObjectsTask( const Objects& os);
  ~AddObjectsTask ();
  void execute( KigDocument& doc );
  void unexecute( KigDocument& doc );
protected:
  bool undone;
  Objects os;
};

class RemoveObjectsTask
  : public AddObjectsTask
{
public:
  RemoveObjectsTask( const Objects& os );
  void execute( KigDocument& );
  void unexecute( KigDocument& );
};

class ChangeObjectImpsTask
  : public KigCommandTask
{
public:
  ChangeObjectImpsTask();
  ~ChangeObjectImpsTask();

  /**
   * add a changed dataobject.  this class gains ownership of the
   * imp, you should pass a copy.. ( @see ObjectImp::copy() )
   */
  void addObject( DataObject* o, ObjectImp* newimp );

  void execute( KigDocument& );
  void unexecute( KigDocument& );
protected:
  class Private;
  Private* d;
};

/**
 * this class monitors a set of DataObjects for changes and returns an
 * appropriate ChangeObjectImpsCommand if necessary..
 * E.g.  MovingMode wants to move certain objects, so it monitors all
 * the parents of the explicitly moving objects:
 *   MonitorDataObjects mon( getAllParents( emo ) );
 * It then moves them around, and when it is finished, it asks for a
 * KigCommand, and applies that..
 *   ChangeObjectImpsCommand* command = mon.finish();
 */
class MonitorDataObjects
{
  class Private;
  Private* d;
public:
  /**
   * all the DataObjects in objs will be watched..
   */
  MonitorDataObjects( const Objects& objs );
  ~MonitorDataObjects();

  /**
   * add objs to the list of objs to be watched, and save their
   * current imp's..
   */
  void monitor( const Objects& objs );

  /**
   * get the command..  text shown will be text..  monitoring stops
   * after this is called..
   */
  ChangeObjectImpsTask* finish();
};

class ChangeCoordSystemTask
  : public KigCommandTask
{
  CoordinateSystem* mcs;
public:
  /**
   * a command that changes the coordinate-system to s..
   */
  ChangeCoordSystemTask( CoordinateSystem* s );
  ~ChangeCoordSystemTask();

  void execute( KigDocument& doc );
  void unexecute( KigDocument& doc );
};

class ChangeParentsAndTypeTask
  : public KigCommandTask
{
  class Private;
  Private* d;
public:
  ChangeParentsAndTypeTask( RealObject* o, const Objects& newparents,
                            const ObjectType* newtype );
  ~ChangeParentsAndTypeTask();

  void execute( KigDocument& doc );
  void unexecute( KigDocument& doc );
};

#endif
