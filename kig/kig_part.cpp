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
#include "kig_view.h"
#include "kig_commands.h"
#include "kig_actions.h"

#include "../modes/normal.h"

#include "../objects/object.h"
#include "../objects/point_imp.h"
#include "../objects/line_type.h"

#include "../misc/guiaction.h"
#include "../misc/object_constructor.h"
#include "../misc/coordinate_system.h"
#include "../misc/calcpaths.h"
#include "../misc/objects.h"

#include "../filters/filter.h"

#include <kparts/genericfactory.h>
#include <kinstance.h>
#include <kfiledialog.h>
#include <kaction.h>
#include <kapplication.h>
#include <ktoolbar.h>
#include <kmainwindow.h>
#include <kstdaction.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kmimetype.h>

#include <qfile.h>
#include <qtimer.h>
#if QT_VERSION >= 0x030100
#include <qeventloop.h>
#endif

#include <algorithm>

// export this library...
typedef KParts::GenericFactory<KigDocument> KigDocumentFactory;
K_EXPORT_COMPONENT_FACTORY ( libkigpart, KigDocumentFactory );

KAboutData* KigDocument::createAboutData()
{
  return kigAboutData( "kigpart", I18N_NOOP( "KigPart" ) );
}

KigDocument::KigDocument( QWidget *parentWidget, const char *,
			  QObject *parent, const char *name,
			  const QStringList& )
  : KParts::ReadWritePart( parent, name ),
    mMode( 0 ),
    numViews(0),
    s( new EuclideanCoords )
{
  documents().push_back( this );

  // we need an instance
  setInstance( KigDocumentFactory::instance() );

  // we need a widget, to actually show the document
  m_widget = new KigView(this, parentWidget, "kig_view");
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

  mMode = new NormalMode( *this );
}

void KigDocument::setupActions()
{
  // save actions..
  KStdAction::saveAs(this, SLOT(fileSaveAs()), actionCollection());
  KStdAction::save(this, SLOT(fileSave()), actionCollection());

  // we need icons...
  KIconLoader* l = KGlobal::iconLoader();
  QPixmap tmp;

  tmp = l->loadIcon( "delete", KIcon::User);
  aDeleteObjects = new KAction(
      i18n("Delete Objects"), "editdelete", Key_Delete, this,
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
    i18n("Unhide All"), 0, this, SLOT( showHidden() ),
    actionCollection(), "edit_unhide_all");
  aShowHidden->setToolTip(i18n("Show all hidden objects"));
  aShowHidden->setEnabled( true );

  tmp = l->loadIcon("gear", KIcon::Toolbar);
  aNewMacro = new KAction(
    i18n("New Macro..."), tmp, 0, this, SLOT(newMacro()),
    actionCollection(), "macro_action");
  aNewMacro->setToolTip(i18n("Define a new macro"));

  aConfigureTypes = new KAction(
    i18n("Types Manager"), 0, this, SLOT(editTypes()),
    actionCollection(), "types_edit");
  aConfigureTypes->setToolTip(i18n("Manage macro types."));

//   tmp = l->loadIcon( "window_fullscreen", KIcon::User );
//   aFullScreen = new KAction(
//     i18n( "Full Screen" ), tmp, 0, this, SLOT( startKiosk() ),
//     actionCollection(), "view_fullscreen" );
//   aFullScreen->setToolTip( i18n( "View this document full-screen." ) );

  tmp = l->loadIcon( "pointxy", KIcon::User );
  aFixedPoint = new AddFixedPointAction( this, tmp, actionCollection() );

  tmp = l->loadIcon( "new", KIcon::User );
  (void) new TestAction( this, tmp, actionCollection() );
};

