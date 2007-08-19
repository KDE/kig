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

#include "kig_part.h"
#include "kig_part.moc"

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
#include "../misc/kigpainter.h"
#include "../misc/lists.h"
#include "../misc/object_constructor.h"
#include "../misc/screeninfo.h"
#include "../modes/normal.h"
#include "../objects/object_drawer.h"
#include "../objects/point_imp.h"

#include <algorithm>
#include <functional>

#include <kaction.h>
#include <kapplication.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <kmessagebox.h>
#include <kmimetype.h>
#include <kprinter.h>
#include <kstandarddirs.h>
#include <kstdaction.h>
#include <ktoolbar.h>
#include <kparts/genericfactory.h>
#include <kdeprint/kprintdialogpage.h>

#include <qcheckbox.h>
#include <qfile.h>
#include <qlayout.h>
#include <qpaintdevicemetrics.h>
#include <qsizepolicy.h>
#include <qtimer.h>
#if QT_VERSION >= 0x030100
#include <qeventloop.h>
#endif

using namespace std;

static const QString typesFile = "macros.kigt";

// export this library...
typedef KParts::GenericFactory<KigPart> KigPartFactory;
K_EXPORT_COMPONENT_FACTORY ( libkigpart, KigPartFactory )

KAboutData* KigPart::createAboutData()
{
  return kigAboutData( "kig", I18N_NOOP( "KigPart" ) );
}

class SetCoordinateSystemAction
  : public KSelectAction
{
  KigPart& md;
public:
  SetCoordinateSystemAction( KigPart& d, KActionCollection* parent );
  void slotActivated( int index );
};

SetCoordinateSystemAction::SetCoordinateSystemAction(
  KigPart& d, KActionCollection* parent )
  : KSelectAction( i18n( "&Set Coordinate System" ), 0, parent, "settings_set_coordinate_system" ),
    md( d )
{
  setItems( CoordinateSystemFactory::names() );
  setCurrentItem( md.document().coordinateSystem().id() );
}

void SetCoordinateSystemAction::slotActivated( int index )
{
  CoordinateSystem* sys = CoordinateSystemFactory::build( index );
  assert( sys );
  md.history()->addCommand( KigCommand::changeCoordSystemCommand( md, sys ) );
  setCurrentItem( index );
}

class KigPrintDialogPage
  : public KPrintDialogPage
{
public:
  KigPrintDialogPage( QWidget* parent = 0, const char* name = 0 );
  ~KigPrintDialogPage();

  void getOptions( QMap<QString,QString>& opts, bool );
  void setOptions( const QMap<QString,QString>& opts );
  bool isValid( QString& );

private:
  QCheckBox *showgrid;
  QCheckBox *showaxes;
};

KigPrintDialogPage::KigPrintDialogPage( QWidget* parent, const char* name )
 : KPrintDialogPage( parent, name )
{
  setTitle( i18n( "Kig Options" ) );

 QVBoxLayout* vl = new QVBoxLayout( this, 0 , 11 );

 showgrid = new QCheckBox( i18n( "Show grid" ), this );
 vl->addWidget( showgrid );

 showaxes = new QCheckBox( i18n( "Show axes" ), this );
 vl->addWidget( showaxes );

 vl->addItem( new QSpacerItem( 10, 10, QSizePolicy::Fixed, QSizePolicy::Expanding ) );
}

KigPrintDialogPage::~KigPrintDialogPage()
{
}

void KigPrintDialogPage::getOptions( QMap< QString, QString >& opts, bool )
{
  opts[ "kde-kig-showgrid" ] = QString::number( showgrid->isChecked() );
  opts[ "kde-kig-showaxes" ] = QString::number( showaxes->isChecked() );
}

void KigPrintDialogPage::setOptions( const QMap< QString, QString >& opts )
{
  QString tmp = opts[ "kde-kig-showgrid" ];
  bool bt = ( tmp != "0" );
  showgrid->setChecked( bt );
  tmp = opts[ "kde-kig-showaxes" ];
  bt = ( tmp != "0" );
  showaxes->setChecked( bt );
}

bool KigPrintDialogPage::isValid( QString& )
{
  return true;
}

