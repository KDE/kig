/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2005  Pino Toscano <toscano.pino@tiscali.it>

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

#ifndef KIG_MISC_KIGINPUTDIALOG_H
#define KIG_MISC_KIGINPUTDIALOG_H

class QString;
class Coordinate;
class Goniometry;
class KigDocument;
class KigInputDialogPrivate;

#include <kdialogbase.h>

/**
 * The KigInputDialog class provides easy ways of interaction with the user.
 * For example, it provides a flexible way to get one or two coordinates at
 * once.
 *
 * It provides several static convenience functions: getCoordinate(),
 * getTwoCoordinates(), getAngle().
 */
class KigInputDialog
  : KDialogBase
{
Q_OBJECT

public:

private:
  KigInputDialog( const QString& caption, const QString& label, QWidget* parent,
        const KigDocument& doc, Coordinate* c1, Coordinate* c2 );
  KigInputDialog( QWidget* parent, const Goniometry& g );

  virtual void keyPressEvent( QKeyEvent* e );

  KigInputDialogPrivate* const d;
  friend class KInputDialogPrivate;

  Coordinate coordinateFirst() const;
  Coordinate coordinateSecond() const;
  Goniometry goniometry() const;

private slots:
  void slotCoordsChanged( const QString& );
  void slotGonioSystemChanged( int index );
  void slotGonioTextChanged( const QString& txt );

public:
  /**
   * Static convenience function to get a Coordinate from the user.
   *
   * \param caption caption of the dialog
   * \param label text of the label of the dialog
   * \param parent parent of the dialog widget
   * \param ok it will be set to true if the user pressed Ok after inserting a
   *           well-formatted Coordinate
   * \param doc the actual Kig document
   * \param cvalue a pointer to a Coordinate class. If the user inserted
   *               successfully a new Coordinate, the value will be stored
   *               here. If this points to a valid Coordinate, then it will be
   *               displayed as initial value of the correspondenting text edit
   */
  static void getCoordinate( const QString& caption, const QString& label,
        QWidget* parent, bool* ok, const KigDocument& doc, Coordinate* cvalue );

  /**
   * Static convenience function to get two Coordinates at once from the user.
   *
   * \param caption caption of the dialog
   * \param label text of the label of the dialog
   * \param parent parent of the dialog widget
   * \param ok it will be set to true if the user pressed Ok after inserting
   *           well-formatted Coordinates
   * \param doc the actual Kig document
   * \param cvalue a pointer to a Coordinate class. If the user inserted
   *               successfully new Coordinates, the value of the first
   *               Coordinate will be stored here. If this points to a valid
   *               Coordinate, then it will be displayed as initial value of
   *               the text edit representing the first Coordinate.
   * \param cvalue2 a pointer to a Coordinate class. If the user inserted
   *                successfully new Coordinates, the value of the second
   *                Coordinate will be stored here. If this points to a valid
   *                Coordinate, then it will be displayed as initial value of
   *                the text edit representing the second Coordinate.
   */
  static void getTwoCoordinates( const QString& caption, const QString& label,
        QWidget* parent, bool* ok, const KigDocument& doc, Coordinate* cvalue,
        Coordinate* cvalue2 );

  /**
   * Static convenience function to get an angle incapsulated in a Goniometry
   * class.
   *
   * \param parent parent of the dialog widget
   * \param ok it will be set to true if the user pressed Ok after inserting a
   *           well-formatted angle
   * \param g the Goniometry class containing the original angle we are going
   *          to modify.
   *
   * \return a Goniometry class containing the new angle
   */
  static Goniometry getAngle( QWidget* parent, bool* ok, const Goniometry& g );
};

#endif