void KigDocument::setupTypes()
{
//   if ( Object::types().empty() )
//   {
//     Object::addBuiltinType( new TStdType<Segment> );
//     Object::addBuiltinType( new TStdType<LineTTP> );
//     Object::addBuiltinType( new TStdType<LinePerpend> );
//     Object::addBuiltinType( new TStdType<LineParallel> );
//     Object::addBuiltinType( new TStdType<CircleBCP> );
//     Object::addBuiltinType( new TStdType<CircleBTP> );
//     Object::addBuiltinType( new TMultiType<LineConicAsymptotes> );
//     Object::addBuiltinType( new TStdType<ConicBAAP> );
//     Object::addBuiltinType( new TStdType<ConicBDFP> );
//     Object::addBuiltinType( new TStdType<RotatedPoint> );
//     Object::addBuiltinType( new TStdType<LineRadical> );
//     Object::addBuiltinType( new TMultiType<LineConicRadical> );
//     Object::addBuiltinType( new TStdType<EllipseBFFP> );
//     Object::addBuiltinType( new TStdType<HyperbolaBFFP> );
//     Object::addBuiltinType( new TStdType<ConicB5P> );
//     Object::addBuiltinType( new TStdType<CubicB9P> );
//     Object::addBuiltinType( new TStdType<ParabolaBTP> );
//     Object::addBuiltinType( new TStdType<EquilateralHyperbolaB4P> );
//     Object::addBuiltinType( new TStdType<MidPoint> );
//     Object::addBuiltinType( new TStdType<IntersectionPoint> );
//     Object::addBuiltinType( new TMultiType<CircleLineIntersectionPoint> );
//     Object::addBuiltinType( new TMultiType<ConicLineIntersectionPoint> );
//     Object::addBuiltinType( new TMultiType<CubicLineIntersectionPoint> );
//     Object::addBuiltinType( new TStdType<TranslatedPoint> );
//     Object::addBuiltinType( new TStdType<MirrorPoint> );
//     Object::addBuiltinType( new TStdType<Locus> );
//     Object::addBuiltinType( new TStdType<Vector> );
//     Object::addBuiltinType( new TStdType<Ray> );
//     Object::addBuiltinType( new TStdType<Angle> );
//     Object::addBuiltinType( new TStdType<LineDirectrix> );
//     Object::addBuiltinType( new TStdType<LinePolar> );
//     Object::addBuiltinType( new TStdType<PointPolar> );
//     Object::addBuiltinType( new TType<TextLabel> );
//     Object::addBuiltinType( new TType<NormalPoint> );
//     Object::addBuiltinType( new TUnconstructibleType<CoordinatePropertyPoint> );

//     // our saved macro types:
//     QStringList relFiles;
//     QStringList dataFiles =
//       KGlobal::dirs()->findAllResources("appdata", "kig-types/*.kigt",
//                                         true, false, relFiles);
//     for ( QStringList::iterator file = dataFiles.begin();
//           file != dataFiles.end();
//           ++file )
//     {
//       kdDebug() << k_funcinfo << " loading types from: " << *file << endl;
//       Types t ( *file);
//       Object::addUserTypes( t, false );
//     }
//   };
//   typedef myvector<Type*> vect;
//   const vect& v = Object::builtinTypes();
//   for ( vect::const_iterator i = v.begin(); i != v.end(); ++i )
//     addType( *i, false );
//   const vect& w = Object::userTypes();
//   for ( vect::const_iterator i = w.begin(); i != w.end(); ++i )
//     addType( *i, true );

//   // hack: we need to plug the action lists _after_ the gui is
//   // built.. i can't find a better solution than this...
//   QTimer::singleShot( 0, this, SLOT( plugActionLists() ) );
};

KigDocument::~KigDocument()
{
  documents().remove( this );

  // remove old types:
  QStringList relFiles;
  QStringList dataFiles =
    KGlobal::dirs()->findAllResources("appdata", "kig-types/*.kigt",
                                        true, false, relFiles);
  for ( QStringList::iterator file = dataFiles.begin();
        file != dataFiles.end();
        ++file )
  {
    QFile f( *file );
    kdDebug() << "removing: " << *file << endl;
    f.remove();
  };

  // save our types...
  QString typesDir = KGlobal::dirs()->saveLocation("appdata", "kig-types");
  if (typesDir[typesDir.length() - 1] != '/') typesDir += '/';
  kdDebug() << k_funcinfo << " : saving types to: " << typesDir << endl;
//   Object::types().saveToDir(typesDir);
  delete_all( mObjs.begin(), mObjs.end() );
  mObjs.clear();

  // cleanup
  delete s;
  delete mMode;
}