KigPart::KigPart( QWidget *parentWidget, const char *,
			  QObject *parent, const char *name,
			  const QStringList& )
  : KParts::ReadWritePart( parent, name ),
    mMode( 0 ), mdocument( new KigDocument() )
{
  // we need an instance
  setInstance( KigPartFactory::instance() );

  mMode = new NormalMode( *this );

  // we need a widget, to actually show the document
  m_widget = new KigView(this, false, parentWidget, "kig_view");
  // notify the part that this is our internal widget
  setWidget( m_widget );

  // create our actions...
  setupActions();

  // set our XML-UI resource file
  setXMLFile("kigpartui.rc");

  // our types...
  setupTypes();

  // construct our command history
  mhistory = new KCommandHistory(actionCollection());
  mhistory->documentSaved();
  connect( mhistory, SIGNAL( documentRestored() ), this, SLOT( setUnmodified() ) );

  // we are read-write by default
  setReadWrite(true);

  setModified (false);

  GUIActionList::instance()->regDoc( this );
}

void KigPart::setupActions()
{
  // save actions..
  (void) KStdAction::saveAs(this, SLOT(fileSaveAs()), actionCollection());
  (void) KStdAction::save(this, SLOT(fileSave()), actionCollection());

  // print actions
  (void) KStdAction::print( this, SLOT( filePrint() ), actionCollection() );
  (void) KStdAction::printPreview( this, SLOT( filePrintPreview() ), actionCollection() );

  // selection actions
  aSelectAll = KStdAction::selectAll(
    this, SLOT( slotSelectAll() ), actionCollection() );
  aDeselectAll = KStdAction::deselect(
    this, SLOT( slotDeselectAll() ), actionCollection() );
  aInvertSelection = new KAction(
    i18n( "Invert Selection" ), "", 0, this,
    SLOT( slotInvertSelection() ), actionCollection(),
    "edit_invert_selection" );

  // we need icons...
  KIconLoader* l = instance()->iconLoader();
  QPixmap tmp;

  aDeleteObjects = new KAction(
      i18n("&Delete Objects"), "editdelete", Key_Delete, this,
      SLOT(deleteObjects()), actionCollection(), "delete_objects");
  aDeleteObjects->setToolTip(i18n("Delete the selected objects"));

  aCancelConstruction = new KAction(
      i18n("Cancel Construction"), "stop", Key_Escape, this,
      SLOT(cancelConstruction()), actionCollection(), "cancel_construction");
  aCancelConstruction->setToolTip(
      i18n("Cancel the construction of the object being constructed"));
  aCancelConstruction->setEnabled(false);

  aShowHidden = new KAction(
    i18n("U&nhide All"), 0, this, SLOT( showHidden() ),
    actionCollection(), "edit_unhide_all");
  aShowHidden->setToolTip(i18n("Show all hidden objects"));
  aShowHidden->setEnabled( true );

  aNewMacro = new KAction(
    i18n("&New Macro..."), "gear", 0, this, SLOT(newMacro()),
    actionCollection(), "macro_action");
  aNewMacro->setToolTip(i18n("Define a new macro"));

  aConfigureTypes = new KAction(
    i18n("Manage &Types..."), 0, this, SLOT(editTypes()),
    actionCollection(), "types_edit");
  aConfigureTypes->setToolTip(i18n("Manage macro types."));

  KigExportManager::instance()->addMenuAction( this, m_widget->realWidget(),
                                               actionCollection() );

  KAction* a = KStdAction::zoomIn( m_widget, SLOT( slotZoomIn() ),
                                   actionCollection() );
  a->setToolTip( i18n( "Zoom in on the document" ) );
  a->setWhatsThis( i18n( "Zoom in on the document" ) );

  a = KStdAction::zoomOut( m_widget, SLOT( slotZoomOut() ),
                           actionCollection() );
  a->setToolTip( i18n( "Zoom out of the document" ) );
  a->setWhatsThis( i18n( "Zoom out of the document" ) );

  a = KStdAction::fitToPage( m_widget, SLOT( slotRecenterScreen() ),
                             actionCollection() );
  // grr.. why isn't there an icon for this..
  a->setIconSet( QIconSet( l->loadIcon( "view_fit_to_page", KIcon::Toolbar ) ) );
  a->setToolTip( i18n( "Recenter the screen on the document" ) );
  a->setWhatsThis( i18n( "Recenter the screen on the document" ) );

#ifdef KDE_IS_VERSION
#if KDE_IS_VERSION(3,1,90)
#define KIG_PART_CPP_STD_FULLSCREEN_ACTION
#endif
#endif
#ifdef KIG_PART_CPP_STD_FULLSCREEN_ACTION
  a = KStdAction::fullScreen( m_widget, SLOT( toggleFullScreen() ), actionCollection(), (QWidget*)(widget()->parent()),"fullscreen" );
#else
  tmp = l->loadIcon( "window_fullscreen", KIcon::Toolbar );
  a = new KAction(
    i18n( "Full Screen" ), tmp, CTRL+SHIFT+Key_F,
    m_widget, SLOT( toggleFullScreen() ),
    actionCollection(), "fullscreen" );
#endif
  a->setToolTip( i18n( "View this document full-screen." ) );
  a->setWhatsThis( i18n( "View this document full-screen." ) );

  // TODO: an icon for this..
  a = new KAction(
    i18n( "&Select Shown Area" ), "viewmagfit", 0, m_widget, SLOT( zoomRect() ),
    actionCollection(), "view_select_shown_rect" );
  a->setToolTip( i18n( "Select the area that you want to be shown in the window." ) );
  a->setWhatsThis( i18n( "Select the area that you want to be shown in the window." ) );

  a = new KAction(
    i18n( "S&elect Zoom Area" ), "viewmag", 0, m_widget, SLOT( zoomArea() ),
    actionCollection(), "view_zoom_area" );
//  a->setToolTip( i18n( "Select the area that you want to be shown in the window." ) );
//  a->setWhatsThis( i18n( "Select the area that you want to be shown in the window." ) );

  aToggleGrid = new KToggleAction(
    i18n( "Show &Grid" ), 0, this, SLOT( toggleGrid() ),
    actionCollection(), "settings_show_grid" );
  aToggleGrid->setToolTip( i18n( "Show or hide the grid." ) );
  aToggleGrid->setChecked( true );

  aToggleAxes = new KToggleAction(
    i18n( "Show &Axes" ), 0, this, SLOT( toggleAxes() ),
    actionCollection(), "settings_show_axes" );
  aToggleAxes->setToolTip( i18n( "Show or hide the axes." ) );
  aToggleAxes->setChecked( true );

  aToggleNightVision = new KToggleAction(
    i18n( "Wear Infrared Glasses" ), 0, this, SLOT( toggleNightVision() ),
    actionCollection(), "settings_toggle_nightvision" );
  aToggleNightVision->setToolTip( i18n( "Enable/Disable hidden objects visibility." ) );
  aToggleNightVision->setChecked( false );

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
    KigGUIAction* ret = new KigGUIAction( *i, *this, actionCollection() );
    aActions.push_back( ret );
    ret->plug( this );
  };
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
  QFileInfo fileinfo( m_file );
  if ( ! fileinfo.exists() )
  {
    KMessageBox::sorry( widget(),
                        i18n( "The file \"%1\" you tried to open does not exist. "
                              "Please verify that you entered the correct path." ).arg( m_file ),
                        i18n( "File Not Found" ) );
    return false;
  };

  // m_file is always local, so we can use findByPath instead of
  // findByURL...
  KMimeType::Ptr mimeType = KMimeType::findByPath ( m_file );
  kdDebug() << k_funcinfo << "mimetype: " << mimeType->name() << endl;
  KigFilter* filter = KigFilters::instance()->find( mimeType->name() );
  if ( !filter )
  {
    // we don't support this mime type...
    KMessageBox::sorry
      (
        widget(),
        i18n( "You tried to open a document of type \"%1\"; unfortunately, "
              "Kig does not support this format. If you think the format in "
              "question would be worth implementing support for, you can "
              "always ask us nicely on mailto:toscano.pino@tiscali.it "
              "or do the work yourself and send me a patch."
          ).arg(mimeType->name()),
        i18n( "Format Not Supported" )
        );
    return false;
  };

  KigDocument* newdoc = filter->load (m_file);
  if ( !newdoc )
  {
    closeURL();
    m_url = KURL();
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
  if ( m_file.isEmpty() || m_bTemp ) return internalSaveAs();
  // mimetype:
  KMimeType::Ptr mimeType = KMimeType::findByPath ( m_file );
  if ( mimeType->name() != "application/x-kig" )
  {
    // we don't support this mime type...
    if( KMessageBox::warningYesNo( widget(),
                     i18n( "Kig does not support saving to any other file format than "
                           "its own. Save to Kig's format instead?" ),
                     i18n( "Format Not Supported" ), i18n("Save Kig Format"), KStdGuiItem::cancel() ) == KMessageBox::No )
      return false;
    internalSaveAs();
  };

  if ( KigFilters::instance()->save( document(), m_file ) )
  {
    setModified ( false );
    mhistory->documentSaved();
    return true;
  }
  return false;
}

