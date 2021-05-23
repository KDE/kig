// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "label.h"

#include "linkslabel.h"
#include "normal.h"
#include "textlabelwizard.h"

#include "../kig/kig_commands.h"
#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/common.h"
#include "../misc/kigpainter.h"
#include "../misc/calcpaths.h"
#include "../objects/bogus_imp.h"
#include "../objects/curve_imp.h"
#include "../objects/object_factory.h"
#include "../objects/point_imp.h"
#include "../objects/text_imp.h"
#include "../objects/text_type.h"

#include <QIcon>
#include <QAction>
#include <QEvent>
#include <QMenu>
#include <QRegExp>
#include <QVariant>
#include <QMouseEvent>

#include <KIconEngine>
#include <KCursor>
#include <QDebug>
#include <KMessageBox>

#include <algorithm>
#include <functional>
#include <iterator>

class TextLabelModeBase::Private
{
public:
  // point last clicked..
  QPoint plc;
  // the currently selected coordinate
  Coordinate mcoord;
  // the possible parent object that defines the location of the label..
  ObjectCalcer* locationparent;

  // the text is only kept in the text input widget, not here
  // QString mtext;

  // the property objects we'll be using as args, we keep a reference
  // to them in the args object, and keep a pointer to them ( or 0 )
  // in the correct order in args ( separately, because we can't use
  // the order of the parents of a ReferenceObject, and certainly
  // can't give 0 as a parent..
  argvect args;

  // if we're ReallySelectingArgs, then this var points to the arg
  // we're currently selecting...
  int mwaaws;

  // last percent count...
  uint lpc;

  TextLabelWizard* wiz;

  // What Are We Doing
  wawdtype mwawd;
};

TextLabelModeBase::~TextLabelModeBase()
{
  delete d->wiz;
  delete d;
}

TextLabelModeBase::TextLabelModeBase( KigPart& doc )
  : KigMode( doc ), d( new Private )
{
  d->locationparent = 0;
  d->lpc = 0;
  d->mwawd = SelectingLocation;
  d->wiz = new TextLabelWizard( doc.widget(), this );
}

void TextLabelModeBase::leftClicked( QMouseEvent* e, KigWidget* )
{
  d->plc = e->pos();
  switch( d->mwawd )
  {
  case RequestingText:
  case SelectingArgs:
    d->wiz->raise();
    d->wiz->activateWindow();
    break;
  default:
    break;
  };
}

void TextLabelModeBase::leftReleased( QMouseEvent* e, KigWidget* v )
{
  leftReleased( e, v, 0 );
}

/*
 * prevlabel is nonzero only if we are redefining an existing label
 * in which case we have to check that the new parents of the label
 * are not children of the label itself, thus causing circular dependence
 * in the object hierarchy
 */