bool KigDocument::openFile()
{
  kdDebug() << k_funcinfo << m_file << endl;

  // m_file is always local, so we can use findByPath instead of
  // findByURL...
  KMimeType::Ptr mimeType = KMimeType::findByPath ( m_file );
  QFile file;
  kdDebug() << k_funcinfo << "mimetype: " << mimeType->name() << endl;
  KigFilter* filter = KigFilters::instance()->find( mimeType->name() );
  if ( !filter )
  {
    // we don't support this mime type...
    KMessageBox::sorry
      (
        widget(),
        i18n( "You tried to open a document of type \"%1\".  Unfortunately, "
              "Kig doesn't support this format.  If you think the format in "
              "question would be worth implementing support for, you can "
              "always ask me nicely on mailto:devriese@kde.org "
              "or do the work yourself and send me a patch."
          ).arg(mimeType->name()),
        i18n( "Format not Supported" )
        );
    return false;
  };
  Objects os;
  if ( filter->load (m_file, os) != KigFilter::OK )
  {
    delete_all( os.begin(), os.end() );
    os.clear();
    KMessageBox::sorry( widget(), i18n(
        "The file you tried to open contains some elements that Kig currently "
        "doesn't understand.  It is possible that the file somehow got "
        "corrupted and is no longer usable.  If you know that the file is "
        "valid, and you think Kig should be able to open it, you can try to "
        "send me a copy of the file and ask me nicely to check it out.  If "
        "you want more certain results, you can always do the work yourself "
        "( since Kig is Free Software ), and send me a patch."
                            ) );
    return false;
  };

  delete_all( mObjs.begin(), mObjs.end() );
  mObjs.clear();
  mObjs = os;
  setModified(false);
  mhistory->clear();

  Objects tmp = calcPath( os );
  tmp.calc( *m_widget->realWidget() );
  emit recenterScreen();
  // we do it again to avoid problems with points on locuses and such,
  // for which the size of the current screen matters..
  tmp.calc( *m_widget->realWidget() );
  emit recenterScreen();
  tmp.calc( *m_widget->realWidget() );

  return true;
}

bool KigDocument::saveFile()
{
  if ( m_file.isEmpty() ) return internalSaveAs();
  // mimetype:
  KMimeType::Ptr mimeType = KMimeType::findByPath ( m_file );
  kdDebug() << k_funcinfo << "mimetype: " << mimeType->name() << endl;
  // filter...
  KigFilter* filter = KigFilters::instance()->find( mimeType->name() );
  if ( !filter )
  {
    // we don't support this mime type...
    KMessageBox::sorry
      (
        widget(),
        i18n( "You tried to save to a file of type \"%1\".  Unfortunately, "
              "Kig doesn't support this format.  If you think the format in "
              "question would be worth implementing support for, you can "
              "always ask me nicely on mailto:devriese@kde.org "
              "or do the work yourself and send me a patch."
          ).arg(mimeType->name()),
        i18n( "Format not Supported" )
        );
    return false;
  };

  KigFilter::Result result = filter->save( mObjs, m_file );
  if ( result == KigFilter::OK )
  {
    setModified ( false );
    mhistory->documentSaved();
    return true;
  }
  else if ( result == KigFilter::FileNotFound )
  {
    // i know i need to change the enum value name, but this means
    // that the file could not be opened...
    KMessageBox::sorry( m_widget,
                        i18n( "The file \"%1\" could not be opened.  Please check if the file permissions are set correctly." )
                        .arg( m_file ) );
    return false;

  }
  else // if ( result == KigFilter::NotSupported )
  {
    // we don't support this mime type...
    KMessageBox::sorry
      (
        widget(),
        i18n( "You tried to save to a file of MIME type \"%1\".  Unfortunately, "
              "Kig doesn't support this format.  If you think the format in "
              "question would be worth implementing support for, you can "
              "always ask me nicely on mailto:devriese@kde.org "
              "or do the work yourself and send me a patch."
          ).arg(mimeType->name()),
        i18n( "Format not Supported" )
        );
  };
  return false;
};

