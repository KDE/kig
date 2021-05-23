/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kig_part.h"

#include <kigpart_export.h>

#include "aboutdata.h"
#include "kig_commands.h"
#include "kig_document.h"
#include "kig_view.h"

#include "../filters/exporter.h"
#include "../filters/filter.h"
#include "../misc/builtin_stuff.h"
#include "../misc/calcpaths.h"
#include "../misc/coordinate_system.h"
#include "../misc/guiaction.h"
#include "../misc/kigcoordinateprecisiondialog.h"
#include "../misc/kigpainter.h"
#include "../misc/lists.h"
#include "../misc/object_constructor.h"
#include "../misc/screeninfo.h"
#include "../modes/normal.h"
#include "../objects/object_drawer.h"
#include "../objects/point_imp.h"

#include <algorithm>
#include <functional>
#include <iterator>

#include <QDirIterator>
#include <QFileDialog>
#include <QFile>
#include <QMimeType>
#include <QMimeDatabase>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QStandardPaths>
#include <QTimer>

#include <KAboutData>
#include <KActionCollection>
#include <KIconLoader>
#include <KIconEngine>
#include <KMessageBox>
#include <KUndoActions>
#include <KPluginFactory>
#include <KStandardAction>
#include <KToggleAction>

#include <KParts/OpenUrlArguments>

using namespace std;

static const QString typesFile = QStringLiteral("macros.kigt");

QStringList getDataFiles( const QString & folder )
{
  QStringList dataFiles;
  const QStringList allFolders = QStandardPaths::locateAll( QStandardPaths::DataLocation, folder, QStandardPaths::LocateDirectory );

  for( const QString & folderPath : allFolders )
  {
    QDirIterator folderIterator( folderPath, QDirIterator::Subdirectories );

    while ( folderIterator.hasNext() )
    {
      const QString fileName = folderIterator.next();

      if ( fileName.endsWith( QLatin1String( ".kigt" ) ) )
      {
        dataFiles << fileName;
      }
    }
  }

  return dataFiles;
}

// export this class from this library...
K_PLUGIN_CLASS_WITH_JSON( KigPart, "kig_part.json")

SetCoordinateSystemAction::SetCoordinateSystemAction(
  KigPart& d, KActionCollection* parent )
  : KSelectAction( i18n( "&Set Coordinate System" ), &d),
    md( d )
{
  setItems( CoordinateSystemFactory::names() );
  setCurrentItem( md.document().coordinateSystem().id() );
  connect( this, SIGNAL(triggered(int)), this, SLOT(slotActivated(int)) );
  if(parent)
    parent->addAction(QStringLiteral("settings_set_coordinate_system"), this);
}

void SetCoordinateSystemAction::slotActivated( int index )
{
  CoordinateSystem* sys = CoordinateSystemFactory::build( index );
  assert( sys );
  md.history()->push( KigCommand::changeCoordSystemCommand( md, sys ) );
  setCurrentItem( index );
}

class KigPrintDialogPage
  : public QWidget
{
public:
  KigPrintDialogPage( QWidget* parent = 0 );
  ~KigPrintDialogPage();

  bool printShowGrid();
  void setPrintShowGrid( bool status );

  bool printShowAxes();
  void setPrintShowAxes( bool status );

  bool isValid( QString& );

private:
  QCheckBox *showgrid;
  QCheckBox *showaxes;
};

KigPrintDialogPage::KigPrintDialogPage( QWidget* parent )
 : QWidget( parent )
{
  setWindowTitle( i18nc("@title:window", "Kig Options") );

  QVBoxLayout* vl = new QVBoxLayout( this );

  showgrid = new QCheckBox( i18n( "Show grid" ), this );
  vl->addWidget( showgrid );

  showaxes = new QCheckBox( i18n( "Show axes" ), this );
  vl->addWidget( showaxes );

  vl->addItem( new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding ) );
}

KigPrintDialogPage::~KigPrintDialogPage()
{
}

bool KigPrintDialogPage::printShowGrid()
{
  return showgrid->isChecked();
}

void KigPrintDialogPage::setPrintShowGrid( bool status )
{
  showgrid->setChecked( status );
}

bool KigPrintDialogPage::printShowAxes()
{
  return showaxes->isChecked();
}

void KigPrintDialogPage::setPrintShowAxes( bool status )
{
  showaxes->setChecked( status );
}

bool KigPrintDialogPage::isValid( QString& )
{
  return true;
}

