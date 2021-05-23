// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

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
   * adds data to \p parent , and returns a type string.
   */
  QString serialize( const ObjectImp& d, QDomElement& parent, QDomDocument& doc ) const;
};

#endif
