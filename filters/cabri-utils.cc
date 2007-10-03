// This file is part of Kig, a KDE program for Interactive Geometry...
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>
// Copyright (C)  2006  Pino Toscano       <toscano.pino@tiscali.it>

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

#include "cabri-utils.h"

#include "cabri-filter.h"

#include <qfile.h>
#include <qregexp.h>
#include <qstring.h>

#include <kdebug.h>
#include <klocale.h>

#define KIG_CABRI_FILTER_PARSE_ERROR \
{ \
  QString locs = i18n( "An error was encountered at line %1 in file %2." ) \
    .arg( __LINE__ ).arg( __FILE__ ); \
  m_filter->publicParseError( file, locs ); \
  return 0; \
}

static std::map<QString, QColor> colormap;
static std::map<QString, QColor> colormap_v12;
typedef std::map<QString, QColor>::iterator cmit;

/**
 * Gives the default color for an object, in case of an object come
 * with no color specified; it's exactly what Cabri 1.2 does.
 */
static QColor defaultColorForObject( const QCString& type )
{
  if ( type == "Line" || type == "Ray" || type == "Seg" )
    return Qt::green;
  if ( type == "Pt" || type == "Pt/" || type == "Locus" )
    return Qt::red;
  if ( type == "Cir" )
    return Qt::blue;
  if ( type == "Pol" )
    return QColor( 254, 0, 255 );
  if ( type == "Arc" )
    return Qt::cyan;
  if ( type == "Vec" )
    return QColor( 0, 0, 128 );
  if ( type == "Text" || type == "Angle" )
    return Qt::black;

  return Qt::blue;
}

static bool extractValuesFromString( const QString& str, std::vector<int>& vec )
{
  if ( !str.isEmpty() )
  {
    QString tmp = str;
    bool ok = true;
    // extracting the digits
    QRegExp ids( "\\d+" );
    int pos = -1;
    while ( ( pos = ids.search( tmp ) ) > -1 )
    {
      vec.push_back( ids.cap( 0 ).toInt( &ok ) );
      if ( !ok ) return false;
      tmp.remove( pos, ids.matchedLength() );
    }
  }
  return true;
}


namespace CabriNS
{
QString readLine( QFile& file )
{
  QString ret;
  file.readLine( ret, 10000L );
  if ( ret[ret.length() - 1] == '\n' )
    ret.truncate( ret.length() - 1 );
  if ( ret[ret.length() - 1] == '\r' )
    ret.truncate( ret.length() - 1 );
  return ret;
}

QString readText( QFile& f, const QString& s, const QString& sep )
{
  QString line = s;
  if ( !line.startsWith( "\"" ) || f.atEnd() )
    // don't blame on failing here
    return QString();

  QString tmp = s;
  QString text = tmp;
  while ( tmp.at( tmp.length() - 1 ) != '"' )
  {
    tmp = readLine( f );
    text += sep + tmp;
  }
  QString ret = text.mid( 1, text.length() - 2 );

kdDebug() << "+++++++++ text: \"" << ret << "\"" << endl;

  return ret;
}

}; // CabriNS

CabriObject::CabriObject()
  : id( 0 ), thick( 1 ), lineSegLength( 0 ), lineSegSplit( 0 ), visible( true ),
    intersectionId( 0 ), ticks( 0 ), side( 0 ), textRect( Rect::invalidRect() ),
    gonio( CabriNS::CG_Deg )
{
}

CabriObject_v10::CabriObject_v10()
  : CabriObject(), specialAppearanceSwitch( 0 ), fixed( false )
{
}

CabriObject_v12::CabriObject_v12()
  : CabriObject(), pointStyle( 1 )
{
}

CabriReader::CabriReader( const KigFilterCabri* filter )
  : m_filter( filter )
{
  initColorMap();
}

