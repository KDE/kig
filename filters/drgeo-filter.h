// drgeo-filter.h
// Copyright (C)  2004  Pino Toscano <toscano.pino@tiscali.it>
// Copyright (C)  2004  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_FILTERS_DRGEO_FILTER_H
#define KIG_FILTERS_DRGEO_FILTER_H

#include "filter.h"
#include "drgeo-filter-chooser.h"

class QDomNode;
class KigDocument;
class QString;

class KigFilterDrgeo
  : public KigFilter
{
protected:
  KigFilterDrgeo();
  ~KigFilterDrgeo();
public:
  static KigFilterDrgeo* instance();

  bool supportMime( const QString& mime );
  bool load( const QString& file, KigDocument& to );
private:
  bool importFigure( QDomNode f, KigDocument& doc, const QString& file, const bool grid );
};

#endif
