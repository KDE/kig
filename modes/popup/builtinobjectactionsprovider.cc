/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2012 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "builtinobjectactionsprovider.h"

#include "popup.h"
#include "popupactionprovider.h"

#include "../../kig/kig_commands.h"
#include "../../kig/kig_part.h"
#include "../../kig/kig_view.h"
#include "../../misc/kigpainter.h"
#include "../../objects/line_imp.h"
#include "../../modes/moving.h"
#include "../../modes/normal.h"
#include "../../objects/object_drawer.h"
#include "../../objects/point_imp.h"

#include <QColorDialog>
#include <QPen>
#include <QRect>

#include <KIconEngine>

struct color_struct
{
  const Qt::GlobalColor color;
  const char* name;
};

static const color_struct colors[] =
{
  { Qt::black, I18N_NOOP( "Black" ) },
  { Qt::gray, I18N_NOOP( "Gray" ) },
  { Qt::red, I18N_NOOP( "Red" ) },
  { Qt::green, I18N_NOOP( "Green" ) },
  { Qt::cyan, I18N_NOOP( "Cyan" ) },
  { Qt::yellow, I18N_NOOP( "Yellow" ) },
  { Qt::darkRed, I18N_NOOP( "Dark Red" ) }
};

const int numberofcolors = 7; // is there a better way to calc that?

void BuiltinObjectActionsProvider::fillUpMenu( NormalModePopupObjects& popup, int menu, int& nextfree )
{
  KIconLoader* l = popup.part().iconLoader();
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    std::vector<ObjectHolder*> os = popup.objects();

    /*
     * mp: we want the "show" action to be visible only
     * if we selected only one object (to be conservative)
     * and if that object is currently hidden.
     * conversely for one hidden object we don't want
     * the "hide" action to be inserted.
     * in any case we have a fixed 'id' associated
     * with the two actions.
     */

    if ( os.size() > 1 || os.front()->shown() )
    {
      popup.addInternalAction( menu, i18n( "&Hide" ), nextfree );
    }
    if ( os.size() == 1 && !os.front()->shown() )
    {
      popup.addInternalAction( menu, i18n( "&Show" ), nextfree+1 );
    }
    nextfree += 2;
    popup.addInternalAction( menu, QIcon( new KIconEngine( "transform-move", l ) ), i18n( "&Move" ), nextfree++ );
    popup.addInternalAction( menu, QIcon( new KIconEngine( "edit-delete", l ) ), i18n( "&Delete" ), nextfree++ );
  }
  else if ( menu == NormalModePopupObjects::SetColorMenu )
  {
    QPixmap p( 20, 20 );
    for( int i = 0; i < numberofcolors; i++ )
    {
      p.fill( QColor( colors[i].color ) );
      popup.addInternalAction( menu, QIcon( p ), i18n( colors[i].name ), nextfree++ );
    }
    popup.addInternalAction( menu, QIcon( new KIconEngine( "color", l ) ), i18n( "&Custom Color" ), nextfree++ );
  }
  else if ( menu == NormalModePopupObjects::SetSizeMenu && !popup.onlyLabels() )
  {
    bool point = true;
    bool samecolor = true;
    std::vector<ObjectHolder*> os = popup.objects();
    QColor color = os.front()->drawer()->color();
    for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
    {
      if ( ! (*i)->imp()->inherits( PointImp::stype() ) )
        point = false;
      if ( (*i)->drawer()->color() != color ) samecolor = false;
    };
    if ( ! samecolor ) color = Qt::blue;
    QPixmap p( 20, 20 );
    for ( int i = 1; i < 8; ++i )
    {
      p.fill( popup.palette().color( popup.backgroundRole() ) );
      QPainter ptr( &p );
      ptr.setPen( QPen( color, 1 ) );
      ptr.setBrush( QBrush( color, Qt::SolidPattern ) );
      if ( point )
      {
        int size = 2*i;
        QRect r( ( 20 - size ) / 2,  ( 20 - size ) / 2, size, size );
        ptr.drawEllipse( r );
      }
      else
      {
        ptr.setPen( QPen( color, -1 + 2*i ) );
        ptr.drawLine( QPoint( 0, 10 ), QPoint( 50, 10 ) );
      };
      ptr.end();
      popup.addInternalAction( menu, QIcon( p ), nextfree++ );
    };
  }
  else if ( menu == NormalModePopupObjects::SetStyleMenu && !popup.onlyLabels() )
  {
    bool samecolor = true;
    int npoints = 0;
    int nothers = 0;
    std::vector<ObjectHolder*> os = popup.objects();
    QColor color = os.front()->drawer()->color();
    for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
    {
      if ( (*i)->imp()->inherits( PointImp::stype() ) )
        npoints++;
      else
        nothers++;
      if ( (*i)->drawer()->color() != color ) samecolor = false;
    };
    bool point = ( npoints > nothers );
    if ( ! samecolor ) color = Qt::blue;
    if ( point )
    {
      for ( int i = 0; i < (int)Kig::NumberOfPointStyles; ++i )
      {
        QPixmap p( 20, 20 );
        p.fill( popup.palette().color( popup.backgroundRole() ) );
        ScreenInfo si( Rect( -1, -1, 2, 2 ), p.rect() );
        KigPainter ptr( si, &p, popup.part().document(), false );
        PointImp pt( Coordinate( 0, 0 ) );
        ObjectDrawer d( color, -1, true, Qt::SolidLine, (Kig::PointStyle)i );
        d.draw( pt, ptr, false );
        popup.addInternalAction( menu, QIcon( p ), nextfree++ );
      }
    }
    else
    {
      Qt::PenStyle penstyles[] = {Qt::SolidLine, Qt::DashLine, Qt::DashDotLine, Qt::DashDotDotLine, Qt::DotLine};
      for ( int i = 0; i < (int) ( sizeof( penstyles ) / sizeof( Qt::PenStyle ) ); ++i )
      {
        QPixmap p( 20, 20 );
        p.fill( popup.palette().color( popup.backgroundRole() ) );
        ScreenInfo si( Rect( -2.5, -1, 2.5, 1 ), p.rect() );
        KigPainter ptr( si, &p, popup.part().document(), false );
        LineImp line( Coordinate( -1, 0 ), Coordinate( 1, 0 ) );
        Qt::PenStyle ps = penstyles[i];
        ObjectDrawer d( color, 2, true, ps, Kig::RoundEmpty );
        d.draw( line, ptr, false );
        popup.addInternalAction( menu, QIcon( p ), nextfree++ );
      };
    }
  }
}