void CabriReader::initColorMap()
{
  static bool colors_initialized = false;
  if ( !colors_initialized )
  {
    colors_initialized = true;
    colormap[ "R" ] = Qt::red;
    colormap[ "O" ] = Qt::magenta;
    colormap[ "Y" ] = Qt::yellow;
    colormap[ "P" ] = Qt::darkMagenta;
    colormap[ "V" ] = Qt::darkBlue;
    colormap[ "Bl" ] = Qt::blue;
    colormap[ "lBl" ] = Qt::cyan;
    colormap[ "G" ] = Qt::green;
    colormap[ "dG" ] = Qt::darkGreen;
    colormap[ "Br" ] = QColor( 165, 42, 42 );
    colormap[ "dBr" ] = QColor( 128, 128, 0 );
    colormap[ "lGr" ] = Qt::lightGray;
    colormap[ "Gr" ] = Qt::gray;
    colormap[ "dGr" ] = Qt::darkGray;
    colormap[ "B" ] = Qt::black;
    colormap[ "W" ] = Qt::white;
  }
}

QColor CabriReader::translateColor( const QString& s )
{
  initColorMap();
  cmit it = colormap.find( s );
  if ( it != colormap.end() ) return (*it).second;

  kdDebug() << k_funcinfo << "unknown color: " << s << endl;
  return Qt::black;
}


CabriReader_v10::CabriReader_v10( const KigFilterCabri* filter )
  : CabriReader( filter )
{
}

bool CabriReader_v10::readWindowMetrics( QFile& f )
{
  QString file = f.name();

  QString line = CabriNS::readLine( f );
  QRegExp first( "^Window center x: (.+) y: (.+) Window size x: (.+) y: (.+)$" );
  if ( !first.exactMatch( line ) )
    KIG_CABRI_FILTER_PARSE_ERROR;

  line = CabriNS::readLine( f );

  return true;
}

