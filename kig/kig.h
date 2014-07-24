// This file is part of Kig, a KDE program for Interactive Geometry...
// Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_KIG_H
#define KIG_KIG_H

#include <KParts/MainWindow>
#include <KParts/ReadWritePart>

class KRecentFilesAction;

/**
 * This is the application "Shell".  It has a menubar, toolbar, and
 * statusbar but relies on the "Part" to do all the real work.
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
   * \param file file to open
   */
  void load( const QUrl &file );

  /**
   * this opens the file specified in \p s in a new window
   *
   * \param s the url of the file to open
   */
  void openUrl( const QUrl &s );

 protected:

  /**
   * The user started dragging something onto us...
   *
   * \param e
   */
  virtual void dragEnterEvent( QDragEnterEvent* e );

  /**
   * The user dropped something onto us...
   *
   * \param e
   */
  virtual void dropEvent( QDropEvent* e );

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
  void saveProperties(KConfigGroup &);

  /**
   * This method is called when this app is restored.  The KConfig
   * object points to the session management config file that was saved
   * with \ref saveProperties
   */
  void readProperties(const KConfigGroup &);

 private slots:
  void fileNew();
  void fileOpen();
  void optionsConfigureToolbars();

  void applyNewToolbarConfig();

  void tipOfDay();
  void startupTipOfDay();

 private:
  void setupActions();

  KParts::ReadWritePart *m_part;
  QStringList m_mimeTypes;
  KRecentFilesAction *m_recentFilesAction;

  KConfig* config;
};

#endif // KIG_KIG_H
