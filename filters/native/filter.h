// filter.h
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef FILTERS_NEWNATIVE_FILTER_H
#define FILTERS_NEWNATIVE_FILTER_H

#include "../filter.h"

class QDomElement;
class KigDocument;
class QDomElement;
class QCString;
class RealObject;

// Kig's native format.  Between versions 0.3.1 and 0.4, there was a
// change in the file format, and this filter is designed to support
// both of them ( for reading, that is...)
class KigFilterNative
  : public KigFilter
{
  Result loadOld( const QDomElement& doc, KigDocument& to );
  Result loadNew( const QDomElement& doc, KigDocument& to );
  bool oldElemToNewObject( const QCString type, const QDomElement& e,
                           RealObject& o, Objects&, KigDocument& );
public:
  KigFilterNative();
  ~KigFilterNative();

  bool supportMime( const QString& mime );
  Result load( const QString& file, KigDocument& to );
  Result save( const KigDocument& data, const QString& file );

};

#endif
