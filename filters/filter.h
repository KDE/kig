/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese

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

class KigFilters
{
public:
  static KigFilters* instance();
  KigFilter* find (const QString& mime);

protected:
  KigFilters();
  static KigFilters* sThis;
  typedef std::vector<KigFilter*> vect;
  vect mFilters;
};

class KigFilter
{
public:
  KigFilter() {};
  virtual ~KigFilter() {};

  typedef enum { OK, FileNotFound, ParseError, NotSupported } Result;

  // can the filter handle this mimetype ?
  virtual bool supportMime ( const QString /*mime*/ ) { return false; };

  // load file file, fill up os.
  virtual Result load ( const QString from, Objects& os ) = 0;

  // ...
  virtual Result save ( const Objects& os, const QString file );
};
#endif
