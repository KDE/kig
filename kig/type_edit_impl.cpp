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

#include "../misc/type.h"
#include "kig_part.h"

#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdebug.h>

#include <qtextstream.h>
#include <qstringlist.h>

/* 
 * constructor
 */
KigTypeEditImpl::KigTypeEditImpl( KigDocument* inDoc)
  : TypeEditDialogUI( inDoc->widget(), "Type Edit Dialog", true ),
    doc(inDoc),
    types(inDoc->getTypes())
{
  for (Types::const_iterator i = types->begin(); i != types->end(); ++i)
    {
      if ((*i)->toMTypeOne()) TypeList->insertItem( new TypeListElement(*i));
    };
}

/*  
 *  Destroys the object and frees any allocated resources
 */
KigTypeEditImpl::~KigTypeEditImpl()
{
  // no need to delete child widgets, Qt does it all for us
}

/* 
 * public slot
 */
void KigTypeEditImpl::helpSlot()
{
  qWarning( "KigTypeEditImpl::helpSlot() not yet implemented!" ); 
}
/* 
 * public slot
 */
void KigTypeEditImpl::okSlot()
{
  accept();
}

/* 
 * protected slot
 */
void KigTypeEditImpl::deleteType()
{
  list<QListBoxItem*> items;
  list<Type*> types;
  for (QListBoxItem* i = TypeList->firstItem(); i; i = i->next())
    {
      if (i->isSelected()) {
	types.push_front(static_cast<TypeListElement*>(i)->getType());
	items.push_front(i);
      };
    };
  if (types.empty()) return;
  if (types.size()==1) {
    if (KMessageBox::warningContinueCancel
	(this,
	 i18n("Are you sure you want to delete the type named \"%1\"?").arg(types.front()->getActionName()),
	 i18n("Are you sure?"),
	 i18n("Continue"),
	 "deleteTypeWarning") ==KMessageBox::Cancel ) return;
  } else { 
    if (KMessageBox::warningContinueCancel
	(this,
	 i18n("Are you sure you want to delete these %1 types?").arg(types.size()),
	 i18n("Are you sure?"),
	 i18n("Continue"),
	 "deleteTypeWarning") == KMessageBox::Cancel ) return;
  };
  for ( list<Type*>::iterator i = types.begin(); i != types.end(); ++i)
    {
      assert (doc);
      doc->removeType(*i);
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
      if (i->isSelected()) types.add(static_cast<TypeListElement*>(i)->getType());
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
      Types t(doc,*i);
      doc->addTypes(t);
      for (Types::iterator i = t.begin(); i != t.end(); ++i) {
	TypeList->insertItem(new TypeListElement(*i));
      };
    };
}
