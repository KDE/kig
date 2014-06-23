/*
 * GeoGebra Filter for Kig
 * Copyright 2013  David E. Narvaez <david.narvaez@computer.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "geogebra-filter.h"

#include <kig/kig_document.h>
#include <objects/object_factory.h>
#include <objects/object_calcer.h>
#include <objects/object_holder.h>
#include <objects/bogus_imp.h>
#include <objects/point_type.h>
#include <objects/line_type.h>
#include <objects/circle_type.h>
#include <objects/conic_types.h>
#include <objects/arc_type.h>

#include <KZip>
#include <KDebug>

#include <QFile>
#include <QXmlQuery>
#include <QXmlName>

KigFilterGeogebra* KigFilterGeogebra::instance()
{
  static KigFilterGeogebra f;
  return &f;
}

bool KigFilterGeogebra::supportMime( const QString& mime )
{
  return mime == "application/vnd.geogebra.file";
}

KigDocument* KigFilterGeogebra::load( const QString& sFrom )
{
  KZip geogebraFile( sFrom );

  m_document = new KigDocument();

  if ( geogebraFile.open( QIODevice::ReadOnly ) )
  {
    const KZipFileEntry* geogebraXMLEntry = dynamic_cast<const KZipFileEntry*>( geogebraFile.directory()->entry( "geogebra.xml" ) );

    if ( geogebraXMLEntry )
    {
      QXmlQuery geogebraXSLT( QXmlQuery::XSLT20, m_np );
      const QString encodedData = QString::fromUtf8( geogebraXMLEntry->data().constData() );
      QFile queryDevice( ":/kig/geogebra/geogebra.xsl" );

      m_currentState = KigFilterGeogebra::ReadingObject;

      queryDevice.open( QFile::ReadOnly );
      geogebraXSLT.setFocus( encodedData );
      geogebraXSLT.setQuery( &queryDevice );
      geogebraXSLT.evaluateTo( this );
    }
  }
  else
  {
    kWarning() << "Failed to open zip archive";
  }

  return m_document;
}

void KigFilterGeogebra::atomicValue( const QVariant& )
{
  // I wish I could use this...
}

void KigFilterGeogebra::attribute( const QXmlName& name, const QStringRef& value )
{
  if( name.localName( m_np ) == QLatin1String( "label" ) )
  {
    const QByteArray objectLabel = value.toAscii();
    bool isDoubleValue;
    const double dblval = value.toString().toDouble( &isDoubleValue );

    switch( m_currentState )
    {
    case KigFilterGeogebra::ReadingObject:
      m_currentObjectLabel = value.toAscii();
      // There must be a better place for this
      m_currentState = KigFilterGeogebra::ReadingArguments;

      break;
    case KigFilterGeogebra::ReadingArguments:
      if( isDoubleValue )
      {
        /* This is to handle the circle-point-radius (and similar) type of Geogebra objects.
         * <command name="Circle">
         * <input a0="A" a1="3"/>
         * <output a0="c"/>
         *
         * Notice the attribute 'a1' of the 'input' element. The value - '3' is the radius of the circle.
         * First, we try to convert that value to Double. If the conversion suceeds, we stack a DoubleImp (Calcer)
         * in the the m_currentArgStack and break. Otherwise, we check the m_objectMap for that label entry.
         */
        DoubleImp * doubleImp = new DoubleImp( dblval );
        m_currentArgStack.push_back( new ObjectConstCalcer( doubleImp ) );
      }
      else if( m_objectMap.contains( objectLabel ) )
      {
        m_currentArgStack.push_back( m_objectMap[objectLabel] );
      }
      else
      {
        parseError( i18n( "Referenced object %1 does not exist", value.toString() ) );
      }

      break;
    default:
      break;
    }
  }
  else if ( name.localName( m_np ) == QLatin1String( "value" ) )
  {
    Q_ASSERT( m_currentState == ReadingDouble );
    DoubleImp * doubleImp = new DoubleImp( value.toString().toDouble() );

    m_currentArgStack.push_back( new ObjectConstCalcer( doubleImp ) );
  }
}

void KigFilterGeogebra::characters( const QStringRef& )
{
}

void KigFilterGeogebra::comment( const QString& )
{
}

void KigFilterGeogebra::endDocument()
{
}

