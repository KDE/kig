// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_FILTERS_NATIVE_FILTER_H
#define KIG_FILTERS_NATIVE_FILTER_H

#include "filter.h"

class QDomElement;
class QDomDocument;
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
  KigDocument* load04( const QDomElement& doc );
  /**
   * this is the load function for the Kig format that is used
   * starting at Kig 0.7
   */
  KigDocument* load07( const QDomElement& doc );

  /**
   * save in the Kig format that is used starting at Kig 0.7
   */
  bool save07( const KigDocument& data, const QString& outfile );
  bool save07( const KigDocument& data, QTextStream& file );

  KigFilterNative();
  ~KigFilterNative();
public:
  static KigFilterNative* instance();

  bool supportMime( const QString& mime ) Q_DECL_OVERRIDE;
  KigDocument* load( const QString& file ) Q_DECL_OVERRIDE;
  KigDocument* load( const QDomDocument& doc );

  bool save( const KigDocument& data, const QString& file );
//  bool save( const KigDocument& data, QTextStream& stream );
};

#endif
