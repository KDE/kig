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

#include "kig_part.h"
#include "kig_part.moc"

#include "aboutdata.h"
#include "kig_commands.h"
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
#include "../objects/point_imp.h"
#include "../objects/object_drawer.h"

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
#include <kparts/genericfactory.h>
#include <kprinter.h>
#include <kstandarddirs.h>
#include <kstdaction.h>
#include <ktoolbar.h>

#include <qfile.h>
#include <qpaintdevicemetrics.h>
#include <qtimer.h>
#if QT_VERSION >= 0x030100
#include <qeventloop.h>
#endif

#include <algorithm>
#include <functional>

using namespace std;

// export this library...
typedef KParts::GenericFactory<KigDocument> KigDocumentFactory;
K_EXPORT_COMPONENT_FACTORY ( libkigpart, KigDocumentFactory )

KAboutData* KigDocument::createAboutData()
{
  return kigAboutData( "kigpart", I18N_NOOP( "KigPart" ) );
}

class SetCoordinateSystemAction
  : public KAction
{
  KigDocument& md;
  uint mn;
public:
  SetCoordinateSystemAction( const QString text, KigDocument& d,
                             uint i, KActionCollection* parent );
  void slotActivated();
};

SetCoordinateSystemAction::SetCoordinateSystemAction(
  const QString text, KigDocument& d,
  uint i, KActionCollection* parent )
  : KAction( text, 0, 0, 0, parent, 0 ),
    md( d ), mn( i )
{
}

void SetCoordinateSystemAction::slotActivated()
{
  CoordinateSystem* sys = CoordinateSystemFactory::build( mn );
  assert( sys );
  md.history()->addCommand( KigCommand::changeCoordSystemCommand( md, sys ) );
}