KigPart::KigPart( QWidget *parentWidget, QObject *parent,
                  const QVariantList& )
  : KParts::ReadWritePart( parent ),
    mMode( 0 ), mRememberConstruction( 0 ), mdocument( new KigDocument() )
{
  mMode = new NormalMode( *this );

  // we need a widget, to actually show the document
  m_widget = new KigView( this, false, parentWidget );
  m_widget->setObjectName( QStringLiteral("kig_view") );
  // notify the part that this is our internal widget
  setWidget( m_widget );

  // create our actions...
  setupActions();

  // set our XML-UI resource file
  setXMLFile(QStringLiteral("kigpartui.rc"));

  // our types...
  setupTypes();

  // construct our command history
  mhistory = new QUndoStack();
  KUndoActions::createUndoAction( mhistory, actionCollection() );
  KUndoActions::createRedoAction( mhistory, actionCollection() );
  connect( mhistory, &QUndoStack::cleanChanged, this, &KigPart::setHistoryClean );

  // we are read-write by default
  setReadWrite(true);

  setModified (false);

  GUIActionList::instance()->regDoc( this );
}

void KigPart::setupActions()
{
  // save actions..
  (void) KStandardAction::saveAs(this, SLOT(fileSaveAs()), actionCollection());
  (void) KStandardAction::save(this, SLOT(fileSave()), actionCollection());

  // print actions
  (void) KStandardAction::print( this, SLOT(filePrint()), actionCollection() );
  (void) KStandardAction::printPreview( this, SLOT(filePrintPreview()), actionCollection() );

  // selection actions
  aSelectAll = KStandardAction::selectAll(
    this, SLOT(slotSelectAll()), actionCollection() );
  aDeselectAll = KStandardAction::deselect(
    this, SLOT(slotDeselectAll()), actionCollection() );
  aInvertSelection  = new QAction(i18n("Invert Selection"), this);
  actionCollection()->addAction(QStringLiteral("edit_invert_selection"), aInvertSelection );
  connect(aInvertSelection, &QAction::triggered, this, &KigPart::slotInvertSelection);

  // we need icons...
  KIconLoader* l = iconLoader();

  aDeleteObjects  = new QAction(QIcon::fromTheme(QStringLiteral("edit-delete")), i18n("&Delete Objects"), this);
  actionCollection()->addAction(QStringLiteral("delete_objects"), aDeleteObjects );
  connect(aDeleteObjects, &QAction::triggered, this, &KigPart::deleteObjects);
  actionCollection()->setDefaultShortcut(aDeleteObjects, QKeySequence(Qt::Key_Delete));
  aDeleteObjects->setToolTip(i18n("Delete the selected objects"));

  aCancelConstruction  = new QAction(QIcon::fromTheme(QStringLiteral("process-stop")), i18n("Cancel Construction"), this);
  actionCollection()->addAction(QStringLiteral("cancel_construction"), aCancelConstruction );
  connect(aCancelConstruction, &QAction::triggered, this, &KigPart::cancelConstruction);
  actionCollection()->setDefaultShortcut(aCancelConstruction, QKeySequence(Qt::Key_Escape));
  aCancelConstruction->setToolTip(
      i18n("Cancel the construction of the object being constructed"));
  aCancelConstruction->setEnabled(false);

  aRepeatLastConstruction = new QAction(QIcon::fromTheme(QStringLiteral("system-run")), i18n("Repeat Construction"), this);
  actionCollection()->addAction(QStringLiteral("repeat_last_construction"), aRepeatLastConstruction );
  connect(aRepeatLastConstruction, &QAction::triggered, this, &KigPart::repeatLastConstruction);
  actionCollection()->setDefaultShortcut(aRepeatLastConstruction, QKeySequence(Qt::Key_Z));
  aRepeatLastConstruction->setToolTip(
      i18n("Repeat the last construction (with new data)"));
  aRepeatLastConstruction->setEnabled(false);

  aShowHidden  = new QAction(i18n("U&nhide All"), this);
  actionCollection()->addAction(QStringLiteral("edit_unhide_all"), aShowHidden );
  connect(aShowHidden, &QAction::triggered, this, &KigPart::showHidden);
  aShowHidden->setToolTip(i18n("Show all hidden objects"));
  aShowHidden->setEnabled( true );

  aNewMacro  = new QAction(QIcon::fromTheme(QStringLiteral("system-run")), i18n("&New Macro..."), this);
  actionCollection()->addAction(QStringLiteral("macro_action"), aNewMacro );
  connect(aNewMacro, &QAction::triggered, this, &KigPart::newMacro);
  aNewMacro->setToolTip(i18n("Define a new macro"));

  aConfigureTypes  = new QAction(i18n("Manage &Types..."), this);
  actionCollection()->addAction(QStringLiteral("types_edit"), aConfigureTypes );
  connect(aConfigureTypes, &QAction::triggered, this, &KigPart::editTypes);
  aConfigureTypes->setToolTip(i18n("Manage macro types."));

  aBrowseHistory  = new QAction(QIcon::fromTheme(QStringLiteral("view-history")), i18n("&Browse History..."), this);
  actionCollection()->addAction(QStringLiteral("browse_history"), aBrowseHistory );
  connect( aBrowseHistory, &QAction::triggered, this, &KigPart::browseHistory );
  aBrowseHistory->setToolTip( i18n( "Browse the history of the current construction." ) );

  KigExportManager::instance()->addMenuAction( this, m_widget->realWidget(),
                                               actionCollection() );

  QAction * a = KStandardAction::zoomIn( m_widget, SLOT(slotZoomIn()),
                                   actionCollection() );
  a->setToolTip( i18n( "Zoom in on the document" ) );
  a->setWhatsThis( i18n( "Zoom in on the document" ) );

  a = KStandardAction::zoomOut( m_widget, SLOT(slotZoomOut()),
                           actionCollection() );
  a->setToolTip( i18n( "Zoom out of the document" ) );
  a->setWhatsThis( i18n( "Zoom out of the document" ) );

  a = KStandardAction::fitToPage( m_widget, SLOT(slotRecenterScreen()),
                             actionCollection() );
  // grr.. why isn't there an icon for this..
  a->setIcon( QIcon( new KIconEngine( "view_fit_to_page", l ) ) );
  a->setToolTip( i18n( "Recenter the screen on the document" ) );
  a->setWhatsThis( i18n( "Recenter the screen on the document" ) );

  a = actionCollection()->addAction(KStandardAction::FullScreen, QStringLiteral("fullscreen"), m_widget, SLOT(toggleFullScreen()));
  a->setToolTip( i18n( "View this document full-screen." ) );
  a->setWhatsThis( i18n( "View this document full-screen." ) );

  // TODO: an icon for this..
  a  = new QAction(QIcon::fromTheme(QStringLiteral("zoom-fit-best")), i18n("&Select Shown Area"), this);
  actionCollection()->addAction(QStringLiteral("view_select_shown_rect"), a );
  connect(a, &QAction::triggered, m_widget, &KigView::zoomRect);
  a->setToolTip( i18n( "Select the area that you want to be shown in the window." ) );
  a->setWhatsThis( i18n( "Select the area that you want to be shown in the window." ) );

  a  = new QAction(QIcon::fromTheme(QStringLiteral("zoom-original")), i18n("S&elect Zoom Area"), this);
  actionCollection()->addAction(QStringLiteral("view_zoom_area"), a );
  connect(a, &QAction::triggered, m_widget, &KigView::zoomArea);
//  a->setToolTip( i18n( "Select the area that you want to be shown in the window." ) );
//  a->setWhatsThis( i18n( "Select the area that you want to be shown in the window." ) );

  aSetCoordinatePrecision = new QAction(i18n("Set Coordinate &Precision..."), this);
  actionCollection()->addAction(QStringLiteral("settings_set_coordinate_precision"), aSetCoordinatePrecision);
  aSetCoordinatePrecision->setToolTip( i18n("Set the floating point precision of coordinates in the document. " ));
  connect(aSetCoordinatePrecision, &QAction::triggered, this, &KigPart::setCoordinatePrecision);
  
  aToggleGrid  = new KToggleAction(i18n("Show &Grid"), this);
  actionCollection()->addAction(QStringLiteral("settings_show_grid"), aToggleGrid );
  aToggleGrid->setToolTip( i18n( "Show or hide the grid." ) );
  aToggleGrid->setChecked( true );
  connect( aToggleGrid, &QAction::triggered, this, &KigPart::toggleGrid );

  aToggleAxes  = new KToggleAction(i18n("Show &Axes"), this);
  actionCollection()->addAction(QStringLiteral("settings_show_axes"), aToggleAxes );
  aToggleAxes->setToolTip( i18n( "Show or hide the axes." ) );
  aToggleAxes->setChecked( true );
  connect( aToggleAxes, &QAction::triggered, this, &KigPart::toggleAxes );

  aToggleNightVision  = new KToggleAction(i18n("Wear Infrared Glasses"), this);
  actionCollection()->addAction(QStringLiteral("settings_toggle_nightvision"), aToggleNightVision );
  aToggleNightVision->setToolTip( i18n( "Enable/disable hidden objects' visibility." ) );
  aToggleNightVision->setChecked( false );
  connect( aToggleNightVision, &QAction::triggered, this, &KigPart::toggleNightVision );

  // select coordinate system KActionMenu..
  aCoordSystem = new SetCoordinateSystemAction( *this, actionCollection() );
}

