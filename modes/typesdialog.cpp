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


#include "typesdialog.h"
#include "typesdialog.moc"

#include "../kig/kig_part.h"
#include "../misc/lists.h"
#include "../misc/guiaction.h"

#include <kfiledialog.h>
#include <kicondialog.h>
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kdebug.h>
#include <klocale.h>
#include <kapplication.h>

#include <qfile.h>
#include <qtextstream.h>
#include <qstringlist.h>

#include <vector>
#include <algorithm>

TypesDialog::TypesDialog( QWidget* parent, const KigDocument& doc )
  : TypesDialogBase( parent, "types_dialog", true ), mdoc( doc )
{
  KIconLoader* l = KGlobal::iconLoader();
  buttonHelp->setIconSet( QIconSet( l->loadIcon( "help", KIcon::Small ) ) );
  buttonOk->setIconSet( QIconSet( l->loadIcon( "button_ok", KIcon::Small ) ) );
  buttonRemove->setIconSet( QIconSet( l->loadIcon( "editdelete", KIcon::Small ) ) );

  // loading macros...
  typedef MacroList::vectype vec;
  const vec& macros = MacroList::instance()->macros();
  for ( vec::const_iterator i = macros.begin(); i != macros.end(); ++i )
  {
    typeList->insertItem( new MacroListElement( *i ) );
  };
  typeIcon->setIcon( "gear" );
  typeIcon->setEnabled( false );
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
  accept();
}

void TypesDialog::deleteType()
{
  std::vector<QListBoxItem*> items;
  std::vector<Macro*> selectedTypes;
  for( QListBoxItem* i = typeList->firstItem(); i; i = i->next() )
  {
    if (i->isSelected())
    {
      selectedTypes.push_back(static_cast<MacroListElement*>(i)->getMacro());
      items.push_back(i);
    };
  };
  if (selectedTypes.empty()) return;
  if (selectedTypes.size()==1)
  {
    if (KMessageBox::warningContinueCancel
	(this,
	 i18n("Are you sure you want to delete the type named \"%1\"?").arg(selectedTypes.front()->action->descriptiveName()),
	 i18n("Are you sure?"),
	 i18n("Continue"),
	 "deleteTypeWarning") ==KMessageBox::Cancel ) return;
  }
  else
    if (KMessageBox::warningContinueCancel
	(this,
	 i18n("Are you sure you want to delete these %1 types?").arg(selectedTypes.size()),
	 i18n("Are you sure?"),
	 i18n("Continue"),
	 "deleteTypeWarning") == KMessageBox::Cancel ) return;
  for ( std::vector<QListBoxItem*>::iterator i = items.begin(); i != items.end(); ++i)
  {
    int appel = typeList->index(*i);
    assert (appel != -1);
    typeList->removeItem(appel);
  };
  for ( std::vector<Macro*>::iterator j = selectedTypes.begin();
        j != selectedTypes.end(); ++j)
    MacroList::instance()->remove( *j );
}

void TypesDialog::exportType()
{
  std::vector<Macro*> types;
  for (QListBoxItem* i = typeList->firstItem(); i; i = i->next())
  {
    if (i->isSelected())
      types.push_back(static_cast<MacroListElement*>(i)->getMacro());
  };
  if (types.empty()) return;
  QString file_name = KFileDialog::getSaveFileName(":macro", i18n("*.kigt|Kig Types Files\n*|All Files"));
  if ( file_name.isNull() )
    return;
  QFile fi( file_name );
  if ( fi.exists() )
    if ( KMessageBox::questionYesNo( 0, i18n( "The file \"%1\" already exists. "
                                       "Do you wish to overwrite it?" ).arg( fi.name() ),
                                       i18n( "Overwrite file?" ) ) == KMessageBox::No )
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
    bool ok = MacroList::instance()->load( *i, nmacros, mdoc );
    if ( ! ok )
      continue;
    std::copy( nmacros.begin(), nmacros.end(), std::back_inserter( macros ) );
  };
  MacroList::instance()->add( macros );

  for ( uint i = 0; i < macros.size(); ++i )
    typeList->insertItem( new MacroListElement( macros[i] ) );
}

void TypesDialog::selectionChangedSlot()
{
  int nselected = 0;
  int lastselected = -1;
  int current = -1;
  for( QListBoxItem* i = typeList->firstItem(); i; i = i->next() )
  {
    current++;
    if (i->isSelected())
    {
      nselected++;
      lastselected = current;
    }
  }
  typedef MacroList::vectype vec;
  const vec& macros = MacroList::instance()->macros();
  if ( nselected == 1 )
  {
//    typeIcon->setEnabled( true );
    typeIcon->setIcon( macros[lastselected]->action->iconFileName() );
  }
  else
  {
    typeIcon->setIcon( "gear" );
//    typeIcon->setEnabled( false );
  }
  
}

void TypesDialog::iconChangedSlot()
{
/*
  int nselected = 0;
  int lastselected = -1;
  int current = -1;
  for( QListBoxItem* i = typeList->firstItem(); i; i = i->next() )
  {
    current++;
    if (i->isSelected())
    {
      nselected++;
      lastselected = current;
    }
  }
  typedef MacroList::vectype vec;
  const vec& macros = MacroList::instance()->macros();
  ObjectHierarchy* macros[lastselected]->ctor->hierarchy();
  QCString actionname = macros[lastselected]->action->descriptiveName();
  QCString iconfile = macros[lastselected]->action->iconFileName();
  char* name = macros[lastselected]->action->actionName();
  
  MacroConstructor* ctor =
      new MacroConstructor( *hierarchy, name.latin1(), description.latin1(), iconfile );
  GUIAction* act = new ConstructibleAction( ctor, actionname );
  Macro* macro = new Macro( act, ctor );

  // TODO: implement insert for MacroList and, of course, for GUIActionList...
  MacroList::instance()->insert( macros, lastselected );

  typeList->removeItem( lastselected );
  typeList->insertItem( new MacroListElement( macros[i] ), lastselected );
*/

}

MacroListElement::MacroListElement( Macro* m )
  : QListBoxText( m->action->descriptiveName() ),
    macro( m )
{
}
