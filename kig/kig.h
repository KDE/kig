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


#ifndef KIG_KIG_H
#define KIG_KIG_H

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
 * @author Dominique Devriese <devriese@kde.org>
 * @version 2.0
 */
class Kig : public KParts::MainWindow
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
   * Open file in this window
   * @param file file to open
   */
  void load (const KURL& file);

  /**
   * this opens the file specified in s in a new window
   *
   * @param s
   */
  void openURL (const QString& s) { openURL(KURL(s)); };
  void openURL (const KURL& s);

 protected:

  /**
   * The user started dragging something onto us...
   *
   * @param e
   */
  void dragEnterEvent(QDragEnterEvent* e);

  /**
   * The user dropped something onto us...
   *
   * @param e
   */
  void dropEvent (QDropEvent* e);

  /**
   * this is called by the framework before closing the window, to
   * allow the user to save his changes... returning false cancels the
   * close request...
   */
  bool queryClose();

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
  void fileOpen();
  void optionsShowToolbar();
  void optionsShowStatusbar();
  void optionsConfigureKeys();
  void optionsConfigureToolbars();

  void applyNewToolbarConfig();

 private:
  void setupActions();

  KParts::ReadWritePart *m_part;

  KToggleAction *m_toolbarAction;
  KToggleAction *m_statusbarAction;
  KRecentFilesAction *m_recentFilesAction;

  static bool kimageioRegistered;
};

#endif // KIG_KIG_H
