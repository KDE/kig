/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2004  Dominique Devriese <devriese@kde.org>
 Copyright (C) 2004  Pino Toscano <toscano.pino@tiscali.it>

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

#ifndef KIG_MISC_ZOOMAREA_H
#define KIG_MISC_ZOOMAREA_H

#include "zoomareabase.h"
#include "../misc/coordinate.h"
#include "kig_document.h"

#include <qvalidator.h>

#include <kiconloader.h>

class ZoomArea
  : public ZoomAreaBase
{
  Q_OBJECT

  const KIconLoader* il;
  KigDocument& mdoc;
  QValidator* mvtor;

  Coordinate mcoord0;
  Coordinate mcoord1;
public:
  ZoomArea( QWidget* parent, KigDocument& doc );
  ~ZoomArea();

  Coordinate coord0() const;
  Coordinate coord1() const;
  void setCoord0( Coordinate& );
  void setCoord1( Coordinate& );

public slots:
  void okSlot();
  void cancelSlot();

protected slots:
  void coordsChangedSlot( const QString& );
};

#endif
