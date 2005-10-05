// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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

#ifndef KIG_OBJECTS_OBJECT_IMP_FACTORY_H
#define KIG_OBJECTS_OBJECT_IMP_FACTORY_H

#include "common.h"

class ObjectImpFactory
{
  ObjectImpFactory();
  ~ObjectImpFactory();
public:
  static const ObjectImpFactory* instance();
  /**
   * loads data from \p parent , and returns a new ObjectImp from the type
   * string \p type .
   */
  ObjectImp* deserialize( const QString& type, const QDomElement& parent, QString& error ) const;
  /**
   * adds data to \p parent , and returns a type string..
   */
  QString serialize( const ObjectImp& d, QDomElement& parent, QDomDocument& doc ) const;
};

#endif
