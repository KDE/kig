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


#include "type_edit_impl.h"
#include "type_edit_impl.moc"

#include "kig_part.h"

#include "../misc/type.h"

#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include <qtextstream.h>
#include <qstringlist.h>

/*
 * constructor
 */
KigTypeEditImpl::KigTypeEditImpl( QWidget* parent )
  : TypeEditDialogUI( parent, "Type Edit Dialog", true )
{
  Types& types = Object::types();
  for ( Types::const_iterator i = types.begin(); i != types.end(); ++i )
  {
    if (dynamic_cast<MType*>(i->second)) TypeList->insertItem( new TypeListElement(i->second));
  };
}

KigTypeEditImpl::~KigTypeEditImpl()
{
}

void KigTypeEditImpl::helpSlot()
{
  qWarning( "KigTypeEditImpl::helpSlot() not yet implemented!" );
}

void KigTypeEditImpl::okSlot()
{
  accept();
}

void KigTypeEditImpl::deleteType()
{
  std::list<QListBoxItem*> items;
  std::list<Type*> selectedTypes;
  for (QListBoxItem* i = TypeList->firstItem(); i; i = i->next())
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
  for ( std::list<Type*>::const_iterator i = selectedTypes.begin(); i != selectedTypes.end(); ++i)
    {
      Object::types().removeType(*i);
    };
  for (list<QListBoxItem*>::iterator i = items.begin(); i != items.end(); ++i)
    {
      int appel = TypeList->index(*i);
      assert (appel != -1);
      TypeList->removeItem(appel);
    };
};

/*
 * protected slot
 */
void KigTypeEditImpl::exportType()
{
  Types types;
  for (QListBoxItem* i = TypeList->firstItem(); i; i = i->next())
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
void KigTypeEditImpl::importTypes()
{
  QStringList file_names = KFileDialog::getOpenFileNames(":importTypes", "*.kigt", this, "Import Types");
  for ( QStringList::Iterator i = file_names.begin(); i != file_names.end(); ++i)
    {
      Types t( *i );
      Object::types().addTypes(t);
      for (Types::iterator i = t.begin(); i != t.end(); ++i) {
	TypeList->insertItem(new TypeListElement(i->second));
      };
    };
}

TypeListElement::TypeListElement( Type* inType )
  : QListBoxText( inType->descriptiveName() ),
    type( inType )
{
};
