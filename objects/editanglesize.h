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

#ifndef KIG_OBJECTS_EDITANGLESIZE_H
#define KIG_OBJECTS_EDITANGLESIZE_H

#include "editanglesizebase.h"

#include "../misc/goniometry.h"

#include <kiconloader.h>

class EditAngleSize : public EditAngleSizeBase
{
  Q_OBJECT

  Goniometry mang;
  Goniometry mang_orig;
  bool isnum;
  const KIconLoader* il;
public:
  EditAngleSize( QWidget* parent, double angle = 0, Goniometry::System system = Goniometry::Rad );
  ~EditAngleSize();
  const double angle() const;
  const Goniometry::System system() const;

public slots:
  void okSlot();
  void cancelSlot();

protected slots:
  void activatedSlot( int index );
  void textChangedSlot( const QString& txt );
};

#endif
