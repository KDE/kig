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

#include <set>
#include <vector>
#include <qcstring.h>
#include <qvaluelist.h>
#include <qstringlist.h>
#include <cassert>
#include "../misc/i18n.h"

class Coordinate;
class KigDocument;
class KigPainter;
class KigWidget;
class NormalMode;
class ObjectCalcer;
class ObjectDrawer;
class ObjectHolder;
class ObjectImp;
class ObjectImpType;
class ObjectPropertyCalcer;
class ObjectType;
class ObjectTypeCalcer;
class QDomDocument;
class QDomElement;
class Rect;
class ScreenInfo;
class Transformation;

typedef std::vector<const ObjectImp*> Args;
typedef QValueList<QCString> QCStringList;

template<typename T>
void vect_remove( std::vector<T>& v, const T& t )
{
  typename std::vector<T>::iterator new_end = std::remove( v.begin(), v.end(), t );
  v.erase( new_end, v.end() );
}

template<typename T>
void delete_all( T begin, T end )
{
  for( ;begin != end; ++begin )
  {
    delete *begin;
  }
}

std::vector<ObjectCalcer*> getCalcers( const std::vector<ObjectHolder*>& os );

#endif
