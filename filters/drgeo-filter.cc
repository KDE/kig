// drgeo-filter.cc
// Copyright (C)  2004  Pino Toscano <toscano.pino@tiscali.it>
// Copyright (C)  2004  Dominique Devriese <devriese@kde.org>

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

namespace {
struct HierarchyElement
{
  QString id;
  std::vector<QString> parents;
};
}

bool KigFilterDrgeo::load( const QString& file, KigDocument& to )
{
  QFile f( file );
  if ( ! f.open( IO_ReadOnly ) )
  {
    fileNotFound( file );
    return false;
  };

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
    warning( i18n( "There are no figures in Dr. Geo file \"%1\"." ).arg( file ) );
    return false;
  }

  int myfig = 0;
// drgeo file has more than 1 figure, let the user choose one...
  if ( figures.count() > 1 ) {
    KigFilterDrgeoChooser* c = new KigFilterDrgeoChooser( figures );
//    c->addFigures( figures );
    myfig = c->exec();
    delete c;
  }

  kdDebug() << "Content of drgeo file:" << endl;
  int curfig = -1;

  for ( QDomNode n = main.firstChild(); ! n.isNull(); n = n.nextSibling() )
  {
    QDomElement e = n.toElement();
    if ( e.isNull() ) continue;
    else if ( e.tagName() == "drgeo" )
    {
      kdDebug() << "- Figure: '" << e.attribute("name") << "'" << endl;
      curfig += 1;
      if ( curfig == myfig )
        return importFigure( e.firstChild(), to, file );
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
}

// constructs a text object with text "%1", location c, and variable
// parts given by the argument arg of obj o.
// static ObjectTypeCalcer* constructTextObject(
//   const Coordinate& c, ObjectCalcer* o,
//   const QCString& arg, const KigDocument& doc )
// {
//   const ObjectFactory* fact = ObjectFactory::instance();
//   ObjectCalcer* propo = fact->propertyObjectCalcer( o, arg );
//   propo->calc( doc );
//   std::vector<ObjectCalcer*> args;
//   args.push_back( propo );
//   return fact->labelCalcer( QString::fromLatin1( "%1" ), c, false, args, doc );
// }

int convertDrgeoIndex( const std::vector<HierarchyElement> es, const QString myid )
{
  for ( uint i = 0; i < es.size(); ++i )
    if ( es[i].id == myid )
      return i;
  return -1;
}

bool KigFilterDrgeo::importFigure( QDomNode f, KigDocument& doc, const QString& file )
{
  using namespace std;
  std::vector<HierarchyElement> elems;
  const ObjectFactory* fact = ObjectFactory::instance();
  int nelems = 0;

  // 1st: fetch relationships and build an appropriate structure
  for (QDomNode a = f; ! a.isNull(); a = a.nextSibling() )
  {
    QDomElement domelem = a.toElement();
    if ( domelem.isNull() ) continue;
    else if ( domelem.tagName() == "point" )
    {
      HierarchyElement elem;
      for ( QDomNode c = domelem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "parent" )
        {
          elem.parents.push_back( ce.attribute( "ref" ) );
        }
        else
        {
          continue;
        }

      }
//      kdDebug() <<  "  * '" << domelem.attribute( "type" ) << "' Point: '" << domelem.attribute( "id" ) << "'" << endl;
      elem.id = domelem.attribute( "id" );
      elems.push_back( elem );
      nelems += 1;
    }
    else if ( domelem.tagName() == "line" )
    {
      HierarchyElement elem;
      for ( QDomNode c = domelem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "parent" )
        {
          elem.parents.push_back( ce.attribute( "ref" ) );
        }
        else
        {
          continue;
        }

      }
//      kdDebug() << "  * Line(" << domelem.attribute("type") << ")" << endl;
      elem.id = domelem.attribute( "id" );
      elems.push_back( elem );
      nelems += 1;
    }
    else if ( domelem.tagName() == "halfLine" )
    {
      HierarchyElement elem;
      for ( QDomNode c = domelem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "parent" )
        {
          elem.parents.push_back( ce.attribute( "ref" ) );
        }
        else
        {
          continue;
        }

      }
//      kdDebug() << "  * HalfLine(" << domelem.attribute("type") << ")" << endl;
      elem.id = domelem.attribute( "id" );
      elems.push_back( elem );
      nelems += 1;
    }
    else if ( domelem.tagName() == "segment" )
    {
      HierarchyElement elem;
      for ( QDomNode c = domelem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "parent" )
        {
          elem.parents.push_back( ce.attribute( "ref" ) );
        }
        else
        {
          continue;
        }

      }
