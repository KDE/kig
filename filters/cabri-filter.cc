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
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
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
#include "../objects/transform_types.h"
#include "../objects/vector_type.h"

#include <qcolor.h>
#include <qfile.h>
#include <qregexp.h>
#include <klocale.h>
#include <kdebug.h>

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

KigFilterCabri::KigFilterCabri()
{
}

KigFilterCabri::~KigFilterCabri()
{
}

bool KigFilterCabri::supportMime( const QString& mime )
{
  return mime == "application/x-cabri";
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
  if ( s == "Br" ) return Qt::black; // TODO: brown
  if ( s == "dBr" ) return Qt::black; // TODO: beige-brown
  if ( s == "lGr" ) return Qt::lightGray;
  if ( s == "Gr" ) return Qt::gray;
  if ( s == "dGr" ) return Qt::darkGray;
  if ( s == "B" ) return Qt::black;
  if ( s == "W" ) return Qt::white;

  kdDebug() << k_funcinfo << "unknown color: " << s << endl;
  return Qt::black;
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
    KIG_FILTER_PARSE_ERROR;

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

  while ( ! f.atEnd() )
  {
    // we do one object each iteration..
    // there are 4 lines per object in the file, so we read them all
    // four now.;
    QString line1, line2, line3;
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
    uint id = tmp.toInt( &ok );
    if ( !ok ) KIG_FILTER_PARSE_ERROR;

    tmp = firstlinere.cap( 2 );
    QCString type = tmp.latin1();

    tmp = firstlinere.cap( 3 );
    // i have no idea what this number means..

    tmp = firstlinere.cap( 4 );
    uint numberOfParents = tmp.toInt( &ok );
    if ( ! ok ) KIG_FILTER_PARSE_ERROR;

    tmp = firstlinere.cap( 5 );
    // i have no idea what this number means..


    QRegExp secondlinere( "^([^,]+), ([^,]+), ([^,]+), DS:([^ ]+) ([^,]+), GT:([^,]+), ([^,]+), (.*)$" );
    if ( ! secondlinere.exactMatch( line2 ) )
      KIG_FILTER_PARSE_ERROR;

    tmp = secondlinere.cap( 1 );
    QColor color = translatecolor( tmp );
    if ( ! color.isValid() ) KIG_FILTER_PARSE_ERROR;

    tmp = secondlinere.cap( 2 );
    QColor fillcolor = translatecolor( tmp );
    if ( ! color.isValid() ) KIG_FILTER_PARSE_ERROR;

    tmp = secondlinere.cap( 3 );
    int thick = tmp == "t" ? 1 : tmp == "tT" ? 2 : 3;

    tmp = secondlinere.cap( 4 );
    int lineseglength = tmp.toInt( &ok );
    // unused
    ( void ) lineseglength;
    if ( ! ok ) KIG_FILTER_PARSE_ERROR;

    tmp = secondlinere.cap( 5 );
    int linesegsplit = tmp.toInt( &ok );
    // unused
    ( void ) linesegsplit;
    if ( ! ok ) KIG_FILTER_PARSE_ERROR;

    tmp = secondlinere.cap( 6 );
    int specialAppearanceSwitch = tmp.toInt( &ok );
    // unused
    ( void )specialAppearanceSwitch;
    if ( ! ok ) KIG_FILTER_PARSE_ERROR;

    tmp = secondlinere.cap( 7 );
    bool visible = tmp == "V";

    tmp = secondlinere.cap( 8 );
    bool fixed = tmp == "St";
    // unused
    ( void )fixed;

    QRegExp thirdlinere( "^(Const: ([^,]*),? ?)?(Val: (.*))?$" );
    if ( ! thirdlinere.exactMatch( line3 ) )
      KIG_FILTER_PARSE_ERROR;

    tmp = thirdlinere.cap( 2 );
    QStringList parentsids = QStringList::split( ' ', tmp );
    std::vector<int> parents;
    for ( QStringList::iterator i = parentsids.begin();
          i != parentsids.end(); ++i )
    {
      bool ok;
      parents.push_back( ( *i ).toInt( &ok ) );
      if ( ! ok ) KIG_FILTER_PARSE_ERROR;
    }
    if ( parents.size() != numberOfParents )
      KIG_FILTER_PARSE_ERROR;

    tmp = thirdlinere.cap( 4 );
    QStringList valIds = QStringList::split( ' ', tmp );
    std::vector<double> data;
    for ( QStringList::iterator i = valIds.begin();
          i != valIds.end(); ++i )
    {
      bool ok;
      data.push_back( ( *i ).toDouble( &ok ) );
      if ( ! ok ) KIG_FILTER_PARSE_ERROR;
    }
//     kdDebug()
//       << k_funcinfo << endl
//       << "id = " << id << endl
//       << "type = " << type << endl
//       << "numberOfParents = " << numberOfParents << endl
//       << "color = " << color.name() << endl
//       << "fillcolor = " << fillcolor.name() << endl
//       << "thick = " << thick << endl
//       << "lineseglength = " << lineseglength << endl
//       << "linesegsplit = " << linesegsplit << endl
//       << "specialAppearanceSwitch = " << specialAppearanceSwitch << endl
//       << "visible = " << visible << endl
//       << "fixed = " << fixed << endl
//       << "parents =" << endl;
//     for ( std::vector<int>::iterator i = parents.begin(); i != parents.end(); ++i )
//       kdDebug() << "	" << *i << endl;
//     kdDebug() << "vals = " << endl;
//     for ( std::vector<double>::iterator i = data.begin(); i != data.end(); ++i )
//       kdDebug() << "	" << *i << endl;


    std::vector<ObjectCalcer*> args;
    for ( std::vector<int>::iterator i = parents.begin();
          i != parents.end(); ++i )
      args.push_back( calcers[*i-3] );

    const ObjectFactory* fact = ObjectFactory::instance();
    // two fake objects at the start ( origin and axes.. )
    if ( id != calcers.size() + 3 ) KIG_FILTER_PARSE_ERROR;
    ObjectCalcer* oc = 0;
    if ( type == "Pt" )
    {
      // different sizes for points..
      thick = thick * 2;
      if ( ! args.empty() ) KIG_FILTER_PARSE_ERROR;
      if ( data.size() != 2 ) KIG_FILTER_PARSE_ERROR;
      oc = fact->fixedPointCalcer( Coordinate( data[0], data[1] ) );
    }
    else if ( type == "Cir" )
    {
      if ( args.size() == 1 )
      {
        if ( data.size() != 1 ) KIG_FILTER_PARSE_ERROR;
        ObjectConstCalcer* radc =
          new ObjectConstCalcer( new DoubleImp( data[0] ) );
        args.push_back( radc );
        oc = new ObjectTypeCalcer( CircleBPRType::instance(), args );
      }
      else if ( args.size() == 2 )
      {
        if ( ! data.empty() ) KIG_FILTER_PARSE_ERROR;
        oc = new ObjectTypeCalcer( CircleBCPType::instance(), args );
      }
      else KIG_FILTER_PARSE_ERROR;
    }
    else if ( type == "Line" || type == "Ray" || type == "Seg" ||
              type == "Vec" )
    {
      if ( args.size() == 1 )
      {
        if ( data.size() != 2 ) KIG_FILTER_PARSE_ERROR;
        Coordinate vect( data[0], data[1] );
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
      if ( type == "Line" ) t = LineABType::instance();
      if ( type == "Ray" ) t = RayABType::instance();
      if ( type == "Seg" ) t = SegmentABType::instance();
      if ( type == "Vec" ) t = VectorType::instance();
      assert( t );
      oc = new ObjectTypeCalcer( t, args );
    }
    else if ( type == "Pt/" )
    {
      // different sizes for points..
      thick *= 2;
      if ( args.size() != 1 || data.size() != 2 )
        KIG_FILTER_PARSE_ERROR;
      ObjectCalcer* parent = args[0];
      if ( !parent->imp()->inherits( CurveImp::stype() ) )
        KIG_FILTER_PARSE_ERROR;
      const CurveImp* curve = static_cast<const CurveImp*>( parent->imp() );
      Coordinate pt = Coordinate( data[0], data[1] );
      double param = curve->getParam( pt, *ret );
      args.push_back( new ObjectConstCalcer( new DoubleImp( param ) ) );
      oc = new ObjectTypeCalcer( ConstrainedPointType::instance(), args );
    }
    else if ( type == "Perp" || type == "Par" )
    {
      if ( args.size() != 2 || data.size() != 0 )
        KIG_FILTER_PARSE_ERROR;
      const ObjectType* t = 0;
      if ( type == "Perp" ) t = LinePerpendLPType::instance();
      else if ( type == "Par" ) t = LineParallelLPType::instance();
      else assert( false );
      oc = new ObjectTypeCalcer( t, args );
    }
    else if ( type == "Arc" )
    {
      if ( args.size() != 3 || ! data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( ArcBTPType::instance(), args );
    }
    else if ( type == "Con" )
    {
      if ( args.size() != 5 || !data.empty() )
        KIG_FILTER_PARSE_ERROR;
      oc = new ObjectTypeCalcer( ConicB5PType::instance(), args );
    }
    else if ( type == "Mid" )
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
      if ( args.size() != 1 || !data.empty() )
        KIG_FILTER_PARSE_ERROR;
      ObjectCalcer* parent = args[0];
      const ObjectFactory* fact = ObjectFactory::instance();
      if ( parent->imp()->inherits( SegmentImp::stype() ) )
        oc = fact->propertyObjectCalcer( parent, "mid-point" ) ;
      else if ( parent->imp()->inherits( VectorImp::stype() ) )
        oc = fact->propertyObjectCalcer( parent, "vect-mid-point" );
      else KIG_FILTER_PARSE_ERROR;
    }
    else if ( type == "PBiss" )
    {
      if ( args.size() == 2 )
      {
        ObjectCalcer* c =
          new ObjectTypeCalcer( SegmentABType::instance(), args );
        c->calc( *ret );
        args.clear();
        args.push_back( c );
      }
      if ( args.size() != 1 || !data.empty() )
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
    else KIG_FILTER_PARSE_ERROR;

    if ( oc == 0 ) KIG_FILTER_PARSE_ERROR;

    oc->calc( *ret );
    calcers.push_back( oc );
    ObjectHolder* oh =
      new ObjectHolder( oc, new ObjectDrawer( color, thick, visible ) );
    holders.push_back( oh );
  }

  ret->addObjects( holders );
  return ret;
}

KigFilterCabri* KigFilterCabri::instance()
{
  static KigFilterCabri t;
  return &t;
}


