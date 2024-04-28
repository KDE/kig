// This file is part of Kig, a KDE program for Interactive Geometry.
// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

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

public Q_SLOTS:
    /**
     * Open file in this window
     * \param file file to open
     */
    void load(const QUrl &file);

    /**
     * this opens the file specified in \p s in a new window
     *
     * \param s the url of the file to open
     */
    void openUrl(const QUrl &s);

protected:
    /**
     * The user started dragging something onto us...
     *
     * \param e
     */
    void dragEnterEvent(QDragEnterEvent *e) override;

    /**
     * The user dropped something onto us...
     *
     * \param e
     */
    void dropEvent(QDropEvent *e) override;

    /**
     * this is called by the framework before closing the window, to
     * allow the user to save his changes... returning false cancels the
     * close request...
     */
    bool queryClose() override;

    /**
     * This method is called when it is time for the app to save its
     * properties for session management purposes.
     */
    void saveProperties(KConfigGroup &) override;

    /**
     * This method is called when this app is restored.  The KConfig
     * object points to the session management config file that was saved
     * with \ref saveProperties
     */
    void readProperties(const KConfigGroup &) override;

private Q_SLOTS:
    void fileNew();
    void fileOpen();
    void optionsConfigureToolbars();

    void applyNewToolbarConfig();

private:
    void setupActions();

    KParts::ReadWritePart *m_part;
    QStringList m_mimeTypes;
    KRecentFilesAction *m_recentFilesAction;

    KConfig *config;
};
