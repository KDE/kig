// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_TEXT_IMP_H
#define KIG_OBJECTS_TEXT_IMP_H

#include "object_imp.h"

#include "../misc/coordinate.h"
#include "../misc/rect.h"

class TextImp
  : public ObjectImp
{
  QString mtext;
  Coordinate mloc;
  bool mframe;
  // with this var, we keep track of the place we drew in, for use in
  // the contains() function..
  mutable Rect mboundrect;
public:
  typedef ObjectImp Parent;
  static const ObjectImpType* stype();

  Coordinate attachPoint() const Q_DECL_OVERRIDE;
  TextImp( const QString& text, const Coordinate& loc, bool frame = false );
  TextImp* copy() const Q_DECL_OVERRIDE;
  ~TextImp();

  ObjectImp* transform( const Transformation& ) const Q_DECL_OVERRIDE;

  void draw( KigPainter& p ) const Q_DECL_OVERRIDE;
  bool contains( const Coordinate& p, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  bool inRect( const Rect& r, int width, const KigWidget& ) const Q_DECL_OVERRIDE;
  bool valid() const;
  Rect surroundingRect() const Q_DECL_OVERRIDE;

  int numberOfProperties() const Q_DECL_OVERRIDE;
  const QByteArrayList properties() const Q_DECL_OVERRIDE;
  const QByteArrayList propertiesInternalNames() const Q_DECL_OVERRIDE;
  ObjectImp* property( int which, const KigDocument& w ) const Q_DECL_OVERRIDE;
  const char* iconForProperty( int which ) const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirementForProperty( int which ) const Q_DECL_OVERRIDE;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const Q_DECL_OVERRIDE;

  const ObjectImpType* type() const Q_DECL_OVERRIDE;
  void visit( ObjectImpVisitor* vtor ) const Q_DECL_OVERRIDE;

  QString text() const;
  const Coordinate coordinate() const;
  bool hasFrame() const;

  bool equals( const ObjectImp& rhs ) const Q_DECL_OVERRIDE;
};

class NumericTextImp
  : public TextImp
{
  double mvalue;
public:
  typedef TextImp Parent;
  static const ObjectImpType* stype();
  NumericTextImp( const QString& text, const Coordinate& loc, bool frame, double value );
  NumericTextImp* copy() const Q_DECL_OVERRIDE;
  double getValue() const;
  const ObjectImpType* type() const Q_DECL_OVERRIDE;

  int numberOfProperties() const Q_DECL_OVERRIDE;
  const QByteArrayList properties() const Q_DECL_OVERRIDE;
  const QByteArrayList propertiesInternalNames() const Q_DECL_OVERRIDE;
  ObjectImp* property( int which, const KigDocument& w ) const Q_DECL_OVERRIDE;
  const char* iconForProperty( int which ) const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirementForProperty( int which ) const Q_DECL_OVERRIDE;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const Q_DECL_OVERRIDE;
};

class BoolTextImp
  : public TextImp
{
  bool mvalue;
public:
  typedef TextImp Parent;
  static const ObjectImpType* stype();
  BoolTextImp( const QString& text, const Coordinate& loc, bool frame, bool value );
  BoolTextImp* copy() const Q_DECL_OVERRIDE;
  bool getValue() const;
  const ObjectImpType* type() const Q_DECL_OVERRIDE;

  int numberOfProperties() const Q_DECL_OVERRIDE;
  const QByteArrayList properties() const Q_DECL_OVERRIDE;
  const QByteArrayList propertiesInternalNames() const Q_DECL_OVERRIDE;
  ObjectImp* property( int which, const KigDocument& w ) const Q_DECL_OVERRIDE;
  const char* iconForProperty( int which ) const Q_DECL_OVERRIDE;
  const ObjectImpType* impRequirementForProperty( int which ) const Q_DECL_OVERRIDE;
  bool isPropertyDefinedOnOrThroughThisImp( int which ) const Q_DECL_OVERRIDE;
};

#endif
