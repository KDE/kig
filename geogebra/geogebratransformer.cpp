/*
    GeogebraTransformer class for Kig
    SPDX-FileCopyrightText: 2014 David E. Narvaez <david.narvaez@computer.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "geogebratransformer.h"

#include <kig/kig_document.h>
#include <objects/object_factory.h>
#include <objects/object_calcer.h>
#include <objects/object_holder.h>
#include <objects/object_drawer.h>
#include <objects/bogus_imp.h>
#include <objects/object_type_factory.h>
#include <misc/point_style.h>

#include <KZip>
#include <QDebug>

#include <QFile>
#include <QXmlQuery>
#include <QXmlName>
#include <QColor>

void GeogebraTransformer::atomicValue( const QVariant& )
{
  // I wish I could use this...
}

void GeogebraTransformer::attribute( const QXmlName& name, const QStringRef& value )
{
  if( name.localName( m_np ) == QLatin1String( "label" ) )
  {
    const QByteArray objectLabel = value.toLatin1();
    bool isDoubleValue;
    const double dblval = value.toString().toDouble( &isDoubleValue );

    switch( m_currentState )
    {
    case GeogebraTransformer::ReadingObject:
      m_currentObjectLabel = value.toLatin1();
      // There must be a better place for this
      m_currentState = GeogebraTransformer::ReadingArguments;

      break;
    case GeogebraTransformer::ReadingArguments:
      if( isDoubleValue )
      {
        /* This is to handle the circle-point-radius, dilate (and similar) type of Geogebra objects.
         * <command name="Circle">
         * <input a0="A" a1="3"/>
         * <output a0="c"/>
         *
         * Notice the attribute 'a1' of the 'input' element. The value - '3' is the radius of the circle.
         * First, we try to convert that value to Double. If the conversion suceeds, we stack a DoubleImp (Calcer)
         * in the m_currentArgStack and break. Otherwise, we check the m_objectMap for that label entry.
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
        //TODO Figure out error reporting
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
  else if ( name.localName( m_np ) == QLatin1String( "Name" ) )
  {
    m_sections[m_nsections - 1].setName( value.toString() );
  }
  else if ( name.localName( m_np ) == QLatin1String( "Description" ) )
  {
    m_sections[m_nsections - 1].setDescription( value.toString() );
  }
  else if ( name.localName( m_np ) == QLatin1String( "Input" ) )
  {
    m_inputObjectLabels.insert( value.toLatin1() );
  }
  else if ( name.localName( m_np ) == QLatin1String( "Output" ) )
  {
    m_outputObjectLabels.insert( value.toLatin1() );
  }
  else if( name.localName( m_np ) == QLatin1String( "show" ) )
  {
    m_show = ( ( value.toString()==QLatin1String("true") ) ? true:false );
  }
  else if( name.localName( m_np ) == QLatin1String( "thickness" ) )
  {
    m_thickness = value.toString().toInt();
  }
  else if( name.localName( m_np ) == QLatin1String( "thickness_point" ) )
  {
    m_thickness = value.toString().toInt();
    m_thickness += 6;
  }
  else if( name.localName( m_np ) == QLatin1String( "type" ) )
  {
    int penType = value.toString().toInt() ;
    switch( penType )
    {
    case SOLIDLINE:
      m_type = Qt::SolidLine;
      break;
    case DASHDOTDOTLINE:
      m_type = Qt::DashDotDotLine;
      break;
    case DASHLINE:
      m_type = Qt::DashLine;
      break;
    case DOTLINE:
      m_type = Qt::DotLine;
      break;
    case DASHDOTLINE:
      m_type = Qt::DashDotLine;
      break;
    default:
      m_type = Qt::SolidLine;
    };
  }
  else if( name.localName( m_np ) == QLatin1String( "pointType" ) )
  {
    int pt = value.toString().toInt();
    if( pt == SOLIDCIRCLEPOINT )
      m_pointType = Kig::pointStyleFromString( QStringLiteral("Round") );
    else if( pt == SOLIDDIAMONDPOINT || pt == UPARROWPOINT || pt == DOWNARROWPOINT || pt == RIGHTARROWPOINT || pt == LEFTARROWPOINT )
      m_pointType = Kig::pointStyleFromString( QStringLiteral("Rectangular") );
    else if( pt == HOLLOWCIRCLEPOINT )
      m_pointType = Kig::pointStyleFromString( QStringLiteral("Round") );//TODO should be mapped to RoundEmpty ( i.e. 1) but for some reason it is not drawing in KIG
    else if( pt == HOLLOWDIAMONDPOINT )
      m_pointType = Kig::pointStyleFromString( QStringLiteral("Rectangular") );//TODO should be mapped to RectangularEmpty ( i.e. 3) but for some reason it is not drawing in KIG
    else if( pt == CROSSPOINT  || pt == PLUSPOINT )
      m_pointType = Kig::pointStyleFromString( QStringLiteral("Cross") );
    else
      m_pointType = Kig::Round;
  }
  else if( name.localName( m_np ) == QLatin1String( "r" ) )
  {
    m_r = value.toString().toInt();
  }
  else if( name.localName( m_np ) == QLatin1String( "g" ) )
  {
    m_g = value.toString().toInt();
  }
  else if( name.localName( m_np ) == QLatin1String( "b" ) )
  {
    m_b = value.toString().toInt();
  }
  else if( name.localName( m_np ) == QLatin1String( "alpha" ) )
  {
    m_alpha = value.toString().toInt();
  }
  else if( name.localName( m_np ) == QLatin1String( "axes" ) )
  {
    bool showAxes = value.toString() == QLatin1String("true") ? true:false;
    m_document->setAxes( showAxes );
  }
  else if( name.localName( m_np ) == QLatin1String( "grid" ) )
  {
    bool showGrid = value.toString() == QLatin1String("true") ? true:false;
    m_document->setGrid( showGrid );
  }
}

void GeogebraTransformer::characters( const QStringRef& )
{
}

void GeogebraTransformer::comment( const QString& )
{
}

void GeogebraTransformer::endDocument()
{
}

void GeogebraTransformer::endElement()
{
  switch( m_currentState )
  {
  case GeogebraTransformer::ReadingObject:
    if ( m_currentObject )
    {
      if ( !m_objectMap.contains( m_currentObjectLabel ) )
      {
        ObjectTypeCalcer * oc = new ObjectTypeCalcer( m_currentObject, m_currentArgStack );

        oc->calc( *m_document );
        m_objectMap.insert( m_currentObjectLabel, oc );

        // Decide where to put this object
        if( m_inputObjectLabels.empty() )
        {
          // Not handling input/output objects, put everything in second
          ObjectDrawer* od = new ObjectDrawer( QColor( m_r, m_g, m_b ), m_thickness, m_show, m_type, m_pointType );

          m_sections[m_nsections - 1].addOutputObject( oc );
          m_sections[m_nsections - 1].addDrawer( od );
        }
        else
        {
          if( m_inputObjectLabels.contains( m_currentObjectLabel ) )
          {
            m_sections[m_nsections - 1].addInputObject( oc );
          }
          else if ( m_outputObjectLabels.contains( m_currentObjectLabel ) )
          {
            m_sections[m_nsections - 1].addOutputObject( oc );
          }
        }
      }

      m_currentArgStack.clear();
      m_currentObject = nullptr;
    }

    break;
  case GeogebraTransformer::ReadingArguments:
  case GeogebraTransformer::ReadingDouble:
    /* We do this because every argument may be the
     * last argument of the stack. If it is not, then
     * startElement() will reset this to ReadingArguments
     */
    m_currentState = GeogebraTransformer::ReadingObject;

    break;
  default:
    break;
  }
}

