/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese

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

#include "../modes/normal.h"
#include "../objects/circle.h"
#include "../objects/segment.h"
#include "../objects/normalpoint.h"
#include "../objects/midpoint.h"
#include "../objects/line.h"
#include "../objects/macro.h"
#include "../objects/intersection.h"
#include "../objects/locus.h"
#include "../misc/type.h"
#include "../misc/coordinate_system.h"
#include "../filters/filter.h"

#include <kparts/genericfactory.h>
#include <kinstance.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kstandarddirs.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kmimetype.h>

#include <qfile.h>

#include <algorithm>

// export this library...
typedef KParts::GenericFactory<KigDocument> KigDocumentFactory;
K_EXPORT_COMPONENT_FACTORY ( libkigpart, KigDocumentFactory );

Types KigDocument::types;

KAboutData* KigDocument::createAboutData()
{
  return kigAboutData( "kigpart", I18N_NOOP( "KigPart" ) );
}

KigDocument::KigDocument( QWidget *parentWidget, const char *widgetName,
			  QObject *parent, const char *name,
			  const QStringList& )
  : KParts::ReadWritePart(parent, name),
    mMode( 0 ),
    numViews(0),
    s( new EuclideanCoords )
{
  // we need an instance
  setInstance( KigDocumentFactory::instance() );

  // we need a widget, to actually show the document
  m_widget = new KigView(this, parentWidget, widgetName);
  // notify the part that this is our internal widget
  setWidget( m_widget );
  insertChildClient( m_widget );

  // create our actions...
  setupActions();

  // our types...
  setupTypes();

  // construct our command history
  mhistory = new KCommandHistory(actionCollection());
  mhistory->documentSaved();
  connect( mhistory, SIGNAL( documentRestored() ), this, SLOT( setUnmodified() ) );

  // set our XML-UI resource file
  setXMLFile("kigpartui.rc");

  // we are read-write by default
  setReadWrite(true);

  setModified (false);

  mMode = new NormalMode( this );
}

void KigDocument::setupActions()
{
  // we need icons...
  KIconLoader* l = KGlobal::iconLoader();
  QPixmap tmp;

  tmp = l->loadIcon( "delete", KIcon::User);
  aDeleteObjects = new KAction(
      i18n("Delete objects"), "editdelete", Key_Delete, this,
      SLOT(deleteObjects()), actionCollection(), "delete_objects");
  aDeleteObjects->setToolTip(i18n("Delete the selected objects"));

  tmp = l->loadIcon( "cancel", KIcon::User);
  aCancelConstruction = new KAction(
      i18n("Cancel construction"), tmp, Key_Escape, this,
      SLOT(cancelConstruction()), actionCollection(), "cancel_construction");
  aCancelConstruction->setToolTip(
      i18n("Cancel the construction of the object being constructed"));
  aCancelConstruction->setEnabled(false);

  aShowHidden = new KAction(
    i18n("Unhide all"), 0, this, SLOT( showHidden() ),
    actionCollection(), "edit_unhide_all");
  aShowHidden->setToolTip(i18n("Show all hidden objects"));
  aShowHidden->setEnabled( true );

  tmp = l->loadIcon("macro", KIcon::User);
  aNewMacro = new KAction(
    i18n("New macro"), tmp, 0, this, SLOT(newMacro()),
    actionCollection(), "macro_action");
  aNewMacro->setToolTip(i18n("Define a new macro"));

  aConfigureTypes = new KAction(
    i18n("Edit Types"), 0, this, SLOT(editTypes()),
    actionCollection(), "types_edit");
  aConfigureTypes->setToolTip(i18n("Edit your defined macro types"));

  tmp = l->loadIcon( "window_fullscreen", KIcon::User );
  aFullScreen = new KAction(
    i18n( "Full screen" ), tmp, 0, this, SLOT( startKiosk() ),
    actionCollection(), "view_fullscreen" );
  aFullScreen->setToolTip( i18n( "View this document full-screen." ) );

  tmp = l->loadIcon( "line", KIcon::User);
  aMNewLine = new KActionMenu (i18n("New Line"), tmp, actionCollection(),
                               "new_line");
  aMNewLine->setToolTip(i18n("Construct a new line"));

  tmp = l->loadIcon( "circle", KIcon::User );
  aMNewCircle = new KActionMenu(i18n("New Circle"), tmp,
                                actionCollection(), "new_circle");
  aMNewCircle->setToolTip(i18n("Construct a new circle"));

  tmp = l->loadIcon( "point4", KIcon::User );
  aMNewPoint = new KActionMenu(i18n("New Point"), tmp, actionCollection(),
                               "new_point");
  aMNewPoint->setToolTip(i18n("Construct a new point"));

  tmp = l->loadIcon( "segment", KIcon::User );
  aMNewSegment = new KActionMenu(i18n("New Segment"), tmp,
                                 actionCollection(), "new_segment");
  aMNewSegment->setToolTip(i18n("Construct a new segment"));

  aMNewOther = new KActionMenu(i18n("New Other"), 0, actionCollection(), "new_other");
  aMNewOther->setToolTip(i18n("Construct a new object of a special type"));
};

