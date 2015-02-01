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

#include "kig.h"


#include <qevent.h>
#include <qtimer.h>

#include <QAction>
#include <QStandardPaths>
#include <QFileDialog>
#include <QUrl>
#include <QMimeData>

#include <KConfigGroup>
#include <QDebug>
#include <kedittoolbar.h>
#include <kshortcutsdialog.h>
#include <klibloader.h>
#include <klocale.h>
#include <kactioncollection.h>
#include <kmessagebox.h>
#include <krecentfilesaction.h>
#include <kstandardaction.h>
#include <ktip.h>
#include <kxmlguifactory.h>
#include <kapplication.h>
#include <assert.h>

#include <KService>
#include <QFileDialog>

Kig::Kig()
  : KParts::MainWindow(), m_part( 0 )
{
  KService::Ptr kigpartService = KService::serviceByDesktopName("kig_part");

  setObjectName( QLatin1String( "Kig" ) );
  // setting the configation file
  config = new KConfig( "kigrc" );
  // set the shell's ui resource file
  setXMLFile("kigui.rc");
  // then, setup our actions
  setupActions();

  if ( kigpartService )
  {
    m_part = kigpartService->createInstance< KParts::ReadWritePart >( this );
    m_mimeTypes = kigpartService->mimeTypes();
    if (m_part)
    {
      // tell the KParts::MainWindow that this is indeed the main widget
      setCentralWidget(m_part->widget());

      // and integrate the part's GUI with the shell's
      createGUI(m_part);

      // finally show tip-of-day ( if the user wants it :) )
      QTimer::singleShot( 0, this, SLOT( startupTipOfDay() ) );
    }
  }
  else
  {
    // if we couldn't find our Part, we exit since the Shell by
    // itself can't do anything useful
    KMessageBox::error(this, i18n( "Could not find the necessary Kig library, check your installation." ) );
    QApplication::exit();
    return;
  }

  // we have drag'n'drop
  setAcceptDrops(true);

  // save the window settings on exit.
  setAutoSaveSettings();
}

Kig::~Kig()
{
  m_recentFilesAction->saveEntries(config->group( QString() ));
  delete config;
}

void Kig::setupActions()
{
  KStandardAction::openNew(this, SLOT(fileNew()), actionCollection());
  KStandardAction::open(this, SLOT(fileOpen()), actionCollection());
  KStandardAction::quit(this, SLOT(close()), actionCollection());

  createStandardStatusBarAction();
  setStandardToolBarMenuEnabled(true);

  // FIXME: this (recent files) should be app-wide, not specific to each window...
  m_recentFilesAction = KStandardAction::openRecent( this, SLOT( openUrl( const QUrl& ) ), actionCollection() );
  m_recentFilesAction->loadEntries(config->group( QString() ) );

  KStandardAction::keyBindings( guiFactory(), SLOT( configureShortcuts() ), actionCollection() );
  KStandardAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());

  KStandardAction::tipOfDay( this, SLOT( tipOfDay() ), actionCollection() );
}

void Kig::saveProperties(KConfigGroup &config)
{
  // the 'config' object points to the session managed
  // config file.  anything you write here will be available
  // later when this app is restored
  config.writePathEntry( "fileName", m_part->url().toLocalFile() );
}

void Kig::readProperties(const KConfigGroup &config)
{
  // the 'config' object points to the session managed
  // config file.  this function is automatically called whenever
  // the app is being restored.  read in here whatever you wrote
  // in 'saveProperties'
  load( QUrl( config.readPathEntry( "fileName", QString() ) ) );
}

void Kig::load( const QUrl &url )
{
  // we check for m_part not being 0, because in the case of us not
  // finding our library, we would otherwise get a crash...
  if ( m_part && m_part->openUrl( url ) ) m_recentFilesAction->addUrl( url );
}

void Kig::fileNew()
{
  // this slot is called whenever the File->New menu is selected,
  // the New shortcut is pressed (usually CTRL+N) or the New toolbar
  // button is clicked

  // create a new window if we aren't in the "initial state" ( see
  // the KDE style guide on the file menu stuff...)
  if ( ! m_part->url().isEmpty() || m_part->isModified() )
    (new Kig)->show();
}

void Kig::openUrl( const QUrl &url )
{
  // Called for opening a file by either the KRecentFilesAction or our
  // own fileOpen() method.
  // if we are in the "initial state", we open the url in this window:
  if ( m_part->url().isEmpty() && ! m_part->isModified() )
  {
    load( url );
  }
  else
  {
    // otherwise we open it in a new window...
    Kig* widget = new Kig;
    widget->load(url);
    widget->show();
  };
}

void Kig::optionsConfigureToolbars()
{
  KConfigGroup configGroup = KGlobal::config()->group( "MainWindow");
  saveMainWindowSettings( configGroup );

  // use the standard toolbar editor
  KEditToolBar dlg(factory());
  connect(&dlg, SIGNAL(newToolBarConfig()),
	  this, SLOT(applyNewToolbarConfig()));
  dlg.exec();
}

void Kig::applyNewToolbarConfig()
{
  applyMainWindowSettings( KGlobal::config()->group( "MainWindow") );
}

bool Kig::queryClose()
{
  return m_part->queryClose();
}

void Kig::dragEnterEvent(QDragEnterEvent* e)
{
  e->setAccepted( e->mimeData()->hasUrls() );
}

void Kig::dropEvent(QDropEvent* e)
{
  const QList<QUrl> urls = e->mimeData()->urls();
  for ( QList<QUrl>::const_iterator u = urls.cbegin(); u != urls.cend(); ++u )
  {
    Kig* k = new Kig();
    k->show();
    k->load( *u );
  }
}

void Kig::fileOpen()
{
  // this slot is connected to the KStandardAction::open action...
  QString currentDir = m_part->url().isLocalFile() ? m_part->url().toLocalFile() : QString();

  if ( currentDir.isNull() )
  {
    currentDir = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
  }

  const QString file_name = QFileDialog::getOpenFileName(0, QString(), currentDir,  m_mimeTypes.join( " " ) );

  if (!file_name.isEmpty()) openUrl( QUrl::fromLocalFile( file_name ) );
}

void Kig::tipOfDay() {
  KTipDialog::showTip(this, "kig/tips", true);
}

void Kig::startupTipOfDay() {
  KTipDialog::showTip(this, "kig/tips");
}