bool BuiltinObjectActionsProvider::executeAction(
  int menu, int& id, const std::vector<ObjectHolder*>& os, NormalModePopupObjects& popup,
  KigPart& doc, KigWidget& w, NormalMode& mode )
{
  if ( menu == NormalModePopupObjects::ToplevelMenu )
  {
    if ( id > 3 )
    {
      id -= 4;
      return false;
    };
    switch( id )
    {
    case 0:
      // hide the objects..
      doc.hideObjects( os );
      break;
    case 1:
      // show the objects..
      doc.showObjects( os );
      break;
    case 2:
    {
      // move
      QCursor::setPos( popup.mapToGlobal( QPoint( 0, 0 ) ) );
      QPoint p = w.mapFromGlobal( QCursor::pos() );
      Coordinate c = w.fromScreen( p );
      MovingMode m( os, c, w, doc );
      doc.runMode( &m );
      // in this case, we return, cause we don't want objects to be
      // unselected... ( or maybe we do ? )
      return true;
    }
    case 3:
      // delete
      doc.delObjects( os );
      break;
    default: assert( false );
    };
    mode.clearSelection();
    return true;
  }
  else if ( menu == NormalModePopupObjects::SetColorMenu )
  {
    if ( id >= numberofcolors + 1 )
    {
      id -= numberofcolors + 1;
      return false;
    };
    QColor color;
    if ( id < numberofcolors )
      color = QColor( colors[id].color );
    else
    {
      if ( os.size() == 1 )
        color = os.front()->drawer()->color();
      color = QColorDialog::getColor( color, &w );
      if ( !color.isValid() ) return true;
    }
    KigCommand* kc = new KigCommand( doc, i18n( "Change Object Color" ) );
    assert( color.isValid() );
    for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
      kc->addTask( new ChangeObjectDrawerTask( *i, ( *i )->drawer()->getCopyColor( color ) ) );
    doc.history()->push( kc );
    return true;
  }
  else if ( menu == NormalModePopupObjects::SetSizeMenu )
  {
    if ( id >= 7 )
    {
      id -= 7;
      return false;
    };

    KigCommand* kc = new KigCommand( doc, i18n( "Change Object Width" ) );
    for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
      kc->addTask( new ChangeObjectDrawerTask( *i, ( *i )->drawer()->getCopyWidth( 1 + 2 * id ) ) );
    doc.history()->push( kc );
    return true;
  }
  else if ( menu == NormalModePopupObjects::SetStyleMenu )
  {
    int npoints = 0;
    int nothers = 0;
    for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
    {
      if ( (*i)->imp()->inherits( PointImp::stype() ) )
        npoints++;
      else
        nothers++;
    };
    if ( id >= Kig::NumberOfPointStyles )
    {
      id -= Kig::NumberOfPointStyles;
      return false;
    };

    if ( npoints > nothers )
    {
      KigCommand* kc = new KigCommand( doc, i18n( "Change Point Style" ) );
      for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
        if ( (*i)->imp()->inherits( PointImp::stype() ) )
          kc->addTask( new ChangeObjectDrawerTask( *i, ( *i )->drawer()->getCopyPointStyle( (Kig::PointStyle)id ) ) );
      doc.history()->push( kc );
      return true;
    }
    else
    {
      Qt::PenStyle penstyles[] = {Qt::SolidLine, Qt::DashLine, Qt::DashDotLine, Qt::DashDotDotLine, Qt::DotLine};
      assert( id < (int)( sizeof( penstyles ) / sizeof( Qt::PenStyle ) ) );
      Qt::PenStyle p = penstyles[id];
      KigCommand* kc = new KigCommand( doc, i18n( "Change Object Style" ) );
      for ( std::vector<ObjectHolder*>::const_iterator i = os.begin(); i != os.end(); ++i )
        if ( ! (*i)->imp()->inherits( PointImp::stype() ) )
          kc->addTask( new ChangeObjectDrawerTask( *i, ( *i )->drawer()->getCopyStyle( p ) ) );
      doc.history()->push( kc );
    }
    return true;
  }
  else return false;
}


