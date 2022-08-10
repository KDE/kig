// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <QWizard>

class ArgsPage;
class LinksLabel;
class TextLabelModeBase;
class TextPage;

class TextLabelWizard : public QWizard
{
    Q_OBJECT
public:
    TextLabelWizard(QWidget *parent, TextLabelModeBase *mode);
    ~TextLabelWizard();

    static const int TextPageId = 0;
    static const int ArgsPageId = 1;

    LinksLabel *linksLabel();
    QString text() const;
    void setText(const QString &newtext);

public Q_SLOTS:
    void reject() override;
    void accept() override;

private Q_SLOTS:
    void textChanged();
    void linkClicked(int which);
    void currentIdChanged(int id);
    void slotHelpClicked();

private:
    TextLabelModeBase *mmode;
    TextPage *mtextPage;
    ArgsPage *margsPage;
};
