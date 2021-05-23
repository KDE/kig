/*
    GeogebraSection class for Kig
    SPDX-FileCopyrightText: 2014 Aniket Anvit <seeanvit@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "geogebrasection.h"

const QString & GeogebraSection::getName() const
{
  return m_name;
}

void GeogebraSection::setName( const QString& name )
{
  m_name = name;
}

const QString & GeogebraSection::getDescription() const
{
  return m_description;
}

void GeogebraSection::setDescription( const QString& desc )
{
  m_description = desc;
}

const std::vector< ObjectCalcer* > & GeogebraSection::getInputObjects() const
{
  return m_inputObjects;
}

void GeogebraSection::addInputObject( ObjectCalcer* inp )
{
  m_inputObjects.push_back( inp );
}

const std::vector< ObjectCalcer* > & GeogebraSection::getOutputObjects() const
{
  return m_outputObjects;
}

void GeogebraSection::addOutputObject( ObjectCalcer* op )
{
  m_outputObjects.push_back( op );
}

const std::vector< ObjectDrawer* > & GeogebraSection::getDrawers() const
{
  return m_drawers;
}

void GeogebraSection::addDrawer( ObjectDrawer* od )
{
  m_drawers.push_back( od );
}
