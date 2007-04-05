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
#include "../kig/kig_part.h"
#include "../misc/guiaction.h"
#include "../misc/object_constructor.h"

#include <kapplication.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kstdguiitem.h>

#include <qfile.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qtextstream.h>

#include <algorithm>
#include <vector>

class MacroListElement
  : public QListViewItem
{
  Macro* macro;
public:
  MacroListElement( KListView* lv, Macro* m );
  Macro* getMacro() const { return macro; }
};

MacroListElement::MacroListElement( KListView* lv, Macro* m )
  : QListViewItem( lv, QString::null, m->action->descriptiveName(), m->action->description() ),
    macro( m )
{
}

TypesDialog::TypesDialog( QWidget* parent, KigPart& part )
  : TypesDialogBase( parent, "types_dialog", true ), mpart( part )
{
  // improving GUI look'n'feel...
  buttonHelp->setGuiItem( KStdGuiItem::help() );
  buttonOk->setGuiItem( KStdGuiItem::ok() );
  buttonCancel->setGuiItem( KStdGuiItem::cancel() );
  il = part.instance()->iconLoader();
  buttonEdit->setIconSet( QIconSet( il->loadIcon( "edit", KIcon::Small ) ) );
  buttonRemove->setIconSet( QIconSet( il->loadIcon( "editdelete", KIcon::Small ) ) );
  buttonExport->setIconSet( QIconSet( il->loadIcon( "fileexport", KIcon::Small ) ) );
  buttonImport->setIconSet( QIconSet( il->loadIcon( "fileimport", KIcon::Small ) ) );

  typeList->setColumnWidth( 0, 22 );
  typeList->setColumnWidth( 1, 140 );
  typeList->setColumnWidth( 2, 240 );

  // loading macros...
  loadAllMacros();

  popup = new QPopupMenu( this );
  popup->insertItem( SmallIcon( "edit" ), i18n( "&Edit..." ), this, SLOT( editType() ) );
  popup->insertItem( SmallIcon( "editdelete" ), i18n( "&Delete" ), this, SLOT( deleteType() ) );
  popup->insertSeparator();
  popup->insertItem( SmallIcon( "fileexport" ), i18n( "E&xport..." ), this, SLOT( exportType() ) );

  // saving types
  part.saveTypes();
}

QListViewItem* TypesDialog::newListItem( Macro* m )
{
  MacroListElement* e = new MacroListElement( typeList, m );
  QCString ifn = m->action->iconFileName();
  if ( !ifn.isNull() )
  {
    QPixmap p = il->loadIcon( ifn, KIcon::Small );
    e->setPixmap( 0, p );
  }
  return e;
}

TypesDialog::~TypesDialog()
{
}

void TypesDialog::helpSlot()
{
  kapp->invokeHelp( QString::fromLatin1( "working-with-types" ),
                    QString::fromLatin1( "kig" ) );
}

void TypesDialog::okSlot()
{
  mpart.saveTypes();
  mpart.deleteTypes();
  mpart.loadTypes();
  accept();
}

void TypesDialog::deleteType()
{
  std::vector<QListViewItem*> items;
  std::vector<Macro*> selectedTypes;
  QListViewItemIterator it( typeList );
  while ( it.current() ) {
    if ( ( it.current() )->isSelected() )
    {
      items.push_back( it.current() );
      selectedTypes.push_back( static_cast<MacroListElement*>( it.current() )->getMacro() );
    }
    ++it;
  }
  if (selectedTypes.empty()) return;
  QStringList types;
  for ( std::vector<Macro*>::iterator j = selectedTypes.begin(); 
        j != selectedTypes.end(); ++j )
    types << ( *j )->action->descriptiveName();
  if ( KMessageBox::warningContinueCancelList( this,
        i18n( "Are you sure you want to delete this type?",
              "Are you sure you want to delete these %n types?", selectedTypes.size() ),
        types, i18n("Are You Sure?"), KStdGuiItem::cont(),
        "deleteTypeWarning") == KMessageBox::Cancel )
     return;
  for ( std::vector<QListViewItem*>::iterator i = items.begin(); i != items.end(); ++i)
  {
    int appel = typeList->itemIndex(*i);
    assert (appel != -1);
    delete *i;
  };
  for ( std::vector<Macro*>::iterator j = selectedTypes.begin();
        j != selectedTypes.end(); ++j)
    MacroList::instance()->remove( *j );
}

void TypesDialog::exportType()
{
  std::vector<Macro*> types;
  QListViewItemIterator it( typeList );
  while ( it.current() ) {
    if ( ( it.current() )->isSelected() )
    {
      types.push_back( static_cast<MacroListElement*>( it.current() )->getMacro() );
    }
    ++it;
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
    typeList->insertItem( newListItem( macros[i] ) );
}

QString TypesDialog::fetchIconFromListItem( QListViewItem* i )
{
  QListViewItemIterator it( typeList );
  Macro* ai = static_cast<MacroListElement*>( i )->getMacro();
  while ( it.current() ) {
    if ( ( it.current() )->isSelected() )
    {
      Macro* ait = static_cast<MacroListElement*>( it.current() )->getMacro();
      if ( ai == ait )
      {
        return ai->ctor->iconFileName( true );
      }
    }
    ++it;
  }
  return "gear";
}

void TypesDialog::editType()
{
  std::vector<QListViewItem*> items;
  QListViewItemIterator it( typeList );
  while ( it.current() ) {
    if ( ( it.current() )->isSelected() )
      items.push_back( it.current() );
    ++it;
  }
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
  QListViewItem* i = items[0];
  EditType* d = new EditType( this, i->text( 1 ), i->text( 2 ), fetchIconFromListItem( i ) );
  if ( d->exec() )
  {
    QString newname = d->name();
    QString newdesc = d->description();
    QString newicon = d->icon();

    Macro* oldmacro = static_cast<MacroListElement*>( i )->getMacro();
//    mpart.unplugActionLists();
    oldmacro->ctor->setName( newname );
    oldmacro->ctor->setDescription( newdesc );
    QCString ncicon( newicon.utf8() );
    oldmacro->ctor->setIcon( ncicon );
//    mpart.plugActionLists();

    typeList->clear();

    loadAllMacros();
  }
  delete d;
}

void TypesDialog::contextMenuRequested( QListViewItem*, const QPoint& p, int )
{
  popup->exec( p );
}

void TypesDialog::loadAllMacros()
{
  const vec& macros = MacroList::instance()->macros();
  for ( vec::const_reverse_iterator i = macros.rbegin(); i != macros.rend(); ++i )
  {
    typeList->insertItem( newListItem( *i ) );
  }
}

void TypesDialog::cancelSlot()
{
  mpart.deleteTypes();
  mpart.loadTypes();
  reject();
}