void KigPart::setupTypes()
{
  setupBuiltinStuff();
  setupBuiltinMacros();
  setupMacroTypes();
  GUIActionList& l = *GUIActionList::instance();
  typedef GUIActionList::avectype::const_iterator iter;
  for ( iter i = l.actions().begin(); i != l.actions().end(); ++i )
  {
    KigGUIAction* ret = new KigGUIAction( *i, *this );
    aActions.push_back( ret );
    ret->plug( this );
  };
}

void KigPart::rememberConstruction( ConstructibleAction* ca )
{
  // mp:
  // mRememberConstruction holds a pointer to the last construction
  // done by the user, so that it can be quickly repeated

  mRememberConstruction = ca;
  aRepeatLastConstruction->setEnabled(true);
  aRepeatLastConstruction->setText( 
      i18n( "Repeat Construction (%1)", ca->descriptiveName() ) );
  aRepeatLastConstruction->setToolTip(
      i18n( "Repeat %1 (with new data)", ca->descriptiveName() ) );
}

KigPart::~KigPart()
{
  GUIActionList::instance()->unregDoc( this );

  // save our types...
  saveTypes();

  // objects get deleted automatically, when mobjsref gets
  // destructed..

  delete_all( aActions.begin(), aActions.end() );
  aActions.clear();

  // cleanup
  delete mMode;
  delete mhistory;

  delete mdocument;
}