void TextLabelModeBase::leftReleased( QMouseEvent* e, KigWidget* v, 
         ObjectTypeCalcer* prevlabel )
{
  switch( d->mwawd )
  {
  case SelectingLocation:
  {
    if ( ( d->plc - e->pos() ).manhattanLength() > 4 ) return;
    /*
     * in case we are redefining a text label:
     * we postpone the circular recursion check untill the
     * finish() method is called.  This is not the optimal
     * solution, since the user receives confusing feedback
     * from the cursor, which seemingly indicates that some
     * curve or point could be selected for the new position
     * although in the end a new fixed point will be created
     * if the circularity (isChild) test fails.
     */
    setCoordinate( v->fromScreen( d->plc ) );
    break;
  }
  case RequestingText:
  case SelectingArgs:
    d->wiz->raise();
    d->wiz->activateWindow();
    break;
  case ReallySelectingArgs:
  {
    if ( ( d->plc - e->pos() ).manhattanLength() > 4 ) break;
    std::vector<ObjectHolder*> os = mdoc.document().whatAmIOn( v->fromScreen( d->plc ), *v );
    if ( os.empty() ) break;
    ObjectHolder* o = os[0];
    /*
     * if prevlabel != 0 then we are redefining an existing label.
     * In this case it is important to check that the new arguments
     * are not children of the label
     */
    if ( prevlabel && isChild( o->calcer(), prevlabel ) ) break;
    QMenu p( v );
    p.setObjectName( QStringLiteral("text_label_select_arg_popup") );
    QAction* act = p.addAction( i18n( "Name" ) );
    act->setData( QVariant::fromValue( 0 ) );
    QByteArrayList l = o->imp()->properties();
    assert( l.size() == o->imp()->numberOfProperties() );
    for ( int i = 0; i < l.size(); ++i )
    {
      QString s = i18n( l[i] );
      const char* iconfile = o->imp()->iconForProperty( i );
      if ( iconfile && *iconfile )
      {
        act = p.addAction( QIcon( new KIconEngine( QLatin1String( iconfile ), mdoc.iconLoader() ) ), s );
      }
      else
      {
        act = p.addAction( s );
      };
      act->setData( QVariant::fromValue( i + 1 ) );
    };
    act = p.exec( v->mapToGlobal( d->plc ) );
    if ( !act ) break;
    int result = act->data().toInt();
    ObjectCalcer::shared_ptr argcalcer;
    if ( result < 0 ) break;
    else if ( result == 0 )
    {
      argcalcer = o->nameCalcer();
      if ( !argcalcer )
      {
        ObjectConstCalcer* c = new ObjectConstCalcer( new StringImp( i18n( "<unnamed object>" ) ) );
        o->setNameCalcer( c );
        argcalcer = c;
      }
    }
    else
    {
      assert( result < l.size() + 1 );
//      argcalcer = new ObjectPropertyCalcer( o->calcer(), result - 1 );
      argcalcer = new ObjectPropertyCalcer( o->calcer(), result - 1, true );
    }
    d->args[d->mwaaws] = argcalcer.get();
    argcalcer->calc( mdoc.document() );

    updateLinksLabel();
    break;
  }
  default:
    assert( false );
    break;
  };
}

void TextLabelModeBase::killMode()
{
  mdoc.doneMode( this );
}

void TextLabelModeBase::cancelConstruction()
{
  killMode();
}

void TextLabelModeBase::enableActions()
{
  KigMode::enableActions();

  mdoc.aCancelConstruction->setEnabled( true );
}

void TextLabelModeBase::mouseMoved( QMouseEvent* e, KigWidget* w )
{
  if ( d->mwawd == ReallySelectingArgs )
  {
    std::vector<ObjectHolder*> os = mdoc.document().whatAmIOn( w->fromScreen( e->pos() ), *w );
    if ( !os.empty() ) w->setCursor( Qt::PointingHandCursor );
    else w->setCursor( Qt::ArrowCursor );
  }
  else if ( d->mwawd == SelectingLocation )
  {
    std::vector<ObjectHolder*> os = mdoc.document().whatAmIOn( w->fromScreen( e->pos() ), *w );
    bool attachable = false;
    d->locationparent = 0;
    for ( std::vector<ObjectHolder*>::iterator i = os.begin(); i != os.end(); ++i )
    {
      if( (*i)->imp()->attachPoint().valid() ||
          (*i)->imp()->inherits( PointImp::stype() ) ||
          (*i)->imp()->inherits( CurveImp::stype() ) )
      {
        attachable = true;
        d->locationparent = (*i)->calcer();
        break;
      };
    };
    w->updateCurPix();
    if ( attachable )
    {
      w->setCursor( Qt::PointingHandCursor );
      QString s = d->locationparent->imp()->type()->attachToThisStatement();
      mdoc.emitStatusBarText( s );

      KigPainter p( w->screenInfo(), &w->curPix, mdoc.document() );

      // set the text next to the arrow cursor
      QPoint point = e->pos();
      point.setX(point.x()+15);

      p.drawTextStd( point, s );
      w->updateWidget( p.overlay() );
    }
    else
    {
      w->setCursor( Qt::CrossCursor );
      mdoc.emitStatusBarText( 0 );
      w->updateWidget();
    };
  }
}

void TextLabelModeBase::enterTextPageEntered()
{
}

void TextLabelModeBase::selectArgumentsPageEntered()
{
  updateLinksLabel();
}

void TextLabelModeBase::cancelPressed()
{
  cancelConstruction();
}

