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
#include "../objects/object.h"

class KAboutData;
class KActionMenu;
class KCommandHistory;
class KPrinter;
class KToolBar;
class KURL;
class QWidget;

class Coordinate;
class CoordinateSystem;
class GUIAction;
class KigGUIAction;
class KigMode;
class KigObjectsPopup;
class KigView;
class MacroWizardImpl;
class Object;
class Rect;
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

  void filePrint();
  void filePrintPreview();

  void slotSelectAll();
  void slotDeselectAll();
  void slotInvertSelection();

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

  // these are the objects that the user is aware of..
  const Objects objects() const;
  // these are the objects that exist, including those that we added
  // behind the user's back..
  const Objects allObjects() const;
  void setObjects( const Objects& os );
  const CoordinateSystem& coordinateSystem() const;

  /**
   * sets the coordinate system to s, and returns the old one..
   */
  CoordinateSystem* switchCoordinateSystem( CoordinateSystem* s );

  /**
   * sets the coordinate system to s, and deletes the old one..
   */
  void setCoordinateSystem( CoordinateSystem* s );
  KigMode* mode() const { return mMode; };
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
  void _delObjects( const Objects& o );

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
  KAction* aSelectAll;
  KAction* aDeselectAll;
  KAction* aInvertSelection;
  KAction* aDeleteObjects;
  KAction* aNewMacro;
  KAction* aShowHidden;
  KAction* aConfigureTypes;
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
  void doPrint( KPrinter& printer );

  std::vector<KigWidget*> mwidgets;

  KigView* m_widget;

  /**
   * Here we keep the objects in the document.  We keep them in a
   * reference object, so noone would get the idea of deleting them..
   * These are not all the objects, there are objects that are not
   * really part of the document, as the user sees it.  Those are
   * internal, their reference count can go to zero if one of their
   * children gets decref'ed.  The objects that we keep here can never
   * reach a ref count of zero, because this object ref's them..
   */
  ReferenceObject mobjsref;

  /**
   * The CoordinateSystem as the user sees it: this has little to do
   * with the internal coordinates of the objects... In fact, it's
   * not so different from an object itself ( uses KigPainter to draw
   * itself too...).
   */
  CoordinateSystem* mcoordsystem;
};

#endif // KIGPART_H

