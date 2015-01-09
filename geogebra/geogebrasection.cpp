/*
 * GeogebraSection class for Kig
 * Copyright 2014  Aniket Anvit <seeanvit@gmail.com>
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
