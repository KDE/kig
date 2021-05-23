// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_FILTERS_KSEG_FILTER_H
#define KIG_FILTERS_KSEG_FILTER_H

#include "filter.h"

class ObjectCalcer;

class KigFilterKSeg
  : public KigFilter
{
  KigFilterKSeg();
  ~KigFilterKSeg();

  ObjectCalcer* transformObject( KigDocument& kigdoc,
                                 std::vector<ObjectCalcer*>& parents,
                                 int subtype, bool& ok );

public:
  static KigFilterKSeg* instance();

  bool supportMime ( const QString& mime ) Q_DECL_OVERRIDE;
  KigDocument* load ( const QString& fromfile ) Q_DECL_OVERRIDE;
};

#endif