void KigFilterGeogebra::endElement()
{
  switch( m_currentState )
  {
  case KigFilterGeogebra::ReadingObject:
    if ( m_currentObject )
    {
      if ( !m_objectMap.contains( m_currentObjectLabel ) )
      {
        ObjectTypeCalcer * oc = new ObjectTypeCalcer( m_currentObject, m_currentArgStack );
        ObjectHolder * oh = new ObjectHolder( oc );

        oc->calc( *m_document );
        m_objectMap.insert( m_currentObjectLabel, oc );
        m_document->addObject( oh );
      }

      m_currentArgStack.clear();
      m_currentObject = nullptr;
    }

    break;
  case KigFilterGeogebra::ReadingArguments:
  case KigFilterGeogebra::ReadingDouble:
    /* We do this because every argument may be the
     * last argument of the stack. If it is not, then
     * startElement() will reset this to ReadingArguments
     */
    m_currentState = KigFilterGeogebra::ReadingObject;

    break;
  default:
    break;
  }
}

void KigFilterGeogebra::endOfSequence()
{
}

void KigFilterGeogebra::namespaceBinding( const QXmlName& )
{
}

void KigFilterGeogebra::processingInstruction( const QXmlName&, const QString& )
{
}

void KigFilterGeogebra::startDocument()
{
}

void KigFilterGeogebra::startElement( const QXmlName& name )
{
  switch( m_currentState )
  {
  case KigFilterGeogebra::ReadingObject:
    if( m_currentObject )
    {
      // We are already building an object
      m_currentState = KigFilterGeogebra::ReadingArguments;
      startElement( name );
      return;
    }

    if( name.localName( m_np ) == QLatin1String( "FixedPoint" ) )
    {
      m_currentObject = FixedPointType::instance();
    }
    else if( name.localName( m_np ) == QLatin1String( "LineAB" ) )
    {
      m_currentObject = LineABType::instance();
    }
    else if( name.localName( m_np ) == QLatin1String( "LineParallelLPType" ) )
    {
      m_currentObject = LineParallelLPType::instance();
    }
    else if( name.localName( m_np ) == QLatin1String( "SegmentAB" ) )
    {
      m_currentObject = SegmentABType::instance();
    }
    else if( name.localName( m_np ) == QLatin1String( "RayAB" ) )
    {
      m_currentObject = RayABType::instance();
    }
    else if( name.localName( m_np ) == QLatin1String( "Midpoint" ) )
    {
      m_currentObject = MidPointType::instance();
    }
    else if( name.localName( m_np ) == QLatin1String( "LinePerpend" ) )
    {
      m_currentObject = LinePerpendLPType::instance();
    }
    else if( name.localName( m_np ) == QLatin1String( "CircleBCPType" ) )
    {
      m_currentObject = CircleBCPType::instance();
    }
    else if( name.localName( m_np ) == QLatin1String( "CircleBPRType" ) )
    {
      m_currentObject = CircleBPRType::instance();
    }
    else if( name.localName( m_np ) == QLatin1String( "CircleBTPType" ) )
    {
      m_currentObject = CircleBTPType::instance();
    }
    else if( name.localName( m_np ) == QLatin1String( "ArcBTPType" ) )
    {
      m_currentObject = ArcBTPType::instance();
    }
    else if( name.localName( m_np ) == QLatin1String( "ParabolaBDPType" ) )
    {
      m_currentObject = ParabolaBDPType::instance();
    }
    else if( name.localName( m_np ) == QLatin1String( "EllipseBFFPType" ) )
    {
      m_currentObject = EllipseBFFPType::instance();
    }
    else if( name.localName( m_np ) == QLatin1String( "HyperbolaBFFPType" ) )
    {
      m_currentObject = HyperbolaBFFPType::instance();
    }
    else if( name.localName( m_np ) == QLatin1String( "ConicB5PType" ) )
    {
      m_currentObject = ConicB5PType::instance();
    }

    break;
  case KigFilterGeogebra::ReadingArguments:
    if ( name.localName( m_np ) == QLatin1String( "Double" ) )
    {
      m_currentState = KigFilterGeogebra::ReadingDouble;
    }

    break;
  default:
    break;
  }
}

void KigFilterGeogebra::startOfSequence()
{
}

