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
protected:
  bool readOnly;
public:
  void setReadOnly (bool ro);

protected:
  // we reimplement these from QWidget to suit our needs
  void mousePressEvent (QMouseEvent* e);
  void mouseMoveEvent (QMouseEvent* e);
  void mouseReleaseEvent (QMouseEvent* e);
  void paintEvent (QPaintEvent* e);
  void resizeEvent(QResizeEvent*);

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
  QPixmap stillPix; // What Do the Still Objects Look Like
  QPixmap curPix; // temporary, gets bitBlt'd (copied) onto the widget (to avoid flickering)
  QPtrList<QRect> objectOverlayList;

public slots:
  void paintOnPartOfWidget() { paintOnWidget(false); };
  void paintOnWidget( bool needRedraw );
  void redrawStillPix();
  void setMode();
  void redrawOneObject(const Object* o);

public slots:
  void startKioskMode();
  void endKioskMode();
signals:
  void changed();
  void setStatusBarText(const QString&);

protected:
  KigDocument* document;
  // what the cursor currently looks like
  QPoint plc; // point last clicked
  QPoint pmt; // point moved to
  Objects oco; // objects clicked on
  void displayText(QString s) { tbd = s; emit setStatusBarText(s); };
  QString tbd; // text being displayed
  // what are we doing?
  typedef enum { CMode, // "constructing" -> something is in document->obc
		 MMode, // "move mode" -> we're moving stuff.
		 NMode, // "nothing" -> none of the other
		 SMode // "selecting" -> selecting objects for a macro obj
  } KGVMode;
  enum KGVMode mode;

  // what is the user doing in this mode
  typedef enum { nmNothing, // we're doing nothing
		 nmClickedNowhere, // the user clicked on no-man's-land
		 nmClickedForNewPoint, // ...
		 nmClickedForSelecting, // the user clicked on an object, and wants to (un)select it
		 nmMoving // the user is simply moving his cursor around
  } NModeEnum;
  enum NModeEnum nmode;
  void setNMode (NModeEnum inNMode);

  // what is the user doing in this mode
  typedef enum { cmNothing, // we're doing nothing
		 cmMoving, // the user is moving his cursor around (without any button clicked
		 cmClicked, // the user clicked on an object
		 cmClickedForNewPoint // the user middle-clicked for a new point, which becomes an argument to the object
  } CModeEnum;
  enum CModeEnum cmode;
  void setCMode (CModeEnum inCMode);

protected:
  QDialog* kiosk;
  KPopupMenu* kiosKontext;
  KigView* kioskView;
  bool isKiosk;
  signals:
  void endKiosk();
};
#endif
