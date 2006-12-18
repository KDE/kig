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

#include "typesdialog.h"
#include "typesdialog.moc"

#include "edittype.h"
#include "ui_typeswidget.h"
#include "../kig/kig_part.h"
#include "../misc/guiaction.h"
#include "../misc/object_constructor.h"

#include <kfiledialog.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <ktoolinvocation.h>

#include <qbytearray.h>
#include <qevent.h>
#include <qfile.h>
#include <qlayout.h>
#include <qlist.h>
#include <qmenu.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include <algorithm>

static QString wrapAt( const QString& str, int col = 50 )
{
  QStringList ret;
  int delta = 0;
  while ( delta + col < str.length() )
  {
    int pos = delta + col;
    while ( !str.at( pos ).isSpace() ) --pos;
    ret << str.mid( delta, pos - delta );
    delta = pos + 1;
  }
  ret << str.mid( delta );
  return ret.join( "<br>" );
}

class BaseListElement
{
protected:
  BaseListElement();

public:
  virtual ~BaseListElement();

  virtual bool isMacro() const { return false; }
  virtual QString name() const = 0;
  virtual QString description() const = 0;
  virtual QString icon( bool canNull = false ) const = 0;
  virtual QString type() const = 0;
};

BaseListElement::BaseListElement()
{
}

BaseListElement::~BaseListElement()
{
}

class MacroListElement
  : public BaseListElement
{
  Macro* mmacro;
public:
  MacroListElement( Macro* m );
  virtual ~MacroListElement();

  Macro* getMacro() const { return mmacro; };
  virtual bool isMacro() const { return true; };
  virtual QString name() const;
  virtual QString description() const;
  virtual QString icon( bool canNull = false ) const;
  virtual QString type() const;
};

MacroListElement::MacroListElement( Macro* m )
  : BaseListElement(), mmacro( m )
{
}

MacroListElement::~MacroListElement()
{
  // do NOT delete the associated macro, but instead safely release the
  // pointer, as it's kept elsewhere
  mmacro = 0;
}

QString MacroListElement::name() const
{
  return mmacro->action->descriptiveName();
}

QString MacroListElement::description() const
{
  return mmacro->action->description();
}

QString MacroListElement::icon( bool canNull ) const
{
  return mmacro->ctor->iconFileName( canNull );
}

QString MacroListElement::type() const
{
  return i18n( "Macro" );
}


TypesModel::TypesModel( QObject* parent )
  : QAbstractTableModel( parent )
{
}

TypesModel::~TypesModel()
{
}

const std::vector<BaseListElement*>& TypesModel::elements() const
{
  return melems;
}

void TypesModel::addElements( const std::vector<BaseListElement*>& elems )
{
  if ( elems.size() < 1 )
    return;

  beginInsertRows( QModelIndex(), melems.size() + 1, melems.size() + elems.size() );

  std::copy( elems.begin(), elems.end(), std::back_inserter( melems ) );

  endInsertRows();
}

void TypesModel::removeElements( const std::vector<BaseListElement*>& elems )
{
  // this way of deleting needs some explanation: the std::vector.erase needs
  // an iterator to the element to remove from the list, while the
  // beginRemoveRows() of Qt needs the index(es). so, for each element to
  // delete, we search it into melems (keeping a count of the id), and when we
  // find it, we free the memory of the BaseListElement and remove the element
  // from the list. in the meanwhile, we notify the model structure of Qt that
  // we're removing a row.
  std::vector<BaseListElement*> newelems = elems;
  for ( std::vector<BaseListElement*>::iterator it = newelems.begin();
        it != newelems.end(); ++it )
  {
    bool found = false;
    int id = 0;
    for ( std::vector<BaseListElement*>::iterator mit = melems.begin();
          mit != melems.end() && !found; )
    {
      if ( *mit == *it )
      {
        found = true;
        beginRemoveRows( QModelIndex(), id, id );

        delete (*mit);
        mit = melems.erase( mit );

        endRemoveRows();
      }
      else
        ++mit;
      ++id;
    }
  }
}

void TypesModel::clear()
{
  for ( std::vector<BaseListElement*>::const_iterator it = melems.begin();
          it != melems.end(); ++it )
    delete *it;
  melems.clear();
}

