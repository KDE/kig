/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <dominique.devriese@student.kuleuven.ac.be>

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


#ifndef KIG_VIEW_H
#define KIG_VIEW_H

#include <qwidget.h>
#include <qpixmap.h>

#include <kparts/part.h>

#include <vector>

#include "../objects/object.h"
#include "../misc/rect.h"
#include "../misc/screeninfo.h"

class KigDocument;

/**
 * This is the widget as represented on-screen.  It is basically a
 * dumb class, which is manipulated by KigMode's.  All events are
 * forwarded to them.  It has some actions which are widget specific
 * (basically the view-menu and printing (once i implement it ;))
 */
class KigView
  : public QWidget
{
  Q_OBJECT

  KAction* aZoomIn;
  KAction* aZoomOut;
  KAction* aCenterScreen;

public:
  KigView( KigDocument* inDoc,
	   QWidget* parent = 0,
	   const char* name = 0
	   );
  ~KigView();

  void setupActions();

public slots:

  // this is connected to KigDocument::suggestRect, check that signal
  // out for an explanation...
  void recenterScreen();
  // ...
  void zoomIn();
  void zoomOut();

  void redrawScreen();

public:
  /**
   * The following are functions used by KigMode's to tell us to draw
   * stuff...
   * i tried to optimise the drawing as much as possible, using
   * much ideas from kgeo
   * DOUBLE BUFFERING:
   * we don't draw on the widget directly, we draw on a QPixmap (
   * curPix ), and bitBlt that onto the widget to avoid flickering.
   * TRIPLE BUFFERING:
   * we also currently keep an extra pixmap of what the widget looks
   * like without objects that are moving... i'm currently wondering
   * whether this isn't a performance loss rather than a gain, but
   * well, i haven't done any measurements (yet ?), so for now it
   * stays in...
   * OVERLAYS
   * Another thing: it turns out that working on the pixmaps isn't
   * that slow, but working on the widget is.  So we try to reduce the
   * amount of work we spend on the widget. (i got this idea from
   * kgeo, all credits for this go to marc.bartsch@web.de)
   * on drawing, KigPainter tells us (appendOverlay) for everything it
   * draws what rects it draws in, so we know on updating the widget (
   * updateWidget() that the rest is still ok and doesn't need to be
   * bitBlt'd again on the widget...
   */

  // clear stillPix...
  void clearStillPix();
  // update curPix (bitBlt stillPix onto curPix..)
  void updateCurPix( const std::vector<QRect>& = std::vector<QRect>());

  // this means bitBlting curPix on the actual widget...
  void updateWidget( const std::vector<QRect>& = std::vector<QRect>() );
  void updateEntireWidget();

protected:
  KigDocument* document;
  // what the cursor currently looks like

protected:
  // we reimplement these from QWidget to suit our needs
  void mousePressEvent (QMouseEvent* e);
  void mouseMoveEvent (QMouseEvent* e);
  void mouseReleaseEvent (QMouseEvent* e);
  void paintEvent (QPaintEvent* e);
  void resizeEvent(QResizeEvent*);

  /************** Mapping between Internal Coordinate Systems **********/
public:
  // there are two coordinate systems:
  // 1 the widget's coordinates: these are simple int's from (0,0) in
  // the topleft of the widget to size() in the bottomRight...
  // 2 the document's coordinates: these are the coordinates used by
  // the KigDocument.  Objects only know of their coordinates as
  // related to this system.
  // These are mapped by the KigView using the ScreenInfo class.

  const Rect showingRect();

  const Coordinate fromScreen( const QPoint& p );
  const Rect fromScreen( const QRect& r );
  double pixelWidth();

  // the part of the document we're currently showing
  // i.e. a rectangle of the document (which has its own coordinate
  // system) which is mapped onto the widget.
  const ScreenInfo& screenInfo();

protected:
  // this is called to match a rect's dimensions to the dimensions of
  // the window before we set mViewRect to it.  This is done cause we
  // always want circles to look like circles etc...
  Rect matchScreenShape( const Rect& r );

  // to avoid stupid syntax.  Actually, the KigMode's implement
  // functionality which is logically inherent to KigView.. So this
  // isn't just an ugly hack ;)
  friend class KigMode;
  friend class NormalMode;
  friend class MovingModeBase;
  friend class StdConstructionMode;
  friend class PointConstructionMode;
  friend class DefineMacroMode;

  // what do the still objects look like
  // wondering if this is appropriate, maybe it should be part of
  // MovingMode ?
  QPixmap stillPix;
  // temporary, gets bitBlt'd (copied) onto the widget
  // (to avoid flickering)
  QPixmap curPix;
  std::vector<QRect> oldOverlay;

  /**
   * this is a class that maps from our widget coordinates to the
   * document's coordinates ( and back ).
   */
  ScreenInfo msi;
};
#endif
