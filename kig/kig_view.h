#ifndef KIG_VIEW_H
#define KIG_VIEW_H

#include <qwidget.h>

#include <kdebug.h>
#include <kpopupmenu.h>

#include "../objects/object.h"

class KigDocument;

class KigView
  : public QWidget
{
  Q_OBJECT
public:
  KigView( KigDocument* inDoc, QWidget* parent = 0, const char* name = 0, bool inIsKiosk = false);
  ~KigView();

public slots:
  void startKioskMode();
  void endKioskMode();
signals:
  void endKiosk();
signals:
  void setStatusBarText(const QString&);

protected:
  void displayText(QString s = QString::null) { tbd = s; emit setStatusBarText(s); if (tbd) drawTbd(); };

  KigDocument* document;
  // what the cursor currently looks like
  QPoint plc; // point last clicked
  QPoint pmt; // point moved to
  Objects oco; // objects clicked on
  QString tbd; // text being displayed
  // if a user clicks on a selected point, this can either be for
  // moving or for deselecting.  We first assume it's for deselecting,
  // until he moves far enough away from the place he clicked.  Then
  // we set this variable to true.
  bool isMovingObjects;
  bool isDraggingRect;

protected:
  // we reimplement these from QWidget to suit our needs
  void mousePressEvent (QMouseEvent* e);
  void mouseMoveEvent (QMouseEvent* e);
  void mouseReleaseEvent (QMouseEvent* e);
  void paintEvent (QPaintEvent* e);
  void resizeEvent(QResizeEvent*);

protected:
  // drawing: i tried to optimise this as much as possible, using ideas
  // from kgeo
  // we keep a picture ( stillPix ) of what the still objects look like,
  // and on moving,  we only draw the other on a copy of this pixmap.
  // furthermore,  on drawing the other, we only draw what is in
  // document->sos->getSpan()
  // another thing: it turns out that working on the pixmaps isn't
  // that slow, but working on the widget is.  So we try to reduce the
  // amount of work we spend on the widget. (i got this idea from the
  // kgeo, all credits for this should go to marc.bartsch@web.de) : objects
  // have a getObjectOverlay function,
  // in which they specify some rects which they occupy.  We only
  // bitBlt those rects onto the widget. This is only done on moving,
  // since that's where speed matters most... 

  // redraw the stillPix (the grid and such...
  void redrawStillPix();
  // this means bitBlting "curPix" on "this"
  void updateWidget( bool needRedraw = false);
  // this means bitBlting "stillPix" on "curPix"
  void updateCurPix();
  // draw a single object (on p)
  void drawObject(const Object* o, QPixmap& p);
  // draw these objects (on p)
  void drawObjects(const Objects& os, QPixmap& p);
  void drawObjects(const Objects& os) { drawObjects(os, curPix); };
  // draw the Text Being Displayed (on curPix)
  // @ref displayText()
  void drawTbd();
  // draw the rect being dragged for selecting objects... (on curPix)
  // @ref isDraggingRect
  void drawRect();
  // draw the grid... (on stillPix)
  void drawGrid();
  // draw the obc preliminarily... (i.e. before it's entirely
  // constructed) (on curPix)
  void drawPrelim(); 

public slots:
  // redraw everything...
  void updateAll()
  {
    redrawStillPix();
    updateCurPix();
    updateWidget();
  };
  // update one object on the screen...
  void updateObject(const Object* o)
  {
    drawObject(o, stillPix);
    drawObject(o, curPix);
    updateWidget();
  };
  // update a few objects...
  void updateObjects(const Objects& o) { drawObjects(o); updateWidget(); };

protected:
  QPixmap stillPix; // What Do the Still Objects Look Like
  QPixmap curPix; // temporary, gets bitBlt'd (copied) onto the widget
		  // (to avoid flickering)
  QPtrList<QRect> overlay, oldOverlay;

  // this maps the coordinates of the objects to the widget...
  EuclideanCoords mCoords;

protected:
  QDialog* kiosk;
  KPopupMenu* kiosKontext;
  KigView* kioskView;
  bool isKiosk;

};
#endif
