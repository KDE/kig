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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#include <kdialog.h>

#ifndef KIGCOORDINATEPRECISIONDIALOG_H
#define KIGCOORDINATEPRECISIONDIALOG_H

#include <ui_kigcoordinateprecisiondialog.h>

class KigCoordinatePrecisionDialog
  : public KDialog
{
Q_OBJECT

public:
  KigCoordinatePrecisionDialog(bool isUserSpecified, int currentPrecision );
  virtual ~KigCoordinatePrecisionDialog();
  int getUserSpecifiedCoordinatePrecision() const;

private:
  Ui::KigCoordinatePrecisionDialog* ui;
  
private slots:
  void toggleCoordinateControls( int state );
};

#endif // KIGCOORDINATEPRECISIONDIALOG_H
