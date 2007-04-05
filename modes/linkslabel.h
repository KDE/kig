// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA

#ifndef KIG_lINKS_LABEL_H
#define KIG_lINKS_LABEL_H

#include <qwidget.h>

#include <vector>
#include <utility>

/**
 * this widget shows a line of text, with some links underlined, and
 * emits a signal if one of the links is clicked...
 */
class LinksLabel : public QWidget
{
  Q_OBJECT

public:
  LinksLabel( QWidget* parent = 0, const char* name = 0 );
  ~LinksLabel();

  class LinksLabelEditBuf
  {
  public:
    friend class LinksLabel;
    ~LinksLabelEditBuf() {}
  private:
    // declare these private so only LinksLabel can use them...
    LinksLabelEditBuf() {}
    typedef std::vector<std::pair<bool,QString> > vec;
    vec data;
  };

  /**
   * start editing, start recording changes in a LinksLabelEditBuf,
   * but don't apply them until after endEdit();
   */
  LinksLabelEditBuf startEdit();
  /**
   * add a piece of normal text..
   */
  void addText( const QString& s, LinksLabelEditBuf& buf );
  /**
   * add a link...
   */
  void addLink( const QString& s, LinksLabelEditBuf& buf );
  /**
   * apply the changes... This clears the current contents and adds
   * the new data...
   */
  void applyEdit( LinksLabelEditBuf& buf );

signals:
  /**
   * the user clicked on a link.  The index is the order in which it
   * was added.  E.g. this signal is emitted with arg 0 if the link
   * you first added is clicked, argument 2 for the third link etc.
   */
  void linkClicked( int i );

private slots:
  void urlClicked();

private:
  class Private;
  Private* p;
};

#endif // KDE_URLS_LABEL_H
