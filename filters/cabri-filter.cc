/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 USA
**/

#include "cabri-filter.h"

#include "../kig/kig_document.h"
#include "../kig/kig_part.h"
#include "../misc/coordinate.h"
#include "../objects/arc_type.h"
#include "../objects/bogus_imp.h"
#include "../objects/circle_type.h"
#include "../objects/conic_types.h"
#include "../objects/curve_imp.h"
#include "../objects/line_imp.h"
#include "../objects/line_type.h"
#include "../objects/object_calcer.h"
#include "../objects/object_drawer.h"
#include "../objects/object_factory.h"
#include "../objects/object_holder.h"
#include "../objects/other_imp.h"
#include "../objects/other_type.h"
#include "../objects/point_type.h"
#include "../objects/polygon_type.h"
#include "../objects/transform_types.h"
#include "../objects/vector_type.h"

#include <qcolor.h>
#include <qfile.h>
#include <qregexp.h>

#include <kdebug.h>
#include <klocale.h>

/**
 * a line:
 * "Nr": Type, Unknown, "CN:"NumberOfParents, "VN:"Unknown
 * Color, FillType, Thickness, "DS":Dots, "GT":SpecialAppearance, Visible, Fixed
 * ["Const": Parents] ["Val": Constants]
 *
 * Nr: Simple sequential numbering of the objects in a file.
 * Type: seen so far: Pt, Axes, Line, Cir
 * NumberOfParents: The number of parents that will be specified in
 *      Parents
 * Color:
 *   R -> red
 *   O -> purple
 *   Y -> yellow
 *   P -> dark purple
 *   V -> dark blue
 *   Bl -> blue
 *   lBl -> bright blue
 *   G -> bright green
 *   dG -> dark green
 *   Br -> brown
 *   dBr -> beige
 *   lGr -> light grey
 *   Gr -> grey
 *   dGr -> dark grey
 *   B -> black
 * FillType:
 *   W -> not filled ( white ? )
 *   all values of the Color item are valid here too..
 * Thickness:
 *   t -> thin
 *   tT -> medium
 *   T -> Thick
 * Dots:
 *   a specification for how a line should be drawn ( with many
 *   dots, with short lines, it's a pretty generic format: the first
 *   number signifies the number of sequential dots to draw first, and
 *   the next is the sum of this first number with the number of
 *   spaces to leave before starting a new segment.
 * SpecialAppearance:
 *   a number indicating some way of specially drawing an object. This
 *   can be modified using the "Modify Appearance" button in
 *   Cabri. For example, for a segment, the number indicates the
 *   amount of ticks to put on the segment, to indicate
 *   correspondances between segments..
 * Visible:
 *   V means visible, I means invisible
 * Fixed:
 *   St means fix this object ( if you move one of its parents, it
 *   won't move ), nSt ( the default ) means don't fix this object.
 * Parents:
 *   The numbers of the objects this object depends on
 * Constants:
 *   Constants whose meaning depends on the type of object.  E.g. for
 *   a point, this means first x, then y component.
 */

struct CabriObject
{
  uint id;
  QCString type;
  uint numberOfParents;
  QColor color;
  QColor fillColor;
  int thick;
  int lineSegLength;
  int lineSegSplit;
  int specialAppearanceSwitch;
  bool visible;
  bool fixed;
  std::vector<int> parents;
  std::vector<double> data;
};

KigFilterCabri::KigFilterCabri()
{
}

KigFilterCabri::~KigFilterCabri()
{
}

bool KigFilterCabri::supportMime( const QString& mime )
{
  // ugly hack to avoid duplicate extension ( XFig and Cabri files
  // have the same .fig extension ).
  return ( mime == "image/x-xfig" ) ||
         ( mime == "application/x-cabri" );
}

static QString readLine( QFile& file )
{
  QString ret;
  file.readLine( ret, 10000L );
  if ( ret[ret.length() - 1] == '\n' )
    ret.truncate( ret.length() - 1 );
  if ( ret[ret.length() - 1] == '\r' )
    ret.truncate( ret.length() - 1 );
  return ret;
}

