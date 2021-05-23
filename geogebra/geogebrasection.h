/*
    GeogebraSection class for Kig
    SPDX-FileCopyrightText: 2014 Aniket Anvit <seeanvit@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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
    const std::vector<ObjectDrawer*> & getDrawers() const;
    void addInputObject( ObjectCalcer* );
    void addOutputObject( ObjectCalcer* );
    void addDrawer( ObjectDrawer* );
    void setDescription( const QString& );
    void setName( const QString& );
private:
    QString m_name;
    QString m_description;
    std::vector<ObjectCalcer*> m_inputObjects;
    std::vector<ObjectCalcer*> m_outputObjects;
    /* There will be exactly one drawer in m_drawers as there will be output objects in m_outputObjects
     * but we can't use a vector-pair of ObjectDrawer* and ObjectCalcer*
     * ( std::vector< std::pair< ObjectCalcer*, ObjectDrawer* > > )
     * since we need a separate vector containing just the outputObjects for building the ObjectHierarchy
     * needed while reading Geogebra-tools.
     */
    std::vector<ObjectDrawer*> m_drawers;
};

#endif //GEOGEBRASECTION_H
