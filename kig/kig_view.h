/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

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
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/


#ifndef KIG_VIEW_H
#define KIG_VIEW_H

#include <qwidget.h>
#include <qpixmap.h>

#include <kparts/part.h>

#include <vector>

#include "../objects/object_holder.h"
#include "../misc/rect.h"
#include "../misc/screeninfo.h"

class QGridLayout;
class QScrollBar;

class KigDocument;
class KigView;

/**
 * This class is the real widget showing the document.  The other is a
 * wrapper, that has the scrollbars...  I'm not using QScrollView
 * cause i've been having problems with that, and it's easier to do
 * the work myself...
 * Internally, this is basically a dumb class, which is manipulated by
 * KigMode's.  All events are forwarded to them.
 */
class KigWidget : public QWidget
{
  Q_OBJECT

  KigPart* mpart;
  KigView* mview;

  // we reimplement these from QWidget to suit our needs
  void mousePressEvent( QMouseEvent* e );
  void mouseMoveEvent( QMouseEvent* e );
  void mouseReleaseEvent( QMouseEvent* e );
  void paintEvent( QPaintEvent* e );
  void wheelEvent( QWheelEvent* e );
  void resizeEvent( QResizeEvent* );
  QSize sizeHint() const;

  /**
   * this is called to match a rect's dimensions to the dimensions of
   * the window before we set mViewRect to it.  This is done cause we
   * always want circles to look like circles etc...
   */
  Rect matchScreenShape( const Rect& r ) const;

public:
  /**
   * what do the still objects look like
   * wondering if this is appropriate, maybe it should be part of
   * MovingMode ?
   */
  QPixmap stillPix;
  /**
   * temporary, gets bitBlt'd (copied) onto the widget
   * (to avoid flickering)
   */
  QPixmap curPix;

protected:
  std::vector<QRect> oldOverlay;

  /**
   * this is a class that maps from our widget coordinates to the
   * document's coordinates ( and back ).
   */
  ScreenInfo msi;

  /**
   * is this a full-screen widget ?
   */
  bool misfullscreen;

public:
  /**
   * standard qwidget constructor.  if fullscreen is true, we're a
   * fullscreen widget.
   */
  KigWidget( KigPart* doc,
             KigView* view,
             QWidget* parent = 0,
             const char* name = 0,
             bool fullscreen = false
	   );
  ~KigWidget();

  bool isFullScreen() const;
  void setFullScreen( bool f );

  const KigView* view() const {
    return mview;
  }

  KigView* view() {
    return mview;
  }

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

  /**
   * clear stillPix...
   */
  void clearStillPix();
  /**
   * update curPix (bitBlt stillPix onto curPix..)
   */
  void updateCurPix( const std::vector<QRect>& = std::vector<QRect>());

  /**
   * this means bitBlting curPix on the actual widget...
   */
  void updateWidget( const std::vector<QRect>& = std::vector<QRect>() );
  void updateEntireWidget();

  /**
   * Mapping between Internal Coordinate Systems
   * there are two coordinate systems:
   * 1 the widget's coordinates: these are simple int's from (0,0) in
   * the topleft of the widget to size() in the bottomRight...
   * 2 the document's coordinates: these are the coordinates used by
   * the KigDocument.  Objects only know of their coordinates as
   * related to this system.
   * These are mapped by the KigView using the ScreenInfo class.
   */
  const Rect showingRect() const;
  void setShowingRect( const Rect& r );

  const Coordinate fromScreen( const QPoint& p );
  const Rect fromScreen( const QRect& r );
  double pixelWidth() const;

  /**
   * the part of the document we're currently showing
   * i.e. a rectangle of the document (which has its own coordinate
   * system) which is mapped onto the widget.
   */
  const ScreenInfo& screenInfo() const;

  Rect entireDocumentRect() const;

  void updateScrollBars();

  void scrollSetBottom( double rhs );
  void scrollSetLeft( double rhs );

  const KigDocument& document() const;

public:
  /**
   * this recenters the screen, that is, resets the shown rect to
   * mpart->document().suggestedRect()..
   */
  void recenterScreen();
  /**
   * this gets called if the user clicks the recenter screen button.
   * It adds a KigCommand to the CommandHistory that recenters the
   * screen..
   */
  void slotRecenterScreen();

  // called when the user clicks the appropriate buttons..
  void slotZoomIn();
  void slotZoomOut();

  void zoomRect();
  void zoomArea();

  void redrawScreen( const std::vector<ObjectHolder*>& selection, bool paintOnWidget = true );
};

/**
 * This class is a wrapper for KigWidget.  It has some actions
 * that belong here, and not in the part.  It also maintains the
 * scrollbars, but it doesn't manipulate them itself.  It forwards
 * most of its functionality to KigWidget...
 */
class KigView
  : public QWidget
{
  Q_OBJECT

  QGridLayout* mlayout;
  QScrollBar* mrightscroll;
  QScrollBar* mbottomscroll;

  /**
   * apparently, QScrollBar also emits its signals when you update it
   * manually, so we ignore them while we're in \ref updateScrollBars()...
   */
  bool mupdatingscrollbars;

  KigWidget* mrealwidget;
  KigPart* mpart;

public:
  KigView( KigPart* part,
           bool fullscreen = false,
	   QWidget* parent = 0,
	   const char* name = 0
	   );
  ~KigView();

  void setupActions();

  const ScreenInfo& screenInfo() const;

  KigWidget* realWidget() const;
  void scrollHorizontal( int delta );
  void scrollVertical( int delta );

public slots:
  void updateScrollBars();
  void slotZoomIn();
  void slotZoomOut();
  void zoomRect();
  void zoomArea();
  void slotInternalRecenterScreen();
  void slotRecenterScreen();
  void toggleFullScreen();

private slots:
  void slotRightScrollValueChanged( int );
  void slotBottomScrollValueChanged( int );
};
#endif