bool KigPart::openFile()
{
  QFileInfo fileinfo( localFilePath() );
  if ( ! fileinfo.exists() )
  {
    KMessageBox::sorry( widget(),
                        i18n( "The file \"%1\" you tried to open does not exist. "
                              "Please verify that you entered the correct path.", localFilePath() ),
                        i18n( "File Not Found" ) );
    return false;
  };

  const QMimeDatabase mimeDb;
  QMimeType mimeType = mimeDb.mimeTypeForName( arguments().mimeType() );
  if ( !mimeType.isValid() )
  {
    // we can always use findByPath instead of findByURL with localFilePath()
    mimeType = mimeDb.mimeTypeForFile( localFilePath() );
  }
  qDebug() << "mimetype: " << mimeType.name();
  KigFilter* filter = KigFilters::instance()->find( mimeType.name() );
  if ( !filter )
  {
    // we don't support this mime type...
    KMessageBox::sorry
      (
        widget(),
        i18n( "You tried to open a document of type \"%1\"; unfortunately, Kig does not support this format. If you think the format in question would be worth implementing support for, you can open a feature request in <a href=\"https://bugs.kde.org/enter_bug.cgi?product=kig&bug_severity=wishlist\">KDE's bug tracking system</a>"
          , mimeType.name()),
        i18n( "Format Not Supported" ),
        KMessageBox::Notify | KMessageBox::AllowLink
        );
    return false;
  };

  KigDocument* newdoc = filter->load (localFilePath());
  if ( !newdoc )
  {
    closeUrl();
    setUrl( QUrl() );
    return false;
  }
  delete mdocument;
  mdocument = newdoc;
  coordSystemChanged( mdocument->coordinateSystem().id() );
  aToggleGrid->setChecked( mdocument->grid() );
  aToggleAxes->setChecked( mdocument->axes() );
  aToggleNightVision->setChecked( mdocument->getNightVision() );

  setModified(false);
  mhistory->clear();

  std::vector<ObjectCalcer*> tmp = calcPath( getAllParents( getAllCalcers( document().objects() ) ) );
  for ( std::vector<ObjectCalcer*>::iterator i = tmp.begin(); i != tmp.end(); ++i )
    ( *i )->calc( document() );
  emit recenterScreen();

  redrawScreen();

  return true;
}

bool KigPart::saveFile()
{
  if ( url().isEmpty() ) return internalSaveAs();
  // mimetype:
  const QMimeDatabase mimeDb;
  const QMimeType mimeType = mimeDb.mimeTypeForFile( localFilePath() );
  if ( mimeType.name() != QLatin1String("application/x-kig") )
  {
    // we don't support this mime type...
    if( KMessageBox::warningYesNo( widget(),
                     i18n( "Kig does not support saving to any other file format than "
                           "its own. Save to Kig's format instead?" ),
                     i18n( "Format Not Supported" ), KGuiItem( i18n( "Save Kig Format" ) ), KStandardGuiItem::cancel() ) == KMessageBox::No )
      return false;
    else
    {
      QFileInfo save( url().toLocalFile() );
      QString extension = save.completeSuffix();

      if ( extension.isEmpty() )
      {
        setUrl( QUrl::fromLocalFile( QString( "%1.kig" ).arg( save.absoluteFilePath() ) ) );
      }
      else
      {
        QString newFileName = save.absoluteFilePath();

        newFileName.replace( newFileName.lastIndexOf( extension ), extension.length(), QLatin1String( "kig" ) );
        setUrl( QUrl::fromLocalFile( newFileName ) );
      }
    }

    internalSaveAs();
  }

  if ( KigFilters::instance()->save( document(), localFilePath() ) )
  {
    setModified ( false );
    mhistory->setClean();
    return true;
  }
  return false;
}

bool KigPart::queryClose()
{
  if ( KParts::ReadWritePart::queryClose() )
  {
    if ( mode()->eventLoop() != 0 )
    {
      cancelConstruction();
    }
    return true;
  }
  return false;
}

void KigPart::addObject(ObjectHolder* o)
{
  if ( !misGroupingObjects )
    mhistory->push( KigCommand::addCommand( *this, o ) );
  else
  {
    _addObject( o );
    mcurrentObjectGroup.push_back( o );
  }
}

