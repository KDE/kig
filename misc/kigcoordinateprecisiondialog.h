/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <QDialog>

#ifndef KIGCOORDINATEPRECISIONDIALOG_H
#define KIGCOORDINATEPRECISIONDIALOG_H

#include <ui_kigcoordinateprecisiondialog.h>

class KigCoordinatePrecisionDialog
  : public QDialog
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
