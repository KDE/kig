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
   Foundation, Inc., 51 Franklin Steet, Fifth Floor, Boston, MA  02110-1301
   USA
**/

#include "typesdialog.h"
#include "typesdialog.moc"

#include "edittype.h"
#include "typeswidget.h"
#include "../kig/kig_part.h"
#include "../misc/guiaction.h"
#include "../misc/object_constructor.h"

#include <kdebug.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <ktoolinvocation.h>

#include <qbytearray.h>
#include <qfile.h>
#include <qlayout.h>
#include <qlist.h>
#include <qmenu.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include <algorithm>
#include <vector>

class BaseListElement
  : public QTreeWidgetItem
{
protected:
  BaseListElement( QTreeWidget* lv );

public:
  virtual bool isMacro() const { return false; }
  virtual QString name() const = 0;
  virtual QString description() const = 0;
  virtual QString icon( bool canNull = false ) const = 0;
  virtual QString type() const = 0;
  void setData();
};

BaseListElement::BaseListElement( QTreeWidget* lv )
  : QTreeWidgetItem( lv )
{
}

void BaseListElement::setData()
{
  QString ifn = icon();
  if ( !ifn.isEmpty() )
    setIcon( 0, SmallIcon( ifn ) );
  setText( 1, type() );
  setText( 2, name() );
  setText( 3, description() );
}

class MacroListElement
  : public BaseListElement
{
  Macro* mmacro;
public:
  MacroListElement( QTreeWidget* lv, Macro* m );
  Macro* getMacro() const { return mmacro; };
  virtual bool isMacro() const { return true; };
  virtual QString name() const;
  virtual QString description() const;
  virtual QString icon( bool canNull = false ) const;
  virtual QString type() const;
};

MacroListElement::MacroListElement( QTreeWidget* lv, Macro* m )
  : BaseListElement( lv ), mmacro( m )
{
  setData();
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

TypesDialog::TypesDialog( QWidget* parent, KigPart& part )
  : KDialogBase( parent, "types-dialog", true, i18n( "Manage Types" ),
                 Help|Ok|Cancel, Ok, true ), mpart( part )
{
  QWidget* base = new QWidget( this );
  setMainWidget( base );
  mtypeswidget = new Ui_TypesWidget();
  mtypeswidget->setupUi( base );
  base->layout()->setMargin( 0 );

  // improving GUI look'n'feel...
  KIconLoader* il = part.instance()->iconLoader();
  mtypeswidget->buttonEdit->setIcon( QIcon( il->loadIcon( "edit", KIcon::Small ) ) );
  mtypeswidget->buttonEdit->setWhatsThis(
        i18n( "Edit the selected type." ) );
  mtypeswidget->buttonRemove->setIcon( QIcon( il->loadIcon( "editdelete", KIcon::Small ) ) );
  mtypeswidget->buttonRemove->setWhatsThis(
        i18n( "Delete all the selected types in the list." ) );
  mtypeswidget->buttonExport->setIcon( QIcon( il->loadIcon( "fileexport", KIcon::Small ) ) );
  mtypeswidget->buttonExport->setWhatsThis(
        i18n( "Export all the selected types to a file." ) );
  mtypeswidget->buttonImport->setIcon( QIcon( il->loadIcon( "fileimport", KIcon::Small ) ) );
  mtypeswidget->buttonImport->setWhatsThis(
        i18n( "Import macros that are contained in one or more files." ) );

  mtypeswidget->typeList->setToolTip(
        i18n( "Select types here..." ) );
  mtypeswidget->typeList->setWhatsThis(
        i18n( "This is a list of the current macro types... You can select, "
              "edit, delete, export and import them..." ) );

  QStringList hl;
  hl << i18n( "Icon" )
     << i18n( "Type" )
     << i18n( "Name" )
     << i18n( "Description" );
  mtypeswidget->typeList->setHeaderLabels( hl );

  // loading macros...
  loadAllMacros();

  mtypeswidget->typeList->sortItems( 2, Qt::AscendingOrder );

  mtypeswidget->typeList->resizeColumnToContents( 0 );
  mtypeswidget->typeList->resizeColumnToContents( 1 );

  popup = new QMenu( this );
  popup->addAction( SmallIcon( "edit" ), i18n( "&Edit..." ), this, SLOT( editType() ) );
  popup->addAction( SmallIcon( "editdelete" ), i18n( "&Delete" ), this, SLOT( deleteType() ) );
  popup->addSeparator();
  popup->addAction( SmallIcon( "fileexport" ), i18n( "E&xport..." ), this, SLOT( exportType() ) );

  // saving types
  mpart.saveTypes();

  connect( mtypeswidget->buttonExport, SIGNAL( clicked() ), this, SLOT( exportType() ) );
  connect( mtypeswidget->buttonImport, SIGNAL( clicked() ), this, SLOT( importTypes() ) );
  connect( mtypeswidget->buttonRemove, SIGNAL( clicked() ), this, SLOT( deleteType() ) );
  connect( mtypeswidget->buttonEdit, SIGNAL( clicked() ), this, SLOT( editType() ) );

  resize( 460, 270 );
}

TypesDialog::~TypesDialog()
{
}

void TypesDialog::slotHelp()
{
  KToolInvocation::invokeHelp( QLatin1String( "working-with-types" ),
                               QLatin1String( "kig" ) );
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
  QList<QTreeWidgetItem*> items = mtypeswidget->typeList->selectedItems();
  for ( int i = 0; i < items.count(); i++ )
  {
    BaseListElement* e = static_cast<BaseListElement*>( items.at( i ) );
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
        i18n( "Are you sure you want to delete this type?",
              "Are you sure you want to delete these %n types?", selectedTypes.size() ),
        types, i18n("Are You Sure?"), KStdGuiItem::cont(),
        "deleteTypeWarning") == KMessageBox::Cancel )
     return;
  for ( int i = 0; i < items.count(); i++ )
  {
    BaseListElement* e = static_cast<BaseListElement*>( items.at( i ) );
    if ( e->isMacro() )
      delete items.at( i );
  }
  for ( std::vector<Macro*>::iterator j = selectedTypes.begin();
        j != selectedTypes.end(); ++j)
    MacroList::instance()->remove( *j );
}

