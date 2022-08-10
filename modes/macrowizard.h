// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QWizard>

class DefineMacroMode;
class GivenArgsPage;
class FinalArgsPage;

class MacroWizard : public QWizard
{
    Q_OBJECT
public:
    MacroWizard(QWidget *parent, DefineMacroMode *m);
    ~MacroWizard();

    static const int GivenArgsPageId = 0;
    static const int FinalArgsPageId = 1;
    static const int MacroInfoPageId = 2;

    void givenArgsChanged();
    void finalArgsChanged();

public Q_SLOTS:
    void reject() override;
    void accept() override;

private Q_SLOTS:
    void currentIdChanged(int id);
    void slotHelpClicked();

private:
    DefineMacroMode *mmode;
    GivenArgsPage *mgivenArgsPage;
    FinalArgsPage *mfinalArgsPage;
};
