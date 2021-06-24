/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "kig.h"

#include <QAction>
#include <QApplication>
#include <QDropEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QProcess>
#include <QStandardPaths>
#include <QTimer>
#include <QUrl>

#include <kxmlgui_version.h>
#include <KActionCollection>
#include <KConfigGroup>
#include <KEditToolBar>
#include <KLocalizedString>
#include <KMessageBox>
#include <KRecentFilesAction>
#include <KService>
#include <KSharedConfig>
#include <KStandardAction>
#include <KTipDialog>
#include <KXMLGUIFactory>

Kig::Kig()
  : KParts::MainWindow(), m_part( 0 )
{
  KService::Ptr kigpartService = KService::serviceByDesktopName(QStringLiteral("kig_part"));

  setObjectName( QStringLiteral( "Kig" ) );
  // setting the configuration file
  config = new KConfig( QStringLiteral("kigrc") );
  // set the shell's ui resource file
  setXMLFile(QStringLiteral("kigui.rc"));
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
      QTimer::singleShot( 0, this, &Kig::startupTipOfDay );
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
  m_recentFilesAction = KStandardAction::openRecent( this, SLOT(openUrl(QUrl)), actionCollection() );
  m_recentFilesAction->loadEntries(config->group( QString() ) );

#if KXMLGUI_VERSION >= QT_VERSION_CHECK(5, 84, 0)
  KStandardAction::keyBindings(guiFactory(), &KXMLGUIFactory::showConfigureShortcutsDialog, actionCollection());
#else
  KStandardAction::keyBindings( guiFactory(), SLOT(configureShortcuts()), actionCollection() );
#endif

  KStandardAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());

  KStandardAction::tipOfDay( this, SLOT(tipOfDay()), actionCollection() );
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
  {
    QString kigBinary = QStandardPaths::findExecutable( QStringLiteral("kig") );
    QProcess::startDetached( kigBinary, QStringList() );
  }
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
  KConfigGroup configGroup = KSharedConfig::openConfig()->group( "MainWindow" );
  saveMainWindowSettings( configGroup );

  // use the standard toolbar editor
  KEditToolBar dlg(factory());
  connect(&dlg, &KEditToolBar::newToolBarConfig,
	  this, &Kig::applyNewToolbarConfig);
  dlg.exec();
}

void Kig::applyNewToolbarConfig()
{
  applyMainWindowSettings( KSharedConfig::openConfig()->group( "MainWindow") );
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
  QString currentDir = m_part->url().isLocalFile() ? QFileInfo( m_part->url().toLocalFile() ).absolutePath() : QString();

  if ( currentDir.isNull() )
  {
    currentDir = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
  }

  QPointer<QFileDialog> fileOpenDialog = new QFileDialog(this);
  fileOpenDialog->setWindowTitle(i18nc("@title:window", "Open File"));
  fileOpenDialog->setDirectory(currentDir);
  fileOpenDialog->setMimeTypeFilters(m_mimeTypes);
  fileOpenDialog->setAcceptMode(QFileDialog::AcceptOpen);
  fileOpenDialog->setFileMode(QFileDialog::ExistingFile);
  if (fileOpenDialog->exec() == QDialog::Accepted) {
      QUrl file_url = fileOpenDialog->selectedUrls().first();
      if (!file_url.isEmpty()) {
          openUrl( file_url );
      }
  }
  delete fileOpenDialog;
}

void Kig::tipOfDay() {
  KTipDialog::showTip(this, QStringLiteral("kig/tips"), true);
}

void Kig::startupTipOfDay() {
  KTipDialog::showTip(this, QStringLiteral("kig/tips"));
}
