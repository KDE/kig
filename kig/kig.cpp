/*
 * kig.cpp
 *
 */
#include "kig.h"

#include <kkeydialog.h>
#include <kconfig.h>
#include <kurl.h>

#include <kedittoolbar.h>

#include <kaction.h>
#include <kstdaction.h>
#include <kstatusbar.h>
#include <klibloader.h>
#include <kmessagebox.h>
#include <kdebug.h>
#include <kapplication.h>
#include <klocale.h>

Kig::Kig()
  : KParts::MainWindow( 0L, "Kig" ),
    KigIface()
{
  // set the shell's ui resource file
  setXMLFile("kig_shell.rc");
  // then, setup our actions
  setupActions();
  
  // and a status bar
  statusBar()->show();
  
  // this routine will find and load our Part.  it finds the Part by
  // name which is a bad idea usually.. but it's alright in this
  // case since our Part is made for this Shell
  KLibFactory *factory = KLibLoader::self()->factory("libkigpart");
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
    }
  resize (640,480);

  if (!kapp->dcopClient()->isRegistered())
    {
      kapp->dcopClient()->registerAs("kigiface");
      kapp->dcopClient()->setDefaultObject( objId() );
    };
}

Kig::~Kig()
{
}

void Kig::load(const KURL& url)
{
  m_recentFilesAction->addURL( url );
  m_part->openURL( url );
}

void Kig::setupActions()
{
  KStdAction::openNew(this, SLOT(fileNew()), actionCollection());
  KStdAction::quit(this, SLOT(close()), actionCollection());

  m_toolbarAction = KStdAction::showToolbar(this, SLOT(optionsShowToolbar()), actionCollection());
  m_statusbarAction = KStdAction::showStatusbar(this, SLOT(optionsShowStatusbar()), actionCollection());
  m_recentFilesAction = KStdAction::openRecent(this, SLOT(load(const KURL&)), actionCollection());

  KStdAction::keyBindings(this, SLOT(optionsConfigureKeys()), actionCollection());
  KStdAction::configureToolbars(this, SLOT(optionsConfigureToolbars()), actionCollection());
}

void Kig::saveProperties(KConfig* config)
{
  // the 'config' object points to the session managed
  // config file.  anything you write here will be available
  // later when this app is restored
  m_recentFilesAction->saveEntries(config);
}

void Kig::readProperties(KConfig* config)
{
  // the 'config' object points to the session managed
  // config file.  this function is automatically called whenever
  // the app is being restored.  read in here whatever you wrote
  // in 'saveProperties'
  m_recentFilesAction->loadEntries(config);
}

void Kig::fileNew()
{
  // this slot is called whenever the File->New menu is selected,
  // the New shortcut is pressed (usually CTRL+N) or the New toolbar
  // button is clicked

  // create a new window
  (new Kig)->show();
}


void Kig::optionsShowToolbar()
{
  // this is all very cut and paste code for showing/hiding the
  // toolbar
  if (m_toolbarAction->isChecked())
    toolBar()->show();
  else
    toolBar()->hide();
}

void Kig::optionsShowStatusbar()
{
  // this is all very cut and paste code for showing/hiding the
  // statusbar
  if (m_statusbarAction->isChecked())
    statusBar()->show();
  else
    statusBar()->hide();
}

void Kig::optionsConfigureKeys()
{
  KKeyDialog::configureKeys(actionCollection(), "kig_shell.rc");
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

void Kig::openURL(const KURL& url)
{
  Kig* widget = new Kig;
  widget->show();
  widget->load(url);
}

bool Kig::queryClose()
{
  if (!m_part->isReadWrite() || !m_part->isModified()) return true;
  switch( KMessageBox::warningYesNoCancel 
	  (
	   widget(),
	   i18n("Save changes to document %1 ?").arg(m_part->url().path()),
	   i18n("Save changes ?")
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

#include "kig.moc"
