/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/


#ifndef FILTER_H
#define FILTER_H

#include <qstring.h>

#include <vector>

#include "../misc/objects.h"

class Object;
class KigFilter;
class ScreenInfo;
class KigDocument;

class KigFilters
{
public:
  static KigFilters* instance();
  KigFilter* find (const QString& mime);

  // saving is always done with the native filter.  We don't support
  // output filters..
  bool save ( const KigDocument& data, const QString& tofile );
protected:
  KigFilters();
  static KigFilters* sThis;
  typedef std::vector<KigFilter*> vect;
  vect mFilters;
};

// use this macro to conveniently return a very useful parse error in
// a filter's load function..
#define KIG_FILTER_PARSE_ERROR \
  { \
    QString locs = i18n( "An error was encountered at " \
                         "line %1 in file %2." ) \
      .arg( __LINE__ ).arg( __FILE__ ); \
    parseError( file, locs ); \
    return false; \
  }

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

  // can the filter handle this mimetype ?
  virtual bool supportMime ( const QString& mime );

  // load file fromfile..  ( don't forget to make this atomic, this
  // means: only really change to's data when you're sure no error
  // will occur in reading/parsing the file..
  virtual bool load ( const QString& fromfile, KigDocument& to ) = 0;
};
#endif