// also used in textlabelwizard.cc
uint percentCount( const QString& s )
{
//  QRegExp re( QString::fromUtf8( "%[0-9]" ) );
  QRegExp re( QLatin1String( "%[\\d]+" ) );
  int offset = 0;
  uint percentcount = 0;
  while ( ( offset = re.indexIn( s, offset ) ) != -1 )
  {
    ++percentcount;
    offset += re.matchedLength();
  };
  return percentcount;
}

bool TextLabelModeBase::canFinish()
{
  bool finish = true;
  QString s = d->wiz->text();

  assert( percentCount( s ) == d->args.size() );
  if ( d->wiz->currentId() == TextLabelWizard::TextPageId )
    assert( d->args.size() == 0 );

  for ( argvect::iterator i = d->args.begin(); i != d->args.end(); ++i )
    finish = finish && ( *i != 0 );

  if ( ! finish )
  {
    KMessageBox::sorry( mdoc.widget(),
                        i18n( "There are '%n' parts in the text that you have not selected a "
                              "value for. Please remove them or select enough arguments." ) );
  };

  return finish;
}

void TextLabelModeBase::finishPressed()
{
  bool needframe = d->wiz->field( QStringLiteral("wantframe") ).toBool();
  QString s = d->wiz->text();

  finish( d->mcoord, s, d->args, needframe, d->locationparent );
  killMode();
}

bool TextLabelModeBase::percentCountChanged( uint percentcount )
{
  bool finish = true;
  if ( d->lpc > percentcount )
  {
    d->args = argvect( d->args.begin(), d->args.begin() + percentcount );
  }
  else if ( d->lpc < percentcount )
  {
    d->args.resize( percentcount, 0 );
  };

  if ( percentcount != 0 )
  {
    bool finished = true;
    for ( argvect::iterator i = d->args.begin(); i != d->args.end(); ++i )
      finished &= ( *i != 0 );
    assert( percentcount == d->args.size() );
    finish = finished;
  };

  d->lpc = percentcount;

  return finish;
}

void TextLabelModeBase::updateLinksLabel()
{
  LinksLabel::LinksLabelEditBuf buf = d->wiz->linksLabel()->startEdit();
  QString s = d->wiz->text();
//  QRegExp re( "%[0-9]" );
  QRegExp re( "%[\\d]+" );
  int prevpos = 0;
  int pos = 0;
  uint count = 0;
  // we split up the string into text and "links"
  while ( ( pos = re.indexIn( s, pos ) ) != -1 )
  {
    // prevpos is the first character after the last match, pos is the
    // first char of the current match..
    if ( prevpos != pos )
    {
      // there is a text part between the previous and the current
      // "link"...
      assert( prevpos < pos );
      // fetch the text part...
      QString subs = s.mid( prevpos, pos - prevpos );
      // and add it...
      d->wiz->linksLabel()->addText( subs, buf );
    };
    // we always need a link part...
    QString linktext( QStringLiteral("%1") );
    assert( count < d->args.size() );
    if ( d->args[count] )
    {
      // if the user has already selected a property, then we show its
      // value...
      d->args[count]->imp()->fillInNextEscape( linktext, mdoc.document() );
    }
    else
      // otherwise, we show a stub...
      linktext = i18n( "argument %1", count + 1 );

    d->wiz->linksLabel()->addLink( linktext, buf );
    // set pos and prevpos to the next char after the last match, so
    // we don't enter infinite loops...
//    pos += 2;
    pos += re.matchedLength();
    prevpos = pos;
    ++count;
  };

  if ( prevpos != s.length() )
    d->wiz->linksLabel()->addText( s.mid( prevpos ), buf );

  d->wiz->linksLabel()->applyEdit( buf );

  d->wiz->resize( d->wiz->size() );
}

void TextLabelModeBase::linkClicked( int i )
{
  mdoc.widget()->activateWindow();
  mdoc.widget()->raise();

  assert( d->args.size() >= static_cast<uint>( i + 1 ) );

  d->mwawd = ReallySelectingArgs;
  d->mwaaws = i;

  mdoc.emitStatusBarText( i18n( "Selecting argument %1", i + 1 ) );
}

void TextLabelModeBase::redrawScreen( KigWidget* w )
{
  w->redrawScreen( std::vector<ObjectHolder*>() );
  w->updateScrollBars();
}

