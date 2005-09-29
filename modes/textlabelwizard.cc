// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "textlabelwizard.h"
#include "textlabelwizard.moc"

#include "label.h"
#include "linkslabel.h"

#include <qtextedit.h>
#include <kapplication.h>

#include <qlayout.h>

TextLabelWizard::TextLabelWizard( QWidget* parent, TextLabelModeBase* mode )
  : TextLabelWizardBase( parent, "TextLabelWizard", false ), mmode( mode )
{
  connect( labelTextInput, SIGNAL( textChanged() ),
           SLOT( textChanged() ) );
  connect( myCustomWidget1, SIGNAL( linkClicked( int ) ),
           SLOT( linkClicked( int ) ) );
  connect( this, SIGNAL( helpClicked() ),
           this, SLOT( slotHelpClicked() ) );
  labelTextInput->setFocus();
}

TextLabelWizard::~TextLabelWizard()
{
}

void TextLabelWizard::back()
{
  if ( currentPage() == select_arguments_page )
  {
    mmode->enterTextPageEntered();
  }
  TextLabelWizardBase::back();
}

void TextLabelWizard::next()
{
  if ( currentPage() == enter_text_page )
  {
    mmode->selectArgumentsPageEntered();
  }
  TextLabelWizardBase::next();
}

void TextLabelWizard::reject()
{
  TextLabelWizardBase::reject();
  mmode->cancelPressed();
}

void TextLabelWizard::accept()
{
  mmode->finishPressed();
}

void TextLabelWizard::textChanged()
{
  mmode->labelTextChanged();
}

void TextLabelWizard::linkClicked( int which )
{
  mmode->linkClicked( which );
}

void TextLabelWizard::relayoutArgsPage()
{
  select_arguments_pageLayout->activate();
  repaint();
}

void TextLabelWizard::slotHelpClicked()
{
  kapp->invokeHelp( QString::fromLatin1( "text-labels" ),
                    QString::fromLatin1( "kig" ) );
}

