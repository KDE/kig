// label.cpp
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include "label.h"
#include "normal.h"

#include "textlabelwizard.h"
#include "linkslabel.h"

#include "../kig/kig_part.h"
#include "../kig/kig_view.h"
#include "../kig/kig_commands.h"
#include "../misc/i18n.h"
#include "../misc/common.h"
#include "../misc/kigpainter.h"
#include "../objects/object.h"
#include "../objects/object_factory.h"
#include "../objects/bogus_imp.h"
#include "../objects/curve_imp.h"
#include "../objects/point_imp.h"
#include "../objects/text_imp.h"
#include "../objects/text_type.h"

#include <kcursor.h>
#include <kmessagebox.h>
#include <kaction.h>
#include <qtextedit.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <qregexp.h>
#include <qpopupmenu.h>
#include <qcheckbox.h>

class TextLabelModeBase::Private
{
public:
  // point last clicked..
  QPoint plc;
  // the currently selected coordinate
  Coordinate mcoord;
  // the possible parent object that defines the location of the label..
  Object* locationparent;

  // the text is only kept in the text input widget, not here
  // QString mtext;

  // the property objects we'll be using as args, we keep a reference
  // to them in the args object, and keep a pointer to them ( or 0 )
  // in the correct order in args ( separately, because we can't use
  // the order of the parents of a ReferenceObject, and certainly
  // can't give 0 as a parent..
  ReferenceObject argsref;
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

TextLabelModeBase::TextLabelModeBase( KigDocument& doc )
  : KigMode( doc ), d( new Private )
{
  d->locationparent = 0;
  d->mwawd = SelectingLocation;
  d->wiz = new TextLabelWizard( doc.widgets()[0], this );
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
    Objects os = mdoc.whatAmIOn( v->fromScreen( d->plc ), *v );
    if ( os.empty() ) break;
    Object* o = os[0];
    QPopupMenu* p = new QPopupMenu( v, "text_label_select_arg_popup" );
    QCStringList l = o->properties();
    assert( l.size() == o->numberOfProperties() );
    for ( int i = 0; static_cast<uint>( i ) < l.size(); ++i )
    {
      QString s = i18n( l[i] );
      const char* iconfile = o->iconForProperty( i );
      int t;
      if ( iconfile && *iconfile )
      {
        QPixmap pix = KGlobal::iconLoader()->loadIcon( iconfile, KIcon::User );
        t = p->insertItem( QIconSet( pix ), s, i );
      }
      else
      {
        t = p->insertItem( s, i );
      };
      assert( t == i );
    };
    int result = p->exec( v->mapToGlobal( d->plc ) );
    if ( result == -1 ) break;
    assert( static_cast<uint>( result ) < l.size() );
    PropertyObject* n = new PropertyObject( o, result );
    if ( d->args[d->mwaaws] )
      d->argsref.delParent( d->args[d->mwaaws] );
    d->argsref.addParent( n );
    d->args[d->mwaaws] = n;

    n->calc( mdoc );
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
    Objects os = mdoc.whatAmIOn( w->fromScreen( e->pos() ), *w );
    if ( !os.empty() ) w->setCursor( KCursor::handCursor() );
    else w->setCursor( KCursor::arrowCursor() );
  }
  else if ( d->mwawd == SelectingLocation )
  {
    Objects os = mdoc.whatAmIOn( w->fromScreen( e->pos() ), *w );
    bool attachable = false;
    d->locationparent = 0;
    for ( Objects::iterator i = os.begin(); i != os.end(); ++i )
    {
      if( (*i)->hasimp( PointImp::stype() ) ||
          (*i)->hasimp( CurveImp::stype() ) )
      {
        attachable = true;
        d->locationparent = *i;
        break;
      };
    };
    w->updateCurPix();
    if ( attachable )
    {
      w->setCursor( KCursor::handCursor() );
      QString s = d->locationparent->imp()->type()->attachToThisStatement();
      mdoc.emitStatusBarText( s );

      KigPainter p( w->screenInfo(), &w->curPix, mdoc );

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
  QRegExp re( QString::fromUtf8( "%[0-9]" ) );
  int offset = 0;
  uint percentcount = 0;
  while ( ( offset = re.search( s, offset ) ) != -1 )
  {
    ++percentcount;
    offset += re.matchedLength();
  };
  return percentcount;
};

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
    // prevent deletion of the objects we still need..
    Objects keepos;
    for ( argvect::iterator i = d->args.begin();
          i != d->args.begin() + percentcount; ++i )
      if (*i)
        keepos.push_back( *i );
    ReferenceObject newref( keepos );
    d->argsref.clearParents();
    d->argsref.setParents( keepos );
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
  QRegExp re( "%[0-9]" );
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
      d->args[count]->imp()->fillInNextEscape( linktext, mdoc );
    }
    else
      // otherwise, we show a stub...
      linktext = i18n( "argument %1" ).arg( count + 1 );

