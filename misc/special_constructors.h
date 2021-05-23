// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_MISC_SPECIAL_CONSTRUCTORS_H
#define KIG_MISC_SPECIAL_CONSTRUCTORS_H

#include "object_constructor.h"

class TwoOrOneIntersectionConstructor
  : public StandardConstructorBase
{
  const ArgsParserObjectType* mtype_std;
  const ArgsParserObjectType* mtype_special;
  ArgsParser margsparser;
public:
  TwoOrOneIntersectionConstructor( const ArgsParserObjectType* t_std,
                                   const ArgsParserObjectType* t_special,
                                   const char* iconfile,
                                   const struct ArgsParser::spec argsspecv[] );
  ~TwoOrOneIntersectionConstructor();

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const Q_DECL_OVERRIDE;
  void plug( KigPart* doc, KigGUIAction* kact ) Q_DECL_OVERRIDE;
  bool isTransform() const Q_DECL_OVERRIDE;
};

class ThreeTwoOneIntersectionConstructor
  : public StandardConstructorBase
{
  const ArgsParserObjectType* mtype_std;
  const ArgsParserObjectType* mtype_special;
  ArgsParser margsparser;
public:
  ThreeTwoOneIntersectionConstructor( const ArgsParserObjectType* t_std,
                                   const ArgsParserObjectType* t_special,
                                   const char* iconfile,
                                   const struct ArgsParser::spec argsspecv[] );
  ~ThreeTwoOneIntersectionConstructor();

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const Q_DECL_OVERRIDE;
  void plug( KigPart* doc, KigGUIAction* kact ) Q_DECL_OVERRIDE;
  bool isTransform() const Q_DECL_OVERRIDE;
};


class PolygonVertexTypeConstructor
  : public StandardConstructorBase
{
  const ArgsParserObjectType* mtype;
  ArgsParser margsparser;
public:
  PolygonVertexTypeConstructor();
  ~PolygonVertexTypeConstructor();

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const Q_DECL_OVERRIDE;
  void plug( KigPart* doc, KigGUIAction* kact ) Q_DECL_OVERRIDE;
  bool isTransform() const Q_DECL_OVERRIDE;
};

class PolygonSideTypeConstructor
  : public StandardConstructorBase
{
  const ArgsParserObjectType* mtype;
  ArgsParser margsparser;
public:
  PolygonSideTypeConstructor();
  ~PolygonSideTypeConstructor();

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const Q_DECL_OVERRIDE;
  void plug( KigPart* doc, KigGUIAction* kact ) Q_DECL_OVERRIDE;
  bool isTransform() const Q_DECL_OVERRIDE;
};

/*
 */

class PointSequenceConstructor
  : public ObjectConstructor
{
  const char* mdescname;
  const char* mdesc;
  const char* miconfile;
  const ObjectType* mtype;
public:
  PointSequenceConstructor( const char* descname,
			    const char* desc,
			    const char* iconfile,
			    const ObjectType* type );

  const QString descriptiveName() const Q_DECL_OVERRIDE;
  const QString description() const Q_DECL_OVERRIDE;
  const QByteArray iconFileName( const bool canBeNull = false ) const Q_DECL_OVERRIDE;
  void handleArgs( const std::vector<ObjectCalcer*>& os,
                           KigPart& d,
                           KigWidget& v
    ) const Q_DECL_OVERRIDE;
  void handlePrelim( KigPainter& p,
                             const std::vector<ObjectCalcer*>& sel,
                             const KigDocument& d,
                             const KigWidget& v
    ) const Q_DECL_OVERRIDE;
  // TODO: move here actual drawprelm when AbstractPolygon is in place!
  virtual void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const = 0;

  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigPart* doc, KigGUIAction* kact ) Q_DECL_OVERRIDE;
  bool isTransform() const Q_DECL_OVERRIDE;
};

/*
 * classes PolygonBNPTypeConstructor, OpenPolygonTypeConstructor and 
 *         BezierCurveTypeConstructor
 * all inherit from the PointSequenceConstructor
 */