static QColor translatecolor( const QString& s )
{
  if ( s == "R" ) return Qt::red;
  if ( s == "O" ) return Qt::magenta;
  if ( s == "Y" ) return Qt::yellow;
  if ( s == "P" ) return Qt::darkMagenta;
  if ( s == "V" ) return Qt::darkBlue;
  if ( s == "Bl" ) return Qt::blue;
  if ( s == "lBl" ) return Qt::cyan; // TODO: bright blue
  if ( s == "G" ) return Qt::green;
  if ( s == "dG" ) return Qt::darkGreen;
  if ( s == "Br" ) return QColor( 165, 42, 42 );
  if ( s == "dBr" ) return QColor( 128, 128, 0 );
  if ( s == "lGr" ) return Qt::lightGray;
  if ( s == "Gr" ) return Qt::gray;
  if ( s == "dGr" ) return Qt::darkGray;
  if ( s == "B" ) return Qt::black;
  if ( s == "W" ) return Qt::white;

  kdDebug() << k_funcinfo << "unknown color: " << s << endl;
  return Qt::black;
}

bool KigFilterCabri::readObject( QFile& f, CabriObject& myobj )
{
  // there are 4 lines per object in the file, so we read them all
  // four now.
  QString line1, line2, line3, s;
  QString file = f.name();
  line1 = readLine( f );
  line2 = readLine( f );
  line3 = readLine( f );
  // ignore line 4, it is empty..
  s = readLine( f );

  QRegExp firstlinere( "^([^:]+): ([^,]+), ([^,]+), CN:([^,]*), VN:(.*)$" );
  if ( ! firstlinere.exactMatch( line1 ) )
    KIG_FILTER_PARSE_ERROR;

  bool ok;
  QString tmp;

  tmp = firstlinere.cap( 1 );
  myobj.id = tmp.toInt( &ok );
  if ( !ok ) KIG_FILTER_PARSE_ERROR;

  tmp = firstlinere.cap( 2 );
  myobj.type = tmp.latin1();

  tmp = firstlinere.cap( 3 );
  // i have no idea what this number means..

  tmp = firstlinere.cap( 4 );
  myobj.numberOfParents = tmp.toInt( &ok );
  if ( ! ok ) KIG_FILTER_PARSE_ERROR;

  tmp = firstlinere.cap( 5 );
  // i have no idea what this number means..

  QRegExp secondlinere( "^([^,]+), ([^,]+), ([^,]+), DS:([^ ]+) ([^,]+), GT:([^,]+), ([^,]+), (.*)$" );
  if ( ! secondlinere.exactMatch( line2 ) )
    KIG_FILTER_PARSE_ERROR;

  tmp = secondlinere.cap( 1 );
  myobj.color = translatecolor( tmp );
//  if ( ! color.isValid() ) KIG_FILTER_PARSE_ERROR;

  tmp = secondlinere.cap( 2 );
  myobj.fillColor = translatecolor( tmp );
//  if ( ! fillcolor.isValid() ) KIG_FILTER_PARSE_ERROR;

  tmp = secondlinere.cap( 3 );
  myobj.thick = tmp == "t" ? 1 : tmp == "tT" ? 2 : 3;

  tmp = secondlinere.cap( 4 );
  myobj.lineSegLength = tmp.toInt( &ok );
  if ( ! ok ) KIG_FILTER_PARSE_ERROR;

  tmp = secondlinere.cap( 5 );
  myobj.lineSegSplit = tmp.toInt( &ok );
  if ( ! ok ) KIG_FILTER_PARSE_ERROR;

  tmp = secondlinere.cap( 6 );
  myobj.specialAppearanceSwitch = tmp.toInt( &ok );
  if ( ! ok ) KIG_FILTER_PARSE_ERROR;

  tmp = secondlinere.cap( 7 );
  myobj.visible = tmp == "V";

  tmp = secondlinere.cap( 8 );
  myobj.fixed = tmp == "St";

  QRegExp thirdlinere( "^(Const: ([^,]*),? ?)?(Val: (.*))?$" );
  if ( ! thirdlinere.exactMatch( line3 ) )
    KIG_FILTER_PARSE_ERROR;

  tmp = thirdlinere.cap( 2 );
  QStringList parentsids = QStringList::split( ' ', tmp );
  for ( QStringList::iterator i = parentsids.begin();
        i != parentsids.end(); ++i )
  {
    myobj.parents.push_back( ( *i ).toInt( &ok ) );
    if ( ! ok ) KIG_FILTER_PARSE_ERROR;
  }
  if ( myobj.parents.size() != myobj.numberOfParents )
    KIG_FILTER_PARSE_ERROR;

  tmp = thirdlinere.cap( 4 );
  QStringList valIds = QStringList::split( ' ', tmp );
  for ( QStringList::iterator i = valIds.begin();
        i != valIds.end(); ++i )
  {
    myobj.data.push_back( ( *i ).toDouble( &ok ) );
    if ( ! ok ) KIG_FILTER_PARSE_ERROR;
  }
//     kdDebug()
//       << k_funcinfo << endl
//       << "id = " << myobj.id << endl
//       << "type = " << myobj.type << endl
//       << "numberOfParents = " << myobj.numberOfParents << endl
//       << "color = " << myobj.color.name() << endl
//       << "fillcolor = " << myobj.fillColor.name() << endl
//       << "thick = " << myobj.thick << endl
//       << "lineseglength = " << myobj.lineSegLength << endl
//       << "linesegsplit = " << myobj.lineSegSplit << endl
//       << "specialAppearanceSwitch = " << myobj.specialAppearanceSwitch << endl
//       << "visible = " << visible << endl
//       << "fixed = " << myobj.fixed << endl
//       << "parents =" << endl;
//     for ( std::vector<int>::iterator i = myobj.parents.begin(); i != myobj.parents.end(); ++i )
//       kdDebug() << "	" << *i << endl;
//     kdDebug() << "vals = " << endl;
//     for ( std::vector<double>::iterator i = myobj.data.begin(); i != myobj.data.end(); ++i )
//       kdDebug() << "	" << *i << endl;

  return true;
}

