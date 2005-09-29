// This file is part of Kig, a KDE program for Interactive Geometry...
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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

#ifndef FILTER_H
#define FILTER_H

#include <qstring.h>

#include <vector>

class KigFilter;
class ScreenInfo;
class KigDocument;

/**
 * This singleton class handles all the input filters.
 */
class KigFilters
{
public:
  static KigFilters* instance();
  KigFilter* find (const QString& mime);

//  bool save ( const KigDocument& data, QTextStream& stream );
  /**
   * saving is always done with the native filter.  We don't support
   * output filters..
   */
  bool save ( const KigDocument& data, const QString& outfile );
protected:
  KigFilters();
  static KigFilters* sThis;
  typedef std::vector<KigFilter*> vect;
  vect mFilters;
};

// KigFilter::load functions should use this macro to conveniently
// return a very useful parse error in a filter's load function..
#define KIG_FILTER_PARSE_ERROR \
  { \
    QString locs = i18n( "An error was encountered at " \
                         "line %1 in file %2." ) \
      .arg( __LINE__ ).arg( __FILE__ ); \
    parseError( file, locs ); \
    return 0; \
  }

/**
 * This is the base class for an input filter.
 *
 * All the needed work to add a new input filter to Kig is
 * subclassing this class and implement supportMime() and load().
 */
class KigFilter
{
protected:
  // shows errors to the user..
  void fileNotFound( const QString& file ) const;
  void parseError( const QString& file, const QString& explanation = QString::null ) const;
  void notSupported( const QString& file, const QString& explanation ) const;
  void warning( const QString& explanation ) const;
public:
  KigFilter();
  virtual ~KigFilter();

  /**
   * can the filter handle the mimetype \p mime ?
   */
  virtual bool supportMime ( const QString& mime );

  /**
   * load file \p fromfile and build a KigDocument from it..  If this
   * function returns 0, that means that an error occurred while
   * loading ( implementations of this function are responsible for
   * showing an error message themselves, using the above error
   * functions ). If this functions returns non-0, the caller owns
   * the returned KigDocument ( that was allocated with "new" ).
   */
  virtual KigDocument* load ( const QString& fromfile ) = 0;
};
#endif
