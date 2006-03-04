// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_FILTERS_IMAGEEXPORTEROPTIONS_H
#define KIG_FILTERS_IMAGEEXPORTEROPTIONS_H

#include <qwidget.h>

class QSize;
class Ui_ImageExporterOptionsWidget;
class Unit;

class ImageExporterOptions
  : public QWidget
{
  Q_OBJECT

  QSize msize;
  Ui_ImageExporterOptionsWidget* expwidget;
  double maspectratio;
  Unit* mxunit;
  Unit* myunit;

  // this is set by slotWidthChanged() when they set the other input
  // widget's value, to avoid reacting to internal changes to the
  // value like to user changes...
  bool minternallysettingstuff;
public:
  ImageExporterOptions( QWidget* parent );
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