//      kdDebug() << "  * Segment(" << domelem.attribute("type") << ")" << endl;
      elem.id = domelem.attribute( "id" );
      elems.push_back( elem );
      nelems += 1;
    }
    else if ( domelem.tagName() == "vector" )
    {
      HierarchyElement elem;
      for ( QDomNode c = domelem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "parent" )
        {
          elem.parents.push_back( ce.attribute( "ref" ) );
        }
        else
        {
          continue;
        }

      }
//      kdDebug() << "  * Vector(" << domelem.attribute("type") << ")'" << endl;
      elem.id = domelem.attribute( "id" );
      elems.push_back( elem );
      nelems += 1;
    }
    else if ( domelem.tagName() == "circle" )
    {
      HierarchyElement elem;
      for ( QDomNode c = domelem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "parent" )
        {
          elem.parents.push_back( ce.attribute( "ref" ) );
        }
        else
        {
          continue;
        }

      }
//      kdDebug() << "  * Circle(" << domelem.attribute("type") << ")" << endl;
      elem.id = domelem.attribute( "id" );
      elems.push_back( elem );
      nelems += 1;
    }
    else if ( domelem.tagName() == "arcCircle" )
    {
      HierarchyElement elem;
      for ( QDomNode c = domelem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "parent" )
        {
          elem.parents.push_back( ce.attribute( "ref" ) );
        }
        else
        {
          continue;
        }

      }
//      kdDebug() << "  * arcCircle(" << domelem.attribute("type") << ")" << endl;
      elem.id = domelem.attribute( "id" );
      elems.push_back( elem );
      nelems += 1;
    }
    else if ( domelem.tagName() == "angle" )
    {
      HierarchyElement elem;
      for ( QDomNode c = domelem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "parent" )
        {
          elem.parents.push_back( ce.attribute( "ref" ) );
        }
        else
        {
          continue;
        }

      }
