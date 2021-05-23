// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_FILTERS_IMAGEEXPORTEROPTIONS_H
#define KIG_FILTERS_IMAGEEXPORTEROPTIONS_H

#include "../misc/unit.h"

#include <QWidget>

class QSize;
class Ui_ImageExporterOptionsWidget;

class ImageExporterOptions
  : public QWidget
{
  Q_OBJECT

  QSize msize;
  Ui_ImageExporterOptionsWidget* expwidget;
  double maspectratio;
  Unit mxunit;
  Unit myunit;

  // this is set by slotWidthChanged() when they set the other input
  // widget's value, to avoid reacting to internal changes to the
  // value like to user changes...
  bool minternallysettingstuff;
public:
  explicit ImageExporterOptions( QWidget* parent );
  ~ImageExporterOptions();

  void setGrid( bool grid );
  bool showGrid() const;

  void setAxes( bool axes );
  bool showAxes() const;

  void setImageSize( const QSize& size );
  QSize imageSize() const;

protected slots:
  void slotWidthChanged( double );
  void slotHeightChanged( double );
  void slotUnitChanged( int );
};

#endif
