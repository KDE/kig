/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2012 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef OBJECTCHOOSERPOPUP_H
#define OBJECTCHOOSERPOPUP_H

#include <QMenu>

#include <vector>

class KigWidget;
class ObjectHolder;

/**
 * This class is useful to choose one object from a list of some, by
 * querying the user via popup menu.
 *
 * You can't use this class directly, but these's a convenience method.
 */
class ObjectChooserPopup
  : public QMenu
{
  Q_OBJECT

public:
  /**
   * Get the index of the chosen object from a list of objects.
   *
   * \param p is the point where execute the popup
   * \param w is the pointer to a KigWidget
   * \param objs is the vector with the objects to chose from
   * \param givepopup true means that we have to show a popup to the user
   *
   * \return the index of the chosen element ( starting from 0 ), or -1
   *         if none was selected.
   */
  static int getObjectFromList( const QPoint& p, KigWidget* w,
                                const std::vector<ObjectHolder*>& objs,
                                bool givepopup = true );

protected:
  ObjectChooserPopup( const QPoint& p, KigWidget& view,
                      const std::vector<ObjectHolder*>& objs );
  ~ObjectChooserPopup();

protected slots:
  void actionActivatedSlot( QAction* );

protected:
  QPoint mplc;
  KigWidget& mview;
  std::vector<ObjectHolder*> mobjs;

  int mselected;
};

#endif // OBJECTCHOOSERPOPUP_H
