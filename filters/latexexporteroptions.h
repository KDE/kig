// Copyright (C)  2005  Pino Toscano       <toscano.pino@tiscali.it>

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
