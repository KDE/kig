/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/

#ifndef KIG_POPUP_H
#define KIG_POPUP_H

#include <kpopupmenu.h>

#include "../objects/object.h"
#include "../misc/objects.h"

class KigDocument;
class KigView;
class NormalMode;

class NormalModePopupObjects
  : public KPopupMenu
{
  Q_OBJECT
  QPoint mp;
  KigDocument* mdoc;
  KigView* mview;
  Objects mobjs;
  NormalMode* mmode;
  QPopupMenu* mcolorpopup;
  QPopupMenu* colorMenu( QWidget* parent );
  const QColor* color( int );
  QPopupMenu* constructMenu( QWidget* parent );
  QPopupMenu* virtualMenu( QWidget* parent );
public:
  NormalModePopupObjects( KigDocument* doc, KigView* view,
                          NormalMode* mode, const Objects& objs );
  ~NormalModePopupObjects() {};
protected slots:
  void activated( int );
  void setColor( int );
};

// class KigObjectsPopup
//   : public QPopupMenu
// {
//   Q_OBJECT
// public slots:
//   void selectObjects();
//   void unselectObjects();
//   void hideObjects();
//   void startMoving();
//   void setColor( int );

// public:
//   KigObjectsPopup( KigDocument*, KigView*, const Objects& os );
//   ~KigObjectsPopup();
//   bool isValid() const { return mValid; };
//   int exec( const QPoint& p );
// protected:
//   KigDocument* mDoc;
//   KigView* mView;
//   Objects mObjs;
//   QPoint mStart;
//   bool mValid;
//   QPopupMenu* mColorPopup;
//   static const QColor* color( int i );
//   static QPopupMenu* colorMenu( QWidget* );
// };

// class KigDocumentPopup
//   : public QPopupMenu
// {
//   KigDocument* mDoc;
// public:
//   KigDocumentPopup( KigDocument* );
// };

#endif
