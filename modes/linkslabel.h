// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QWidget>

#include <utility>
#include <vector>

/**
 * this widget shows a line of text, with some links underlined, and
 * emits a signal if one of the links is clicked...
 */
class LinksLabel : public QWidget
{
    Q_OBJECT

public:
    explicit LinksLabel(QWidget *parent = nullptr);
    ~LinksLabel();

    class LinksLabelEditBuf
    {
    public:
        friend class LinksLabel;
        ~LinksLabelEditBuf()
        {
        }

    private:
        // declare these private so only LinksLabel can use them...
        LinksLabelEditBuf()
        {
        }
        typedef std::vector<std::pair<bool, QString>> vec;
        vec data;
    };

    /**
     * start editing, start recording changes in a LinksLabelEditBuf,
     * but don't apply them until after endEdit();
     */
    LinksLabelEditBuf startEdit();
    /**
     * add a piece of normal text.
     */
    void addText(const QString &s, LinksLabelEditBuf &buf);
    /**
     * add a link...
     */
    void addLink(const QString &s, LinksLabelEditBuf &buf);
    /**
     * apply the changes... This clears the current contents and adds
     * the new data...
     */
    void applyEdit(LinksLabelEditBuf &buf);

Q_SIGNALS:
    /**
     * the user clicked on a link.  The index is the order in which it
     * was added.  E.g. this signal is emitted with arg 0 if the link
     * you first added is clicked, argument 2 for the third link etc.
     */
    void linkClicked(int i);

    /**
     * the structure of links and texts was changed
     */
    void changed();

private Q_SLOTS:
    void urlClicked();

private:
    class Private;
    Private *p;
};