KigDocument* KigFilterCabri::load( const QString& file )
{
  QFile f( file );
  if ( ! f.open( IO_ReadOnly ) )
  {
    fileNotFound( file );
    return 0;
  }

  KigDocument* ret = new KigDocument();

  QString s = readLine( f );
  QString a = s.left( 21 );
  QString b = s.mid( 21 );
  if( a != "FIGURE CabriII vers. " ||
      ( b != "DOS 1.0" && b != "MS-Windows 1.0" ) )
  {
    if ( s.left( 5 ) == "#FIG " )
    {
      notSupported( file, i18n( "This is an XFig file, not a Cabri figure." ) );
      return 0;
    }
    else
      KIG_FILTER_PARSE_ERROR;
  }

  // next we have:
  // line 2: empty line
  // line 3: window dimensions -> we don't need/use that...
  // line 4: empty line
  // line 5 through 8: center point
  // line 9 through 12: axes
  // so we skip 11 lines...
  for( int i = 0; i != 11; ++i)
    s = readLine( f );

  // all Cabri files seem to at least have these center and axes...
  if ( f.atEnd() )
    KIG_FILTER_PARSE_ERROR;

  std::vector<ObjectHolder*> holders;
  std::vector<ObjectCalcer*> calcers;

  const ObjectFactory* fact = ObjectFactory::instance();

  std::vector<ObjectCalcer*> args;
  ObjectCalcer* oc = 0;

  while ( ! f.atEnd() )
  {
    CabriObject obj;
    // we do one object each iteration..
    if ( !readObject( f, obj ) )
      return 0;

// reading linestyle
    Qt::PenStyle ls = Qt::SolidLine;
    if ( ( obj.lineSegLength > 1 ) && ( obj.lineSegLength < 6 ) &&
         ( obj.lineSegSplit > 1 ) && ( obj.lineSegSplit <= 10 ) )
      ls = Qt::DotLine;
    else if ( ( obj.lineSegLength >= 6 ) && ( obj.lineSegSplit > 10 ) )
      ls = Qt::DashLine;
    int ps = 0;

    args.clear();
    for ( std::vector<int>::iterator i = obj.parents.begin();
          i != obj.parents.end(); ++i )
      args.push_back( calcers[*i-3] );

    // two fake objects at the start ( origin and axes.. )
    if ( obj.id != calcers.size() + 3 ) KIG_FILTER_PARSE_ERROR;
    oc = 0;
    if ( obj.type == "Pt" )
    {
      if ( ! args.empty() ) KIG_FILTER_PARSE_ERROR;
      if ( obj.data.size() != 2 ) KIG_FILTER_PARSE_ERROR;

      switch ( obj.specialAppearanceSwitch )
      {
        case 0:
        {
          obj.thick -= 1;
          break;
        }
        case 2:
        {
          obj.thick += 1;
          break;
        }
        case 3:
        {
          obj.thick += 1;
          ps = 1;
          break;
        }
        case 4:
        {
          obj.thick += 2;
          ps = 4;
          break;
        }
      }
      // different sizes for points..
      obj.thick *= 2;

      oc = fact->fixedPointCalcer( Coordinate( obj.data[0], obj.data[1] ) );
    }
    else if ( obj.type == "Cir" )
    {
      if ( args.size() == 1 )
      {
        if ( obj.data.size() != 1 ) KIG_FILTER_PARSE_ERROR;
        ObjectConstCalcer* radc =
          new ObjectConstCalcer( new DoubleImp( obj.data[0] ) );
        args.push_back( radc );
        oc = new ObjectTypeCalcer( CircleBPRType::instance(), args );
      }
      else if ( args.size() == 2 )
      {
        if ( ! obj.data.empty() ) KIG_FILTER_PARSE_ERROR;
        oc = new ObjectTypeCalcer( CircleBCPType::instance(), args );
      }
      else KIG_FILTER_PARSE_ERROR;
    }
    else if ( obj.type == "Line" || obj.type == "Ray" || obj.type == "Seg" ||
              obj.type == "Vec" )
    {
      if ( args.size() == 1 )
      {
        if ( obj.data.size() != 2 ) KIG_FILTER_PARSE_ERROR;
        Coordinate vect( obj.data[0], obj.data[1] );
        ObjectConstCalcer* vectorcalcer =
          new ObjectConstCalcer( new VectorImp( Coordinate( 0, 0 ), vect ) );
        args.push_back( vectorcalcer );
        ObjectTypeCalcer* secondpoint =
          new ObjectTypeCalcer( TranslatedType::instance(), args );
        secondpoint->calc( *ret );
        args[1] = secondpoint;
      }
      if ( args.size() != 2 ) KIG_FILTER_PARSE_ERROR;
      const ObjectType* t = 0;
      if ( obj.type == "Line" ) t = LineABType::instance();
      else if ( obj.type == "Ray" ) t = RayABType::instance();
      else if ( obj.type == "Seg" ) t = SegmentABType::instance();
      else if ( obj.type == "Vec" ) t = VectorType::instance();
      else assert( t );
      oc = new ObjectTypeCalcer( t, args );
    }
    else if ( obj.type == "Pt/" )
    {
      // different sizes for points..
      obj.thick *= 2;
      if ( args.size() != 1 || obj.data.size() != 2 )
        KIG_FILTER_PARSE_ERROR;
      ObjectCalcer* parent = args[0];
      if ( !parent->imp()->inherits( CurveImp::stype() ) )
        KIG_FILTER_PARSE_ERROR;
      const CurveImp* curve = static_cast<const CurveImp*>( parent->imp() );
      Coordinate pt = Coordinate( obj.data[0], obj.data[1] );
      double param = curve->getParam( pt, *ret );
      args.push_back( new ObjectConstCalcer( new DoubleImp( param ) ) );
      oc = new ObjectTypeCalcer( ConstrainedPointType::instance(), args );
    }
    else if ( obj.type == "Perp" || obj.type == "Par" )
    {
      if ( args.size() != 2 || obj.data.size() != 0 )
        KIG_FILTER_PARSE_ERROR;
      const ObjectType* t = 0;
      if ( obj.type == "Perp" ) t = LinePerpendLPType::instance();
      else if ( obj.type == "Par" ) t = LineParallelLPType::instance();
      else assert( false );
      oc = new ObjectTypeCalcer( t, args );
    }
    else if ( obj.type == "Arc" )
    {
      if ( args.size() != 3 || ! obj.data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( ArcBTPType::instance(), args );
    }
    else if ( obj.type == "Con" )
    {
      if ( args.size() != 5 || !obj.data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( ConicB5PType::instance(), args );
    }
    else if ( obj.type == "Mid" )
    {
      if ( args.size() == 2 )
      {
        ObjectCalcer* c =
          new ObjectTypeCalcer( SegmentABType::instance(), args );
        c->calc( *ret );
        args.clear();
        args.push_back( c );
      }
      // midpoint -> this can be the midpoint of a segment, two
      // points, or a vector...
      if ( args.size() != 1 || !obj.data.empty() )
        KIG_FILTER_PARSE_ERROR;
      ObjectCalcer* parent = args[0];
      if ( parent->imp()->inherits( SegmentImp::stype() ) )
        oc = fact->propertyObjectCalcer( parent, "mid-point" ) ;
      else if ( parent->imp()->inherits( VectorImp::stype() ) )
        oc = fact->propertyObjectCalcer( parent, "vect-mid-point" );
      else KIG_FILTER_PARSE_ERROR;
    }
    else if ( obj.type == "PBiss" )
    {
      if ( args.size() == 2 )
      {
        ObjectCalcer* c =
          new ObjectTypeCalcer( SegmentABType::instance(), args );
        c->calc( *ret );
        args.clear();
        args.push_back( c );
      }
      if ( args.size() != 1 || !obj.data.empty() )
        KIG_FILTER_PARSE_ERROR;
      ObjectCalcer* parent = args[0];
      ObjectCalcer* midpoint = 0;
      if ( parent->imp()->inherits( SegmentImp::stype() ) )
        midpoint = fact->propertyObjectCalcer( parent, "mid-point" ) ;
//       else if ( parent->imp()->inherits( VectorImp::stype() ) )
//         midpoint = fact->propertyObjectCalcer( parent, "vect-mid-point" );
      else KIG_FILTER_PARSE_ERROR;
      midpoint->calc( *ret );
      args.push_back( midpoint );
      oc = new ObjectTypeCalcer( LinePerpendLPType::instance(), args );
    }
    else if ( obj.type == "Pol" )
    {
      if ( args.size() < 3 || !obj.data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( PolygonBNPType::instance(), args );
    }
    else if ( obj.type == "Locus" )
    {
      if ( args.size() != 2 || !obj.data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = fact->locusCalcer( args[0], args[1] );
    }
    else if ( obj.type == "Refl" )
    {
      if ( args.size() != 2 || !obj.data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( LineReflectionType::instance(), args );
    }
    else if ( obj.type == "Sym" )
    {
      if ( args.size() != 2 || !obj.data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( PointReflectionType::instance(), args );
    }
    else if ( obj.type == "Tran" )
    {
      if ( args.size() != 2 || !obj.data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( TranslatedType::instance(), args );
    }
    else
    {
      notSupported( file, i18n( "This Cabri file contains a \"%1\" object, "
                                "which Kig does not currently support." ).arg( obj.type ) );
      return 0;
    }

    if ( oc == 0 ) KIG_FILTER_PARSE_ERROR;

    oc->calc( *ret );
    calcers.push_back( oc );
    ObjectDrawer* d = new ObjectDrawer( obj.color, obj.thick, obj.visible, ls, ps );
    ObjectHolder* oh = new ObjectHolder( oc, d );
    holders.push_back( oh );

    oc = 0;
  }

  ret->addObjects( holders );
  ret->setGrid( false );
  ret->setAxes( false );
  return ret;
}

KigFilterCabri* KigFilterCabri::instance()
{
  static KigFilterCabri t;
  return &t;
}