class PolygonBNPTypeConstructor
  : public PointSequenceConstructor
{
public:
  PolygonBNPTypeConstructor();
  ~PolygonBNPTypeConstructor();

  bool isAlreadySelectedOK( const std::vector<ObjectCalcer*>& os, const uint& ) const Q_DECL_OVERRIDE;
  int wantArgs( const std::vector<ObjectCalcer*>& os, const KigDocument& d, const KigWidget& v) const Q_DECL_OVERRIDE;
  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel,
                           const KigDocument& d, const KigWidget& v
    ) const Q_DECL_OVERRIDE;
  QString selectStatement(
    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    const KigWidget& w ) const Q_DECL_OVERRIDE;

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
};


class OpenPolygonTypeConstructor
  : public PointSequenceConstructor
{
public:
  OpenPolygonTypeConstructor();
  ~OpenPolygonTypeConstructor();

  bool isAlreadySelectedOK( const std::vector<ObjectCalcer*>& os, const uint& ) const Q_DECL_OVERRIDE;
  int wantArgs( const std::vector<ObjectCalcer*>& os, const KigDocument& d, const KigWidget& v) const Q_DECL_OVERRIDE;
  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel,
                           const KigDocument& d, const KigWidget& v
    ) const Q_DECL_OVERRIDE;
  QString selectStatement(
    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    const KigWidget& w ) const Q_DECL_OVERRIDE;

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
};

class PolygonBCVConstructor
  : public ObjectConstructor
{
  const ObjectType* mtype;
public:
  PolygonBCVConstructor();
  ~PolygonBCVConstructor();

  const QString descriptiveName() const Q_DECL_OVERRIDE;
  const QString description() const Q_DECL_OVERRIDE;
  const QByteArray iconFileName( const bool canBeNull = false ) const Q_DECL_OVERRIDE;
  bool isAlreadySelectedOK( const std::vector<ObjectCalcer*>& os, const uint& ) const Q_DECL_OVERRIDE;
  int wantArgs( const std::vector<ObjectCalcer*>& os,
                              const KigDocument& d,
                              const KigWidget& v
    ) const Q_DECL_OVERRIDE;
  void handleArgs( const std::vector<ObjectCalcer*>& os,
                           KigPart& d,
                           KigWidget& v
    ) const Q_DECL_OVERRIDE;
  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel,
                           const KigDocument& d, const KigWidget& v
    ) const Q_DECL_OVERRIDE;
  QString selectStatement(
    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    const KigWidget& w ) const Q_DECL_OVERRIDE;
  void handlePrelim( KigPainter& p,
                             const std::vector<ObjectCalcer*>& sel,
                             const KigDocument& d,
                             const KigWidget& v
    ) const Q_DECL_OVERRIDE;
  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigPart* doc, KigGUIAction* kact ) Q_DECL_OVERRIDE;
  bool isTransform() const Q_DECL_OVERRIDE;
  int computeNsides( const Coordinate& c, const Coordinate& v, const Coordinate& cntrl, int& winding ) const;
  Coordinate getRotatedCoord( const Coordinate& c1,
               const Coordinate& c2, double alpha ) const;
};

class BezierCurveTypeConstructor
  : public PointSequenceConstructor
{
public:
  BezierCurveTypeConstructor();
  ~BezierCurveTypeConstructor();

  bool isAlreadySelectedOK( const std::vector<ObjectCalcer*>& os, const uint& ) const Q_DECL_OVERRIDE;
  int wantArgs( const std::vector<ObjectCalcer*>& os, const KigDocument& d, const KigWidget& v) const Q_DECL_OVERRIDE;
  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel,
                           const KigDocument& d, const KigWidget& v
    ) const Q_DECL_OVERRIDE;
  QString selectStatement(
    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    const KigWidget& w ) const Q_DECL_OVERRIDE;

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
};