void KigDocument::setupTypes()
{
  Types& types = Object::types();
  if ( types.empty() )
  {
    types.addType( new TType<Segment> );
    types.addType( new TType<LineTTP> );
    types.addType( new TType<LinePerpend> );
    types.addType( new TType<LineParallel> );
    types.addType( new TType<LineRadical> );
    types.addType( new TType<CircleBCP> );
    types.addType( new TType<CircleBTP> );
    types.addType( new TType<NormalPoint> );
    types.addType( new TType<MidPoint> );
    types.addType( new TType<IntersectionPoint> );
    types.addType( new TType<Locus> );

    // our saved macro types:
    QStringList relFiles;
    QStringList dataFiles =
      KGlobal::dirs()->findAllResources("appdata", "kig-types/*.kigt",
                                        true, false, relFiles);
    for ( QStringList::iterator file = dataFiles.begin();
          file != dataFiles.end();
          ++file )
    {
      kdDebug() << k_funcinfo << " loading types from: " << *file << endl;
      Types t ( *file);
      types.addTypes( t );
    };
  };
  for ( Types::iterator i = types.begin(); i != types.end(); ++i )
  {
    Type* t = i->second;
    // FIXME: memory leak: ConstructAction's aren't delete'd...
    KAction* a = t->constructAction( this );
    if ( ! a ) continue;
    if (t->baseTypeName()==Point::sBaseTypeName())
      aMNewPoint->insert( a );
    else if (t->baseTypeName() == Line::sBaseTypeName())
      aMNewLine->insert( a );
    else if (t->baseTypeName() == Circle::sBaseTypeName())
      aMNewCircle->insert( a );
    else if (t->baseTypeName() == Segment::sBaseTypeName())
      aMNewSegment->insert( a );
    else
      aMNewOther->insert( a );
  };
};

KigDocument::~KigDocument()
{
  // save our types...
  QString typesDir = KGlobal::dirs()->saveLocation("appdata", "kig-types");
  if (typesDir[typesDir.length() - 1] != '/') typesDir += '/';
  kdDebug() << k_funcinfo << " : saving types to: " << typesDir << endl;
  Object::types().saveToDir(typesDir);
  delete_all( mObjs.begin(), mObjs.end() );
  mObjs.clear();
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
              "always ask me nicely on mailto:fritmebufstek@pandora.be "
              "or do the work yourself and send me a patch."
          ).arg(mimeType->name()),
        i18n( "Format not supported" )
        );
    return false;
  };
  Objects os;
  if ( filter->load (m_file, os) != KigFilter::OK )
  {
    delete_all( os.begin(), os.end() );
    os.clear();
    return false;
  };

  delete_all( mObjs.begin(), mObjs.end() );
  mObjs.clear();
  mObjs = os;
  setModified(false);
  mhistory->clear();
  emit recenterScreen();

  return true;
}

bool KigDocument::saveFile()
{
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
              "always ask me nicely on mailto:fritmebufstek@pandora.be "
              "or do the work yourself and send me a patch."
          ).arg(mimeType->name()),
        i18n( "Format not supported" )
        );
    return false;
  };

  if ( filter->save( mObjs, m_file ) == KigFilter::OK )
  {
    setModified ( false );
    mhistory->documentSaved();
    return true;
  }
  else
  {
    return false;
  };
};

void KigDocument::addObject(Object* o)
{
  mhistory->addCommand( new AddObjectsCommand(this, o) );
};

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

Objects KigDocument::whatAmIOn(const Coordinate& p, const double miss ) const
{
  Objects tmp;
  Objects nonpoints;
  for ( Objects::const_iterator i = mObjs.begin(); i != mObjs.end(); ++i )
  {
    if(!(*i)->contains(p, miss)) continue;
    if ( (*i)->toPoint()) tmp.push_back(*i);
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
    if(! (*i)->inRect( p )) continue;
    if ((*i)->toPoint()) tmp.push_back(*i);
    else nonpoints.push_back(*i);
  };
  std::copy( nonpoints.begin(), nonpoints.end(), std::back_inserter( tmp ) );
  return tmp;
};

Rect KigDocument::suggestedRect()
{
  if( mObjs.empty() ) return Rect( -2, -2, 2, 2 );
  bool rectInited = false;
  Rect r(0,0,0,0);
  Point* p;
  for (Objects::const_iterator i = mObjs.begin(); i != mObjs.end(); ++i )
  {
    if ((p = (*i)->toPoint()))
    {
      if( !rectInited )
      {
        r.setCenter( p->getCoord() );
        rectInited = true;
      }
      else
        r.setContains(p->getCoord());
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

const CoordinateSystem* KigDocument::coordinateSystem()
{
  return s;
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
