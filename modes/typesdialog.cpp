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


#include "typesdialog.h"
#include "typesdialog.moc"

#include "../kig/kig_part.h"

#include "../misc/type.h"

#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <klocale.h>

#include <qtextstream.h>
#include <qstringlist.h>

/*
 * constructor
 */
TypesDialog::TypesDialog( QWidget* parent )
  : TypesDialogBase( parent, "Manage Types Dialog", true )
{
  Types& types = Object::types();
  for ( Types::const_iterator i = types.begin(); i != types.end(); ++i )
  {
    if (dynamic_cast<MType*>(i->second)) typeList->insertItem( new TypeListElement(i->second));
  };

}

TypesDialog::~TypesDialog()
{
}

void TypesDialog::helpSlot()
{
  qWarning( "TypesDialog::helpSlot() not yet implemented!" );
}

void TypesDialog::okSlot()
{
  accept();
}

void TypesDialog::deleteType()
{
  std::list<QListBoxItem*> items;
  std::list<Type*> selectedTypes;
  for (QListBoxItem* i = typeList->firstItem(); i; i = i->next())
  {
    if (i->isSelected())
    {
      selectedTypes.push_front(static_cast<TypeListElement*>(i)->getType());
      items.push_front(i);
    };
  };
  if (selectedTypes.empty()) return;
  if (selectedTypes.size()==1)
  {
    if (KMessageBox::warningContinueCancel
	(this,
	 i18n("Are you sure you want to delete the type named \"%1\"?").arg(selectedTypes.front()->descriptiveName()),
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
  for (list<QListBoxItem*>::iterator i = items.begin(); i != items.end(); ++i)
  {
    int appel = typeList->index(*i);
    assert (appel != -1);
    typeList->removeItem(appel);
  };
  for ( std::list<Type*>::iterator i = selectedTypes.begin();
        i != selectedTypes.end(); ++i)
  {
    Object::types().removeType(*i);
    // we don't delete the type, because there might be objects around
    // depending on the ObjectHierarchy of the type..
    // what we do is to tell it to delete all actions it has defined,
    // so it will seem to the user that the type is gone...
    // TODO: a type MacroObjectHierarchy, which maintains a reference
    // count of objects using it, and a bool telling it whether its
    // type is still visible.  On saving we should also save any
    // ObjectHierarchy's that are used...
    (*i)->deleteActions();
  };
};

/*
 * protected slot
 */
void TypesDialog::exportType()
{
  Types types;
  for (QListBoxItem* i = typeList->firstItem(); i; i = i->next())
  {
    if (i->isSelected()) types.addType(static_cast<TypeListElement*>(i)->getType());
  };
  if (types.empty()) return;
  QString file_name = KFileDialog::getSaveFileName(":macro", i18n("*.kigt|Kig Types files\n*"));
  if (!file_name) return;
  types.saveToFile(file_name);
};

/*
 * protected slot
 */
void TypesDialog::importTypes()
{
  QStringList file_names =
    KFileDialog::getOpenFileNames(":importTypes", "*.kigt", this, "Import Types");
  for ( QStringList::Iterator i = file_names.begin();
        i != file_names.end(); ++i)
  {
    Types t( *i );
    for (Types::iterator i = t.begin(); i != t.end(); ++i)
      typeList->insertItem(new TypeListElement(i->second));
    Object::types().addTypes(t);
  };
}

TypeListElement::TypeListElement( Type* inType )
  : QListBoxText( inType->descriptiveName() ),
    type( inType )
{
};