void GeogebraTransformer::endOfSequence()
{
}

void GeogebraTransformer::namespaceBinding( const QXmlName& )
{
}

void GeogebraTransformer::processingInstruction( const QXmlName&, const QString& )
{
}

void GeogebraTransformer::startDocument()
{
}

void GeogebraTransformer::startElement( const QXmlName& name )
{
  if( name.localName( m_np ) == QLatin1String("Section") )
  {
    m_nsections++;
    m_sections.push_back( GeogebraSection() );

    // Clear stacks
    m_inputObjectLabels.clear();
    m_outputObjectLabels.clear();
    m_currentArgStack.clear();
    m_objectMap.clear();
    return;
  }

  switch( m_currentState )
  {
  case GeogebraTransformer::ReadingObject:
    if( m_currentObject )
    {
      // We are already building an object
      m_currentState = GeogebraTransformer::ReadingArguments;
      startElement( name );
      return;
    }

    {
      resetDrawerVars();
      const QByteArray nameData = name.localName( m_np ).toLatin1();
      m_currentObject = ObjectTypeFactory::instance()->find( nameData );

      if ( !m_currentObject )
      {
        qWarning() << name.localName( m_np ) << " object not found!";
      }
    }

    break;
  case GeogebraTransformer::ReadingArguments:
    if ( name.localName( m_np ) == QLatin1String( "Double" ) )
    {
      m_currentState = GeogebraTransformer::ReadingDouble;
    }

    break;
  default:
    break;
  }
}

void GeogebraTransformer::startOfSequence()
{
}
