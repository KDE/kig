// drgeo-filter.cc
// Copyright (C)  2004  Pino Toscano <toscano.pino@tiscali.it>
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

#include "drgeo-filter.h"

#include "../objects/object_holder.h"
#include "../objects/object_drawer.h"
#include "../objects/object_calcer.h"
#include "../objects/object_factory.h"
#include "../objects/other_imp.h"
#include "../objects/line_imp.h"
#include "../objects/bogus_imp.h"
#include "../objects/point_type.h"
#include "../objects/other_type.h"
#include "../objects/conic_types.h"
#include "../objects/transform_types.h"
#include "../objects/line_type.h"
#include "../objects/curve_imp.h"
#include "../objects/circle_type.h"
#include "../misc/coordinate.h"
#include "../kig/kig_part.h"

#include <qfile.h>
//#include <qdom.h>
#include <klocale.h>

KigFilterDrgeo::KigFilterDrgeo()
{
}

KigFilterDrgeo::~KigFilterDrgeo()
{
}

bool KigFilterDrgeo::supportMime( const QString& mime )
{
// TODO: is this right? it's necessary to check...
  return mime == "text/xml";
}

bool KigFilterDrgeo::load( const QString& file, KigDocument& to )
{
  QFile f( file );
  if ( ! f.open( IO_ReadOnly ) )
  {
    fileNotFound( file );
    return false;
  };

// Kig elements to draw -- BEGIN
  fact = ObjectFactory::instance();
  oc = 0;
// END

  QStringList figures;
  QDomDocument doc( "drgenius" );
  if ( !doc.setContent( &f ) )
    KIG_FILTER_PARSE_ERROR;
  QDomElement main = doc.documentElement();
  for ( QDomNode n = main.firstChild(); ! n.isNull(); n = n.nextSibling() )
  {
    QDomElement e = n.toElement();
    if ( e.isNull() ) continue;
    else if ( e.tagName() == "drgeo" )
    {
      figures.append(e.attribute("name"));
    }
/*
    else if ( e.tagName() == "text" )
    {
      s = "- Text: '" + e.attribute("name") + "'\n";
      s += "  <<" + e.text() + ">>\n";
    }
*/
    else
    {
      continue;
    }
  }
  if ( figures.isEmpty() ) {
    warning( i18n( "There are no figures in DrGeo file \"%1\"." ).arg( file ) );
    return false;
  }

// drgeo file has more than 1 figure, let the user choose one...
  int myfig = 0;
  if ( figures.count() > 1 ) {
    KigFilterDrgeoChooser* c = new KigFilterDrgeoChooser( figures );
//    c->addFigures( figures );
    myfig = c->exec();
    delete c;
  }

  kdDebug() << "Content of drgeo file:" << endl;
  int curfig = -1;
//  delete main;
//  main = doc.documentElement();

  for ( QDomNode n = main.firstChild(); ! n.isNull(); n = n.nextSibling() )
  {
    QDomElement e = n.toElement();
    if ( e.isNull() ) continue;
    else if ( e.tagName() == "drgeo" )
    {
      kdDebug() << "- Figure: '" << e.attribute("name") << "'" << endl;
      curfig += 1;
      if ( curfig == myfig )
        importFigure( e.firstChild() );
    }
/*
    else if ( e.tagName() == "text" )
    {
      s = "- Text: '" + e.attribute("name") + "'\n";
      s += "  <<" + e.text() + ">>\n";
    }
*/
    else
    {
      continue;
    }
  }

  return false;

// ...
// all right, let's draw all..
//  to.setCoordinateSystem( "Euclidean" );
// ...

//  return true;
}

void KigFilterDrgeo::importFigure( QDomNode f )
{
  for ( ; ! f.isNull(); f = f.nextSibling() )
  {
    QDomElement elem = f.toElement();
    if ( elem.isNull() ) continue;
    else if ( elem.tagName() == "point" )
    {
      QString x;
      QString y;
      for ( QDomNode c = elem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "x" )
        {
          x = ce.text();
        }
        else if ( ce.tagName() == "y" )
        {
          y = ce.text();
        }
        else
        {
          continue;
        }
      }
      bool ok;
      bool ok2;
      double xd = x.toDouble( &ok );
      double yd = y.toDouble( &ok2 );
      if ( ok && ok2 )
        oc = fact->fixedPointCalcer( Coordinate( xd, yd ) );
      kdDebug() <<  "  * '" << elem.attribute("type") << "' Point(" << x << ";" << y << "): '" << elem.attribute("id") << "'" << endl;
    }
    else if ( elem.tagName() == "line" )
    {
      QStringList p;
      for ( QDomNode c = elem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        int nParents = 0;
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "parent" )
        {
          p.append(ce.attribute("ref"));
          nParents += 1;
        }
        else
        {
          continue;
        }

      }
      kdDebug() << "  * Line(" << elem.attribute("type") << "): '" << p[0] << "' '" << p[1] << "'" << endl;
    }
    else if ( elem.tagName() == "halfline" )
    {
      QStringList p;
      for ( QDomNode c = elem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        int nParents = 0;
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "parent" )
        {
          p.append(ce.attribute("ref"));
          nParents += 1;
        }
        else
        {
          continue;
        }

      }
      kdDebug() << "  * HalfLine(" << elem.attribute("type") << "): '" << p[0] << "' '" << p[1] << "'" << endl;
    }
    else if ( elem.tagName() == "segment" )
    {
      QStringList p;
      for ( QDomNode c = elem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        int nParents = 0;
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "parent" )
        {
          p.append(ce.attribute("ref"));
          nParents += 1;
        }
        else
        {
          continue;
        }

      }
      kdDebug() << "  * Segment(" << elem.attribute("type") << "): '" << p[0] << "' '" << p[1] << "'" << endl;
    }
    else if ( elem.tagName() == "vector" )
    {
      QStringList p;
      QString x;
      QString y;
      for ( QDomNode c = elem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        int nParents = 0;
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "parent" )
        {
          p.append(ce.attribute("ref"));
          nParents += 1;
        }
        else if ( ce.tagName() == "Ox" )
        {
          x = ce.text();
        }
        else if ( ce.tagName() == "Oy" )
        {
          y = ce.text();
        }
        else
        {
          continue;
        }

      }
      kdDebug() << "  * Vector(" << elem.attribute("type") << "): '" << p[0] << "' '" << p[1] << "'" << endl;
      kdDebug() << "      from (" << x << ";" << y << ")" << endl;
    }
    else if ( elem.tagName() == "circle" )
    {
      QStringList p;
      for ( QDomNode c = elem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        int nParents = 0;
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "parent" )
        {
          p.append(ce.attribute("ref"));
          nParents += 1;
        }
        else
        {
        continue;
        }

      }
      kdDebug() << "  * Circle(" << elem.attribute("type") << "): '" << p[0] << "' '" << p[1] << "'" << endl;
    }
    else if ( elem.tagName() == "arcCircle" )
    {
      QStringList p;
      for ( QDomNode c = elem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        int nParents = 0;
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "parent" )
        {
          p.append(ce.attribute("ref"));
          nParents += 1;
        }
        else
        {
          continue;
        }

      }
      kdDebug() << "  * arcCircle(" << elem.attribute("type") << "):" << endl;
      kdDebug() << "      '" << p[0] << "' '" << p[1] << "' '" << p[2] << "'" << endl;
    }
    else
    {
      continue;
    }
  }

}

KigFilterDrgeo* KigFilterDrgeo::instance()
{
  static KigFilterDrgeo f;
  return &f;
}