int TypesModel::columnCount( const QModelIndex& ) const
{
  return 4;
}

QVariant TypesModel::data( const QModelIndex& index, int role ) const
{
  if ( !index.isValid() )
    return QVariant();

  if ( ( index.row() < 0 ) || ( index.row() >= static_cast<int>( melems.size() ) ) )
    return QVariant();

  switch ( role )
  {
    case Qt::DecorationRole:
    {
      if ( index.column() == 0 )
        return KIcon( melems[ index.row() ]->icon() );
      else
        return QVariant();
      break;
    }
    case Qt::DisplayRole:
    {
      switch ( index.column() )
      {
        case 1: return melems[ index.row() ]->type(); break;
        case 2: return melems[ index.row() ]->name(); break;
        case 3: return melems[ index.row() ]->description(); break;
        default:
          return QVariant();
      }
      break;
    }
    case Qt::ToolTipRole:
    {
      static QString macro_with_image(
          "<qt><table cellspacing=\"5\"><tr><td><b>%1</b> (%4)</td>"
          "<td rowspan=\"2\" align=\"right\"><img src=\"%3\"></td></tr>"
          "<tr><td>%2</td></tr></table></qt>" );
      static QString macro_no_image(
          "<qt><b>%1</b> (%3)<br>%2</qt>" );

      if ( melems[ index.row() ]->icon( true ).isEmpty() )
        return macro_no_image
                 .arg( melems[ index.row() ]->name() )
                 .arg( wrapAt( melems[ index.row() ]->description() ) )
                 .arg( melems[ index.row() ]->type() );
      else
        return macro_with_image
                 .arg( melems[ index.row() ]->name() )
                 .arg( wrapAt( melems[ index.row() ]->description() ) )
                 .arg( KGlobal::iconLoader()->iconPath( melems[ index.row() ]->icon(), - K3Icon::SizeMedium ) )
                 .arg( melems[ index.row() ]->type() );
    }
    default:
      return QVariant();
  }
}

QVariant TypesModel::headerData( int section, Qt::Orientation orientation, int role ) const
{
  if ( orientation != Qt::Horizontal )
    return QVariant();

  if ( role == Qt::TextAlignmentRole )
    return QVariant( Qt::AlignLeft );

  if ( role != Qt::DisplayRole )
    return QVariant();

  switch ( section )
  {
    case 0: return i18n( "Icon" ); break;
    case 1: return i18n( "Type" ); break;
    case 2: return i18n( "Name" ); break;
    case 3: return i18n( "Description" ); break;
    default:
      return QVariant();
  }
}

int TypesModel::rowCount( const QModelIndex& ) const
{
  return melems.size();
}


TypesDialog::TypesDialog( QWidget* parent, KigPart& part )
  : KDialog( parent ),
    mpart( part )
{
  setCaption( i18n( "Manage Types" ) );
  setButtons( Help | Ok | Cancel );

  QWidget* base = new QWidget( this );
  setMainWidget( base );
  mtypeswidget = new Ui_TypesWidget();
  mtypeswidget->setupUi( base );
  base->layout()->setMargin( 0 );

  // model creation and usage
  mmodel = new TypesModel();
  mtypeswidget->typeList->setModel( mmodel );

  mtypeswidget->typeList->installEventFilter( this );

  // improving GUI look'n'feel...
  KIconLoader* il = part.instance()->iconLoader();
  mtypeswidget->buttonEdit->setIcon( KIcon("edit", il) );
  mtypeswidget->buttonRemove->setIcon( KIcon("editdelete", il) );
  mtypeswidget->buttonExport->setIcon( KIcon("fileexport", il) );
  mtypeswidget->buttonImport->setIcon( KIcon("fileimport", il) );

  std::vector<BaseListElement*> el;
  // loading macros...
  loadAllMacros( el );
  // .. and filling the model
  mmodel->addElements( el );

//  mtypeswidget->typeList->sortItems( 2, Qt::AscendingOrder );

  mtypeswidget->typeList->resizeColumnToContents( 0 );
  mtypeswidget->typeList->resizeColumnToContents( 1 );

  popup = new QMenu( this );
  popup->addAction( KIcon( "edit" ), i18n( "&Edit..." ), this, SLOT( editType() ) );
  popup->addAction( KIcon( "editdelete" ), i18n( "&Delete" ), this, SLOT( deleteType() ) );
  popup->addSeparator();
  popup->addAction( KIcon( "fileexport" ), i18n( "E&xport..." ), this, SLOT( exportType() ) );

  // saving types
  mpart.saveTypes();

  connect( mtypeswidget->buttonExport, SIGNAL( clicked() ), this, SLOT( exportType() ) );
  connect( mtypeswidget->buttonImport, SIGNAL( clicked() ), this, SLOT( importTypes() ) );
  connect( mtypeswidget->buttonRemove, SIGNAL( clicked() ), this, SLOT( deleteType() ) );
  connect( mtypeswidget->buttonEdit, SIGNAL( clicked() ), this, SLOT( editType() ) );
  connect( this, SIGNAL( helpClicked() ), this, SLOT( slotHelp() ) );
  connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );
  connect( this, SIGNAL( cancelClicked() ), this, SLOT( slotCancel() ) );

  resize( 460, 270 );
}

