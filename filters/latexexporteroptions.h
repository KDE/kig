// SPDX-FileCopyrightText: 2005 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_FILTERS_LATEXEXPORTEROPTIONS_H
#define KIG_FILTERS_LATEXEXPORTEROPTIONS_H

#include <QWidget>

class Ui_LatexExporterOptionsWidget;

class LatexExporterOptions
  : public QWidget
{
  Q_OBJECT

  Ui_LatexExporterOptionsWidget* expwidget;

public:

  enum LatexOutputFormat
  {
    PSTricks,
    TikZ,
    Asymptote,
    FormatCount
  };

  explicit LatexExporterOptions( QWidget* parent );
  ~LatexExporterOptions();

  void setFormat( LatexOutputFormat format );
  LatexOutputFormat format();

  void setStandalone( bool standalone );
  bool standalone();

  void setGrid( bool grid );
  bool showGrid() const;

  void setAxes( bool axes );
  bool showAxes() const;

  void setExtraFrame( bool frame );
  bool showExtraFrame() const;
};

#endif