void TextLabelModeBase::setCoordinate( const Coordinate& coord )
{
  d->mcoord = coord;
  if ( d->mwawd == SelectingLocation )
  {
    d->mwawd = RequestingText;
    d->wiz->show();
  };
}

void TextLabelModeBase::setText( const QString& s )
{
  d->wiz->setText( s );
}

void TextLabelModeBase::setPropertyObjects( const argvect& props )
{
  d->args = props;
  for ( argvect::iterator i = d->args.begin(); i != d->args.end(); ++i )
    (*i)->calc( mdoc.document() );
}

TextLabelConstructionMode::TextLabelConstructionMode( KigPart& d )
  : TextLabelModeBase( d )
{
}

TextLabelConstructionMode::~TextLabelConstructionMode()
{
}

void TextLabelConstructionMode::finish(
  const Coordinate& coord, const QString& s,
  const argvect& props, bool needframe,
  ObjectCalcer* locationparent )
{
  std::vector<ObjectCalcer*> args;
  for ( argvect::const_iterator i = props.begin();
        i != props.end(); ++i )
    args.push_back( i->get() );

  ObjectHolder* label = 0;
  if ( locationparent )
    label = ObjectFactory::instance()->attachedLabel( s, locationparent, coord, needframe, args, mdoc.document() );
  else
    label = ObjectFactory::instance()->label( s, coord, needframe, args, mdoc.document() );
  mdoc.addObject( label );
}

TextLabelRedefineMode::TextLabelRedefineMode( KigPart& d, ObjectTypeCalcer* label )
  : TextLabelModeBase( d ), mlabel( label )
{
  assert( label->imp()->inherits( TextImp::stype() ) );
  std::vector<ObjectCalcer*> parents = label->parents();
  assert( parents.size() >= 3 );
  std::vector<ObjectCalcer*> firstthree( parents.begin(), parents.begin() + 3 );
  std::vector<ObjectCalcer*> rest( parents.begin() + 3, parents.end() );
  firstthree = TextType::instance()->argParser().parse( firstthree );

  assert( firstthree[0]->imp()->inherits( IntImp::stype() ) );
  assert( firstthree[1]->imp()->inherits( PointImp::stype() ) );
  assert( firstthree[2]->imp()->inherits( StringImp::stype() ) );

  bool frame = static_cast<const IntImp*>( firstthree[0]->imp() )->data() != 0;
  Coordinate coord = static_cast<const PointImp*>( firstthree[1]->imp() )->coordinate();
  QString text = static_cast<const StringImp*>( firstthree[2]->imp() )->data();

  // don't set it, let the user redefine it..
//  setCoordinate( coord );
  setText( text );
  setFrame( frame );

  argvect v;
  for ( uint i = 0; i < rest.size(); ++i )
  {
    v.push_back( rest[i] );
  };
  assert( v.size() == rest.size() );

  setPropertyObjects( v );
}

TextLabelRedefineMode::~TextLabelRedefineMode()
{
}

void TextLabelRedefineMode::leftReleased( QMouseEvent* e, KigWidget* v )
{
  TextLabelModeBase::leftReleased( e, v, mlabel );
}