CabriObject* CabriReader_v10::readObject( QFile& f )
{
  CabriObject_v10* myobj = new CabriObject_v10();

  QString file = f.name();

  bool ok;
  QString tmp;

  QString line1 = CabriNS::readLine( f );
  QRegExp namelinere( "^\"([^\"]+)\", NP: ([\\d-]+), ([\\d-]+), NS: (\\d+), (\\d+)$" );
  if ( namelinere.exactMatch( line1 ) )
  {
    tmp = namelinere.cap( 1 );
    myobj->name = tmp;

    line1 = CabriNS::readLine( f );
  }

  QRegExp firstlinere( "^([^:]+): ([^,]+), ([^,]+), CN:([^,]*), VN:(.*)$" );
  if ( ! firstlinere.exactMatch( line1 ) )
    KIG_CABRI_FILTER_PARSE_ERROR;

  tmp = firstlinere.cap( 1 );
  myobj->id = tmp.toUInt( &ok );
  if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;

  tmp = firstlinere.cap( 2 );
  myobj->type = tmp.latin1();

  tmp = firstlinere.cap( 3 );
  // i have no idea what this number means..

  tmp = firstlinere.cap( 4 );
  uint numberOfParents = tmp.toUInt( &ok );
  if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;

  tmp = firstlinere.cap( 5 );
  // i have no idea what this number means..

  QString line2 = CabriNS::readLine( f );
  QRegExp secondlinere( "^([^,]+), ([^,]+), ([^,]+), DS:([^ ]+) ([^,]+), GT:([^,]+), ([^,]+), (.*)$" );
  if ( ! secondlinere.exactMatch( line2 ) )
    KIG_CABRI_FILTER_PARSE_ERROR;

  tmp = secondlinere.cap( 1 );
  myobj->color = translateColor( tmp );

  tmp = secondlinere.cap( 2 );
  myobj->fillColor = translateColor( tmp );

  tmp = secondlinere.cap( 3 );
  myobj->thick = tmp == "t" ? 1 : tmp == "tT" ? 2 : 3;

  tmp = secondlinere.cap( 4 );
  myobj->lineSegLength = tmp.toInt( &ok );
  if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;

  tmp = secondlinere.cap( 5 );
  myobj->lineSegSplit = tmp.toInt( &ok );
  if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;

  tmp = secondlinere.cap( 6 );
  myobj->specialAppearanceSwitch = tmp.toInt( &ok );
  if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;

  tmp = secondlinere.cap( 7 );
  myobj->visible = tmp == "V";

  tmp = secondlinere.cap( 8 );
  myobj->fixed = tmp == "St";

  QString line3 = CabriNS::readLine( f );
  QRegExp thirdlinere( "^(Const: ([^,]*),? ?)?(Val: ([^,]*)(,(.*))?)?$" );
  if ( ! thirdlinere.exactMatch( line3 ) )
    KIG_CABRI_FILTER_PARSE_ERROR;

  tmp = thirdlinere.cap( 2 );
  QStringList parentsids = QStringList::split( ' ', tmp );
  for ( QStringList::iterator i = parentsids.begin();
        i != parentsids.end(); ++i )
  {
    myobj->parents.push_back( ( *i ).toInt( &ok ) );
    if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;
  }
  if ( myobj->parents.size() != numberOfParents )
    KIG_CABRI_FILTER_PARSE_ERROR;

  tmp = thirdlinere.cap( 4 );
  QStringList valIds = QStringList::split( ' ', tmp );
  for ( QStringList::iterator i = valIds.begin();
        i != valIds.end(); ++i )
  {
    myobj->data.push_back( ( *i ).toDouble( &ok ) );
    if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;
  }

  QString thirdlineextra = thirdlinere.cap( 6 );
  if ( myobj->type == "Text" )
  {
    // per Pino: nel file .fig che ho io ci sono degli spazi dopo le virgole...
    QRegExp textMetrics( "TP: *[\\s]*([^,]+), *[\\s]*([^,]+), *TS:[\\s]*([^,]+), *[\\s]*([^,]+)" );
    if ( textMetrics.search( thirdlineextra ) != -1 )
    {
      double xa = textMetrics.cap( 1 ).toDouble( &ok );
      if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;
      double ya = textMetrics.cap( 2 ).toDouble( &ok );
      if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;
      double tw = textMetrics.cap( 3 ).toDouble( &ok );
      if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;
      double th = textMetrics.cap( 4 ).toDouble( &ok );
      if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;
      myobj->textRect = Rect( xa, ya, tw, th );
    }
  }

  QString line4 = CabriNS::readLine( f );
  if ( !line4.isEmpty() )
  {
    myobj->text = CabriNS::readText( f, line4, "\n" );
    line4 = CabriNS::readLine( f );
  }
  if ( !line4.isEmpty() )
  {
    QRegExp fontlinere( "^p: (\\d+), ([^,]+), S: (\\d+) C: (\\d+) Fa: (\\d+)$" );
    if ( !fontlinere.exactMatch( line4 ) )
      KIG_CABRI_FILTER_PARSE_ERROR;

    line4 = CabriNS::readLine( f );
  }

//     kdDebug()
//       << k_funcinfo << endl
//       << "id = " << myobj->id << endl
//       << "type = " << myobj->type << endl
//       << "numberOfParents = " << numberOfParents << endl
//       << "color = " << myobj->color.name() << endl
//       << "fillcolor = " << myobj->fillColor.name() << endl
//       << "thick = " << myobj->thick << endl
//       << "lineseglength = " << myobj->lineSegLength << endl
//       << "linesegsplit = " << myobj->lineSegSplit << endl
//       << "specialAppearanceSwitch = " << myobj->specialAppearanceSwitch << endl
//       << "visible = " << myobj->visible << endl
//       << "fixed = " << myobj->fixed << endl
//       << "parents =" << endl;
//     for ( std::vector<int>::iterator i = myobj->parents.begin(); i != myobj->parents.end(); ++i )
//       kdDebug() << "	" << *i << endl;
//     kdDebug() << "vals = " << endl;
//     for ( std::vector<double>::iterator i = myobj->data.begin(); i != myobj->data.end(); ++i )
//       kdDebug() << "	" << *i << endl;

  return myobj;
}

