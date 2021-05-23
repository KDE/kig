// This file is part of Kig, a KDE program for Interactive Geometry...
// SPDX-FileCopyrightText: 2004 Dominique Devriese <devriese@kde.org>
// SPDX-FileCopyrightText: 2004 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_MISC_GONIOMETRY_H
#define KIG_MISC_GONIOMETRY_H

#include <QStringList>

/**
 * Manage an angle and convert it from/to other goniometric systems.
 */
class Goniometry
{
public:
  enum System { Deg, Rad, Grad };
  Goniometry();
  Goniometry( double value, Goniometry::System system );
  ~Goniometry();
  void setValue( double value );
  double value() const;
  /**
   * Set the system of the current angle to \p system, but it doesn't
   * convert the value to the new system.
   *
   * \see convertTo()
   */
  void setSystem( Goniometry::System system );
  /**
   * Set the system of the current angle to \p system and convert the
   * value to the new system using \ref convert().
   *
   * \see setSystem()
   */
  void convertTo( Goniometry::System system );
  Goniometry::System system() const;
  double getValue( Goniometry::System system );
  /**
   * The most useful method of this class: convert the specified
   * \p angle from the system \p from to the system \p to.
   */
  static double convert( const double angle, const Goniometry::System from, const Goniometry::System to );
  /**
   * Get a list of the supported goniometric systems.
   */
  static QStringList systemList();
  static Goniometry::System intToSystem( const int index );

  Goniometry( const Goniometry &g ) = default;
  Goniometry& operator= ( const Goniometry& g ) = default;

private:
  double mvalue;
  typedef Goniometry::System goniosys;
  goniosys msys;
};

#endif
