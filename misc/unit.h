/*
   This file is part of Kig, a KDE program for Interactive Geometry...
   Copyright (C) 2006  Pino Toscano <toscano.pino@tiscali.it>

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/

#ifndef KIG_MISC_UNIT_H
#define KIG_MISC_UNIT_H

#include <QStringList>

/**
 * This small class server as helper to perform conversions between
 * metrical units.
 */
class Unit
{
public:
  /**
   * The kinds of metrical units we support.
   */
  enum MetricalUnit { pixel = 0, cm, in };
  explicit Unit( double value = 0.0, Unit::MetricalUnit unit = cm, int dpi = 1 );
  ~Unit();
  void setValue( double value );
  double value() const;
  /**
   * Set the unit of the current object to \p unit, but it doesn't
   * convert the value to the new unit.
   *
   * \see convertTo()
   */
  void setUnit( Unit::MetricalUnit unit );
  /**
   * Set the unit of the current object to \p unit and convert the
   * value to the new unit using \ref convert().
   *
   * \see setUnit()
   */
  void convertTo( Unit::MetricalUnit unit );
  Unit::MetricalUnit unit() const;
  double getValue( Unit::MetricalUnit unit ) const;
  void setDpi( int dpi );
  int dpi() const;
  /**
   * The most useful method of this class: convert the specified
   * \p value from the unit \p from to the unit \p to, using the
   * specified \p dpi in case of conversions from/to pixels.
   */
  static double convert( double value, Unit::MetricalUnit from, Unit::MetricalUnit to, int dpi = 1 );
  /**
   * Get a list of the supported metrical units.
   */
  static QStringList unitList();
  static Unit::MetricalUnit intToUnit( int index );

  /**
   * How many decimals the \p unit have.
   */
  static int precision( Unit::MetricalUnit unit );

  Unit& operator=( const Unit& u );

private:
  double mvalue;
  Unit::MetricalUnit munit;
  int mdpi;
};

#endif