//      kdDebug() << "  * arcCircle(" << domelem.attribute("type") << ")" << endl;
      elem.id = domelem.attribute( "id" );
      elems.push_back( elem );
      nelems += 1;
    }
    else if ( domelem.tagName() == "polygon" )
    {
      HierarchyElement elem;
      for ( QDomNode c = domelem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "parent" )
        {
          elem.parents.push_back( ce.attribute( "ref" ) );
        }
        else
        {
          continue;
        }

      }
      kdDebug() << "  * polygon(" << domelem.attribute("type") << ")" << endl;
      elem.id = domelem.attribute( "id" );
      elems.push_back( elem );
      nelems += 1;
    }
    else if ( domelem.tagName() == "locus" )
    {
      HierarchyElement elem;
      for ( QDomNode c = domelem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "parent" )
        {
          elem.parents.push_back( ce.attribute( "ref" ) );
        }
        else
        {
          continue;
        }

      }
      kdDebug() << "  * locus(" << domelem.attribute("type") << ")" << endl;
      elem.id = domelem.attribute( "id" );
      elems.push_back( elem );
      nelems += 1;
    }
    else if ( domelem.tagName() == "numeric" )
    {
      HierarchyElement elem;
//      kdDebug() << "  * arcCircle(" << domelem.attribute("type") << ")" << endl;
      elem.id = domelem.attribute( "id" );
      elems.push_back( elem );
      nelems += 1;
    }
    else
    {
      continue;
    }
  }

  kdDebug() << "---- 2ND STEP ----" << endl;
  // 2nd: let's draw!
  int curid = 0;
  std::vector<ObjectHolder*> holders;

  for (QDomNode a = f; ! a.isNull(); a = a.nextSibling() )
  {
    kdDebug() << "+++ id: " << curid << endl;
    const HierarchyElement& el = elems[curid];
    std::vector<ObjectCalcer*> parents;
    for ( uint j = 0; j < el.parents.size(); ++j )
    {
      int parentid = convertDrgeoIndex( elems, el.parents[j] );
      parents.push_back( holders[parentid]->calcer() );
    };
    if ( parents.size() > 1 ) {
      kdDebug() << "+++++++++ parents: " << parents[1] << " " << parents[2] << endl;
    } else {
      kdDebug() << "+++++++++ parents: NO" << endl;
    }

    ObjectCalcer* oc = 0;

    kdDebug() << ">>>>>>>>> Scanning domelem" << endl;
    QDomElement domelem = a.toElement();
    if ( domelem.isNull() ) continue;
    else if ( domelem.tagName() == "point" )
    {
      QString xs;
      QString ys;
      for ( QDomNode c = domelem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "x" )
        {
          xs = ce.text();
        }
        else if ( ce.tagName() == "y" )
        {
          ys = ce.text();
        }
        else
        {
          continue;
        }
      }
      kdDebug() << "+++++++++ Point - " << domelem.attribute( "type" ) << endl;
      bool ok;
      bool ok2;
      double x = xs.toDouble( &ok );
      double y = ys.toDouble( &ok2 );
//      kdDebug() << "+++++++++ ok: " << ok << endl;
//      kdDebug() << "+++++++++ ok2: " << ok2 << endl;
      if ( domelem.attribute( "type" ) == "Free" )
      {
        if ( ! ( ok && ok2 ) )
          KIG_FILTER_PARSE_ERROR;
        oc = fact->fixedPointCalcer( Coordinate( x, y ) );
      }
      else if ( domelem.attribute( "type" ) == "Middle_2pts" )
        oc = new ObjectTypeCalcer( MidPointType::instance(), parents );
      else if ( domelem.attribute( "type" ) == "Reflexion" )
        oc = new ObjectTypeCalcer( PointReflectionType::instance(), parents );
      kdDebug() << "+++++++++ oc:" << oc << endl;
    }
    else if ( domelem.tagName() == "line" )
    {
      kdDebug() << "+++++++++ Line" << endl;
      if ( domelem.attribute( "type" ) == "2pts" )
        oc = new ObjectTypeCalcer( LineABType::instance(), parents );
      else if ( domelem.attribute( "type" ) == "perpendicular" )
        oc = new ObjectTypeCalcer( LinePerpendLPType::instance(), parents );
      else if ( domelem.attribute( "type" ) == "parallel" )
        oc = new ObjectTypeCalcer( LineParallelLPType::instance(), parents );
      kdDebug() << "+++++++++ oc:" << oc << endl;
    }
    else if ( domelem.tagName() == "halfLine" )
    {
      kdDebug() << "+++++++++ halfLine" << endl;
      if ( domelem.attribute( "type" ) == "2pts" )
        oc = new ObjectTypeCalcer( RayABType::instance(), parents );
      kdDebug() << "+++++++++ oc:" << oc << endl;
    }
    else if ( domelem.tagName() == "segment" )
    {
      kdDebug() << "+++++++++ segment" << endl;
      if ( domelem.attribute( "type" ) == "2pts" )
        oc = new ObjectTypeCalcer( SegmentABType::instance(), parents );
      kdDebug() << "+++++++++ oc:" << oc << endl;
    }
    else if ( domelem.tagName() == "vector" )
    {
      kdDebug() << "+++++++++ vector" << endl;
      if ( domelem.attribute( "type" ) == "2pts" )
        oc = new ObjectTypeCalcer( VectorType::instance(), parents );
      kdDebug() << "+++++++++ oc:" << oc << endl;
    }
    else if ( domelem.tagName() == "circle" )
    {
      kdDebug() << "+++++++++ circle" << endl;
      if ( domelem.attribute( "type" ) == "2pts" )
        oc = new ObjectTypeCalcer( CircleBCPType::instance(), parents );
      else if ( domelem.attribute( "type" ) == "Translation" )
        oc = new ObjectTypeCalcer( TranslatedType::instance(), parents );
      kdDebug() << "+++++++++ oc:" << oc << endl;
    }
    else if ( domelem.tagName() == "numeric" )
    {
      QString xs;
      QString ys;
      QString value;
      for ( QDomNode c = domelem.firstChild(); ! c.isNull(); c = c.nextSibling() )
      {
        QDomElement ce = c.toElement();
        if ( ce.isNull() ) continue;
        else if ( ce.tagName() == "x" )
        {
          xs = ce.text();
        }
        else if ( ce.tagName() == "y" )
        {
          ys = ce.text();
        }
        else if ( ce.tagName() == "value" )
        {
          value = ce.text();
        }
        else
        {
          continue;
        }
      }
      kdDebug() << "+++++++++ numeric" << endl;
      bool ok;
      bool ok2;
      double x = xs.toDouble( &ok );
      double y = ys.toDouble( &ok2 );
      if ( ! ( ok && ok2 ) )
        KIG_FILTER_PARSE_ERROR;
      if ( domelem.attribute( "type" ) == "value" )
        oc = fact->labelCalcer( value, Coordinate( x, y ), false, std::vector<ObjectCalcer*>(), doc );
      kdDebug() << "+++++++++ oc:" << oc << endl;
    }
    else if ( domelem.tagName() == "arcCircle" )
    {
      kdDebug() << "+++++++++ arcCircle" << endl;
        oc = new ObjectTypeCalcer( ArcBTPType::instance(), parents );
      kdDebug() << "+++++++++ oc:" << oc << endl;
    }
