// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#ifndef KIG_MODE_LABEL_H
#define KIG_MODE_LABEL_H

#include "mode.h"

#include "../objects/object_calcer.h"

#include <vector>

class TextLabelWizard;
class NormalMode;
class Coordinate;
class QString;
class ObjectPropertyCalcer;
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
  void labelTextChanged();
  void linkClicked( int );
  void redrawScreen( KigWidget* w );

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
   * that have no children..
   */
  void setPropertyObjects( const argvect& props );
  void setFrame( bool f );

  virtual void finish( const Coordinate& c, const QString& s,
                       const argvect& props, bool needframe,
                       ObjectCalcer* locationparent ) = 0;

private:
  // the KigMode interface..
  void leftClicked( QMouseEvent*, KigWidget* );
  void leftReleased( QMouseEvent*, KigWidget* );

  void mouseMoved( QMouseEvent*, KigWidget* );

  void enableActions();

  void cancelConstruction();

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

  void updateWiz();
  void updateLinksLabel();
};

class TextLabelConstructionMode
  : public TextLabelModeBase
{
public:
  TextLabelConstructionMode( KigPart& d );
  ~TextLabelConstructionMode();

  void finish( const Coordinate& coord, const QString& s,
               const argvect& props, bool needframe,
               ObjectCalcer* locationparent );
};

class TextLabelRedefineMode
  : public TextLabelModeBase
{
  ObjectTypeCalcer* mlabel;
  void finish( const Coordinate& coord, const QString& s,
               const argvect& props, bool needframe,
               ObjectCalcer* locationparent );
public:
  TextLabelRedefineMode( KigPart& d, ObjectTypeCalcer* label );
  ~TextLabelRedefineMode();
};

#endif