void CabriReader_v10::decodeStyle( CabriObject* obj, Qt::PenStyle& ps, int& pointType )
{
  CabriObject_v10* myobj = (CabriObject_v10*)obj;

  if ( ( myobj->type == "Pt" ) || ( myobj->type == "Pt/" ) )
  {
    switch ( myobj->specialAppearanceSwitch )
    {
      case 0:
      {
        myobj->thick -= 1;
        break;
      }
      case 2:
      {
        myobj->thick += 1;
        break;
      }
      case 3:
      {
        myobj->thick += 1;
        pointType = 1;
        break;
      }
      case 4:
      {
        myobj->thick += 2;
        pointType = 4;
        break;
      }
    }
    myobj->thick *= 2;
  }
  else
  {
    if ( ( myobj->lineSegLength > 1 ) && ( myobj->lineSegLength < 6 ) &&
         ( myobj->lineSegSplit > 1 ) && ( myobj->lineSegSplit <= 10 ) )
      ps = Qt::DotLine;
    else if ( ( myobj->lineSegLength >= 6 ) && ( myobj->lineSegSplit > 10 ) )
      ps = Qt::DashLine;
  }
}

CabriReader_v12::CabriReader_v12( const KigFilterCabri* filter )
  : CabriReader( filter )
{
  initColorMap();
}

void CabriReader_v12::initColorMap()
{
  static bool colors_initialized = false;
  if ( !colors_initialized )
  {
    colors_initialized = true;
    CabriReader::initColorMap();
    colormap_v12 = colormap;
    colormap_v12[ "dkg" ] = QColor( 0, 100, 0 );
    colormap_v12[ "old" ] = QColor( 107, 142, 35 );
    colormap_v12[ "olv" ] = QColor( 128, 128, 0 );
    colormap_v12[ "lig" ] = QColor( 50, 205, 50 );
    colormap_v12[ "gry" ] = QColor( 173, 255, 47 );
    colormap_v12[ "gor" ] = QColor( 218, 165, 32 );
    colormap_v12[ "msg" ] = QColor( 0, 250, 154 );
    colormap_v12[ "spg" ] = QColor( 0, 255, 127 );
    colormap_v12[ "pag" ] = QColor( 152, 251, 152 );
    colormap_v12[ "kki" ] = QColor( 240, 230, 140 );
    colormap_v12[ "O" ] = QColor( 255, 140, 0 );
    colormap_v12[ "Br" ] = QColor( 165, 42, 42 );
    colormap_v12[ "tea" ] = QColor( 0, 128, 128 );
    colormap_v12[ "pat" ] = QColor( 175, 238, 238 );
    colormap_v12[ "ltp" ] = QColor( 255, 182, 193 );
    colormap_v12[ "dBr" ] = QColor( 128, 0, 0 );
    colormap_v12[ "lsg" ] = QColor( 32, 178, 170 );
    colormap_v12[ "dob" ] = QColor( 30, 144, 255 );
    colormap_v12[ "skb" ] = QColor( 135, 206, 235 );
    colormap_v12[ "plm" ] = QColor( 221, 160, 221 );
    colormap_v12[ "dep" ] = QColor( 255, 20, 147 );
    colormap_v12[ "crs" ] = QColor( 220, 20, 60 );
    colormap_v12[ "rob" ] = QColor( 65, 105, 225 );
    colormap_v12[ "blv" ] = QColor( 138, 43, 226 );
    colormap_v12[ "ma" ] = QColor( 254, 0, 255 );
    colormap_v12[ "mvr" ] = QColor( 199, 21, 133 );
    colormap_v12[ "ind" ] = QColor( 75, 0, 130 );
    colormap_v12[ "meo" ] = QColor( 186, 85, 211 );
    colormap_v12[ "Gr" ] = QColor( 128, 128, 128 );
    colormap_v12[ "dGr" ] = QColor( 64, 64, 64 );
  }
}

bool CabriReader_v12::readWindowMetrics( QFile& f )
{
  QString file = f.name();

  QString line = CabriNS::readLine( f );
  QRegExp first( "^Window center x: (.+) y: (.+) Window size x: (.+) y: (.+)$" );
  if ( !first.exactMatch( line ) )
    KIG_CABRI_FILTER_PARSE_ERROR;

  QString line2 = CabriNS::readLine( f );
  QRegExp second( "^Resolution: (\\d+) ppc$" );
  if ( !second.exactMatch( line2 ) )
    KIG_CABRI_FILTER_PARSE_ERROR;

  line = CabriNS::readLine( f );

  return true;
}

