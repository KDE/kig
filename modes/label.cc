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
#include "../objects/object_factory.h"
#include "../objects/bogus_imp.h"
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

TextLabelModeBase::~TextLabelModeBase()
{
  delete mwiz;
  delete_all( margs.begin(), margs.end() );
}

TextLabelModeBase::TextLabelModeBase( KigDocument& d )
  : KigMode( d ), mlpc( 0 ), mwiz( 0 ), mwawd( SelectingLocation )
{
  const std::vector<KigWidget*>& widgets = mdoc.widgets();
  for ( uint i = 0; i < widgets.size(); ++i )
  {
    KigWidget* w = widgets[i];
    w->setCursor( KCursor::crossCursor() );
  };
  mwiz = new TextLabelWizard( d.widgets()[0], this );
}

void TextLabelModeBase::leftClicked( QMouseEvent* e, KigWidget* )
{
  mplc = e->pos();
  switch( mwawd )
  {
  case RequestingText:
  case SelectingArgs:
    mwiz->raise();
    mwiz->setActiveWindow();
    break;
  default:
    break;
  };
}

void TextLabelModeBase::leftReleased( QMouseEvent* e, KigWidget* v )
{
  switch( mwawd )
  {
  case SelectingLocation:
  {
    if ( ( mplc - e->pos() ).manhattanLength() > 4 ) return;
    setCoordinate( v->fromScreen( mplc ) );
    break;
  }
  case RequestingText:
  case SelectingArgs:
    mwiz->raise();
    mwiz->setActiveWindow();
    break;
  case ReallySelectingArgs:
  {
    if ( ( mplc - e->pos() ).manhattanLength() > 4 ) break;
    Objects os = mdoc.whatAmIOn( v->fromScreen( mplc ), *v );
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
    int result = p->exec( v->mapToGlobal( mplc ) );
    if ( result == -1 ) break;
    assert( static_cast<uint>( result ) < l.size() );
    delete margs[mwaaws];
    margs[mwaaws] = new PropertyObject( o, result );
    margs[mwaaws]->calc( mdoc );
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
  if ( mwawd == ReallySelectingArgs )
  {
    Objects os = mdoc.whatAmIOn( w->fromScreen( e->pos() ), *w );
    if ( !os.empty() ) w->setCursor( KCursor::handCursor() );
    else w->setCursor( KCursor::arrowCursor() );
  };
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
  bool needframe = mwiz->needFrameCheckBox->isChecked();
  QString s = mwiz->labelTextInput->text();

  assert( percentCount( s ) == margs.size() );
  if ( mwiz->currentPage() == mwiz->enter_text_page )
    assert( margs.size() == 0 );

  bool finished = true;
  for ( argvect::iterator i = margs.begin(); i != margs.end(); ++i )
  {
    finished &= ( *i != 0 );
  };

  if ( ! finished )
    KMessageBox::sorry( mdoc.widget(),
                        i18n( "There are '%n' parts in the text that you have not selected a "
                              "value for. Please remove them or select enough arguments." ) );
  else
  {
    finish( mcoord, s, margs, needframe );
    margs.clear();
    killMode();
  };
}

void TextLabelModeBase::updateWiz()
{
  QString s = mwiz->labelTextInput->text();
  uint percentcount = percentCount( s );
  if ( mlpc > percentcount )
  {
    delete_all( margs.begin() + percentcount, margs.end() );
    margs.clear();
    margs.resize( percentcount );
  }
  else if ( mlpc < percentcount )
  {
    margs.resize( percentcount, 0 );
  };

  if ( percentcount == 0 && ! s.isEmpty() )
  {
    mwiz->setNextEnabled( mwiz->enter_text_page, false );
    mwiz->setFinishEnabled( mwiz->enter_text_page, true );
    mwiz->setAppropriate( mwiz->select_arguments_page, false );
  }
  else
  {
    mwiz->setAppropriate( mwiz->select_arguments_page, !s.isEmpty() );
    mwiz->setNextEnabled( mwiz->enter_text_page, ! s.isEmpty() );
    mwiz->setFinishEnabled( mwiz->enter_text_page, false );
    bool finished = true;
    for ( argvect::iterator i = margs.begin(); i != margs.end(); ++i )
    {
      finished &= ( *i != 0 );
    };
    assert( percentCount( s ) == margs.size() );

    mwiz->setFinishEnabled( mwiz->select_arguments_page, finished );
  };

  mlpc = percentcount;
}

void TextLabelModeBase::labelTextChanged()
{
  updateWiz();
}

void TextLabelModeBase::updateLinksLabel()
{
  LinksLabel::LinksLabelEditBuf buf = mwiz->myCustomWidget1->startEdit();
  QString s = mwiz->labelTextInput->text();
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
      mwiz->myCustomWidget1->addText( subs, buf );
    };
    // we always need a link part...
    QString linktext( "%1" );
    assert( count < margs.size() );
    if ( margs[count] )
    {
      // if the user has already selected a property, then we show its
      // value...
      margs[count]->imp()->fillInNextEscape( linktext, mdoc );
    }
    else
      // otherwise, we show a stub...
      linktext = i18n( "argument %1" ).arg( count + 1 );

    mwiz->myCustomWidget1->addLink( linktext, buf );
    // set pos and prevpos to the next char after the last match, so
    // we don't enter infinite loops...
    pos += 2;
    prevpos = pos;
    ++count;
  };

  if ( static_cast<uint>( prevpos ) != s.length() )
    mwiz->myCustomWidget1->addText( s.mid( prevpos ), buf );

  mwiz->myCustomWidget1->applyEdit( buf );
  mwiz->relayoutArgsPage();

  mwiz->resize( mwiz->size() );
}