void KigPart::addObjects( const std::vector<ObjectHolder*>& os )
{
  if ( !misGroupingObjects )
    mhistory->push( KigCommand::addCommand( *this, os ) );
  else
  {
    _addObjects( os );
    mcurrentObjectGroup.insert( mcurrentObjectGroup.end(), os.begin(), os.end() );
  }
}

void KigPart::_addObject( ObjectHolder* o )
{
  document().addObject( o );
  setModified(true);
}

void KigPart::delObject( ObjectHolder* o )
{
  // we delete all children and their children etc. too...
  std::vector<ObjectHolder*> os;
  os.push_back( o );
  delObjects( os );
}

void KigPart::_delObjects( const std::vector<ObjectHolder*>& o )
{
  document().delObjects( o );
  setModified( true );
}

void KigPart::_delObject(ObjectHolder* o)
{
  document().delObject( o );
  setModified(true);
}

void KigPart::setMode( KigMode* m )
{
  mMode = m;
  m->enableActions();
  redrawScreen();
}

void KigPart::_addObjects( const std::vector<ObjectHolder*>& os )
{
  document().addObjects( os );
  setModified( true );
}

void KigPart::deleteObjects()
{
  mode()->deleteObjects();
}

void KigPart::startObjectGroup()
{
  if ( mcurrentObjectGroup.size() > 0 )
    qWarning() << "New object group started while already having objects in object group. Current group will be lost";
  
  mcurrentObjectGroup.clear();
  misGroupingObjects = true;
}

void KigPart::cancelObjectGroup()
{
  misGroupingObjects = false;
  _delObjects( mcurrentObjectGroup );
  mcurrentObjectGroup.clear();
}

void KigPart::finishObjectGroup()
{
  misGroupingObjects = false;
  _delObjects( mcurrentObjectGroup );
  addObjects( mcurrentObjectGroup );
  mcurrentObjectGroup.clear();
}

void KigPart::cancelConstruction()
{
  mode()->cancelConstruction();
}

void KigPart::repeatLastConstruction()
{
  if ( mRememberConstruction )
  {
    ConstructibleAction* ca = mRememberConstruction;
    ca->act( *this );
  }
}

void KigPart::showHidden()
{
  mode()->showHidden();
}

void KigPart::newMacro()
{
  mode()->newMacro();
}

void KigPart::editTypes()
{
  mode()->editTypes();
}

void KigPart::browseHistory()
{
  mode()->browseHistory();
}

void KigPart::setHistoryClean( bool clean )
{
  setModified( !clean );
}

void KigPart::setCoordinatePrecision()
{
  KigCoordinatePrecisionDialog dlg(document().isUserSpecifiedCoordinatePrecision(), document().getCoordinatePrecision() );
  
  if( dlg.exec() == QDialog::Accepted )
  {
    int coordinatePrecision = dlg.getUserSpecifiedCoordinatePrecision();
    
    document().setCoordinatePrecision( coordinatePrecision );
  }
}

QUndoStack* KigPart::history()
{
  return mhistory;
}

void KigPart::delObjects( const std::vector<ObjectHolder*>& os )
{
  if ( os.size() < 1 ) return;
  std::set<ObjectHolder*> delobjs;

  std::set<ObjectCalcer*> delcalcers = getAllChildren( getAllCalcers( os ) );
  std::map<ObjectCalcer*, ObjectHolder*> holdermap;

  std::set<ObjectHolder*> curobjs = document().objectsSet();

  for ( std::set<ObjectHolder*>::iterator i = curobjs.begin();
        i != curobjs.end(); ++i )
    holdermap[( *i )->calcer()] = *i;

  for ( std::set<ObjectCalcer*>::iterator i = delcalcers.begin();
        i != delcalcers.end(); ++i )
  {
    std::map<ObjectCalcer*, ObjectHolder*>::iterator j = holdermap.find( *i );
    if ( j != holdermap.end() )
      delobjs.insert( j->second );
  }

  assert( delobjs.size() >= os.size() );

  std::vector<ObjectHolder*> delobjsvect( delobjs.begin(), delobjs.end() );
  mhistory->push( KigCommand::removeCommand( *this, delobjsvect ) );
}

void KigPart::enableConstructActions( bool enabled )
{
  for_each( aActions.begin(), aActions.end(),
            bind2nd( mem_fun( &QAction::setEnabled ),
                     enabled ) );
}

void KigPart::unplugActionLists()
{
  unplugActionList( QStringLiteral("user_conic_types") );
  unplugActionList( QStringLiteral("user_segment_types") );
  unplugActionList( QStringLiteral("user_point_types") );
  unplugActionList( QStringLiteral("user_circle_types") );
  unplugActionList( QStringLiteral("user_line_types") );
  unplugActionList( QStringLiteral("user_other_types") );
  unplugActionList( QStringLiteral("user_types") );
}