TypesDialog::~TypesDialog()
{
}

void TypesDialog::slotHelp()
{
  KToolInvocation::invokeHelp( "working-with-types", "kig" );
}

void TypesDialog::slotOk()
{
  mpart.saveTypes();
  mpart.deleteTypes();
  mpart.loadTypes();
  accept();
}

void TypesDialog::deleteType()
{
  std::vector<Macro*> selectedTypes;
  std::set<int> rows = selectedRows();
  const std::vector<BaseListElement*>& el = mmodel->elements();
  for ( std::set<int>::const_iterator it = rows.begin(); it != rows.end(); ++it )
  {
    BaseListElement* e = el[ *it ];
    if ( e->isMacro() )
      selectedTypes.push_back( static_cast<MacroListElement*>( e )->getMacro() );
  }

  if (selectedTypes.empty()) return;
  QStringList types;
  for ( std::vector<Macro*>::iterator j = selectedTypes.begin();
        j != selectedTypes.end(); ++j )
    types << ( *j )->action->descriptiveName();
  types.sort();
  if ( KMessageBox::warningContinueCancelList( this,
        i18np( "Are you sure you want to delete this type?",
              "Are you sure you want to delete these %n types?", selectedTypes.size() ),
        types, i18n("Are You Sure?"), KStdGuiItem::cont(),
        "deleteTypeWarning") == KMessageBox::Cancel )
     return;
  std::vector<BaseListElement*> todelete;
  for ( std::set<int>::const_iterator it = rows.begin(); it != rows.end(); ++it )
  {
    BaseListElement* e = el[ *it ];
    if ( e->isMacro() )
    {
      todelete.push_back( e );
    }
  }
  bool updates = mtypeswidget->typeList->updatesEnabled();
  mtypeswidget->typeList->setUpdatesEnabled( false );
  mmodel->removeElements( todelete );
  mtypeswidget->typeList->setUpdatesEnabled( updates );
  for ( std::vector<Macro*>::iterator j = selectedTypes.begin();
        j != selectedTypes.end(); ++j)
    MacroList::instance()->remove( *j );
}

void TypesDialog::exportType()
{
  std::vector<Macro*> types;
  std::set<int> rows = selectedRows();
  const std::vector<BaseListElement*>& el = mmodel->elements();
  for ( std::set<int>::const_iterator it = rows.begin(); it != rows.end(); ++it )
  {
    BaseListElement* e = el[ *it ];
    if ( e->isMacro() )
      types.push_back( static_cast<MacroListElement*>( e )->getMacro() );
  }
  if (types.empty()) return;
  QString file_name = KFileDialog::getSaveFileName( KUrl( "kfiledialog:///macro" ), i18n("*.kigt|Kig Types Files\n*|All Files"), this, i18n( "Export Types" ) );
  if ( file_name.isNull() )
    return;
  QFile fi( file_name );
  if ( fi.exists() )
    if ( KMessageBox::warningContinueCancel( this, i18n( "The file \"%1\" already exists. "
                                       "Do you wish to overwrite it?", fi.fileName() ),
                                       i18n( "Overwrite File?" ), KStdGuiItem::overwrite() ) == KMessageBox::Cancel )
       return;
  MacroList::instance()->save( types, file_name );
}

