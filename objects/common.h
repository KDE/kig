// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_COMMON_H
#define KIG_OBJECTS_COMMON_H

#include <set>
#include <vector>
#include <cassert>

#include <QByteArray>
#include <QList>
#include <QStringList>

#include <KI18n/KLocalizedString>

class Coordinate;
class KigDocument;
class KigPainter;
class KigPart;
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
typedef QList<QByteArray> QByteArrayList;

template<typename T>
void delete_all( T begin, T end )
{
  for( ; begin != end; ++begin )
  {
    delete *begin;
  }
}

/**
 * get the calcers that the holders represent and their namecalcers
 */
std::vector<ObjectCalcer*> getAllCalcers( const std::vector<ObjectHolder*>& os );

/**
 * get the calcers that the holders represent ( not their namecalcers )
 */
std::vector<ObjectCalcer*> getCalcers( const std::vector<ObjectHolder*>& os );

/**
 * The below is a trick.  ObjectType's implement the singleton
 * pattern.  There can be only one of them at each time.  This one
 * instance of them needs to be constructed at some time, within the
 * following restrictions:
 *
 * 1. They need to be constructed in the right order: if one ObjectType
 * uses another in its constructor, the other needs to be constructed
 * first.  To achieve this, we use a scheme with \::instance()
 * functions, that have a static variable in the body of the function
 * ( if we would define them static outside of the function body, C++
 * would provide no guarantee on the order they would be called in ).
 *
 * 2. They need to be constructed before Kig tries to use them.  They
 * need to be added to the ObjectTypeFactory before anyone tries to
 * use that class to fetch a type.  The below is a trick to achieve
 * that in combination with the previous.  Basically, we add a fake
 * static instance of an empty class that receives the instance of the
 * ObjectType as an argument to its constructor.  C++ then guarantees
 * that these things will be constructed before main() is entered.
 *
 * Thus, for your own ObjectType classes: use the scheme with the
 * static \::instance methods, and add
 * \code
 * KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( MyObjectType)
 * \endcode
 * to the .cpp file of your class.
 */
class FakeClass {
public:
  FakeClass( const ObjectType* ) {
  }
};

#define KIG_INSTANTIATE_OBJECT_TYPE_INSTANCE( type ) static class FakeClass _fake_class_instance_##type( type::instance() );

#endif