void KigPart::addObject(ObjectHolder* o)
{
  mhistory->addCommand( KigCommand::addCommand( *this, o ) );
}

void KigPart::addObjects( const std::vector<ObjectHolder*>& os )
{
  mhistory->addCommand( KigCommand::addCommand( *this, os ) );
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

void KigPart::cancelConstruction()
{
  mode()->cancelConstruction();
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

void KigPart::setUnmodified()
{
  setModified( false );
}

KCommandHistory* KigPart::history()
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
  mhistory->addCommand( KigCommand::removeCommand( *this, delobjsvect ) );
}

void KigPart::enableConstructActions( bool enabled )
{
  for_each( aActions.begin(), aActions.end(),
            bind2nd( mem_fun( &KAction::setEnabled ),
                     enabled ) );
}

void KigPart::unplugActionLists()
{
  unplugActionList( "user_conic_types" );
  unplugActionList( "user_segment_types" );
  unplugActionList( "user_point_types" );
  unplugActionList( "user_circle_types" );
  unplugActionList( "user_line_types" );
  unplugActionList( "user_other_types" );
  unplugActionList( "user_types" );
}

void KigPart::plugActionLists()
{
  plugActionList( "user_conic_types", aMNewConic );
  plugActionList( "user_segment_types", aMNewSegment );
  plugActionList( "user_point_types", aMNewPoint );
  plugActionList( "user_circle_types", aMNewCircle );
  plugActionList( "user_line_types", aMNewLine );
  plugActionList( "user_other_types", aMNewOther );
  plugActionList( "user_types", aMNewAll );
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
  // this slot is connected to the KStdAction::saveAs action...
  QString formats = i18n( "*.kig|Kig Documents (*.kig)\n"
                          "*.kigz|Compressed Kig Documents (*.kigz)" );

  //  formats += "\n";
  //  formats += KImageIO::pattern( KImageIO::Writing );

  QString file_name = KFileDialog::getSaveFileName(":document", formats );
  if (file_name.isEmpty()) return false;
  else if ( QFileInfo( file_name ).exists() )
  {
    int ret = KMessageBox::warningContinueCancel( m_widget,
                                         i18n( "The file \"%1\" already exists. Do you wish to overwrite it?" )
                                         .arg( file_name ), i18n( "Overwrite File?" ), i18n("Overwrite") );
    if ( ret != KMessageBox::Continue )
    {
      return false;
    }
  }
  saveAs(KURL::fromPathOrURL( file_name ));
  return true;
}

void KigPart::runMode( KigMode* m )
{
  KigMode* prev = mMode;

  setMode( m );

#if QT_VERSION >= 0x030100
  (void) kapp->eventLoop()->enterLoop();
#else
  (void) kapp->enter_loop();
#endif

  setMode( prev );
  redrawScreen();
}

void KigPart::doneMode( KigMode* d )
{
  assert( d == mMode );
  // pretend to use this var..
  (void)d;
#if QT_VERSION >= 0x030100
  kapp->eventLoop()->exitLoop();
#else
  kapp->exit_loop();
#endif
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
  aMNewSegment.remove( rem );
  aMNewConic.remove( rem );
  aMNewPoint.remove( rem );
  aMNewCircle.remove( rem );
  aMNewLine.remove( rem );
  aMNewOther.remove( rem );
  aMNewAll.remove( rem );
  t.push_back( rem );
}

void KigPart::actionAdded( GUIAction* a, GUIUpdateToken& )
{
  KigGUIAction* ret = new KigGUIAction( a, *this, actionCollection() );
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
    QStringList dataFiles =
      KGlobal::dirs()->findAllResources("appdata", "kig-types/*.kigt",
                                        true, false );
    std::vector<Macro*> macros;
    for ( QStringList::iterator file = dataFiles.begin();
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
  QTimer::singleShot( 0, this, SLOT( plugActionLists() ) );
}

void KigPart::setupBuiltinMacros()
{
  static bool alreadysetup = false;
  if ( ! alreadysetup )
  {
    alreadysetup = true;
    // builtin macro types ( we try to make the user think these are
    // normal types )..
    QStringList builtinfiles =
      KGlobal::dirs()->findAllResources( "appdata", "builtin-macros/*.kigt", true, false );
    for ( QStringList::iterator file = builtinfiles.begin();
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
  KPrinter printer;
  printer.setPreviewOnly( true );
  doPrint( printer );
}

void KigPart::filePrint()
{
  KPrinter printer;
  KigPrintDialogPage* kp = new KigPrintDialogPage();
  printer.addDialogPage( kp );
  printer.setFullPage( true );
  printer.setOption( "kde-kig-showgrid", QString::number( document().grid() ) );
  printer.setOption( "kde-kig-showaxes", QString::number( document().axes() ) );
  printer.setPageSelection( KPrinter::ApplicationSide );
  if ( printer.setup( m_widget, i18n("Print Geometry") ) )
  {
    doPrint( printer );
  };
}

void KigPart::doPrint( KPrinter& printer )
{
  QPaintDeviceMetrics metrics( &printer );
  Rect rect = document().suggestedRect();
  QRect qrect( 0, 0, metrics.width(), metrics.height() );
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
  bool sg = true;
  bool sa = true;
  if ( !printer.previewOnly() )
  {
    sg = ( printer.option( "kde-kig-showgrid" ) != "0" );
    sa = ( printer.option( "kde-kig-showaxes" ) != "0" );
  }
  else
  {
    sg = document().grid();
    sg = document().axes();
  }
  painter.drawGrid( document().coordinateSystem(), sg, sa );
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
  else kc = new KigCommand( *this, i18n( "Hide %n Object", "Hide %n Objects", os.size() ) );
  for ( std::vector<ObjectHolder*>::iterator i = os.begin();
        i != os.end(); ++i )
    kc->addTask( new ChangeObjectDrawerTask( *i, ( *i )->drawer()->getCopyShown( false ) ) );
  mhistory->addCommand( kc );
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
  else kc = new KigCommand( *this, i18n( "Show %n Object", "Show %n Objects", os.size() ) );
  for ( std::vector<ObjectHolder*>::iterator i = os.begin();
        i != os.end(); ++i )
    kc->addTask( new ChangeObjectDrawerTask( *i, ( *i )->drawer()->getCopyShown( true ) ) );
  mhistory->addCommand( kc );
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

extern "C" int convertToNative( const KURL& url, const QCString& outfile )
{
  kdDebug() << "converting " << url.prettyURL() << " to " << outfile << endl;

  if ( ! url.isLocalFile() )
  {
    // TODO
    kdError() << "--convert-to-native only supports local files for now." << endl;
    return -1;
  }

  QString file = url.path();

  QFileInfo fileinfo( file );
  if ( ! fileinfo.exists() )
  {
    kdError() << "The file \"" << file << "\" does not exist" << endl;
    return -1;
  };

  KMimeType::Ptr mimeType = KMimeType::findByPath ( file );
  kdDebug() << k_funcinfo << "mimetype: " << mimeType->name() << endl;
  KigFilter* filter = KigFilters::instance()->find( mimeType->name() );
  if ( !filter )
  {
    kdError() << "The file \"" << file << "\" is of a filetype not currently supported by Kig." << endl;
    return -1;
  };

  KigDocument* doc = filter->load (file);
  if ( !doc )
  {
    kdError() << "Parse error in file \"" << file << "\"." << endl;
    return -1;
  }

  std::vector<ObjectCalcer*> tmp = calcPath( getAllParents( getAllCalcers( doc->objects() ) ) );
  for ( std::vector<ObjectCalcer*>::iterator i = tmp.begin(); i != tmp.end(); ++i )
    ( *i )->calc( *doc );
  for ( std::vector<ObjectCalcer*>::iterator i = tmp.begin(); i != tmp.end(); ++i )
    ( *i )->calc( *doc );

  QString out = ( outfile == "-" ) ? QString::null : outfile;
  bool success = KigFilters::instance()->save( *doc, out );
  if ( !success )
  {
    kdError() << "something went wrong while saving" << endl;
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
  QString typesDir = KGlobal::dirs()->saveLocation( "appdata", "kig-types" );
  if ( typesDir[ typesDir.length() - 1 ] != '/' )
    typesDir += '/';
  QString typesFileWithPath = typesDir + typesFile;

  // removing existant types file
  if ( QFile::exists( typesFileWithPath ) )
    QFile::remove( typesFileWithPath );

  MacroList* macrolist = MacroList::instance();
  macrolist->save( macrolist->macros(), typesFileWithPath );
}

void KigPart::loadTypes()
{
  QString typesDir = KGlobal::dirs()->saveLocation( "appdata", "kig-types" );
  if ( typesDir[ typesDir.length() - 1 ] != '/' )
    typesDir += '/';
  QString typesFileWithPath = typesDir + typesFile;

  if ( QFile::exists( typesFileWithPath ) )
  {
    std::vector<Macro*> macros;
    MacroList::instance()->load( typesFileWithPath, macros, *this );
    MacroList::instance()->add( macros );
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