    d->wiz->myCustomWidget1->addLink( linktext, buf );
    // set pos and prevpos to the next char after the last match, so
    // we don't enter infinite loops...
    pos += 2;
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

void TextLabelModeBase::redrawScreen()
{
  const std::vector<KigWidget*>& widgets = mdoc.widgets();
  for ( uint i = 0; i < widgets.size(); ++i )
  {
    KigWidget* w = widgets[i];
    w->redrawScreen();
    w->updateScrollBars();
  };
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
  Objects n( props.begin(), props.end() );
  d->argsref.setParents( n );
  d->args = props;
  for ( argvect::iterator i = d->args.begin(); i != d->args.end(); ++i )
    (*i)->calc( mdoc );
}

TextLabelConstructionMode::TextLabelConstructionMode( KigDocument& d )
  : TextLabelModeBase( d )
{
}

TextLabelConstructionMode::~TextLabelConstructionMode()
{
}

void TextLabelConstructionMode::finish(
  const Coordinate& coord, const QString& s,
  const argvect& props, bool needframe,
  Object* locationparent )
{
  Objects args( props.begin(), props.end() );
  Object* label = 0;
  if ( locationparent )
    label = ObjectFactory::instance()->attachedLabel( s, locationparent, coord, needframe, args, mdoc );
  else
    label = ObjectFactory::instance()->label( s, coord, needframe, args, mdoc );
  mdoc.addObject( label );
}

TextLabelRedefineMode::TextLabelRedefineMode( KigDocument& d, RealObject* label )
  : TextLabelModeBase( d ), mlabel( label )
{
  assert( label->hasimp( TextImp::stype() ) );
  Objects parents = label->parents();
  assert( parents.size() >= 3 );
  Objects firstthree( parents.begin(), parents.begin() + 3 );
  Objects rest( parents.begin() + 3, parents.end() );
  firstthree = TextType::instance()->argParser().parse( firstthree );

  assert( firstthree[0]->hasimp( IntImp::stype() ) );
  assert( firstthree[1]->hasimp( PointImp::stype() ) );
  assert( firstthree[2]->hasimp( StringImp::stype() ) );

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
    assert( rest[i]->inherits( Object::ID_PropertyObject ) );
    PropertyObject* o = static_cast<PropertyObject*>( rest[i] );
    PropertyObject* n = new PropertyObject( o->parent(), o->propId() );
    v.push_back( n );
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
  Object* locationparent )
{
  Objects parents = mlabel->parents();
  assert( parents.size() >= 3 );
  Objects firstthree( parents.begin(), parents.begin() + 3 );
  Objects rest( parents.begin() + 3, parents.end() );
  firstthree = TextType::instance()->argParser().parse( firstthree );

  KigCommand* kc = new KigCommand( mdoc, i18n( "Change Label" ) );
  MonitorDataObjects mon( firstthree );

  assert( firstthree[0]->hasimp( IntImp::stype() ) );
  assert( firstthree[1]->hasimp( PointImp::stype() ) );
  assert( firstthree[2]->hasimp( StringImp::stype() ) );

  assert( firstthree[0]->inherits( Object::ID_DataObject ) );
  assert( firstthree[1]->inherits( Object::ID_DataObject ) );
  assert( firstthree[2]->inherits( Object::ID_DataObject ) );
  static_cast<DataObject*>( firstthree[0] )->setImp( new IntImp( needframe ? 1 : 0 ) );

  // we don't do this, because
  // 1 this isn't necessarily a DataObject, we also support it to be a
  //   user-known point, or an internal constrained point..
  // 2 we don't know that we don't want it to become a user-known
  //   point or an internal constrained point, instead of a
  //   DataObject..
  // static_cast<DataObject*>( firstthree[1] )->setImp( new PointImp(
  // coord ) );

  static_cast<DataObject*>( firstthree[2] )->setImp( new StringImp( s ) );
  kc->addTask( mon.finish() );

  Objects oldparents = mlabel->parents();
  Objects p( props.begin(), props.end() );
  p.calc( mdoc );

  Objects np = firstthree;
  if ( locationparent && locationparent->hasimp( CurveImp::stype() ) )
  {
    double param = static_cast<const CurveImp*>( locationparent->imp() )->getParam( coord, mdoc );
    np[1] = ObjectFactory::instance()->constrainedPoint( locationparent, param );
    np[1]->setShown( false );
    np[1]->calc( mdoc );
  }
  else if ( locationparent )
  {
    assert( locationparent->hasimp( PointImp::stype() ) );
    np[1] = locationparent;
  }
  else
    np[1] = new DataObject( new PointImp( coord ) );

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

void TextLabelModeBase::setLocationParent( Object* o )
{
  d->locationparent = o;
}
