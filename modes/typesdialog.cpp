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
#include <algorithm>
using namespace std;

TypesDialog::TypesDialog( QWidget* parent, const KigDocument& doc )
  : TypesDialogBase( parent, "types_dialog", true ), mdoc( doc )
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
};

void TypesDialog::exportType()
{
  myvector<Macro*> types;
  for (QListBoxItem* i = typeList->firstItem(); i; i = i->next())
  {
    if (i->isSelected())
      types.push_back(static_cast<MacroListElement*>(i)->getMacro());
  };
  if (types.empty()) return;
  QString file_name = KFileDialog::getSaveFileName(":macro", i18n("*.kigt|Kig Types files\n*"));
  if ( !file_name.isNull() )
    MacroList::instance()->save( types, file_name );
};

void TypesDialog::importTypes()
{
  QStringList file_names =
    KFileDialog::getOpenFileNames(":importTypes", i18n("*.kigt|Kig Types files\n*"), this, "Import Types");

  myvector<Macro*> macros;

  for ( QStringList::Iterator i = file_names.begin();
        i != file_names.end(); ++i)
  {
    myvector<Macro*> nmacros;
    bool ok = MacroList::instance()->load( *i, nmacros, mdoc );
    if ( ! ok )
    {
      KMessageBox::sorry( this, i18n( "Could not open macro file '%1'" ).arg( *i ) );
      continue;
    };
    copy( nmacros.begin(), nmacros.end(), back_inserter( macros ) );
  };
  MacroList::instance()->add( macros );

  for ( uint i = 0; i < macros.size(); ++i )
    typeList->insertItem( new MacroListElement( macros[i] ) );
}

MacroListElement::MacroListElement( Macro* m )
  : QListBoxText( m->action->descriptiveName() ),
    macro( m )
{
};