KigDocument::KigDocument( QWidget *parentWidget, const char *,
			  QObject *parent, const char *name,
			  const QStringList& )
  : KParts::ReadWritePart( parent, name ),
    mMode( 0 ),
    mcoordsystem( new EuclideanCoords )
{
  // we need an instance
  setInstance( KigDocumentFactory::instance() );

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

void KigDocument::setupActions()
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
  KIconLoader* l = KGlobal::iconLoader();
  QPixmap tmp;

  tmp = l->loadIcon( "editdelete", KIcon::Toolbar);
  aDeleteObjects = new KAction(
      i18n("&Delete Objects"), "editdelete", Key_Delete, this,
      SLOT(deleteObjects()), actionCollection(), "delete_objects");
  aDeleteObjects->setToolTip(i18n("Delete the selected objects"));

  tmp = l->loadIcon( "stop", KIcon::Toolbar);
  aCancelConstruction = new KAction(
      i18n("Cancel Construction"), tmp, Key_Escape, this,
      SLOT(cancelConstruction()), actionCollection(), "cancel_construction");
  aCancelConstruction->setToolTip(
      i18n("Cancel the construction of the object being constructed"));
  aCancelConstruction->setEnabled(false);

  aShowHidden = new KAction(
    i18n("Unhide &All"), 0, this, SLOT( showHidden() ),
    actionCollection(), "edit_unhide_all");
  aShowHidden->setToolTip(i18n("Show all hidden objects"));
  aShowHidden->setEnabled( true );

  tmp = l->loadIcon("gear", KIcon::Toolbar);
  aNewMacro = new KAction(
    i18n("&New Macro..."), tmp, 0, this, SLOT(newMacro()),
    actionCollection(), "macro_action");
  aNewMacro->setToolTip(i18n("Define a new macro"));

  aConfigureTypes = new KAction(
    i18n("Manage &Types"), 0, this, SLOT(editTypes()),
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
  a->setIconSet( QIconSet( l->loadIcon( "view_fit_to_page", KIcon::User ) ) );
  a->setToolTip( i18n( "Recenter the screen on the document" ) );
  a->setWhatsThis( i18n( "Recenter the screen on the document" ) );

#ifdef KDE_IS_VERSION
#if KDE_IS_VERSION(3,1,90)
#define KIG_PART_CPP_STD_FULLSCREEN_ACTION
#endif
#endif
#ifdef KIG_PART_CPP_STD_FULLSCREEN_ACTION
  a = KStdAction::fullScreen( m_widget, SLOT( toggleFullScreen() ), actionCollection(), 0 );
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
  tmp = l->loadIcon( "viewmagfit", KIcon::Toolbar );
  a = new KAction(
    i18n( "Select Shown Area" ), tmp, 0, m_widget, SLOT( zoomRect() ),
    actionCollection(), "view_select_shown_rect" );
  a->setToolTip( i18n( "Select the area that you want to be shown in the window." ) );
  a->setWhatsThis( i18n( "Select the area that you want to be shown in the window." ) );

  // select coordinate system KActionMenu..
  KActionMenu* am =
    new KActionMenu( i18n( "&Set Coordinate System" ), actionCollection(),
                     "settings_set_coordinate_system" );
  QStringList csnames = CoordinateSystemFactory::names();
  for ( uint i = 0; i < csnames.size(); ++i )
    am->insert( new SetCoordinateSystemAction( csnames[i], *this, i,
                                               actionCollection() ) );
}

void KigDocument::setupTypes()
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

KigDocument::~KigDocument()
{
  GUIActionList::instance()->unregDoc( this );

  // TODO: remove some time in the future..  doesn't really hurt, but
  // well, it's not really necessary either..  However, I'm not going
  // to remove this in the next release yet, since it would cause
  // duplication of macro types combined with the changing to use
  // macro.kigt instead of macro files named after the macro they
  // contain..
  // remove old types:
  QStringList relFiles;
  QStringList dataFiles =
    KGlobal::dirs()->findAllResources("appdata", "kig-types/*.kigt",
                                        true, false, relFiles);
  for ( QStringList::iterator file = dataFiles.begin();
        file != dataFiles.end(); ++file )
  {
    QFile f( *file );
    f.remove();
  };

  // save our types...
  QString typesDir = KGlobal::dirs()->saveLocation("appdata", "kig-types");
  if (typesDir[typesDir.length() - 1] != '/') typesDir += '/';
  MacroList* macrolist = MacroList::instance();
  macrolist->save( macrolist->macros(), typesDir + "macros.kigt" );

  // objects get deleted automatically, when mobjsref gets
  // destructed..

  delete_all( aActions.begin(), aActions.end() );
  aActions.clear();

  // cleanup
  delete mcoordsystem;
  delete mMode;
  delete mhistory;
}

bool KigDocument::openFile()
{
  QFileInfo fileinfo( m_file );
  if ( ! fileinfo.exists() )
  {
    KMessageBox::sorry( widget(),
                        i18n( "The file \"%1\" you tried to open does not exist.  "
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
        i18n( "You tried to open a document of type \"%1\". Unfortunately, "
              "Kig doesn't support this format. If you think the format in "
              "question would be worth implementing support for, you can "
              "always ask me nicely on mailto:devriese@kde.org "
              "or do the work yourself and send me a patch."
          ).arg(mimeType->name()),
        i18n( "Format not Supported" )
        );
    return false;
  };
  if ( !filter->load (m_file, *this) )
    return false;

  setModified(false);
  mhistory->clear();

  std::vector<ObjectCalcer*> tmp = calcPath( getAllParents( getCalcers( objects() ) ) );
  for ( std::vector<ObjectCalcer*>::iterator i = tmp.begin(); i != tmp.end(); ++i )
    ( *i )->calc( *this );
  emit recenterScreen();

  redrawScreen();

  return true;
}

bool KigDocument::saveFile()
{
  if ( m_file.isEmpty() ) return internalSaveAs();
  // mimetype:
  KMimeType::Ptr mimeType = KMimeType::findByPath ( m_file );
  if ( mimeType->name() != "application/x-kig" )
  {
    // we don't support this mime type...
    KMessageBox::sorry
      (
        widget(),
        i18n( "Kig does not support saving to any other file format than "
              "its own." ),
//              "its own." ).arg( mimeType->name() ),
        i18n( "Format not Supported" )
        );
    return false;
  };

  if ( KigFilters::instance()->save( *this, m_file ) )
  {
    setModified ( false );
    mhistory->documentSaved();
    return true;
  }
  return false;
}

void KigDocument::addObject(ObjectHolder* o)
{
  mhistory->addCommand( KigCommand::addCommand( *this, o ) );
}

void KigDocument::addObjects( const std::vector<ObjectHolder*>& os )
{
  mhistory->addCommand( KigCommand::addCommand( *this, os ) );
}

void KigDocument::_addObject( ObjectHolder* o )
{
  mobjs.insert( o );
  setModified(true);
}

void KigDocument::delObject( ObjectHolder* o )
{
  // we delete all children and their children etc. too...
  std::vector<ObjectHolder*> os;
  os.push_back( o );
  delObjects( os );
}

void KigDocument::_delObjects( const std::vector<ObjectHolder*>& o )
{
  for ( std::vector<ObjectHolder*>::const_iterator i = o.begin();
        i != o.end(); ++i )
  {
    mobjs.erase( *i );
  };
  setModified( true );
}

void KigDocument::_delObject(ObjectHolder* o)
{
  mobjs.erase( o );
  setModified(true);
}

std::vector<ObjectHolder*> KigDocument::whatAmIOn(const Coordinate& p, const KigWidget& w ) const
{
  std::vector<ObjectHolder*> ret;
  std::vector<ObjectHolder*> nonpoints;
  for ( std::set<ObjectHolder*>::const_iterator i = mobjs.begin();
        i != mobjs.end(); ++i )
  {
    if(!(*i)->contains(p, w)) continue;
    if ( (*i)->imp()->inherits( PointImp::stype() ) ) ret.push_back( *i );
    else nonpoints.push_back( *i );
  };
  std::copy( nonpoints.begin(), nonpoints.end(), std::back_inserter( ret ) );
  return ret;
}

std::vector<ObjectHolder*> KigDocument::whatIsInHere( const Rect& p, const KigWidget& w )
{
  std::vector<ObjectHolder*> ret;
  std::vector<ObjectHolder*> nonpoints;
  for ( std::set<ObjectHolder*>::const_iterator i = mobjs.begin();
        i != mobjs.end(); ++i )
  {
    if(! (*i)->inRect( p, w ) ) continue;
    if ( (*i)->imp()->inherits( PointImp::stype() ) ) ret.push_back( *i );
    else nonpoints.push_back( *i );
  };
  std::copy( nonpoints.begin(), nonpoints.end(), std::back_inserter( ret ) );
  return ret;
}

Rect KigDocument::suggestedRect() const
{
  bool rectInited = false;
  Rect r(0.,0.,0.,0.);
  for ( std::set<ObjectHolder*>::const_iterator i = mobjs.begin();
        i != mobjs.end(); ++i )
  {
    if ( (*i)->shown() && (*i)->imp()->inherits( PointImp::stype() ) )
    {
      if( !rectInited )
      {
        r.setCenter( static_cast<const PointImp*>( (*i)->imp() )->coordinate() );
        rectInited = true;
      }
      else
        r.setContains( static_cast<const PointImp*>( (*i)->imp() )->coordinate() );
    };
  };

  if ( ! rectInited )
    return Rect( -5.5, -5.5, 11., 11. );
  r.setContains( Coordinate( 0, 0 ) );
  if( r.width() == 0 ) r.setWidth( 1 );
  if( r.height() == 0 ) r.setHeight( 1 );
  Coordinate center = r.center();
  r *= 2;
  r.setCenter(center);
  return r;
}

const CoordinateSystem& KigDocument::coordinateSystem() const
{
  assert( mcoordsystem );
  return *mcoordsystem;
}

void KigDocument::setMode( KigMode* m )
{
  mMode = m;
  m->enableActions();
  redrawScreen();
}

void KigDocument::_addObjects( const std::vector<ObjectHolder*>& os )
{
  for ( std::vector<ObjectHolder*>::const_iterator i = os.begin();
        i != os.end(); ++i )
    ( *i )->calc( *this );
  std::copy( os.begin(), os.end(), std::inserter( mobjs, mobjs.begin() ) );
  setModified( true );
}

void KigDocument::deleteObjects()
{
  mode()->deleteObjects();
}

void KigDocument::cancelConstruction()
{
  mode()->cancelConstruction();
}

void KigDocument::showHidden()
{
  mode()->showHidden();
}

void KigDocument::newMacro()
{
  mode()->newMacro();
}

void KigDocument::editTypes()
{
  mode()->editTypes();
}

void KigDocument::setUnmodified()
{
  setModified( false );
}

KCommandHistory* KigDocument::history()
{
  return mhistory;
}

void KigDocument::delObjects( const std::vector<ObjectHolder*>& os )
{
  if ( os.size() < 1 ) return;
  std::set<ObjectHolder*> delobjs;

  std::set<ObjectCalcer*> delcalcers = getAllChildren( getCalcers( os ) );
  std::map<ObjectCalcer*, ObjectHolder*> holdermap;

  for ( std::set<ObjectHolder*>::iterator i = mobjs.begin();
        i != mobjs.end(); ++i )
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

void KigDocument::enableConstructActions( bool enabled )
{
  for_each( aActions.begin(), aActions.end(),
            bind2nd( mem_fun( &KAction::setEnabled ),
                     enabled ) );
}

void KigDocument::unplugActionLists()
{
  unplugActionList( "user_conic_types" );
  unplugActionList( "user_segment_types" );
  unplugActionList( "user_point_types" );
  unplugActionList( "user_circle_types" );
  unplugActionList( "user_line_types" );
  unplugActionList( "user_other_types" );
  unplugActionList( "user_types" );
}

void KigDocument::plugActionLists()
{
  plugActionList( "user_conic_types", aMNewConic );
  plugActionList( "user_segment_types", aMNewSegment );
  plugActionList( "user_point_types", aMNewPoint );
  plugActionList( "user_circle_types", aMNewCircle );
  plugActionList( "user_line_types", aMNewLine );
  plugActionList( "user_other_types", aMNewOther );
  plugActionList( "user_types", aMNewAll );
}

void KigDocument::emitStatusBarText( const QString& text )
{
  emit setStatusBarText( text );
}

void KigDocument::fileSaveAs()
{
  internalSaveAs();
}

void KigDocument::fileSave()
{
  save();
}

bool KigDocument::internalSaveAs()
{
  // this slot is connected to the KStdAction::saveAs action...
  QString formats;
  formats = i18n("*.kig|Kig Documents (*.kig)");

  //  formats += "\n";
  //  formats += KImageIO::pattern( KImageIO::Writing );

  QString file_name = KFileDialog::getSaveFileName(":document", formats );
  if (file_name.isEmpty()) return false;
  else if ( QFileInfo( file_name ).exists() )
  {
    int ret = KMessageBox::warningYesNo( m_widget,
                                         i18n( "The file \"%1\" already exists. Do you wish to overwrite it?" )
                                         .arg( file_name ) );
    if ( ret != KMessageBox::Yes )
    {
      return false;
    }
  }
  saveAs(KURL::fromPathOrURL( file_name ));
  return true;
}

void KigDocument::runMode( KigMode* m )
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

void KigDocument::doneMode( KigMode* d )
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

void KigDocument::setObjects( const std::vector<ObjectHolder*>& os )
{
  assert( objects().empty() );
  mobjs.clear();
  mobjs.insert( os.begin(), os.end() );
  for ( std::vector<ObjectHolder*>::const_iterator i = os.begin();
        i != os.end(); ++i )
    ( *i )->calc( *this );
}

void KigDocument::setCoordinateSystem( CoordinateSystem* cs )
{
  delete mcoordsystem;
  mcoordsystem = cs;
}

void KigDocument::actionRemoved( GUIAction* a, GUIUpdateToken& t )
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

void KigDocument::actionAdded( GUIAction* a, GUIUpdateToken& )
{
  KigGUIAction* ret = new KigGUIAction( a, *this, actionCollection() );
  aActions.push_back( ret );
  ret->plug( this );
}

void KigDocument::endGUIActionUpdate( GUIUpdateToken& t )
{
  unplugActionLists();
  plugActionLists();
  delete_all( t.begin(), t.end() );
  t.clear();
}

KigDocument::GUIUpdateToken KigDocument::startGUIActionUpdate()
{
  return GUIUpdateToken();
}

void KigDocument::setupMacroTypes()
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

void KigDocument::setupBuiltinMacros()
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

void KigDocument::addWidget( KigWidget* v )
{
  mwidgets.push_back( v );
}

void KigDocument::delWidget( KigWidget* v )
{
  std::remove( mwidgets.begin(), mwidgets.end(), v );
}

const std::vector<KigWidget*>& KigDocument::widgets()
{
  return mwidgets;
}

CoordinateSystem* KigDocument::switchCoordinateSystem( CoordinateSystem* s )
{
  CoordinateSystem* ret = mcoordsystem;
  mcoordsystem = s;
  return ret;
}

void KigDocument::filePrintPreview()
{
  KPrinter printer;
  printer.setPreviewOnly( true );
  doPrint( printer );
}

void KigDocument::filePrint()
{
  KPrinter printer;
  if ( printer.setup( m_widget, i18n("Print Geometry") ) )
  {
    doPrint( printer );
  };
}

void KigDocument::doPrint( KPrinter& printer )
{
  QPaintDeviceMetrics metrics( &printer );
  Rect rect = suggestedRect();
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
  KigPainter painter( si, &printer, *this );
  painter.setWholeWinOverlay();
  painter.drawGrid( coordinateSystem() );
  painter.drawObjects( objects(), false );
}

const std::vector<ObjectHolder*> KigDocument::objects() const
{
  std::vector<ObjectHolder*> ret( mobjs.begin(), mobjs.end() );
  return ret;
}

void KigDocument::slotSelectAll()
{
  mMode->selectAll();
}

void KigDocument::slotDeselectAll()
{
  mMode->deselectAll();
}

void KigDocument::slotInvertSelection()
{
  mMode->invertSelection();
}

void KigDocument::hideObjects( const std::vector<ObjectHolder*>& inos )
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
  else kc = new KigCommand( *this, i18n( "Hide %n object", "Hide %n objects", os.size() ) );
  for ( std::vector<ObjectHolder*>::iterator i = os.begin();
        i != os.end(); ++i )
    kc->addTask( new ChangeObjectDrawerTask( *i, ( *i )->drawer()->getCopyShown( false ) ) );
  mhistory->addCommand( kc );
}

void KigDocument::showObjects( const std::vector<ObjectHolder*>& inos )
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
  else kc = new KigCommand( *this, i18n( "Show %n object", "Show %n objects", os.size() ) );
  for ( std::vector<ObjectHolder*>::iterator i = os.begin();
        i != os.end(); ++i )
    kc->addTask( new ChangeObjectDrawerTask( *i, ( *i )->drawer()->getCopyShown( true ) ) );
  mhistory->addCommand( kc );
}

const std::set<ObjectHolder*> KigDocument::objectsSet() const
{
  return mobjs;
}

void KigDocument::redrawScreen( KigWidget* w )
{
  mode()->redrawScreen( w );
}

void KigDocument::redrawScreen()
{
  for ( std::vector<KigWidget*>::iterator i = mwidgets.begin();
        i != mwidgets.end(); ++i )
  {
    mode()->redrawScreen( *i );
  }
}
