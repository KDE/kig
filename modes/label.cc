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

#include "label.h"
#include "normal.h"

#include "textlabelwizard.h"
#include "linkslabel.h"

#include "../kig/kig_commands.h"
#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../misc/common.h"
#include "../misc/kigpainter.h"
#include "../objects/bogus_imp.h"
#include "../objects/curve_imp.h"
#include "../objects/object_factory.h"
#include "../objects/point_imp.h"
#include "../objects/text_imp.h"
#include "../objects/text_type.h"

#include <kcursor.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <klocale.h>
#include <qtextedit.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <qregexp.h>
#include <qpopupmenu.h>
#include <qcheckbox.h>

#include <algorithm>
#include <functional>

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
    d->wiz->setActiveWindow();
    break;
  default:
    break;
  };
}

void TextLabelModeBase::leftReleased( QMouseEvent* e, KigWidget* v )
{
  switch( d->mwawd )
  {
  case SelectingLocation:
  {
    if ( ( d->plc - e->pos() ).manhattanLength() > 4 ) return;
    setCoordinate( v->fromScreen( d->plc ) );
    break;
  }
  case RequestingText:
  case SelectingArgs:
    d->wiz->raise();
    d->wiz->setActiveWindow();
    break;
  case ReallySelectingArgs:
  {
    if ( ( d->plc - e->pos() ).manhattanLength() > 4 ) break;
    std::vector<ObjectHolder*> os = mdoc.document().whatAmIOn( v->fromScreen( d->plc ), *v );
    if ( os.empty() ) break;
    ObjectHolder* o = os[0];
    QPopupMenu* p = new QPopupMenu( v, "text_label_select_arg_popup" );
    p->insertItem( i18n( "Name" ), 0 );
    QCStringList l = o->imp()->properties();
    assert( l.size() == o->imp()->numberOfProperties() );
    for ( int i = 0; static_cast<uint>( i ) < l.size(); ++i )
    {
      QString s = i18n( l[i] );
      const char* iconfile = o->imp()->iconForProperty( i );
      int t;
      if ( iconfile && *iconfile )
      {
        QPixmap pix = mdoc.instance()->iconLoader()->loadIcon( iconfile, KIcon::User );
        t = p->insertItem( QIconSet( pix ), s, i + 1 );
      }
      else
      {
        t = p->insertItem( s, i + 1 );
      };
      assert( t == i + 1 );
    };
    int result = p->exec( v->mapToGlobal( d->plc ) );
    ObjectCalcer::shared_ptr argcalcer;
    if ( result == -1 ) break;
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
      assert( static_cast<uint>( result ) < l.size() + 1 );
      argcalcer = new ObjectPropertyCalcer( o->calcer(), result - 1 );
    }
    d->args[d->mwaaws] = argcalcer.get();
    argcalcer->calc( mdoc.document() );

    updateLinksLabel();
    updateWiz();
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
    if ( !os.empty() ) w->setCursor( KCursor::handCursor() );
    else w->setCursor( KCursor::arrowCursor() );
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
      w->setCursor( KCursor::handCursor() );
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
      w->setCursor( KCursor::crossCursor() );
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

static uint percentCount( const QString& s )
{
//  QRegExp re( QString::fromUtf8( "%[0-9]" ) );
  QRegExp re( QString::fromUtf8( "%[\\d]+" ) );
  int offset = 0;
  uint percentcount = 0;
  while ( ( offset = re.search( s, offset ) ) != -1 )
  {
    ++percentcount;
    offset += re.matchedLength();
  };
  return percentcount;
}

void TextLabelModeBase::finishPressed()
{
  bool needframe = d->wiz->needFrameCheckBox->isChecked();
  QString s = d->wiz->labelTextInput->text();

  assert( percentCount( s ) == d->args.size() );
  if ( d->wiz->currentPage() == d->wiz->enter_text_page )
    assert( d->args.size() == 0 );

  bool finished = true;
  for ( argvect::iterator i = d->args.begin(); i != d->args.end(); ++i )
    finished &= ( *i != 0 );

  if ( ! finished )
    KMessageBox::sorry( mdoc.widget(),
                        i18n( "There are '%n' parts in the text that you have not selected a "
                              "value for. Please remove them or select enough arguments." ) );
  else
  {
    finish( d->mcoord, s, d->args, needframe, d->locationparent );
    killMode();
  };
}

void TextLabelModeBase::updateWiz()
{
  QString s = d->wiz->labelTextInput->text();
  uint percentcount = percentCount( s );
  if ( d->lpc > percentcount )
  {
    d->args = argvect( d->args.begin(), d->args.begin() + percentcount );
  }
  else if ( d->lpc < percentcount )
  {
    d->args.resize( percentcount, 0 );
  };

  if ( percentcount == 0 && ! s.isEmpty() )
  {
    d->wiz->setNextEnabled( d->wiz->enter_text_page, false );
    d->wiz->setFinishEnabled( d->wiz->enter_text_page, true );
    d->wiz->setAppropriate( d->wiz->select_arguments_page, false );
  }
  else
  {
    d->wiz->setAppropriate( d->wiz->select_arguments_page, !s.isEmpty() );
    d->wiz->setNextEnabled( d->wiz->enter_text_page, ! s.isEmpty() );
    d->wiz->setFinishEnabled( d->wiz->enter_text_page, false );
    bool finished = true;
    for ( argvect::iterator i = d->args.begin(); i != d->args.end(); ++i )
      finished &= ( *i != 0 );
    assert( percentCount( s ) == d->args.size() );

    d->wiz->setFinishEnabled( d->wiz->select_arguments_page, finished );
  };

  d->lpc = percentcount;
}

void TextLabelModeBase::labelTextChanged()
{
  updateWiz();
}

void TextLabelModeBase::updateLinksLabel()
{
  LinksLabel::LinksLabelEditBuf buf = d->wiz->myCustomWidget1->startEdit();
  QString s = d->wiz->labelTextInput->text();
//  QRegExp re( "%[0-9]" );
  QRegExp re( "%[\\d]+" );
  int prevpos = 0;
  int pos = 0;
  uint count = 0;
  // we split up the string into text and "links"
  while ( ( pos = re.search( s, pos ) ) != -1 )
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
      d->wiz->myCustomWidget1->addText( subs, buf );
    };
    // we always need a link part...
    QString linktext( "%1" );
    assert( count < d->args.size() );
    if ( d->args[count] )
    {
      // if the user has already selected a property, then we show its
      // value...
      d->args[count]->imp()->fillInNextEscape( linktext, mdoc.document() );
    }
    else
      // otherwise, we show a stub...
      linktext = i18n( "argument %1" ).arg( count + 1 );

    d->wiz->myCustomWidget1->addLink( linktext, buf );
    // set pos and prevpos to the next char after the last match, so
    // we don't enter infinite loops...
//    pos += 2;
    pos += re.matchedLength();
    prevpos = pos;
    ++count;
  };

  if ( static_cast<uint>( prevpos ) != s.length() )
    d->wiz->myCustomWidget1->addText( s.mid( prevpos ), buf );

  d->wiz->myCustomWidget1->applyEdit( buf );
  d->wiz->relayoutArgsPage();

  d->wiz->resize( d->wiz->size() );
}

void TextLabelModeBase::linkClicked( int i )
{
  mdoc.widget()->setActiveWindow();
  mdoc.widget()->raise();

  assert( d->args.size() >= static_cast<uint>( i + 1 ) );

  d->mwawd = ReallySelectingArgs;
  d->mwaaws = i;

  mdoc.emitStatusBarText( i18n( "Selecting argument %1" ).arg( i + 1 ) );
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
    updateWiz();
    d->wiz->show();
    // shouldn't be necessary, but seems to be anyway.. :(
    updateWiz();
  };
}

void TextLabelModeBase::setText( const QString& s )
{
  d->wiz->labelTextInput->setText( s );
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

  mdoc.history()->addCommand( kc );
}

void TextLabelModeBase::setFrame( bool f )
{
  d->wiz->needFrameCheckBox->setChecked( f );
}

void TextLabelModeBase::setLocationParent( ObjectCalcer* o )
{
  d->locationparent = o;
}