void TextLabelModeBase::linkClicked( int i )
{
  mdoc.widget()->setActiveWindow();
  mdoc.widget()->raise();

  assert( margs.size() >= static_cast<uint>( i + 1 ) );

  mwawd = ReallySelectingArgs;
  mwaaws = i;

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
  mcoord = coord;
  if ( mwawd == SelectingLocation )
  {
    mwawd = RequestingText;
    updateWiz();
    mwiz->show();
    // shouldn't be necessary, but seems to be anyway.. :(
    updateWiz();
  };
}

void TextLabelModeBase::setText( const QString& s )
{
  mwiz->labelTextInput->setText( s );
}

void TextLabelModeBase::setPropertyObjects( const argvect& props )
{
  delete_all( margs.begin(), margs.end() );
  margs = props;
  for ( argvect::iterator i = margs.begin(); i != margs.end(); ++i )
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
  const argvect& props, bool needframe )
{
  Objects args( props.begin(), props.end() );
  Objects labelos = ObjectFactory::instance()->label( s, coord, needframe, args );
  labelos.calc( mdoc );
  copy( labelos.begin(), labelos.end(), back_inserter( args ) );
  mdoc.addObjects( args );
}

TextLabelRedefineMode::TextLabelRedefineMode( KigDocument& d, RealObject* label )
  : TextLabelModeBase( d ), mlabel( label )
{
  assert( label->hasimp( ObjectImp::ID_TextImp ) );
  Objects parents = label->parents();
  assert( parents.size() >= 3 );
  Objects firstthree( parents.begin(), parents.begin() + 3 );
  Objects rest( parents.begin() + 3, parents.end() );
  firstthree = TextType::instance()->argParser().parse( firstthree );

  assert( firstthree[0]->hasimp( ObjectImp::ID_IntImp ) );
  assert( firstthree[1]->hasimp( ObjectImp::ID_PointImp ) );
  assert( firstthree[2]->hasimp( ObjectImp::ID_StringImp ) );

  bool frame = static_cast<const IntImp*>( firstthree[0]->imp() )->data() != 0;
  Coordinate coord = static_cast<const PointImp*>( firstthree[1]->imp() )->coordinate();
  QString text = static_cast<const StringImp*>( firstthree[2]->imp() )->data();

  setCoordinate( coord );
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
  const argvect& props, bool needframe )
{
  Objects parents = mlabel->parents();
  assert( parents.size() >= 3 );
  Objects firstthree( parents.begin(), parents.begin() + 3 );
  Objects rest( parents.begin() + 3, parents.end() );
  firstthree = TextType::instance()->argParser().parse( firstthree );

  KigCommand* kc = new KigCommand( mdoc, i18n( "Change a Label" ) );
  MonitorDataObjects mon( firstthree );

  assert( firstthree[0]->hasimp( ObjectImp::ID_IntImp ) );
  assert( firstthree[1]->hasimp( ObjectImp::ID_PointImp ) );
  assert( firstthree[2]->hasimp( ObjectImp::ID_StringImp ) );

  assert( firstthree[0]->inherits( Object::ID_DataObject ) );
  assert( firstthree[1]->inherits( Object::ID_DataObject ) );
  assert( firstthree[2]->inherits( Object::ID_DataObject ) );
  static_cast<DataObject*>( firstthree[0] )->setImp( new IntImp( needframe ? 1 : 0 ) );
  static_cast<DataObject*>( firstthree[1] )->setImp( new PointImp( coord ) );
  static_cast<DataObject*>( firstthree[2] )->setImp( new StringImp( s ) );
  kc->addTask( mon.finish() );

  Objects oldparents = mlabel->parents();
  Objects p( props.begin(), props.end() );
  p.calc( mdoc );
  mdoc._addObjects( p );

  Objects np = firstthree;
  copy( p.begin(), p.end(), back_inserter( np ) );

  kc->addTask(
    new ChangeParentsAndTypeTask(
      mlabel, np, TextType::instance() ) );

  mdoc.history()->addCommand( kc );
}

void TextLabelModeBase::setFrame( bool f )
{
  mwiz->needFrameCheckBox->setChecked( f );
}