void KigDocument::addObject(Object* o)
{
  mhistory->addCommand( new AddObjectsCommand(this, o) );
};

void KigDocument::addObjects( const Objects& os )
{
  mhistory->addCommand( new AddObjectsCommand(this, os) );
}

void KigDocument::_addObject( Object* o )
{
  Q_ASSERT (o != 0);
  mObjs.push_back( o );
  setModified(true);
};

void KigDocument::delObject(Object* o)
{
  // we delete all children and their children etc. too...
  Objects all = o->getAllChildren();
  all.upush(o);
  mhistory->addCommand( new RemoveObjectsCommand(this, all) );
};

void KigDocument::_delObject(Object* o)
{
  mObjs.remove( o );
  o->setSelected(false);
  setModified(true);
};

Objects KigDocument::whatAmIOn(const Coordinate& p, const ScreenInfo& si ) const
{
  Objects tmp;
  Objects nonpoints;
  for ( Objects::const_iterator i = mObjs.begin(); i != mObjs.end(); ++i )
  {
    if(!(*i)->contains(p, si) || !(*i)->shown() || !(*i)->valid()) continue;
    if ( (*i)->has( ObjectImp::ID_PointImp ) ) tmp.push_back(*i);
    else nonpoints.push_back( *i );
  };
  std::copy( nonpoints.begin(), nonpoints.end(), std::back_inserter( tmp ) );
  return tmp;
}

Objects KigDocument::whatIsInHere( const Rect& p )
{
  Objects tmp;
  Objects nonpoints;
  for ( Objects::iterator i = mObjs.begin(); i != mObjs.end(); ++i )
  {
    if(! (*i)->inRect( p ) || !(*i)->shown() || ! (*i)->valid() ) continue;
    if ((*i)->has( ObjectImp::ID_PointImp ) ) tmp.push_back(*i);
    else nonpoints.push_back(*i);
  };
  std::copy( nonpoints.begin(), nonpoints.end(), std::back_inserter( tmp ) );
  return tmp;
};

