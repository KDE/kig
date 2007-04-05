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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/


#ifndef KIGPART_H
#define KIGPART_H

#include <qptrlist.h>

#include <kparts/part.h>

#include "../objects/common.h"

class KAboutData;
class KActionMenu;
class KCommandHistory;
class KPrinter;
class KSelectAction;
class KToolBar;
class KToggleAction;
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
class ObjectHolder;
class Rect;
class ScreenInfo;

/**
 * This is a "Part".  It that does all the real work in a KPart
 * application.
 * Briefly, it holds the data of the document, and acts as an
 * interface to shells
 *
 * @short Main Part
 */
class KigPart : public KParts::ReadWritePart
{
  Q_OBJECT
public:
  /**
   * Default constructor
   */
  KigPart( QWidget* parentWidget, const char* widgetName,
	       QObject* parent = 0, const char* name = 0,
	       const QStringList& = QStringList()
	       );

  /**
   * Destructor
   */
  virtual ~KigPart();

/*********************** KPart interface *************************/

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
  void redrawScreen();
  void redrawScreen( KigWidget* w );

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

  void toggleGrid();
  void toggleAxes();
  void toggleNightVision();

  /**
   * equivalent to setModified( false ); ( did i mention i don't like
   * signals/slots for being this inflexible...
   * this is connected to mhistory->documentRestored();
   */
  void setUnmodified();

  /****************** cooperation with stuff ******************/
public:
  void addWidget( KigWidget* );
  void delWidget( KigWidget* );

  KigMode* mode() const { return mMode; }
  void setMode( KigMode* );
  void runMode( KigMode* );
  void doneMode( KigMode* );

  void coordSystemChanged( int );

signals: // these signals are for telling KigView it should do something...
  /**
   * emitted when we want to suggest a new size for the view
   * ( basically after loading a file, and on startup... )
   */
  void recenterScreen();

/************** working with our internal document **********/
public:
  // guess what these do...
  // actually, they only add a command object to the history, the real
  // work is done in _addObject() and _delObject()
  void addObject(ObjectHolder* inObject);
  void addObjects( const std::vector<ObjectHolder*>& os );
  void delObject(ObjectHolder* inObject);
  void delObjects( const std::vector<ObjectHolder*>& os );
  void hideObjects( const std::vector<ObjectHolder*>& os );
  void showObjects( const std::vector<ObjectHolder*>& os );

  void _addObject( ObjectHolder* inObject );
  void _addObjects( const std::vector<ObjectHolder*>& o);
  void _delObject( ObjectHolder* inObject );
  void _delObjects( const std::vector<ObjectHolder*>& o );

/************* internal stuff *************/
protected:
  bool internalSaveAs();

public:
  static KAboutData* createAboutData();
protected:
  void setupActions();
  void setupTypes();
  void setupBuiltinMacros();
  void setupMacroTypes();

protected:
  KigMode* mMode;
  KSelectAction* aCoordSystem;

  /**
   * the command history
   */
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
  KToggleAction* aToggleGrid;
  KToggleAction* aToggleAxes;
  KToggleAction* aToggleNightVision;
  std::vector<KigGUIAction*> aActions;

  /**
   * the "token" keeps some objects that should be deleted, we only
   * delete them after we replug the actionLists..  calling these
   * functions should be done like:
   * \code
   * GUIUpdateToken t = doc->startGUIActionUpdate();
   * doc->action[Added|Removed]( act, t );
   * ...
   * doc->endGUIActionUpdate( t );
   * \endcode
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

  KigDocument* mdocument;
public:
  const KigDocument& document() const;
  KigDocument& document();

/***************** types handling *******************/
  void saveTypes();
  void loadTypes();
  void deleteTypes();

};

#endif // KIGPART_H

