// This file is part of Kig, a KDE program for Interactive Geometry...
// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef FILTER_H
#define FILTER_H

#include <QString>

#include <vector>

class KigFilter;
class KigDocument;

/**
 * This singleton class handles all the input filters.
 */
class KigFilters
{
public:
  static KigFilters* instance();
  KigFilter* find (const QString& mime);

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
                         "line %1 in file %2.", \
        __LINE__, __FILE__ ); \
    parseError( locs ); \
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
  void parseError( const QString& explanation = QString() ) const;
  void notSupported( const QString& explanation ) const;
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
