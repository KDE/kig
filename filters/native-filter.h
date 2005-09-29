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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#ifndef KIG_FILTERS_NATIVE_FILTER_H
#define KIG_FILTERS_NATIVE_FILTER_H

#include "filter.h"

class QDomElement;
class KigDocument;
class QTextStream;
class QString;

/**
 * Kig's native format.  Between versions 0.3.1 and 0.4, there was a
 * change in the file format.  This filter no longer supports pre-0.4
 * formats, it did up until Kig 0.6.
 */
class KigFilterNative
  : public KigFilter
{
private:
  /**
   * this is the load function for the Kig format that is used,
   * starting at Kig 0.4
   */
  KigDocument* load04( const QString& file, const QDomElement& doc );
  /**
   * this is the load function for the Kig format that is used
   * starting at Kig 0.7
   */
  KigDocument* load07( const QString& file, const QDomElement& doc );

  /**
   * save in the Kig format that is used starting at Kig 0.7
   */
  bool save07( const KigDocument& data, const QString& outfile );
  bool save07( const KigDocument& data, QTextStream& file );

  KigFilterNative();
  ~KigFilterNative();
public:
  static KigFilterNative* instance();

  bool supportMime( const QString& mime );
  KigDocument* load( const QString& file );

  bool save( const KigDocument& data, const QString& file );
//  bool save( const KigDocument& data, QTextStream& stream );
};

#endif