void KigPart::plugActionLists()
{
  plugActionList( QStringLiteral("user_conic_types"), aMNewConic );
  plugActionList( QStringLiteral("user_segment_types"), aMNewSegment );
  plugActionList( QStringLiteral("user_point_types"), aMNewPoint );
  plugActionList( QStringLiteral("user_circle_types"), aMNewCircle );
  plugActionList( QStringLiteral("user_line_types"), aMNewLine );
  plugActionList( QStringLiteral("user_other_types"), aMNewOther );
  plugActionList( QStringLiteral("user_types"), aMNewAll );
}

void KigPart::emitStatusBarText( const QString& text )
{
  emit setStatusBarText( text );
}

void KigPart::fileSaveAs()
{
  internalSaveAs();
}

void KigPart::fileSave()
{
  save();
}

bool KigPart::internalSaveAs()
{
  // this slot is connected to the KStandardAction::saveAs action...
  QString formats = i18n( "Kig Documents (*.kig);;Compressed Kig Documents (*.kigz)" );
  QString currentDir = url().toLocalFile();

  if ( currentDir.isNull() )
  {
    currentDir = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
  }

  const QString file_name = QFileDialog::getSaveFileName( 0, QString(), currentDir, formats );
  if (file_name.isEmpty()) return false;
  else if ( QFileInfo( file_name ).exists() )
  {
    int ret = KMessageBox::warningContinueCancel( m_widget,
                                         i18n( "The file \"%1\" already exists. Do you wish to overwrite it?" ,
                                           file_name ), i18n( "Overwrite File?" ), KStandardGuiItem::overwrite() );
    if ( ret != KMessageBox::Continue )
    {
      return false;
    }
  }
  saveAs( QUrl::fromLocalFile( file_name ) );
  return true;
}

void KigPart::runMode( KigMode* m )
{
  KigMode* prev = mMode;

  setMode( m );

  QEventLoop e;
  m->setEventLoop( &e );
  e.exec( QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents );

  setMode( prev );
  redrawScreen();
}

void KigPart::doneMode( KigMode* d )
{
  assert( d == mMode );

  if ( d->eventLoop() )
    d->eventLoop()->exit();
}

void KigPart::actionRemoved( GUIAction* a, GUIUpdateToken& t )
{
  KigGUIAction* rem = 0;
  for ( std::vector<KigGUIAction*>::iterator i = aActions.begin(); i != aActions.end(); ++i )
  {
    if ( (*i)->guiAction() == a )
    {
      rem = *i;
      aActions.erase( i );
      break;
    }
  };
  assert( rem );
  aMNewSegment.removeAll( rem );
  aMNewConic.removeAll( rem );
  aMNewPoint.removeAll( rem );
  aMNewCircle.removeAll( rem );
  aMNewLine.removeAll( rem );
  aMNewOther.removeAll( rem );
  aMNewAll.removeAll( rem );
  t.push_back( rem );
}

void KigPart::actionAdded( GUIAction* a, GUIUpdateToken& )
{
  KigGUIAction* ret = new KigGUIAction( a, *this );
  aActions.push_back( ret );
  ret->plug( this );
}

void KigPart::endGUIActionUpdate( GUIUpdateToken& t )
{
  unplugActionLists();
  plugActionLists();
  delete_all( t.begin(), t.end() );
  t.clear();
}

KigPart::GUIUpdateToken KigPart::startGUIActionUpdate()
{
  return GUIUpdateToken();
}

void KigPart::setupMacroTypes()
{
  static bool alreadysetup = false;
  if ( ! alreadysetup )
  {
    alreadysetup = true;

    // the user's saved macro types:
    const QStringList dataFiles = getDataFiles( QStringLiteral("kig-types") );
    std::vector<Macro*> macros;
    for ( QStringList::const_iterator file = dataFiles.begin();
          file != dataFiles.end(); ++file )
    {
      std::vector<Macro*> nmacros;
      bool ok = MacroList::instance()->load( *file, nmacros, *this );
      if ( ! ok ) continue;
      copy( nmacros.begin(), nmacros.end(), back_inserter( macros ) );
    }
    MacroList::instance()->add( macros );
  };
  // hack: we need to plug the action lists _after_ the gui is
  // built.. i can't find a better solution than this...
  QTimer::singleShot( 0, this, &KigPart::plugActionLists );
}