class RationalBezierCurveTypeConstructor
   : public ObjectConstructor
{
public:
  RationalBezierCurveTypeConstructor();
  ~RationalBezierCurveTypeConstructor();

  const QString descriptiveName() const Q_DECL_OVERRIDE;
  const QString description() const Q_DECL_OVERRIDE;
  const QByteArray iconFileName( const bool canBeNull = false ) const Q_DECL_OVERRIDE;
 
  bool isAlreadySelectedOK( const std::vector<ObjectCalcer*>& os, const uint& ) const Q_DECL_OVERRIDE;

  int wantArgs( const std::vector<ObjectCalcer*>& os, const KigDocument& d, const KigWidget& v) const Q_DECL_OVERRIDE;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void handleArgs( const std::vector<ObjectCalcer*>& os,
                   KigPart& d,
                   KigWidget& v ) const Q_DECL_OVERRIDE;
 
 QString useText( const ObjectCalcer& o, 
                   const std::vector<ObjectCalcer*>& sel,
                   const KigDocument& d, 
                   const KigWidget& v ) const Q_DECL_OVERRIDE;
  QString selectStatement( const std::vector<ObjectCalcer*>& sel, 
                           const KigDocument& d,
                           const KigWidget& w ) const Q_DECL_OVERRIDE;


  void handlePrelim( KigPainter& p,
                     const std::vector<ObjectCalcer*>& sel,
                     const KigDocument& d,
                     const KigWidget& v ) const Q_DECL_OVERRIDE;
  void drawprelim( const ObjectDrawer& drawer, 
                   KigPainter& p, 
                   const std::vector<ObjectCalcer*>& parents, 
                   const KigDocument& d ) const;

 void plug( KigPart* doc, KigGUIAction* kact ) Q_DECL_OVERRIDE;
  bool isTransform() const Q_DECL_OVERRIDE;
};

class MeasureTransportConstructor
  : public ObjectConstructor
{
  const ObjectType* mtype;
public:
  MeasureTransportConstructor();
  ~MeasureTransportConstructor();

  const QString descriptiveName() const Q_DECL_OVERRIDE;
  const QString description() const Q_DECL_OVERRIDE;
  const QByteArray iconFileName( const bool canBeNull = false ) const Q_DECL_OVERRIDE;
  bool isAlreadySelectedOK( const std::vector<ObjectCalcer*>& os,
                              const uint& ) const Q_DECL_OVERRIDE;
  int wantArgs( const std::vector<ObjectCalcer*>& os,
                              const KigDocument& d,
                              const KigWidget& v
    ) const Q_DECL_OVERRIDE;
  void handleArgs( const std::vector<ObjectCalcer*>& os,
                           KigPart& d,
                           KigWidget& v
    ) const Q_DECL_OVERRIDE;
  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel,
                           const KigDocument& d, const KigWidget& v
    ) const Q_DECL_OVERRIDE;
  QString selectStatement(
    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    const KigWidget& w ) const Q_DECL_OVERRIDE;
  void handlePrelim( KigPainter& p,
                             const std::vector<ObjectCalcer*>& sel,
                             const KigDocument& d,
                             const KigWidget& v
    ) const Q_DECL_OVERRIDE;

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigPart* doc, KigGUIAction* kact ) Q_DECL_OVERRIDE;
  bool isTransform() const Q_DECL_OVERRIDE;
};

class ConicRadicalConstructor
  : public StandardConstructorBase
{
  const ArgsParserObjectType* mtype;
  const ArgsParser mparser;
public:
  ConicRadicalConstructor();
  ~ConicRadicalConstructor();
  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
                   const KigWidget& v ) const Q_DECL_OVERRIDE;
  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const Q_DECL_OVERRIDE;
  void plug( KigPart* doc, KigGUIAction* kact ) Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class LocusConstructor
  : public StandardConstructorBase
{
  ArgsParser margsparser;
public:
  LocusConstructor();
  ~LocusConstructor();
  /**
   * we override the wantArgs() function, since we need to see
   * something about the objects that an ArgsParser can't know about,
   * namely, whether the first point is a constrained point...
   */
  int wantArgs(
    const std::vector<ObjectCalcer*>& os, const KigDocument& d,
    const KigWidget& v
    ) const Q_DECL_OVERRIDE;
  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
                   const KigWidget& v ) const Q_DECL_OVERRIDE;

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const Q_DECL_OVERRIDE;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const Q_DECL_OVERRIDE;
  void plug( KigPart* doc, KigGUIAction* kact ) Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class GenericAffinityConstructor
  : public MergeObjectConstructor
{
public:
  GenericAffinityConstructor();
  ~GenericAffinityConstructor();
  bool isAlreadySelectedOK(const std::vector< ObjectCalcer* >& os, const uint& ) const Q_DECL_OVERRIDE;
};

