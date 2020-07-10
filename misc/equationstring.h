// Copyright (C)  2005  Pino Toscano <toscano.pino@tiscali.it>

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

#ifndef KIG_EQUATIONSTRING_H
#define KIG_EQUATIONSTRING_H

#include <QString>

/**
 * Simple class that represents an equation.
 *
 * It does not do any calculation, it serves only as a helper to write
 * equations like \f$ 7 xy + 3 y^2 - 5 x - 2 y + 8 = 0 \f$ in a pretty form.
 * It can handle equations of max two variables (named \em x and \em y ) with
 * no limits on the grade of the terms. (well, the limit is 15, but I doubt
 * anyone will ever use such a factor for a power...)
 */

// * Use the addTerm() method to add a term to the equation, and removeTerm() to
// * remove one. After that, use prettyString() to get the equation written in a
// * nice form.
// *
// * \note
// * If you want to use only the \em x variable, you can unspecify the last
// * parameters in the term manipulation methods, like addTerm(), term() and
// * removeTerm().
// *
// * @author Pino Toscano
// */

 /*
  * What follows is based on code by Maurizio Paolini
  */

class EquationString
 : public QString
{
public:
  EquationString( const QString& string );
  double trunc( double );
  void prettify( void );
  void addTerm( double coeff, const QString& unknowns, bool& needsign );

  const QString x3() const;
  const QString y3() const;
  const QString x2y() const;
  const QString xy2() const;
  const QString x2() const;
  const QString y2() const;
  const QString xy() const;
  const QString x() const;
  const QString y() const;
  const QString xnym(int n, int m) const;
};

#endif
