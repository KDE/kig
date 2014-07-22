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

#ifndef GEOGEBRASECTION_H
#define GEOGEBRASECTION_H

#include "../objects/object_calcer.h"

/* This class serves the purpose of storing the information about Geogebra-Tool ( i.e name of the tool, description
 * about what the tool does etc.., what objects are to be taken by the tool as input, and what output objects are to be drawn )
 * or about the objects contained in the Geogebra-Worksheet ( while reading a worksheet ).
 */
class GeogebraSection
{
public:
    GeogebraSection() :
    m_name( QString() ),
    m_description( QString() )
    { }
    ~GeogebraSection() { }

    const QString & getName() const;
    const QString & getDescription() const;
    const std::vector<ObjectCalcer*> & getInputObjects() const;
    const std::vector<ObjectCalcer*> & getOutputObjects() const;
    void addInputObject( ObjectCalcer* );
    void addOutputObject( ObjectCalcer* );
    void setDescription( const QString& );
    void setName( const QString& );
private:
    QString m_name;
    QString m_description;
    std::vector<ObjectCalcer*> m_inputObjects;
    std::vector<ObjectCalcer*> m_outputObjects;
};

#endif //GEOGEBRASECTION_H