void TypesDialog::importTypes()
{
  QStringList file_names =
    KFileDialog::getOpenFileNames( KUrl( "kfiledialog:///importTypes" ), i18n("*.kigt|Kig Types Files\n*|All Files"), this, i18n( "Import Types" ));

  std::vector<Macro*> macros;

  for ( QStringList::Iterator i = file_names.begin();
        i != file_names.end(); ++i)
  {
    std::vector<Macro*> nmacros;
    bool ok = MacroList::instance()->load( *i, nmacros, mpart );
    if ( ! ok )
      continue;
    std::copy( nmacros.begin(), nmacros.end(), std::back_inserter( macros ) );
  };
  MacroList::instance()->add( macros );

  std::vector<BaseListElement*> el;
  for ( uint i = 0; i < macros.size(); ++i )
    el.push_back( new MacroListElement( macros[i] ) );
  mmodel->addElements( el );
}

void TypesDialog::editType()
{
  std::set<int> rows = selectedRows();
  if ( rows.empty() )
    return;
  if ( rows.size() > 1 )
  {
    KMessageBox::sorry( this,
                        i18n( "There is more than one type selected. You can "
                              "only edit one type at a time. Please select "
                              "only the type you want to edit and try again." ),
                        i18n( "More Than One Type Selected" ) );
    return;
  }
  bool refresh = false;
  const std::vector<BaseListElement*>& el = mmodel->elements();
  // we get the iterator pointing at the beginning and use it to point to the
  // only object inside. this is done as we know that there's only one object
  // inside.
  std::set<int>::const_iterator first = rows.begin();
  BaseListElement* e = el[ *first ];
  if ( e->isMacro() )
  {
    EditType* d = new EditType( this, e->name(), e->description(), e->icon() );
    if ( d->exec() )
    {
      QString newname = d->name();
      QString newdesc = d->description();
      QString newicon = d->icon();
      delete d;

      Macro* oldmacro = static_cast<MacroListElement*>( e )->getMacro();
//      mpart.unplugActionLists();
      oldmacro->ctor->setName( newname );
      oldmacro->ctor->setDescription( newdesc );
      QByteArray ncicon( newicon.toUtf8() );
      oldmacro->ctor->setIcon( ncicon );
//      mpart.plugActionLists();
      refresh = true;
    }
  }
  if ( refresh )
  {
    bool updates = mtypeswidget->typeList->updatesEnabled();
    mtypeswidget->typeList->setUpdatesEnabled( false );
    mmodel->clear();

    std::vector<BaseListElement*> newelements;
    loadAllMacros( newelements );
    mmodel->addElements( newelements );
    mtypeswidget->typeList->setUpdatesEnabled( updates );
  }
}

void TypesDialog::loadAllMacros( std::vector<BaseListElement*>& el )
{
  const vec& macros = MacroList::instance()->macros();
  for ( vec::const_iterator i = macros.begin(); i != macros.end(); ++i )
  {
    el.push_back( new MacroListElement( *i ) );
  }
}

void TypesDialog::slotCancel()
{
  mpart.deleteTypes();
  mpart.loadTypes();
  reject();
}

std::set<int> TypesDialog::selectedRows() const
{
  QModelIndexList indexes = mtypeswidget->typeList->selectionModel()->selectedIndexes();
  std::set<int> rows;
  for ( int i = 0; i < indexes.count(); ++i )
    rows.insert( indexes.at(i).row() );
  return rows;
}

bool TypesDialog::eventFilter( QObject* obj, QEvent* event )
{
  (void)obj;
  if ( event->type() == QEvent::ContextMenu )
  {
    if ( !selectedRows().empty() )
    {
      QContextMenuEvent* e = (QContextMenuEvent*)event;
      popup->exec( e->globalPos() );
      e->accept();
      return true;
    }
    else
      return false;
  }
  return false;
}
