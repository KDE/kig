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
#include "../objects/object.h"

#include <kfiledialog.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <klocale.h>

#include <qtextstream.h>
#include <qstringlist.h>

#include <vector>

TypesDialog::TypesDialog( QWidget* parent )
  : TypesDialogBase( parent, "types_dialog", true )
{
  typedef MacroList::vectype vec;
  const vec& macros = MacroList::instance()->macros();
  for ( vec::const_iterator i = macros.begin(); i != macros.end(); ++i )
  {
    typeList->insertItem( new MacroListElement( *i ) );
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
  std::vector<QListBoxItem*> items;
  std::vector<Macro*> selectedTypes;
  for( QListBoxItem* i = typeList->firstItem(); i; i = i->next() )
  {
    if (i->isSelected())
    {
      selectedTypes.push_back(static_cast<MacroListElement*>(i)->getType());
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
};

void TypesDialog::exportType()
{
//   Types types;
//   for (QListBoxItem* i = typeList->firstItem(); i; i = i->next())
//   {
//     if (i->isSelected())
//       types.addType(static_cast<TypeListElement*>(i)->getType());
//   };
//   if (types.empty()) return;
//   QString file_name = KFileDialog::getSaveFileName(":macro", i18n("*.kigt|Kig Types files\n*"));
//   if ( ! file_name.isNull() ) types.saveToFile(file_name);
//   types.clear();
};

void TypesDialog::importTypes()
{
//   QStringList file_names =
//     KFileDialog::getOpenFileNames(":importTypes", "*.kigt", this, "Import Types");
//   for ( QStringList::Iterator i = file_names.begin();
//         i != file_names.end(); ++i)
//   {
//     Types t( *i );
//     for (Types::iterator i = t.begin(); i != t.end(); ++i)
//       typeList->insertItem(new TypeListElement(i->second));
//     Object::addUserTypes(t);
//   };
}

MacroListElement::MacroListElement( Macro* m )
  : QListBoxText( m->action->descriptiveName() ),
    macro( m )
{
};
