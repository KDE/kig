#ifndef KIG_H
#define KIG_H

#include "kig_iface.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif 

#include <kapp.h>
#include <kparts/mainwindow.h>
#include <dcopclient.h>
 
class KToggleAction;
class KRecentFilesAction;

/**
 * This is the application "Shell".  It has a menubar, toolbar, and
 * statusbar but relies on the "Part" to do all the real work.
 *
 * @short Application Shell
 * @author Dominique Devriese <fritmebufstek@pandora.be>
 * @version 2.0
 */
class Kig : public KParts::MainWindow, virtual public KigIface
{
  Q_OBJECT
 public:
  /**
   * Default Constructor
   */
  Kig();

  /**
   * Default Destructor
   */
  virtual ~Kig();
 public slots:
  /**
   * this opens url in this window
   */
  void load(const KURL& url);

  /** 
   * this opens the file specified in s in a new window
   * 
   * @param s 
   */
  void openURL (const QString& s) { openURL(KURL(s)); };
  void openURL (const KURL& url);
 protected:
  /**
   * This method is called when it is time for the app to save its
   * properties for session management purposes.
   */
  void saveProperties(KConfig *);
  
  /**
   * This method is called when this app is restored.  The KConfig
   * object points to the session management config file that was saved
   * with @ref saveProperties
   */
  void readProperties(KConfig *);

 private slots:
  void fileNew();
  void optionsShowToolbar();
  void optionsShowStatusbar();
  void optionsConfigureKeys();
  void optionsConfigureToolbars();
  
  void applyNewToolbarConfig();

 private:
  void setupAccel();
  void setupActions();

 private:
  KParts::ReadWritePart *m_part;

  KToggleAction *m_toolbarAction;
  KToggleAction *m_statusbarAction;
  KRecentFilesAction *m_recentFilesAction;
};

#endif // KGEO_H
