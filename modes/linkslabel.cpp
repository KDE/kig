// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later


#include "linkslabel.h"


#include <QLabel>
#include <kurllabel.h>
#include <QLayout>

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
  std::vector<KUrlLabel*> urllabels;
};

LinksLabel::LinksLabel( QWidget* parent )
  : QWidget( parent )
{
  p = new Private;
  p->layout = new QHBoxLayout( this );

  QLabel* l = new QLabel( QStringLiteral( "Dit is een " ), this );
  p->labels.push_back( l );
  p->layout->addWidget( l );

  KUrlLabel* u = new KUrlLabel( QStringLiteral( "https://www.kde.org/" ),
                                QStringLiteral( "url"), this );
  p->urllabels.push_back( u );
  p->layout->addWidget( u );

  l = new QLabel( QStringLiteral( " !" ), this );
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
  std::vector<KUrlLabel*>::iterator i = std::find( p->urllabels.begin(), p->urllabels.end(), static_cast<const KUrlLabel*>( o ) );
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
      // we need a KUrlLabel...
      // the url is an unused stub...
      KUrlLabel* l = new KUrlLabel( QStringLiteral( "https://edu.kde.org/kig" ),
                                    i->second, this );
      p->urllabels.push_back( l );
      p->layout->addWidget( l );
      connect( l, SIGNAL(leftClickedUrl()), SLOT(urlClicked()) );
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

  emit changed();
}
