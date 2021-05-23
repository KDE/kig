/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2004 Dominique Devriese <devriese@kde.org>
    SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "edittype.h"

#include "ui_edittypewidget.h"

#include <KIconDialog>
#include <KLineEdit>
#include <KMessageBox>
#include <KHelpClient>
#include <QDialogButtonBox>
#include <KConfigGroup>
#include <QPushButton>
#include <QVBoxLayout>

EditType::EditType( QWidget* parent, const QString& name, const QString& desc,
                    const QString& icon )
  : QDialog( parent ),
    mname( name ), mdesc( desc ), micon( icon )
{
  setWindowTitle( i18nc("@title:window", "Edit Type") );
  QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok|QDialogButtonBox::Cancel|QDialogButtonBox::Help);
  QWidget *mainWidget = new QWidget(this);
  QVBoxLayout *mainLayout = new QVBoxLayout;
  setLayout(mainLayout);
  mainLayout->addWidget(mainWidget);
  QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
  okButton->setDefault(true);
  okButton->setShortcut(Qt::CTRL | Qt::Key_Return);
  connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  mainLayout->addWidget(buttonBox);

  medittypewidget = new Ui_EditTypeWidget();
  medittypewidget->setupUi( mainWidget );
  mainWidget->layout()->setContentsMargins( 0, 0, 0, 0 );

  medittypewidget->editName->setText( mname );
  medittypewidget->editName->setWhatsThis(
        i18n( "Here you can edit the name of the current macro type." ) );
  medittypewidget->editDescription->setText( mdesc );
  medittypewidget->editDescription->setWhatsThis(
        i18n( "Here you can edit the description of the current macro type. "
              "This field is optional, so you can also leave this empty: if "
              "you do so, then your macro type will have no description." ) );
  medittypewidget->typeIcon->setIcon( !micon.isEmpty() ? micon : QStringLiteral("system-run") );
  medittypewidget->typeIcon->setWhatsThis(
        i18n( "Use this button to change the icon of the current macro type." ) );

  connect( this, SIGNAL(helpClicked()), this, SLOT(slotHelp()) );
  connect(okButton, &QAbstractButton::clicked, this, &EditType::slotOk );
  connect(buttonBox->button(QDialogButtonBox::Cancel), &QAbstractButton::clicked, this, &EditType::slotCancel );
}

EditType::~EditType()
{
  delete medittypewidget;
}

void EditType::slotHelp()
{
  KHelpClient::invokeHelp( QStringLiteral("working-with-types"), QStringLiteral("kig") );
}

void EditType::slotOk()
{
  QString tmp = medittypewidget->editName->text();
  if ( tmp.isEmpty() )
  {
    KMessageBox::information( this, i18n( "The name of the macro can not be empty." ) );
    return;
  }

  bool namechanged = false;
  bool descchanged = false;
  bool iconchanged = false;
  if ( tmp != mname )
  {
    mname = tmp;
    namechanged = true;
  }
  tmp = medittypewidget->editDescription->text();
  if ( tmp != mdesc )
  {
    mdesc = tmp;
    descchanged = true;
  }
  tmp = medittypewidget->typeIcon->icon();
  if ( tmp != micon )
  {
    micon = tmp;
    iconchanged = true;
  }
  done( namechanged || descchanged || iconchanged );
}

void EditType::slotCancel()
{
  done( 0 );
}

QString EditType::name() const
{
  return mname;
}

QString EditType::description() const
{
  return mdesc;
}

QString EditType::icon() const
{
  return micon;
}