void TextLabelRedefineMode::finish(
  const Coordinate& coord, const QString& s,
  const argvect& props, bool needframe,
  ObjectCalcer* locationparent )
{
  std::vector<ObjectCalcer*> parents = mlabel->parents();
  assert( parents.size() >= 3 );
  std::vector<ObjectCalcer*> firstthree( parents.begin(), parents.begin() + 3 );
  std::vector<ObjectCalcer*> rest( parents.begin() + 3, parents.end() );
  firstthree = TextType::instance()->argParser().parse( firstthree );

  KigCommand* kc = new KigCommand( mdoc, i18n( "Change Label" ) );
  MonitorDataObjects mon( firstthree );

  assert( firstthree[0]->imp()->inherits( IntImp::stype() ) );
  assert( firstthree[1]->imp()->inherits( PointImp::stype() ) );
  assert( firstthree[2]->imp()->inherits( StringImp::stype() ) );

  assert( dynamic_cast<ObjectConstCalcer*>( firstthree[0] ) );
  assert( dynamic_cast<ObjectConstCalcer*>( firstthree[2] ) );
  static_cast<ObjectConstCalcer*>( firstthree[0] )->setImp( new IntImp( needframe ? 1 : 0 ) );

  // we don't do this, because
  // 1 this isn't necessarily a DataObject, we also support it to be a
  //   user-known point, or an internal constrained point..
  // 2 we don't know that we don't want it to become a user-known
  //   point or an internal constrained point, instead of a
  //   DataObject..
  // static_cast<DataObject*>( firstthree[1] )->setImp( new PointImp(
  // coord ) );

  static_cast<ObjectConstCalcer*>( firstthree[2] )->setImp( new StringImp( s ) );
  mon.finish( kc );

  std::vector<ObjectCalcer*> oldparents = mlabel->parents();
  std::vector<ObjectCalcer*> p;
  for ( argvect::const_iterator i = props.begin();
        i != props.end(); ++i )
    p.push_back( i->get() );
  for ( std::vector<ObjectCalcer*>::iterator i = p.begin();
        i != p.end(); ++i )
    ( *i )->calc( mdoc.document() );

  std::vector<ObjectCalcer*> np = firstthree;
  /*
   * this avoids circular recursion in case the location parent
   * is actually a child of this label
   */
  if ( locationparent && isChild( locationparent, mlabel ) )
    locationparent = 0;
  /*
   * take advantage of the method "getAttachPoint" that should
   * do all the work; it is also used when creating a new label
   */
  np[1] = ObjectFactory::instance()->getAttachPoint( locationparent, coord, mdoc.document() );

/* this is the old code, just in case... */
//  if ( locationparent && locationparent->imp()->inherits( CurveImp::stype() ) )
//  {
//    double param = static_cast<const CurveImp*>( locationparent->imp() )->getParam( coord, mdoc.document() );
//    np[1] = ObjectFactory::instance()->constrainedPointCalcer( locationparent, param );
//    np[1]->calc( mdoc.document() );
//  }
//  else if ( locationparent )
//  {
//    assert( locationparent->imp()->inherits( PointImp::stype() ) );
//    np[1] = locationparent;
//  }
//  else
//    np[1] = new ObjectConstCalcer( new PointImp( coord ) );

  copy( p.begin(), p.end(), back_inserter( np ) );

  kc->addTask(
    new ChangeParentsAndTypeTask(
      mlabel, np, TextType::instance() ) );

  mdoc.history()->push( kc );
}

void TextLabelModeBase::setFrame( bool f )
{
  d->wiz->setField( QStringLiteral("wantframe"), f );
}

void TextLabelModeBase::setLocationParent( ObjectCalcer* o )
{
  d->locationparent = o;
}

NumericLabelMode::NumericLabelMode( KigPart& doc )
  : KigMode( doc )
{
}

NumericLabelMode::~NumericLabelMode()
{
}

void NumericLabelMode::leftClicked( QMouseEvent* e, KigWidget* )
{
  mplc = e->pos();
}

void NumericLabelMode::leftReleased( QMouseEvent* e, KigWidget* v )
{
  if ( ( mplc - e->pos() ).manhattanLength() > 4 ) return;

  bool ok;
  double val = getDoubleFromUser(
      i18n( "Set Value" ), i18n( "Enter value:" ),
      0.0, v, &ok, -2147483647, 2147483647, 7 );

  if ( !ok )
  {
    cancelConstruction();
    return;
  }
  Coordinate loc = v->fromScreen( mplc );
  ObjectHolder* p = ObjectFactory::instance()->numericValue( val, loc, mdoc.document() );
  p->calc( mdoc.document() );
  mdoc.addObject( p );
  killMode();
}

void NumericLabelMode::killMode()
{
  mdoc.doneMode( this );
}

void NumericLabelMode::cancelConstruction()
{
  killMode();
}

void NumericLabelMode::enableActions()
{
  KigMode::enableActions();

  mdoc.aCancelConstruction->setEnabled( true );

  mdoc.emitStatusBarText( i18n( "Select the position for the new numeric value..." ) );
}

void NumericLabelMode::mouseMoved( QMouseEvent*, KigWidget* w )
{
  w->setCursor( Qt::CrossCursor );
}

void NumericLabelMode::redrawScreen( KigWidget* w )
{
  w->redrawScreen( std::vector<ObjectHolder*>() );
  w->updateScrollBars();
}
