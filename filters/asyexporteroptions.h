// SPDX-FileCopyrightText: 2010, 2011 Raoul Bourquin <raoulb@bluewin.ch>
//
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_FILTERS_ASYEXPORTEROPTIONS_H
#define KIG_FILTERS_ASYEXPORTEROPTIONS_H

#include <QWidget>

class Ui_AsyExporterOptionsWidget;

class AsyExporterOptions
  : public QWidget
{
  Q_OBJECT

  Ui_AsyExporterOptionsWidget* expwidget;

public:
  explicit AsyExporterOptions( QWidget* parent );
  ~AsyExporterOptions();

  void setGrid( bool grid );
  bool showGrid() const;

  void setAxes( bool axes );
  bool showAxes() const;

  void setExtraFrame( bool frame );
  bool showExtraFrame() const;
};

#endif