void TypesDialog::exportType()
{
  std::vector<Macro*> types;
  QList<QTreeWidgetItem*> items = mtypeswidget->typeList->selectedItems();
  for ( int i = 0; i < items.count(); i++ )
  {
    BaseListElement* e = static_cast<BaseListElement*>( items.at( i ) );
    if ( e->isMacro() )
      types.push_back( static_cast<MacroListElement*>( e )->getMacro() );
  }
  if (types.empty()) return;
  QString file_name = KFileDialog::getSaveFileName(":macro", i18n("*.kigt|Kig Types Files\n*|All Files"), this, i18n( "Export Types" ) );
  if ( file_name.isNull() )
    return;
  QFile fi( file_name );
  if ( fi.exists() )
    if ( KMessageBox::warningContinueCancel( this, i18n( "The file \"%1\" already exists. "
                                       "Do you wish to overwrite it?" ).arg( fi.name() ),
                                       i18n( "Overwrite File?" ), i18n("Overwrite") ) == KMessageBox::Cancel )
       return;
  MacroList::instance()->save( types, file_name );
}

void TypesDialog::importTypes()
{
  QStringList file_names =
    KFileDialog::getOpenFileNames(":importTypes", i18n("*.kigt|Kig Types Files\n*|All Files"), this, i18n( "Import Types" ));

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

  for ( uint i = 0; i < macros.size(); ++i )
    new MacroListElement( mtypeswidget->typeList, macros[i] );
  mtypeswidget->typeList->sortItems( 2, Qt::AscendingOrder );
}

void TypesDialog::editType()
{
  QList<QTreeWidgetItem*> items = mtypeswidget->typeList->selectedItems();
  if ( items.size() == 0 )
    return;
  if ( items.size() > 1 )
  {
    KMessageBox::sorry( this,
                        i18n( "There is more than one type selected. You can "
                              "only edit one type at a time. Please select "
                              "only the type you want to edit and try again." ),
                        i18n( "More Than One Type Selected" ) );
    return;
  }
  bool refresh = false;
  BaseListElement* e = static_cast<BaseListElement*>( items.at( 0 ) );
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
      QByteArray ncicon( newicon.utf8() );
      oldmacro->ctor->setIcon( ncicon );
//      mpart.plugActionLists();
      refresh = true;
    }
  }
  if ( refresh )
  {
    mtypeswidget->typeList->clear();

    loadAllMacros();
    mtypeswidget->typeList->sortItems( 2, Qt::AscendingOrder );
  }
}

/*
void TypesDialog::contextMenuRequested( Q3ListViewItem*, const QPoint& p, int )
{
  popup->exec( p );
}
*/

void TypesDialog::loadAllMacros()
{
  const vec& macros = MacroList::instance()->macros();
  for ( vec::const_reverse_iterator i = macros.rbegin(); i != macros.rend(); ++i )
  {
    new MacroListElement( mtypeswidget->typeList, ( *i ) );
  }
}

void TypesDialog::slotCancel()
{
  mpart.deleteTypes();
  mpart.loadTypes();
  reject();
}
