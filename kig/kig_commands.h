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

class KigCommand
  : public QObject, public KNamedCommand
{
  Q_OBJECT
public:
  KigCommand (KigDocument& inDoc, const QString& name) : KNamedCommand(name), document( inDoc ) {};
public:
  KigDocument& document;
};

class AddObjectsCommand
  : public KigCommand
{
  Q_OBJECT
public:
  AddObjectsCommand (KigDocument& inDoc, const Objects& os);
  AddObjectsCommand( KigDocument& inDoc, Object* o );
  ~AddObjectsCommand ();
  void execute();
  void unexecute();
protected:
  bool undone;
  Objects os;
};

class RemoveObjectsCommand
  : public KigCommand
{
  Q_OBJECT
public:
  /**
   * make sure that when you delete something, you are also deleting
   * its parents.  This class assumes you've done that.
   * KigDocument::delObjects takes care of this for you.
   */
  RemoveObjectsCommand(KigDocument& inDoc, const Objects& o);
  RemoveObjectsCommand( KigDocument& inDoc, Object* o );
  ~RemoveObjectsCommand ();
  void execute();
  void unexecute();
protected:
  bool undone;
  Objects os;
};

class MoveCommand
  : public KigCommand
{
public:
  /**
   * Construct a MoveCommand that will have "Move n objects" as text
   */
  MoveCommand( KigDocument& doc, int n );
  /**
   * Construct a MoveCommand and set the text explicitly ( you should
   * use ObjectImp::moveAStatement() if you're moving a single
   * object..
   */
  MoveCommand( KigDocument& doc, const QString& text );

  ~MoveCommand();

  /**
   * add a changed dataobject.  this class gains ownership of the
   * imp's, you should pass copies.. ( @see ObjectImp::copy() )
   */
  void addObject( DataObject* o, ObjectImp* oldimp, ObjectImp* newimp );

  void execute();
  void unexecute();
protected:
  class Private;
  Private* d;
};

#endif
