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

#include "kig.h"
#include "kig.moc"

#include <kkeydialog.h>
#include <kconfig.h>
#include <kurl.h>
#include <kurldrag.h>
#include <kedittoolbar.h>
#include <kaction.h>
#include <kfiledialog.h>
#include <kstdaction.h>
#include <kstatusbar.h>
#include <klibloader.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kapplication.h>
#include <klocale.h>
#include <assert.h>

Kig::Kig()
  : KParts::MainWindow( 0L, "Kig" ), m_part( 0 )
{
  // set the shell's ui resource file
  setXMLFile("kigui.rc");
  // then, setup our actions
  setupActions();

  // this routine will find and load our Part.  it finds the Part by
  // name which is a bad idea usually.. but it's alright in this
  // case since our Part is made for this Shell

  // we use globalLibrary() because if we use python scripting, then
  // we need the python symbols to be exported, in order for python to
  // be able to load its dll modules..
  KLibrary* library = KLibLoader::self()->globalLibrary("libkigpart");
  KLibFactory* factory = 0;
  if ( library ) factory = library->factory();
  if (factory)
  {
      // now that the Part is loaded, we cast it to a Part to get
      // our hands on it
      m_part = static_cast<KParts::ReadWritePart*>
               (factory->create(this, "kig_part", "KParts::ReadWritePart" ));
      if (m_part)
      {
	  // tell the KParts::MainWindow that this is indeed the main widget
	  setCentralWidget(m_part->widget());

	  // and integrate the part's GUI with the shell's
	  createGUI(m_part);
      }
  }
  else
  {
      // if we couldn't find our Part, we exit since the Shell by
      // itself can't do anything useful
      KMessageBox::error(this, "Could not find the necessary kig library, check your installation.");
      KApplication::exit();
      return;
  }

  // we have drag'n'drop
  setAcceptDrops(true);

  // save the window settings on exit.
  setAutoSaveSettings();
}

Kig::~Kig()
{
  KConfig* config = new KConfig("kigrc");
  m_recentFilesAction->saveEntries(config);
  delete config;
}

void Kig::setupActions()
{
  KStdAction::openNew(this, SLOT(fileNew()), actionCollection());
  KStdAction::open(this, SLOT(fileOpen()), actionCollection());
  KStdAction::quit(this, SLOT(close()), actionCollection());

#ifdef KIG_DONT_USE_NEW_KMAINWINDOW_FEATURES
  m_toolbarAction = KStdAction::showToolbar(this, SLOT(optionsShowToolbar()), actionCollection());
  m_statusbarAction = KStdAction::showStatusbar(this, SLOT(optionsShowStatusbar()), actionCollection());
#else
  createStandardStatusBarAction();
  setStandardToolBarMenuEnabled(true);
#endif

  // FIXME: this (recent files) should be app-wide, not specific to each window...
  m_recentFilesAction = KStdAction::openRecent(this, SLOT(openURL(const KURL&)), actionCollection());
  KConfig* config = new KConfig("kigrc");
  m_recentFilesAction->loadEntries(config);
  delete config;

  KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
  KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
}

void Kig::saveProperties(KConfig* config)
{
  // the 'config' object points to the session managed
  // config file.  anything you write here will be available
  // later when this app is restored
  config->writePathEntry("fileName", m_part->url().path());
}

void Kig::readProperties(KConfig* config)
{
  // the 'config' object points to the session managed
  // config file.  this function is automatically called whenever
  // the app is being restored.  read in here whatever you wrote
  // in 'saveProperties'
  load ( KURL(config->readPathEntry("fileName")));
}

void Kig::load(const KURL& url)
{
  // we check for m_part not being 0, because in the case of us not
  // finding our library, we would otherwise get a crash...
  if ( m_part && m_part->openURL( url ) ) m_recentFilesAction->addURL( url );
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

void Kig::openURL(const KURL& url)
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

void Kig::optionsConfigureKeys()
{
  KKeyDialog dlg( true, this );
  dlg.insert( actionCollection() );
  dlg.insert( m_part->actionCollection() );
  (void) dlg.configure( true );
}

void Kig::optionsConfigureToolbars()
{
  saveMainWindowSettings(KGlobal::config(), "MainWindow");

  // use the standard toolbar editor
  KEditToolbar dlg(factory());
  connect(&dlg, SIGNAL(newToolbarConfig()),
	  this, SLOT(applyNewToolbarConfig()));
  dlg.exec();
}

void Kig::applyNewToolbarConfig()
{
  applyMainWindowSettings(KGlobal::config(), "MainWindow");
}

bool Kig::queryClose()
{
  if (!m_part->isReadWrite() || !m_part->isModified()) return true;
  switch( KMessageBox::warningYesNoCancel
	  (
	   widget(),
	   i18n("Save changes to document %1?").arg(m_part->url().path()),
	   i18n("Save Changes?")
	   ))
    {
    case KMessageBox::Yes:
      if (m_part->save()) return true;
      else return false;
      break;
    case KMessageBox::No:
      return true;
      break;
    default: // cancel
      return false;
      break;
    };
}

void Kig::dragEnterEvent(QDragEnterEvent* e)
{
  e->accept(KURLDrag::canDecode(e));
}

void Kig::dropEvent(QDropEvent* e)
{
  KURL::List urls;
  if ( KURLDrag::decode (e, urls) )
    {
      for (KURL::List::iterator u = urls.begin(); u != urls.end(); ++u)
	{
	   Kig* k = new Kig;
	   k->show();
	   k->load(*u);
	};
    };
}

void Kig::fileOpen()
{
  // this slot is connected to the KStdAction::open action...
  QString file_name = KFileDialog::getOpenFileName(":document", i18n("*.kig *.kgeo *.seg|All Supported Files (*.kig *.kgeo *.seg)\n"
                                                                     "*.kig|Kig Documents (*.kig)\n"
                                                                     "*.kgeo|KGeo Documents (*.kgeo)\n"
                                                                     "*.seg|KSeg Documents (*.seg)" ));

  if (!file_name.isEmpty()) openURL(file_name);
}

// ifdef's disabled, cause Qt moc doesn't handle ifdef's..
// #ifdef KIG_DONT_USE_NEW_KMAINWINDOW_FEATURES
void Kig::optionsShowToolbar()
{
#ifdef KIG_DONT_USE_NEW_KMAINWINDOW_FEATURES
  if (m_toolbarAction->isChecked())
    toolBar()->show();
  else
    toolBar()->hide();
#else
  assert( false );
#endif
}

void Kig::optionsShowStatusbar()
{
#ifdef KIG_DONT_USE_NEW_KMAINWINDOW_FEATURES
  if (m_statusbarAction->isChecked())
    statusBar()->show();
  else
    statusBar()->hide();
#else
  assert( false );
#endif
}
// #endif