void KigPart::setupBuiltinMacros()
{
  static bool alreadysetup = false;
  if ( ! alreadysetup )
  {
    alreadysetup = true;
    // builtin macro types ( we try to make the user think these are
    // normal types )..
    const QStringList builtinfiles = getDataFiles( QStringLiteral("builtin-macros") );
    for ( QStringList::const_iterator file = builtinfiles.begin();
          file != builtinfiles.end(); ++file )
    {
      std::vector<Macro*> macros;
      bool ok = MacroList::instance()->load( *file, macros, *this );
      if ( ! ok ) continue;
      for ( uint i = 0; i < macros.size(); ++i )
      {
        ObjectConstructorList* ctors = ObjectConstructorList::instance();
        GUIActionList* actions = GUIActionList::instance();
        Macro* macro = macros[i];
        macro->ctor->setBuiltin( true );
        ctors->add( macro->ctor );
        actions->add( macro->action );
        macro->ctor = 0;
        macro->action = 0;
        delete macro;
      };
    };
  };
}

void KigPart::addWidget( KigWidget* v )
{
  mwidgets.push_back( v );
}

void KigPart::delWidget( KigWidget* v )
{
  mwidgets.erase( std::remove( mwidgets.begin(), mwidgets.end(), v ), mwidgets.end() );
}

void KigPart::filePrintPreview()
{
  QPrinter printer;
  QPrintPreviewDialog printPreview( &printer );
  connect( &printPreview, &QPrintPreviewDialog::paintRequested, [this](QPrinter *p){
      doPrint( *p, document().grid(), document().axes() );
  });
  printPreview.exec();
}

void KigPart::filePrint()
{
  QPrinter printer;
  KigPrintDialogPage* kp = new KigPrintDialogPage();
  QPrintDialog printDialog( &printer, m_widget );
  printDialog.setWindowTitle( i18nc("@title:window", "Print Geometry") );
  printDialog.setOptionTabs( { kp } );
  printer.setFullPage( true );
  //Unsupported in Qt
  //printer.setPageSelection( QPrinter::ApplicationSide );
  kp->setPrintShowGrid( document().grid() );
  kp->setPrintShowAxes( document().axes() );
  if (printDialog.exec())
  {
    doPrint( printer, kp->printShowGrid(), kp->printShowAxes() );
  }
}

void KigPart::doPrint( QPrinter& printer, bool printGrid, bool printAxes )
{
  Rect rect = document().suggestedRect();
  QRect qrect( 0, 0, printer.width(), printer.height() );
  if ( rect.width() * qrect.height() > rect.height() * qrect.width() )
  {
    // qrect is too high..
    int nh = static_cast<int>( qrect.width() * rect.height() / rect.width() );
    int rest = qrect.height() - nh;
    qrect.setTop( qrect.top() - rest / 2 );
    qrect.setTop( rest / 2 );
  }
  else
  {
    // qrect is too wide..
    int nw = static_cast<int>( qrect.height() * rect.width() / rect.height() );
    int rest = qrect.width() - nw;
    qrect.setLeft( rest / 2 );
    qrect.setRight( qrect.right() - rest / 2 );
  };
  ScreenInfo si( rect, qrect );
  KigPainter painter( si, &printer, document() );
  painter.setWholeWinOverlay();
  painter.drawGrid( document().coordinateSystem(), printGrid, printAxes );
  painter.drawObjects( document().objects(), false );
}

void KigPart::slotSelectAll()
{
  mMode->selectAll();
}

void KigPart::slotDeselectAll()
{
  mMode->deselectAll();
}

void KigPart::slotInvertSelection()
{
  mMode->invertSelection();
}

void KigPart::hideObjects( const std::vector<ObjectHolder*>& inos )
{
  std::vector<ObjectHolder*> os;
  for (std::vector<ObjectHolder*>::const_iterator i = inos.begin(); i != inos.end(); ++i )
  {
    if ( (*i)->shown() )
      os.push_back( *i );
  };
  KigCommand* kc = 0;
  if ( os.size() == 0 ) return;
  else if ( os.size() == 1 )
    kc = new KigCommand( *this, os[0]->imp()->type()->hideAStatement() );
  else kc = new KigCommand( *this, i18np( "Hide %1 Object", "Hide %1 Objects", os.size() ) );
  for ( std::vector<ObjectHolder*>::iterator i = os.begin();
        i != os.end(); ++i )
    kc->addTask( new ChangeObjectDrawerTask( *i, ( *i )->drawer()->getCopyShown( false ) ) );
  mhistory->push( kc );
}

void KigPart::showObjects( const std::vector<ObjectHolder*>& inos )
{
  std::vector<ObjectHolder*> os;
  for (std::vector<ObjectHolder*>::const_iterator i = inos.begin(); i != inos.end(); ++i )
  {
    if ( !(*i)->shown() )
      os.push_back( *i );
  };
  KigCommand* kc = 0;
  if ( os.size() == 0 ) return;
  else if ( os.size() == 1 )
    kc = new KigCommand( *this, os[0]->imp()->type()->showAStatement() );
  else kc = new KigCommand( *this, i18np( "Show %1 Object", "Show %1 Objects", os.size() ) );
  for ( std::vector<ObjectHolder*>::iterator i = os.begin();
        i != os.end(); ++i )
    kc->addTask( new ChangeObjectDrawerTask( *i, ( *i )->drawer()->getCopyShown( true ) ) );
  mhistory->push( kc );
}

