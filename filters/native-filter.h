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

#ifndef KIG_FILTERS_NATIVE_FILTER_H
#define KIG_FILTERS_NATIVE_FILTER_H

#include "filter.h"

class QDomElement;
class KigDocument;
class QDomElement;
class QCString;

// Kig's native format.  Between versions 0.3.1 and 0.4, there was a
// change in the file format, and this filter is designed to support
// both of them ( for reading, that is...)
class KigFilterNative
  : public KigFilter
{
  bool loadOld( const QString& file, const QDomElement& doc, KigDocument& to );
  bool loadNew( const QString& file, const QDomElement& doc, KigDocument& to );

  KigFilterNative();
  ~KigFilterNative();
public:
  static KigFilterNative* instance();

  bool supportMime( const QString& mime );
  bool load( const QString& file, KigDocument& to );

  bool save( const KigDocument& data, const QString& file );
};

#endif
