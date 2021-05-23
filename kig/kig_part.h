/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/


#ifndef KIGPART_H
#define KIGPART_H

#include <QList>

#include <KParts/ReadWritePart>
#include <KSelectAction>

#include <vector>

class KAboutData;
class KToggleAction;
class QUndoStack;
class QWidget;
class QPrinter;

class GUIAction;
class KigGUIAction;
class KigDocument;
class KigMode;
class KigPart;
class KigView;
class KigWidget;
class ObjectHolder;
class ConstructibleAction;

class SetCoordinateSystemAction
  : public KSelectAction
{
  Q_OBJECT
  KigPart& md;
public:
  SetCoordinateSystemAction( KigPart& d, KActionCollection* parent );
private slots:
  void slotActivated( int index );
};

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
  explicit KigPart( QWidget* parentWidget, QObject* parent = 0,
           const QVariantList& = QVariantList() );

  /**
   * Destructor
   */
  virtual ~KigPart();

/*********************** KPart interface *************************/

protected:
  /**
   * load our internal document from m_file
   */
  bool openFile() Q_DECL_OVERRIDE;

  /**
   * save our internal document to m_file
   */
  bool saveFile() Q_DECL_OVERRIDE;

  /**
   * KigPart cannot be safely destroyed if it's in a nested event loop
   * (construct mode). As a workaround, call this first so it can cancel the
   * construction.
   */
  bool queryClose() Q_DECL_OVERRIDE;

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
  void repeatLastConstruction();
  void showHidden();
  void newMacro();
  void editTypes();
  void browseHistory();

  void toggleGrid();
  void toggleAxes();
  void toggleNightVision();

  void setHistoryClean( bool );
  
  void setCoordinatePrecision();

  /****************** cooperation with stuff ******************/
public:
  void addWidget( KigWidget* );
  void delWidget( KigWidget* );

  KigMode* mode() const { return mMode; }
  void setMode( KigMode* );
  void runMode( KigMode* );
  void doneMode( KigMode* );

  void rememberConstruction( ConstructibleAction* );
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
  
  /**
   * Call this method to start an object group which will
   * be deleted as a whole if the construction is canceled
   * or undone
   * \sa See also cancelObjectGroup finishObjectGroup
   */
  void startObjectGroup();
  
  /**
   * Deletes the current group of objects from the document
   * without adding them to the Undo history
   * \sa See also startObjectGroup finishObjectGroup
   */
  void cancelObjectGroup();
  
  /**
   * Draws the current group of objects and add them
   * as a whole to the Undo history
   * \sa See also startObjectGroup cancelObjectGroup
   */
  void finishObjectGroup();

/************* internal stuff *************/
protected:
  bool internalSaveAs();

protected:
  void setupActions();
  void setupTypes();
  void setupBuiltinMacros();
  void setupMacroTypes();

protected:
  KigMode* mMode;
  KSelectAction* aCoordSystem;
  ConstructibleAction* mRememberConstruction;

  /**
   * the command history
   */
  QUndoStack* mhistory;

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
  QList<QAction*> aMNewSegment;
  QList<QAction*> aMNewPoint;
  QList<QAction*> aMNewCircle;
  QList<QAction*> aMNewLine;
  QList<QAction*> aMNewOther;
  QList<QAction*> aMNewAll;
  QList<QAction*> aMNewConic;


  QAction * aCancelConstruction;
  QAction * aRepeatLastConstruction;
  QAction * aSelectAll;
  QAction * aDeselectAll;
  QAction * aInvertSelection;
  QAction * aDeleteObjects;
  QAction * aNewMacro;
  QAction * aShowHidden;
  QAction * aConfigureTypes;
  QAction * aBrowseHistory;
  QAction * aSetCoordinatePrecision;
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

  QUndoStack* history();

  void enableConstructActions( bool enabled );

protected:
  void doPrint( QPrinter& printer, bool printGrid, bool printAxes );

  std::vector<KigWidget*> mwidgets;

  KigView* m_widget;

  KigDocument* mdocument;
  
  /**
   * Indicates whether objects added to the documents
   * are being grouped or not.
   * \sa See also startObjectGroup cancelObjectGroup finishObjectGroup
   */
  bool misGroupingObjects;
  
  /**
   * Stores the current group of object being drawn
   * if an object group has been started.
   * \sa See also startObjectGroup cancelObjectGroup finishObjectGroup
   */
  std::vector<ObjectHolder*> mcurrentObjectGroup;
  
public:
  const KigDocument& document() const;
  KigDocument& document();

/***************** types handling *******************/
  void saveTypes();
  void loadTypes();
  void deleteTypes();

};

#endif // KIGPART_H