void KigPart::redrawScreen( KigWidget* w )
{
  mode()->redrawScreen( w );
}

void KigPart::redrawScreen()
{
  for ( std::vector<KigWidget*>::iterator i = mwidgets.begin();
        i != mwidgets.end(); ++i )
  {
    mode()->redrawScreen( *i );
  }
}

const KigDocument& KigPart::document() const
{
  return *mdocument;
}

KigDocument& KigPart::document()
{
  return *mdocument;
}

extern "C" KIGPART_EXPORT int convertToNative( const QUrl &url, const QByteArray& outfile )
{
  qDebug() << "converting " << url.toDisplayString( QUrl::PrettyDecoded ) << " to " << outfile;

  if ( ! url.isLocalFile() )
  {
    // TODO
    qCritical() << "--convert-to-native only supports local files for now.";
    return -1;
  }

  QString file = url.toLocalFile();

  QFileInfo fileinfo( file );
  if ( ! fileinfo.exists() )
  {
    qCritical() << "The file \"" << file << "\" does not exist";
    return -1;
  };

  const QMimeDatabase mimeDb;
  const QMimeType mimeType = mimeDb.mimeTypeForFile( file );
  qDebug() << "mimetype: " << mimeType.name();
  KigFilter* filter = KigFilters::instance()->find( mimeType.name() );
  if ( !filter )
  {
    qCritical() << "The file \"" << file << "\" is of a filetype not currently supported by Kig.";
    return -1;
  };

  KigDocument* doc = filter->load (file);
  if ( !doc )
  {
    qCritical() << "Parse error in file \"" << file << "\".";
    return -1;
  }

  std::vector<ObjectCalcer*> tmp = calcPath( getAllParents( getAllCalcers( doc->objects() ) ) );
  for ( std::vector<ObjectCalcer*>::iterator i = tmp.begin(); i != tmp.end(); ++i )
    ( *i )->calc( *doc );
  for ( std::vector<ObjectCalcer*>::iterator i = tmp.begin(); i != tmp.end(); ++i )
    ( *i )->calc( *doc );

  QString out = ( outfile == "-" ) ? QString() : outfile;
  bool success = KigFilters::instance()->save( *doc, out );
  if ( !success )
  {
    qCritical() << "something went wrong while saving";
    return -1;
  }

  delete doc;

  return 0;
}

void KigPart::toggleGrid()
{
  bool toshow = !mdocument->grid();
  aToggleGrid->setChecked( toshow );
  mdocument->setGrid( toshow );

  redrawScreen();
}

void KigPart::toggleAxes()
{
  bool toshow = !mdocument->axes();
  aToggleAxes->setChecked( toshow );
  mdocument->setAxes( toshow );

  redrawScreen();
}

void KigPart::toggleNightVision()
{
  bool nv = !mdocument->getNightVision();
  aToggleNightVision->setChecked( nv );
  mdocument->setNightVision( nv );

  redrawScreen();
}

void KigPart::coordSystemChanged( int id )
{
  aCoordSystem->setCurrentItem( id );
}

void KigPart::saveTypes()
{
  const QDir writeableDataLocation ( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) );
  const QDir typesDir( writeableDataLocation.absoluteFilePath( QStringLiteral("kig-types") ) );

  if ( !typesDir.exists() )
  {
    writeableDataLocation.mkpath( QStringLiteral("kig-types") );
  }

  const QString typesFileWithPath =  typesDir.absoluteFilePath( typesFile );

  // removing existent types file
  if ( QFile::exists( typesFileWithPath ) )
    QFile::remove( typesFileWithPath );

  MacroList* macrolist = MacroList::instance();
  macrolist->save( macrolist->macros(), typesFileWithPath );
}

void KigPart::loadTypes()
{
  const QDir writeableDataLocation ( QStandardPaths::writableLocation( QStandardPaths::DataLocation ) );
  const QDir typesDir( writeableDataLocation.absoluteFilePath( QStringLiteral("kig-types") ) );

  if ( typesDir.exists() )
  {
    const QString typesFileWithPath =  typesDir.absoluteFilePath( typesFile );

    if ( QFile::exists( typesFileWithPath ) )
    {
        std::vector<Macro*> macros;
        MacroList::instance()->load( typesFileWithPath, macros, *this );
        MacroList::instance()->add( macros );
    }
  }
}

void KigPart::deleteTypes()
{
  unplugActionLists();
  typedef MacroList::vectype vec;
  MacroList* macrolist = MacroList::instance();
  const vec& macros = macrolist->macros();
  for ( vec::const_reverse_iterator i = macros.rbegin(); i != macros.rend(); ++i )
  {
    macrolist->remove( *i );
  }
  plugActionLists();
}

#include "kig_part.moc"
