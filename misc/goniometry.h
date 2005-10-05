// This file is part of Kig, a KDE program for Interactive Geometry...
// Copyright (C) 2004  Dominique Devriese <devriese@kde.org>
// Copyright (C) 2004  Pino Toscano <toscano.pino@tiscali.it>

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

#ifndef KIG_MISC_GONIOMETRY_H
#define KIG_MISC_GONIOMETRY_H

#include <qstringlist.h>

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
  const double value() const;
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
  const Goniometry::System system() const;
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

  Goniometry& operator= ( const Goniometry& g );

private:
  double mvalue;
  typedef Goniometry::System goniosys;
  goniosys msys;
};

#endif
