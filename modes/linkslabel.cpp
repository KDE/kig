// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301  USA


#include "linkslabel.h"
#include "linkslabel.moc"

#include <qlabel.h>
#include <kurllabel.h>
#include <qlayout.h>

#include <vector>
#include <algorithm>
#include <functional>

#include <assert.h>
using namespace std;

class LinksLabel::Private
{
public:
  QHBoxLayout* layout;
  std::vector<QLabel*> labels;
  std::vector<KURLLabel*> urllabels;
};

LinksLabel::LinksLabel( QWidget* parent, const char* name )
  : QWidget( parent, name )
{
  p = new Private;
  p->layout = new QHBoxLayout( this );

  QLabel* l = new QLabel( QString::fromUtf8( "Dit is een " ), this );
  p->labels.push_back( l );
  p->layout->addWidget( l );

  KURLLabel* u = new KURLLabel( QString::fromUtf8( "http://www.kde.org/" ),
                                QString::fromUtf8( "url"), this );
  p->urllabels.push_back( u );
  p->layout->addWidget( u );

  l = new QLabel( QString::fromUtf8( " !" ), this );
  p->labels.push_back( l );
  p->layout->addWidget(l );

  p->layout->activate();
}

LinksLabel::~LinksLabel()
{
  delete p;
}

void LinksLabel::urlClicked()
{
  const QObject* o = sender();
  std::vector<KURLLabel*>::iterator i = std::find( p->urllabels.begin(), p->urllabels.end(), static_cast<const KURLLabel*>( o ) );
  assert( i != p->urllabels.end() );
  emit linkClicked( i - p->urllabels.begin() );
}

LinksLabel::LinksLabelEditBuf LinksLabel::startEdit()
{
  return LinksLabelEditBuf();
}

void LinksLabel::addText( const QString& s, LinksLabelEditBuf& buf )
{
  buf.data.push_back( std::pair<bool, QString>( false, s ) );
}

void LinksLabel::addLink( const QString& s, LinksLabelEditBuf& buf )
{
  buf.data.push_back( std::pair<bool, QString>( true, s ) );
}

namespace {
  void deleteObj( QObject* o ) { delete o; }
}

void LinksLabel::applyEdit( LinksLabelEditBuf& buf )
{
  std::for_each( p->urllabels.begin(), p->urllabels.end(), deleteObj );
  std::for_each( p->labels.begin(), p->labels.end(), deleteObj );
  p->urllabels.clear();
  p->labels.clear();

  delete p->layout;
  p->layout = new QHBoxLayout( this );

  for ( LinksLabelEditBuf::vec::iterator i = buf.data.begin(); i != buf.data.end(); ++i )
  {
    if ( i->first )
    {
      // we need a KURLLabel...
      // the url is an unused stub...
      KURLLabel* l = new KURLLabel( QString::fromUtf8( "http://edu.kde.org/kig" ),
                                    i->second, this );
      p->urllabels.push_back( l );
      p->layout->addWidget( l );
      connect( l, SIGNAL( leftClickedURL() ), SLOT( urlClicked() ) );
    }
    else
    {
      // we need a normal label...
      QLabel* l = new QLabel( i->second, this );
      p->labels.push_back( l );
      p->layout->addWidget( l );
    };
  };

  QSpacerItem* spacer = new QSpacerItem( 40,  20,  QSizePolicy::Expanding,  QSizePolicy::Minimum );

  p->layout->addItem( spacer );

  p->layout->activate();

  std::for_each( p->urllabels.begin(), p->urllabels.end(), mem_fun( &QWidget::show ) );
  std::for_each( p->labels.begin(), p->labels.end(), mem_fun( &QWidget::show ) );
}