CabriObject* CabriReader_v12::readObject( QFile& f )
{
  CabriObject_v12* myobj = new CabriObject_v12();

  QString file = f.name();
  QString line = CabriNS::readLine( f );

#ifdef CABRI_DEBUG
  kdDebug() << "+++++++++ line: \"" << line << "\"" << endl;
#endif
  QRegExp firstlinere( "^([^:]+): ([^,]+), (Const: [,0-9\\s]+)?(int ind:([^,]+),\\s)?(cart, )?(side:(\\d+), )?(inc\\.elmts: ([,0-9\\s]+))?(axis:(x|y), )?(on mark, )?(Val: ([^,]+))?(.*)$" );
  if ( !firstlinere.exactMatch( line ) )
    KIG_CABRI_FILTER_PARSE_ERROR;

  bool ok = true;
  QString tmp;

  // id
  tmp = firstlinere.cap( 1 );
  myobj->id = tmp.toUInt( &ok );
  if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;

  // type
  tmp = firstlinere.cap( 2 );
  myobj->type = tmp.latin1();

  // parents
  tmp = firstlinere.cap( 3 );
  if ( !extractValuesFromString( tmp, myobj->parents ) )
    KIG_CABRI_FILTER_PARSE_ERROR;

  // datas
  tmp = firstlinere.cap( 15 );
  QStringList valIds = QStringList::split( ' ', tmp );
  for ( QStringList::iterator i = valIds.begin(); i != valIds.end(); ++i )
  {
    myobj->data.push_back( ( *i ).toDouble( &ok ) );
    if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;
  }

  // intersection id (if present)
  tmp = firstlinere.cap( 5 );
  if ( !tmp.isEmpty() )
  {
    long intId = tmp.toLong( &ok, 16 );
    if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;
#ifdef CABRI_DEBUG
    kdDebug() << "+++++++++ intId: " << intId << endl;
#endif
    if ( intId == 0 ) myobj->intersectionId = -1;
    else if ( intId == 0x10000 ) myobj->intersectionId = 1;
    else KIG_CABRI_FILTER_PARSE_ERROR;
  }

  // side of a polygon (if present)
  tmp = firstlinere.cap( 8 );
  if ( !tmp.isEmpty() )
  {
    myobj->side = tmp.toInt( &ok );
    if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;
  }

  // inc.elements (if present)
  tmp = firstlinere.cap( 10 );
  if ( !extractValuesFromString( tmp, myobj->incs ) )
    KIG_CABRI_FILTER_PARSE_ERROR;

  line = CabriNS::readLine( f );
  while ( !line.isEmpty() )
  {
    QString txt;
    if ( !( txt = CabriNS::readText( f, line ) ).isEmpty() )
    {
      myobj->text = txt;
      if ( myobj->type != "Text" )
        myobj->name = txt;
      // text label metrics
      line = CabriNS::readLine( f );
      QRegExp textMetrics( "^TP:[\\s]*([^,]+),[\\s]*([^,]+), TS:[\\s]*([^,]+),[\\s]*([^,]+)$" );
      if ( !textMetrics.exactMatch( line ) )
        KIG_CABRI_FILTER_PARSE_ERROR;
      double xa = textMetrics.cap( 1 ).toDouble( &ok );
      if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;
      double ya = textMetrics.cap( 2 ).toDouble( &ok );
      if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;
      double tw = textMetrics.cap( 3 ).toDouble( &ok );
      if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;
      double th = textMetrics.cap( 4 ).toDouble( &ok );
      if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;
      myobj->textRect = Rect( xa, ya, tw, th );
      // font settings
      line = CabriNS::readLine( f );
    }
    else
    {
      if ( !readStyles( file, line, myobj ) )
        return 0;
    }

    line = CabriNS::readLine( f );
  }

  // default color
  if ( !myobj->color.isValid() )
    myobj->color = defaultColorForObject( myobj->type );

  return myobj;
}