Rect KigDocument::suggestedRect()
{
  if( mObjs.empty() ) return Rect( -7, -7, 7, 7 );
  bool rectInited = false;
  Rect r(0,0,0,0);
  for (Objects::const_iterator i = mObjs.begin(); i != mObjs.end(); ++i )
  {
    if (( (*i)->has( ObjectImp::ID_PointImp ) ) && (*i)->shown() && (*i)->valid())
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
  r.setContains( Coordinate( 0, 0 ) );
  Coordinate centre = r.center();
  r.setWidth(r.width()*2);
  if (r.width() == 0) r.setWidth( 1 );
  r.setHeight(r.height()*2);
  if (r.height() == 0) r.setHeight( 1 );
  r.setCenter(centre);
  return r;
}

const CoordinateSystem& KigDocument::coordinateSystem() const
{
  assert( s );
  return *s;
}

void KigDocument::setMode( KigMode* m )
{
  mMode = m;
  m->enableActions();
}
void KigDocument::_addObjects( Objects& o)
{
  for( Objects::iterator i = o.begin(); i != o.end(); ++i )
    _addObject( *i );
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

void KigDocument::startKiosk()
{
  mode()->startKiosk();
}

void KigDocument::setUnmodified()
{
  setModified( false );
}

KCommandHistory* KigDocument::history()
{
  return mhistory;
}

void KigDocument::delObjects( const Objects& os )
{
  Objects dos;
  dos = os;
  for ( Objects::const_iterator i = os.begin(); i != os.end(); ++i )
    dos.upush( (*i)->getAllChildren() );
  if ( dos.empty() ) return;
  mhistory->addCommand( new RemoveObjectsCommand( this, dos ) );
}

// void KigDocument::addType( Type* t, bool user )
// {
//   KAction* a = t->constructAction( this );
//   if ( ! a ) return;
//   if ( user )
//   {
//     aMNewAll.append( a );
//     if (t->baseTypeName() == Point::sBaseTypeName())
//       aMNewPoint.append( a );
//     else if (t->baseTypeName() == Line::sBaseTypeName())
//       aMNewLine.append( a );
//     else if (t->baseTypeName() == Circle::sBaseTypeName())
//       aMNewCircle.append( a );
//     else if (t->baseTypeName() == Conic::sBaseTypeName())
//       aMNewConic.append( a );
//     else if (t->baseTypeName() == Segment::sBaseTypeName())
//       aMNewSegment.append( a );
//     else
//       aMNewOther.append( a );
//   };

//   aActions.push_back( a );
// }

// grr.. stupid QPtrList.. yay for the STL..
void setEnabled( QPtrList<KAction>& l, bool e )
{
  for ( KAction* a = l.first(); a; a = l.next() )
  {
    a->setEnabled( e );
  };
};

void KigDocument::enableConstructActions( bool enabled )
{
  std::for_each( aActions.begin(), aActions.end(),
                 std::bind2nd( std::mem_fun( &KAction::setEnabled ),
                               enabled ) );
  aFixedPoint->setEnabled( enabled );

  setEnabled( aMNewConic, enabled );
  setEnabled( aMNewSegment, enabled );
  setEnabled( aMNewPoint, enabled );
  setEnabled( aMNewCircle, enabled );
  setEnabled( aMNewLine, enabled );
  setEnabled( aMNewOther, enabled );
  setEnabled( aMNewAll, enabled );
}

myvector<KigDocument*>& KigDocument::documents()
{
  static myvector<KigDocument*> vect;
  return vect;
}

// void KigDocument::removeAction( KAction* a )
// {
//   aMNewSegment.remove( a );
//   aMNewConic.remove( a );
//   aMNewPoint.remove( a );
//   aMNewCircle.remove( a );
//   aMNewLine.remove( a );
//   aMNewOther.remove( a );
//   aMNewAll.remove( a );
//   aActions.remove( a );
// }

void KigDocument::unplugActionLists()
{
  unplugActionList( "user_conic_types" );
  unplugActionList( "user_segment_types" );
  unplugActionList( "user_point_types" );
  unplugActionList( "user_circle_types" );
  unplugActionList( "user_line_types" );
  unplugActionList( "user_other_types" );
  unplugActionList( "user_types" );
};

void KigDocument::plugActionLists()
{
  plugActionList( "user_conic_types", aMNewConic );
  plugActionList( "user_segment_types", aMNewSegment );
  plugActionList( "user_point_types", aMNewPoint );
  plugActionList( "user_circle_types", aMNewCircle );
  plugActionList( "user_line_types", aMNewLine );
  plugActionList( "user_other_types", aMNewOther );
  plugActionList( "user_types", aMNewAll );
};

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
  formats = QString::fromUtf8("*.kig|Kig Documents (*.kig)");

  //  formats += "\n";
  //  formats += KImageIO::pattern( KImageIO::Writing );

  QString file_name = KFileDialog::getSaveFileName(":document", formats );
  if (file_name.isEmpty()) return false;
  else if ( QFileInfo( file_name ).exists() )
  {
    int ret = KMessageBox::warningYesNo( m_widget,
                                         i18n( "The file \"%1\" already exists.  Do you wish to overwrite it?" )
                                         .arg( file_name ) );
    if ( ret != KMessageBox::Yes )
    {
      return false;
    }
  }
  saveAs(file_name);
  return true;
}

KigView* KigDocument::mainWidget()
{
  return m_widget;
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
}

void KigDocument::doneMode( KigMode* d )
{
  assert( d == mMode );
#if QT_VERSION >= 0x030100
  kapp->eventLoop()->exitLoop();
#else
  kapp->exit_loop();
#endif
}
