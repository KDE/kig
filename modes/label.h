// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_MODE_LABEL_H
#define KIG_MODE_LABEL_H

#include "mode.h"

#include "../objects/object_calcer.h"

#include <QPoint>

#include <vector>

class TextLabelWizard;
class Coordinate;
class QString;
class ObjectTypeCalcer;
class ObjectCalcer;

/**
 * this is the base class for TextLabelConstructionMode and
 * TextLabelRedefineMode..  most of the work is done in this class,
 * with some specific things delegated to the children..  Template
 * method pattern, right ? :)
 */
class TextLabelModeBase
  : public KigMode
{
  class Private;
  Private* d;

public:
  // below is the interface towards TextLabelWizard...
  void cancelPressed();
  void finishPressed();
  void enterTextPageEntered();
  void selectArgumentsPageEntered();
  void linkClicked( int );
  void redrawScreen( KigWidget* w ) Q_DECL_OVERRIDE;
  bool percentCountChanged( uint percentcount );
  bool canFinish();

protected:
  typedef std::vector<ObjectCalcer::shared_ptr> argvect;
  // the protected interface for subclasses
  TextLabelModeBase( KigPart& d );
  ~TextLabelModeBase();

  void setCoordinate( const Coordinate& coord );
  void setText( const QString& s );
  void setLocationParent( ObjectCalcer* o );
  /**
   * objects you pass here, should be newly created property objects,
   * that have no children.
   */
  void setPropertyObjects( const argvect& props );
  void setFrame( bool f );

  virtual void finish( const Coordinate& c, const QString& s,
                       const argvect& props, bool needframe,
                       ObjectCalcer* locationparent ) = 0;
  void leftReleased( QMouseEvent*, KigWidget*, ObjectTypeCalcer* prevlabel = 0 );

private:
  // the KigMode interface.
  void leftClicked( QMouseEvent*, KigWidget* ) Q_DECL_OVERRIDE;
  void leftReleased( QMouseEvent*, KigWidget* ) Q_DECL_OVERRIDE;

  void mouseMoved( QMouseEvent*, KigWidget* ) Q_DECL_OVERRIDE;

  void enableActions() Q_DECL_OVERRIDE;

  void cancelConstruction() Q_DECL_OVERRIDE;

  void killMode();

private:
  /**
   * \internal
   * What Are We Doing...
   * the diff between SelectingArgs and ReallySelectingArgs is that
   * the latter means the user is selecting an arg in the kig window,
   * whereas the first only means that he's looking at the second
   * page of the wizard...
   */
  typedef enum { SelectingLocation, RequestingText, SelectingArgs, ReallySelectingArgs } wawdtype;

  void updateLinksLabel();
};

class TextLabelConstructionMode
  : public TextLabelModeBase
{
public:
  explicit TextLabelConstructionMode( KigPart& d );
  ~TextLabelConstructionMode();

  void finish( const Coordinate& coord, const QString& s,
               const argvect& props, bool needframe,
               ObjectCalcer* locationparent ) Q_DECL_OVERRIDE;
};

class TextLabelRedefineMode
  : public TextLabelModeBase
{
  ObjectTypeCalcer* mlabel;
  void finish( const Coordinate& coord, const QString& s,
               const argvect& props, bool needframe,
               ObjectCalcer* locationparent ) Q_DECL_OVERRIDE;
public:
  TextLabelRedefineMode( KigPart& d, ObjectTypeCalcer* label );
  ~TextLabelRedefineMode();
  void leftReleased( QMouseEvent*, KigWidget* ) Q_DECL_OVERRIDE;
};

/**
 * How to construct a numeric label in a clean way.
 */
class NumericLabelMode
  : public KigMode
{
public:
  explicit NumericLabelMode( KigPart& d );
  ~NumericLabelMode();

  void redrawScreen( KigWidget* w ) Q_DECL_OVERRIDE;

  void leftClicked( QMouseEvent*, KigWidget* ) Q_DECL_OVERRIDE;
  void leftReleased( QMouseEvent*, KigWidget* ) Q_DECL_OVERRIDE;
  void mouseMoved( QMouseEvent*, KigWidget* ) Q_DECL_OVERRIDE;

  void enableActions() Q_DECL_OVERRIDE;

  void cancelConstruction() Q_DECL_OVERRIDE;

  void killMode();

private:
  QPoint mplc;
};


#endif
