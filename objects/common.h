// common.h
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef KIG_OBJECTS_COMMON_H
#define KIG_OBJECTS_COMMON_H

#include <assert.h>
#include <vector>
#include <utility>
#include <qcstring.h>
#include <qvaluelist.h>
#include "../misc/objects.h"
#include "../misc/i18n.h"

class Object;
class RealObject;
class DataObject;
class ObjectImp;
class ObjectType;
class CustomType;
class Property;
class KigPainter;
class Coordinate;
class ScreenInfo;
class Rect;
class KigWidget;
class KigDocument;
class Transformation;
class QDomElement;
class QDomDocument;

typedef std::vector<const ObjectImp*> Args;
typedef QValueList<QCString> QCStringList;

#endif
