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


#ifndef KIGPART_H
#define KIGPART_H

#include <kparts/part.h>
#include <qptrlist.h>

#include "../misc/objects.h"
#include "../misc/rect.h"

class QWidget;
class KURL;
class KActionMenu;
class KCommandHistory;
class KAboutData;
class KToolBar;

class KigMode;
class KigObjectsPopup;
class CoordinateSystem;
class MacroWizardImpl;
class KigView;
class Object;
class KigGUIAction;
class GUIAction;
class ScreenInfo;

/**
 * This is a "Part".  It that does all the real work in a KPart
 * application.
 * Briefly, it holds the data of the document, and acts as an
 * interface to shells
 *
 * @short Main Part
 * @author Dominique Devriese <devriese@kde.org>
 */
class KigDocument : public KParts::ReadWritePart
{
  Q_OBJECT

  friend class AddObjectsCommand;
  friend class RemoveObjectsCommand;
public:
  /**
   * Default constructor
   */
  KigDocument( QWidget* parentWidget, const char* widgetName,
	       QObject* parent = 0, const char* name = 0,
	       const QStringList& = QStringList()
	       );

  /**
   * Destructor
   */
  virtual ~KigDocument();

  const std::vector<KigWidget*>& widgets();

/*********************** KPart interface *************************/

public:
  static KAboutData* createAboutData();

protected:
  /**
   * load our internal document from m_file
   */
  virtual bool openFile();

  /**
   * save our internal document to m_file
   */
  virtual bool saveFile();

public:
  void emitStatusBarText( const QString& text );

public slots:
  void fileSaveAs();
  void fileSave();

  void unplugActionLists();
  void plugActionLists();

  void deleteObjects();
  void cancelConstruction();
  void showHidden();
  void newMacro();
  void editTypes();
  // equivalent to setModified( false ); ( did i mention i don't like
  // signals/slots for being this inflexible...
  // this is connected to mhistory->documentRestored();
  void setUnmodified();

  /****************** cooperation with stuff ******************/
public:
  void addWidget( KigWidget* );
  void delWidget( KigWidget* );

  const Objects& objects() const { return mObjs;};
  void setObjects( const Objects& os );
  const CoordinateSystem& coordinateSystem() const;
  void setCoordinateSystem( CoordinateSystem* s );
  KigMode* mode() { return mMode; };
  void setMode( KigMode* );
  void runMode( KigMode* );
  void doneMode( KigMode* );

  // what objects are under point p
  Objects whatAmIOn( const Coordinate& p, const KigWidget& si ) const;

  Objects whatIsInHere( const Rect& p, const KigWidget& );

  // a rect containing most of the objects, which would be a fine
  // suggestion to mapt to the widget...
  Rect suggestedRect() const;

signals: // these signals are for telling KigView it should do something...
  // emitted when we want to suggest a new size for the view (
  // basically after loading a file, and on startup... )
  void recenterScreen();

/************** working with our internal document **********/
public:
  // guess what these do...
  // actually, they only add a command object to the history, the real
  // work is done in _addObject() and _delObject()
  void addObject(Object* inObject);
  void addObjects( const Objects& os );
  void delObject(Object* inObject);
  void delObjects( const Objects& os );

/************* internal stuff *************/
protected:
  bool internalSaveAs();

public:
  void _addObject( Object* inObject );
  void _addObjects( const Objects& o);
  void _delObject( Object* inObject );

protected:
  void setupActions();
  void setupTypes();
  void setupBuiltinMacros();
  void setupMacroTypes();

protected:
  KigMode* mMode;

  // the command history
  KCommandHistory* mhistory;

public:
  // actions: this is an annoying case, didn't really fit into my
  // model with KigModes.. This is how it works now:
  // the actions are owned by the Part, because we need them on
  // constructing the GUI ( actions appearing when you switch modes
  // would not be nice, imho ).  On setting the KigPart mode, we
  // connect the actions to the current mode, and disconnect them from
  // the previous mode.  Enabling/disabling is done at the same time,
  // of course..
  // some MenuActions..
  QPtrList<KAction> aMNewSegment;
  QPtrList<KAction> aMNewPoint;
  QPtrList<KAction> aMNewCircle;
  QPtrList<KAction> aMNewLine;
  QPtrList<KAction> aMNewOther;
  QPtrList<KAction> aMNewAll;
  QPtrList<KAction> aMNewConic;

  KAction* aCancelConstruction;
  KAction* aDeleteObjects;
  KAction* aNewMacro;
  KAction* aShowHidden;
  KAction* aConfigureTypes;
  KAction* aFullScreen;
  KAction* aFixedPoint;
  myvector<KigGUIAction*> aActions;

  /**
   * the "token" keeps some objects that should be deleted, we only
   * delete them after we replug the actionLists..  calling these
   * functions should be done like:
   * GUIUpdateToken t = doc->startGUIActionUpdate();
   * doc->action[Added|Removed]( act, t );
   * ...
   * doc->endGUIActionUpdate( t );
   */
  typedef std::vector<KigGUIAction*> GUIUpdateToken;
  GUIUpdateToken startGUIActionUpdate();
  void actionAdded( GUIAction* a, GUIUpdateToken& t );
  void actionRemoved( GUIAction* a, GUIUpdateToken& t );
  void endGUIActionUpdate( GUIUpdateToken& t );

  KCommandHistory* history();

  void enableConstructActions( bool enabled );

protected:
  std::vector<KigWidget*> mwidgets;

  KigView* m_widget;

  /**
   * Our internal document data:
   * first all objects we contain: all of them are in there, except
   * the obc.
   * This is the one that owns all objects, all other object
   * containers only contain pointers to the objects, and don't own
   * them.
   */
  Objects mObjs;

  /**
   * The CoordinateSystem as the user sees it: this has little to do
   * with the internal coordinates of the objects... In fact, it's
   * not so different from an object itself ( uses KigPainter to draw
   * itself too...).
   */
  CoordinateSystem* mcoordsystem;
};

#endif // KIGPART_H

