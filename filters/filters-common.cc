// SPDX-FileCopyrightText: 2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "filters-common.h"

#include <vector>

#include <QByteArray>
#include <QString>

#include "../objects/object_calcer.h"
#include "../objects/object_factory.h"

ObjectTypeCalcer* filtersConstructTextObject(
  const Coordinate& c, ObjectCalcer* o,
  const QByteArray& arg, const KigDocument& doc, bool needframe )
{
  const ObjectFactory* fact = ObjectFactory::instance();
  ObjectCalcer* propo = fact->propertyObjectCalcer( o, arg );
  propo->calc( doc );
  std::vector<ObjectCalcer*> args;
  args.push_back( propo );
  return fact->labelCalcer( QStringLiteral( "%1" ), c, needframe,
                            args, doc );
}