class GenericProjectivityConstructor
  : public MergeObjectConstructor
{
public:
  GenericProjectivityConstructor();
  ~GenericProjectivityConstructor();
  bool isAlreadySelectedOK(const std::vector< ObjectCalcer* >& os, const uint& ) const Q_DECL_OVERRIDE;
};

class InversionConstructor
  : public MergeObjectConstructor
{
public:
  InversionConstructor();
  ~InversionConstructor();
};

class GenericIntersectionConstructor
  : public MergeObjectConstructor
{
public:
  GenericIntersectionConstructor();
  ~GenericIntersectionConstructor();

  bool isIntersection() const Q_DECL_OVERRIDE;

  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
                   const KigWidget& v ) const Q_DECL_OVERRIDE;
  QString selectStatement(
    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
    const KigWidget& w ) const Q_DECL_OVERRIDE;
};

class MidPointOfTwoPointsConstructor
  : public StandardConstructorBase
{
  ArgsParser mparser;
public:
  MidPointOfTwoPointsConstructor();
  ~MidPointOfTwoPointsConstructor();
  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                   const KigDocument& ) const Q_DECL_OVERRIDE;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d,
                 KigWidget& w ) const Q_DECL_OVERRIDE;
  void plug( KigPart* doc, KigGUIAction* kact ) Q_DECL_OVERRIDE;
  bool isTransform() const Q_DECL_OVERRIDE;
};

class GoldenPointOfTwoPointsConstructor
  : public StandardConstructorBase
{
  ArgsParser mparser;
public:
  GoldenPointOfTwoPointsConstructor();
  ~GoldenPointOfTwoPointsConstructor();
  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                   const KigDocument& ) const Q_DECL_OVERRIDE;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d,
                 KigWidget& w ) const Q_DECL_OVERRIDE;
  void plug( KigPart* doc, KigGUIAction* kact ) Q_DECL_OVERRIDE;
  bool isTransform() const Q_DECL_OVERRIDE;
};

class TestConstructor
  : public StandardConstructorBase
{
  const ArgsParserObjectType* mtype;
public:
  TestConstructor( const ArgsParserObjectType* type, const char* descname,
    const char* desc, const char* iconfile );
  ~TestConstructor();
  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                   const KigDocument& ) const Q_DECL_OVERRIDE;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d,
                 KigWidget& w ) const Q_DECL_OVERRIDE;
  int wantArgs( const std::vector<ObjectCalcer*>& os,
                      const KigDocument& d, const KigWidget& v ) const Q_DECL_OVERRIDE;
  void plug( KigPart* doc, KigGUIAction* kact ) Q_DECL_OVERRIDE;
  bool isTransform() const Q_DECL_OVERRIDE;
  bool isTest() const Q_DECL_OVERRIDE;

  BaseConstructMode* constructMode( KigPart& doc ) Q_DECL_OVERRIDE;
};

class TangentConstructor
  : public MergeObjectConstructor
{
public:
  TangentConstructor();
  ~TangentConstructor();

  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
                   const KigWidget& v ) const Q_DECL_OVERRIDE;
//  QString selectStatement(
//    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
//    const KigWidget& w ) const;
};

class CocConstructor
  : public MergeObjectConstructor
{
public:
  CocConstructor();
  ~CocConstructor();

  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
                   const KigWidget& v ) const Q_DECL_OVERRIDE;
//  QString selectStatement(
//    const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
//    const KigWidget& w ) const;
};

bool relativePrimes( int n, int p );

std::vector<ObjectCalcer*> 
removeDuplicatedPoints( std::vector<ObjectCalcer*> points );
bool coincidentPoints( const ObjectImp* p1, const ObjectImp* p2 );

#endif
