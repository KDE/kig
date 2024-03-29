// SPDX-FileCopyrightText: 2005 Pino Toscano <toscano.pino@tiscali.it>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QWidget>

class Ui_SVGExporterOptionsWidget;

class SVGExporterOptions : public QWidget
{
    Q_OBJECT

    Ui_SVGExporterOptionsWidget *expwidget;

public:
    explicit SVGExporterOptions(QWidget *parent);
    ~SVGExporterOptions();

    void setGrid(bool grid);
    bool showGrid() const;
    void setAxes(bool axes);
    bool showAxes() const;
};