void CabriReader_v12::decodeStyle( CabriObject* obj, Qt::PenStyle& ps, int& pointType )
{
  CabriObject_v12* myobj = (CabriObject_v12*)obj;

  if ( ( myobj->type == "Pt" ) || ( myobj->type == "Pt/" ) )
  {
      // different sizes for points..
      myobj->thick *= 2;
      switch ( myobj->pointStyle )
      {
        case 0:
        {
          myobj->thick /= 2;
          break;
        }
        case 1:
        {
          pointType = 2;
          break;
        }
        case 2:
        {
          pointType = 1;
          break;
        }
        case 3:
        {
          pointType = 4;
          break;
        }
        case 4:
        {
          break;
        }
      }
  }
  else
  {
    if ( ( myobj->lineSegLength > 1 ) && ( myobj->lineSegLength < 6 ) &&
         ( myobj->lineSegSplit > 1 ) && ( myobj->lineSegSplit <= 10 ) )
      ps = Qt::DotLine;
    else if ( ( myobj->lineSegLength >= 6 ) && ( myobj->lineSegSplit > 10 ) )
      ps = Qt::DashLine;
  }
}

QColor CabriReader_v12::translateColor( const QString& s )
{
  initColorMap();
  cmit it = colormap_v12.find( s );
  if ( it != colormap_v12.end() ) return (*it).second;

  kdDebug() << k_funcinfo << "unknown color: " << s << endl;
  return CabriReader::translateColor( s );
}

bool CabriReader_v12::readStyles( const QString& file, const QString& line, CabriObject_v12* myobj )
{
#ifdef CABRI_DEBUG
  kdDebug() << ">>>>>>>>> style line: \"" << line << "\"" << endl;
#endif
  QStringList styles = QStringList::split( ", ", line );
  bool ok = true;
  for ( QStringList::iterator it = styles.begin(); it != styles.end(); ++it )
  {
    if ( (*it) == "invisible" )
    {
      myobj->visible = false;
    }
    else if ( (*it).startsWith( "DS:" ) )
    {
      QRegExp ticks( "DS:(\\d+)\\s(\\d+)" );
      if ( ticks.exactMatch( (*it) ) )
      {
        myobj->lineSegLength = ticks.cap( 1 ).toInt( &ok );
        if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;
        myobj->lineSegSplit = ticks.cap( 2 ).toInt( &ok );
        if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;
      }
    }
    // colors
    else if ( (*it).startsWith( "color:" ) )
    {
      QString color = (*it).remove( 0, 6 );
      myobj->color = translateColor( color );
    }
    else if ( (*it).startsWith( "fill color:" ) )
    {
      QString color = (*it).remove( 0, 11 );
      myobj->fillColor = translateColor( color );
    }
    // styles
    else if ( (*it) == "thicker" )
    {
      myobj->thick = 2;
    }
    else if ( (*it) == "thick" )
    {
      myobj->thick = 3;
    }
    // point types
    else if ( (*it) == "1x1" )
    {
      myobj->pointStyle = 0;
    }
    else if ( (*it) == "CIRCLE" )
    {
      myobj->pointStyle = 2;
    }
    else if ( (*it) == "TIMES" )
    {
      myobj->pointStyle = 3;
    }
    else if ( (*it) == "PLUS" )
    {
      myobj->pointStyle = 4;
    }
    // goniometry systems
    else if ( (*it) == "deg" )
    {
      myobj->gonio = CabriNS::CG_Deg;
    }
    // object ticks
    else if ( (*it).startsWith( "marks nb:" ) )
    {
      QString strticks = (*it).remove( 0, 9 );
      myobj->ticks = strticks.toInt( &ok );
      if ( !ok ) KIG_CABRI_FILTER_PARSE_ERROR;
    }
#ifdef CABRI_DEBUG
    else
    {
      kdDebug() << ">>>>>>>>> UNKNOWN STYLE STRING: \"" << *it << "\"" << endl;
    }
#endif
  }
  return true;
}
