// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "mode.h"

#include <QPoint>
#include <vector>

class KigWidget;
class ObjectHolder;

/**
 * The BaseMode is a particular mode that allows an easy mapping of
 * the mouse actions to real, logical actions of the mode.
 */
class BaseMode : public KigMode
{
    QPoint mplc;
    std::vector<ObjectHolder *> moco;

public:
    void leftClicked(QMouseEvent *e, KigWidget *v) override;
    void leftMouseMoved(QMouseEvent *, KigWidget *) override;
    void leftReleased(QMouseEvent *e, KigWidget *v) override;
    void midClicked(QMouseEvent *e, KigWidget *v) override;
    void midReleased(QMouseEvent *e, KigWidget *v) override;
    void rightClicked(QMouseEvent *, KigWidget *) override;
    void mouseMoved(QMouseEvent *e, KigWidget *v) override;

protected:
    void enableActions() override;

    std::vector<ObjectHolder *> oco();
    QPoint pointLocation();

protected:
    virtual void dragRect(const QPoint &p, KigWidget &w);
    virtual void dragObject(const std::vector<ObjectHolder *> &os, const QPoint &pointClickedOn, KigWidget &w, bool ctrlOrShiftDown);
    virtual void leftClickedObject(ObjectHolder *o, const QPoint &p, KigWidget &w, bool ctrlOrShiftDown) = 0;
    virtual void midClicked(const QPoint &p, KigWidget &w) = 0;
    virtual void rightClicked(const std::vector<ObjectHolder *> &oco, const QPoint &p, KigWidget &w) = 0;
    virtual void mouseMoved(const std::vector<ObjectHolder *> &os, const QPoint &p, KigWidget &w, bool shiftpressed) = 0;

protected:
    BaseMode(KigPart &);
    ~BaseMode();
};
