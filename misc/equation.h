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

#ifndef KIG_EQUATION_H
#define KIG_EQUATION_H

#include <qstring.h>

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
  EquationString( const QString string );
  double trunc( double );
  void prettify( void );
  void addTerm( double coeff, const QString unknowns, bool& needsign );

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

//class Equation
//{
//public:
//  /**
//   * Constructs a new empty equation.
//   */
//  Equation();
//
//  /**
//   * Adds a new term to the equation. Use \p xpower and \p ypower to specify
//   * the power of the relative variables. For example, if you would add
//   * \f$-5xy^2 \f$ to the equation, then do:
//   * \code
//   * Equation eq;
//   * // other code ...
//   * eq.addTerm( -5, 1, 2 );
//   * \endcode
//   * \param coeff is the coefficient of the new term
//   * \param xpower is the power of the \em x variable
//   * \param ypower is the power of the \em y variable
//   */
//  void addTerm( double coeff, int xpower, int ypower = 0 );
//  /**
//   * Removes the term with coefficients \p xpower and \p ypower for the
//   * equations.
//   * \param xpower is the power of the \em x variable
//   * \param ypower is the power of the \em y variable
//   */
//  void removeTerm( int xpower, int ypower = 0 );
//  /**
//   * Gives the value of the coefficient of the term with coefficients
//   * \p xpower and \p ypower .
//   * \param value will contain the value of the found term, or 0 if not found
//   * \param xpower is the power of the \em x variable
//   * \param ypower is the power of the \em y variable
//   * \return true or false whether the term was found
//   */
//  bool term( double& value, int xpower, int ypower = 0 );
//
//  /**
//   * Removes all the term of the equation.
//   */
//  void clear();
//
//  /**
//   * Set whether to show a second member for the equation, like \f$... = 0 \f$.
//   * Note that the second member contents can be only a numeric value.
//   * \param show whether to show the second member
//   */
//  void setSecondMemberShown( bool show );
//  bool isSecondMemberShown() const;
//  /**
//   * Sets \p value as the value for the second member. It is displayed only if
//   * you activated it using setShowSecondMember().
//   * The default value for the second member is 0 (zero).
//   */
//  void setSecondMemberValue( double value );
//  double secondMemberValue() const;
//
//  /**
//   * Returns a string with the current equation written in a nice form.
//   * \param with_spaces specifies whether expand a bit the equation with spaces
//   */
//  QString prettyString( bool with_spaces = false ) const;
//
//private:
//  class Private;
//  Private* d;
//};
//
#endif