// FIXME: this is the real angle meant by drgeo...
// pino: I'll use a simple one, to avoid compiling problem... :-(
/*
    else if ( domelem.tagName() == "angle" )
    {
      kdDebug() << "+++++++++ arcCircle" << endl;
      if ( domelem.attribute( "type" ) == "3pts" )
      {
        if ( parents.size() == 3 )
        {
          ObjectTypeCalcer* ao = new ObjectTypeCalcer( AngleType::instance(), parents );
          ao->calc( doc );
          parents.clear();
          parents.push_back( ao );
        };
        if ( parents.size() != 1 ) KIG_FILTER_PARSE_ERROR;
        ObjectCalcer* angle = parents[0];
        parents.clear();
        const Coordinate c = static_cast<const PointImp*>( angle->parents()[1]->imp() )->coordinate();
        oc = constructTextObject( c, angle, "angle-degrees", doc );
      }
      kdDebug() << "+++++++++ oc:" << oc << endl;
    }
*/
// simple angle object...
    else if ( domelem.tagName() == "angle" )
    {
      kdDebug() << "+++++++++ arcCircle" << endl;
      if ( domelem.attribute( "type" ) == "3pts" )
        oc = new ObjectTypeCalcer( AngleType::instance(), parents );
      kdDebug() << "+++++++++ oc:" << oc << endl;
    }
    else if ( domelem.tagName() == "locus" )
    {
      kdDebug() << "+++++++++ locus" << endl;
      if ( domelem.attribute( "type" ) == "None" )
      {
        ObjectCalcer* t = parents[1];
        parents[1] = parents[0];
        parents[0] = t;
        oc = new ObjectTypeCalcer( LocusType::instance(), parents );
      }
      kdDebug() << "+++++++++ oc:" << oc << endl;
    }
    else if ( domelem.tagName() == "polygon" )
    {
      kdDebug() << "+++++++++ polygon" << endl;
//      if ( domelem.attribute( "type" ) == "npts" )
//        oc = new ObjectTypeCalcer( ??? ::instance(), parents );
      kdDebug() << "+++++++++ oc:" << oc << endl;
    }
    else
    {
      kdDebug() << "+++++++++ unknown:" << domelem.tagName() << endl;
    }
    curid++;
    kdDebug() << "+++++++++ holders.size(): " << holders.size() << endl;
    if ( oc == 0 )
      continue;
    kdDebug() << ">>>>>>>>> Using oc newly created" << endl;
    ObjectDrawer* d = 0;

    QColor co = domelem.attribute( "color" );
    if( co.isValid() )
      d = new ObjectDrawer( co );
    else
      d = new ObjectDrawer;

    kdDebug() << ">>>>>>>>> assert(d)" << endl;
    assert( d );

    kdDebug() << ">>>>>>>>> Creating ObjectHolder*" << endl;
    ObjectHolder* o = new ObjectHolder( oc, d );
    kdDebug() << ">>>>>>>>> push_back" << endl;
    holders.push_back( o );
    kdDebug() << ">>>>>>>>> calc" << endl;
    holders[curid-1]->calc( doc );
    oc = 0;
  }
/*
// Kig elements to draw


//    if ( oc == 0 ) KIG_FILTER_PARSE_ERROR;

//    ObjectHolder* oh = new ObjectHolder( oc, new ObjectDrawer( Qt::blue ) );

//    holders.push_back( oh );

    if ( oc != 0 )
    {
      ObjectHolder* oh = new ObjectHolder( oc, new ObjectDrawer( Qt::blue ) );
      holders.push_back( oh );
      oc = 0;
    }
*/

  doc.setObjects( holders );
  return true;
}

KigFilterDrgeo* KigFilterDrgeo::instance()
{
  static KigFilterDrgeo f;
  return &f;
}
