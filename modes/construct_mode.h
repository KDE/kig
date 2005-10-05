// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_MODES_CONSTRUCT_MODE_H
#define KIG_MODES_CONSTRUCT_MODE_H

#include "base_mode.h"

#include "../objects/object_calcer.h"

class ArgsParserObjectType;
class ObjectConstructor;
class ObjectCalcer;

class PointConstructMode
  : public BaseMode
{
  /**
   * this is the point that we move around, for the user to add
   * somewhere..
   */
  ObjectTypeCalcer::shared_ptr mpt;
public:
  PointConstructMode( KigPart& d );
  ~PointConstructMode();
protected:
  void leftClickedObject( ObjectHolder* o, const QPoint& p,
                                KigWidget& w, bool ctrlOrShiftDown );
  void midClicked( const QPoint& p, KigWidget& w );
  void rightClicked( const std::vector<ObjectHolder*>& oco, const QPoint& p, KigWidget& w );
  void mouseMoved( const std::vector<ObjectHolder*>& os, const QPoint& p, KigWidget& w, bool shiftpressed );

  void enableActions();
  void cancelConstruction();

  void redrawScreen( KigWidget* );
};

class BaseConstructMode
  : public BaseMode
{
  /**
   * this is the point that we move around, in case the user wants to
   * add a point somewhere..
   */
  ObjectTypeCalcer::shared_ptr mpt;
  /**
   * mp: this point always follows the cursor
   *
   * IMPORTANT: this Calcer must NEVER be added to the document, since
   * its is used in constructors that need more input from the user
   * to decide parameters of the constructed object that will be fixed
   * afterwards (like the number of sides of a regular polygon)
   */
  ObjectTypeCalcer* mcursor;
  // we also allocate here the corresponding objectholder, since the
  // only sensible place where to deallocate it is in the destructor
  // of this class
//  ObjectHolder* mcursorholder;
  std::vector<ObjectHolder*> mparents;

  void leftReleased( QMouseEvent* e, KigWidget* v );

public:
  void selectObject( ObjectHolder* o, KigWidget& w );
  void selectObjects( const std::vector<ObjectHolder*>& os, KigWidget& w );
  virtual ~BaseConstructMode();
protected:
  BaseConstructMode( KigPart& d );
protected:
  void leftClickedObject( ObjectHolder* o, const QPoint& p,
                          KigWidget& w, bool ctrlOrShiftDown );
  void midClicked( const QPoint& p, KigWidget& w );
  void rightClicked( const std::vector<ObjectHolder*>& oco, const QPoint& p, KigWidget& w );
  void mouseMoved( const std::vector<ObjectHolder*>& os, const QPoint& p, KigWidget& w, bool shiftpressed );

  void enableActions();
  void cancelConstruction();
  void finish();

protected:
  virtual void handlePrelim( const std::vector<ObjectCalcer*>& os, const QPoint& p, KigPainter&, KigWidget& w ) = 0;
  virtual QString selectStatement( const std::vector<ObjectCalcer*>& args, const KigWidget& w ) = 0;
  virtual int isAlreadySelectedOK( const std::vector<ObjectCalcer*>&, const int& ) = 0;
  virtual int wantArgs( const std::vector<ObjectCalcer*>&, KigDocument& d, KigWidget& w ) = 0;
  virtual void handleArgs( const std::vector<ObjectCalcer*>& args, KigWidget& w ) = 0;

  void redrawScreen( KigWidget* );
};

class ConstructMode
  : public BaseConstructMode
{
  const ObjectConstructor* mctor;
public:
  ConstructMode( KigPart& d, const ObjectConstructor* ctor );
  ~ConstructMode();

  void handlePrelim( const std::vector<ObjectCalcer*>& os, const QPoint& p, KigPainter&, KigWidget& w );
  QString selectStatement( const std::vector<ObjectCalcer*>& args, const KigWidget& w );
  int isAlreadySelectedOK( const std::vector<ObjectCalcer*>&, const int& );
  int wantArgs( const std::vector<ObjectCalcer*>&, KigDocument& d, KigWidget& w );
  void handleArgs( const std::vector<ObjectCalcer*>& args, KigWidget& w );
};

/**
 * This class constructs a test object.  It has special needs over
 * ConstructMode because first the arguments need to be chosen, and
 * then the location for the resulting TextImp needs to be chosen.  It
 * also needs special code for the drawPrelim and wantArgs code.
 *
 * Therefore, we inherit from BaseConstructMode, and override the
 * event callbacks, so that this mode behaves like a
 * BaseConstructMode, until handleArgs is called.  After that, mresult
 * is no longer 0, and then the mode behaves in its own way, allowing
 * the user to choose a location for the new label object.
 */
class TestConstructMode
  : public BaseConstructMode
{
  const ArgsParserObjectType* mtype;
  ObjectCalcer::shared_ptr mresult;
public:
  TestConstructMode( KigPart& d, const ArgsParserObjectType* type );
  ~TestConstructMode();

  void handlePrelim( const std::vector<ObjectCalcer*>& os, const QPoint& p, KigPainter&, KigWidget& w );
  QString selectStatement( const std::vector<ObjectCalcer*>& args, const KigWidget& w );
  int isAlreadySelectedOK( const std::vector<ObjectCalcer*>&, const int& );
  int wantArgs( const std::vector<ObjectCalcer*>&, KigDocument& d, KigWidget& w );
  void handleArgs( const std::vector<ObjectCalcer*>& args, KigWidget& w );

  void leftClickedObject( ObjectHolder* o, const QPoint& p,
                          KigWidget& w, bool ctrlOrShiftDown );
  void midClicked( const QPoint& p, KigWidget& w );
  void rightClicked( const std::vector<ObjectHolder*>& oco, const QPoint& p, KigWidget& w );
  void mouseMoved( const std::vector<ObjectHolder*>& os, const QPoint& p, KigWidget& w, bool shiftpressed );
};

#endif
