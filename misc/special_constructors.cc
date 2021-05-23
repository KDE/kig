// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#include "special_constructors.h"

#include <math.h>

#include "calcpaths.h"
#include "common.h"
#include "conic-common.h"
#include "guiaction.h"
#include "kigpainter.h"

#include "../kig/kig_part.h"
#include "../kig/kig_document.h"
#include "../modes/construct_mode.h"
#include "../objects/special_imptypes.h"
#include "../objects/bogus_imp.h"
#include "../objects/centerofcurvature_type.h"
#include "../objects/circle_imp.h"
#include "../objects/conic_imp.h"
#include "../objects/conic_types.h"
#include "../objects/cubic_imp.h"
#include "../objects/intersection_types.h"
#include "../objects/inversion_type.h"
#include "../objects/line_imp.h"
#include "../objects/line_type.h"
#include "../objects/locus_imp.h"
#include "../objects/object_calcer.h"
#include "../objects/object_drawer.h"
#include "../objects/object_factory.h"
#include "../objects/object_holder.h"
#include "../objects/object_imp.h"
#include "../objects/object_type.h"
#include "../objects/other_imp.h"
#include "../objects/other_type.h"
#include "../objects/point_imp.h"
#include "../objects/point_type.h"
#include "../objects/polygon_imp.h"
#include "../objects/polygon_type.h"
#include "../objects/bezier_imp.h"
#include "../objects/bezier_type.h"
#include "../objects/tangent_type.h"
#include "../objects/text_imp.h"
#include "../objects/transform_types.h"

#include <QPen>

#include <algorithm>
#include <functional>
#include <iterator>

/*
 * conic-line and circle-circle intersection (with search for already computed
 * intersections)
 * the previous "ConicLineIntersectionConstructor" is now
 * dead code, which could be remove in the future
 */

TwoOrOneIntersectionConstructor::TwoOrOneIntersectionConstructor(
                                       const ArgsParserObjectType* t_std,
                                       const ArgsParserObjectType* t_special,
                                       const char* iconfile,
                                       const struct ArgsParser::spec argsspecv[] )
  : StandardConstructorBase( "SHOULD NOT BE SEEN", "SHOULD NOT BE SEEN",
       iconfile, margsparser ),
    mtype_std( t_std ),
    mtype_special( t_special ),
    margsparser( argsspecv, 2 )
{
}

TwoOrOneIntersectionConstructor::~TwoOrOneIntersectionConstructor()
{
}

void TwoOrOneIntersectionConstructor::drawprelim(
           const ObjectDrawer& drawer, 
           KigPainter& p, 
           const std::vector<ObjectCalcer*>& parents,
           const KigDocument& doc ) const
{
  Args args;
  if ( parents.size() != 2 ) return;
  transform( parents.begin(), parents.end(),
             back_inserter( args ), std::mem_fun( &ObjectCalcer::imp ) );

  for ( int i = -1; i <= 1; i += 2 )
  {
    IntImp param( i );
    args.push_back( &param );
    ObjectImp* data = mtype_std->calc( args, doc );
    drawer.draw( *data, p, true );
    delete data;
    args.pop_back();
  }
}

std::vector<ObjectCalcer*> removeDuplicatedPoints( std::vector<ObjectCalcer*> points )
{
  std::vector<ObjectCalcer*> ret;

  for ( std::vector<ObjectCalcer*>::iterator i = points.begin();
        i != points.end(); ++i )
  {
    for ( std::vector<ObjectCalcer*>::iterator j = ret.begin();
          j != ret.end(); ++j )
    {
      if ( coincidentPoints( (*i)->imp(), (*j)->imp() ) ) break;
    }
    ret.push_back( *i );
  }
  return ret;
}

bool coincidentPoints( const ObjectImp* p1, const ObjectImp* p2 )
{
  const PointImp* pt1 = dynamic_cast<const PointImp*>( p1 );
  if ( !pt1 ) return false;
  const PointImp* pt2 = dynamic_cast<const PointImp*>( p2 );
  if ( !pt2 ) return false;

  Coordinate diff = pt1->coordinate() - pt2->coordinate();
  if ( diff.squareLength() < 1e-12 ) return true;
  return false;
}

std::vector<ObjectHolder*> TwoOrOneIntersectionConstructor::build( 
           const std::vector<ObjectCalcer*>& parents, 
           KigDocument& doc, 
           KigWidget& ) const
{
  std::vector<ObjectHolder*> ret;
  assert( parents.size() == 2 );

  std::vector<ObjectCalcer*> points = doc.findIntersectionPoints( parents[0], parents[1] );
  std::vector<ObjectCalcer*> uniquepoints = removeDuplicatedPoints( points );

  if ( uniquepoints.size() == 1 )
  {
    std::vector<ObjectCalcer*> args( parents );
    args.push_back( uniquepoints[0] );
    ret.push_back( new ObjectHolder( new ObjectTypeCalcer(
      mtype_special, args
      ) ) );
    return ret;
  }
  for ( int i = -1; i <= 1; i += 2 )
  {
    ObjectConstCalcer* d = new ObjectConstCalcer( new IntImp( i ) );
    std::vector<ObjectCalcer*> args( parents );
    args.push_back( d );

    ret.push_back( new ObjectHolder( new ObjectTypeCalcer(
      mtype_std, args
      ) ) );
  }
  return ret;
}

void TwoOrOneIntersectionConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool TwoOrOneIntersectionConstructor::isTransform() const
{
  return false;
}

ThreeTwoOneIntersectionConstructor::ThreeTwoOneIntersectionConstructor(
                                       const ArgsParserObjectType* t_std,
                                       const ArgsParserObjectType* t_special,
                                       const char* iconfile,
                                       const struct ArgsParser::spec argsspecv[] )
  : StandardConstructorBase( "SHOULD NOT BE SEEN", "SHOULD NOT BE SEEN",
       iconfile, margsparser ),
    mtype_std( t_std ),
    mtype_special( t_special ),
    margsparser( argsspecv, 2 )
{
}

ThreeTwoOneIntersectionConstructor::~ThreeTwoOneIntersectionConstructor()
{
}

void ThreeTwoOneIntersectionConstructor::drawprelim(
           const ObjectDrawer& drawer, 
           KigPainter& p, 
           const std::vector<ObjectCalcer*>& parents,
           const KigDocument& doc ) const
{
  Args args;
  if ( parents.size() != 2 ) return;
  transform( parents.begin(), parents.end(),
             back_inserter( args ), std::mem_fun( &ObjectCalcer::imp ) );

  for ( int i = 1; i <= 3; i += 1 )
  {
    IntImp param( i );
    args.push_back( &param );
    ObjectImp* data = mtype_std->calc( args, doc );
    drawer.draw( *data, p, true );
    delete data;
    args.pop_back();
  }
}

std::vector<ObjectHolder*> ThreeTwoOneIntersectionConstructor::build( 
           const std::vector<ObjectCalcer*>& parents, 
           KigDocument& doc, 
           KigWidget& ) const
{
  std::vector<ObjectHolder*> ret;
  assert( parents.size() == 2 );

  std::vector<ObjectCalcer*> points = doc.findIntersectionPoints( parents[0], parents[1] );
  std::vector<ObjectCalcer*> uniquepoints = removeDuplicatedPoints( points );

  if ( uniquepoints.size() == 2 )
  {
    std::vector<ObjectCalcer*> args( parents );
    args.push_back( uniquepoints[0] );
    args.push_back( uniquepoints[1] );
    ret.push_back( new ObjectHolder( new ObjectTypeCalcer(
      mtype_special, args
      ) ) );
    return ret;
  }
  if ( uniquepoints.size() == 1 )
  {
  for ( int i = -1; i <= 1; i += 2 )
  {
     std::vector<ObjectCalcer*> args( parents );
     args.push_back( uniquepoints[0] );
     ObjectConstCalcer* d = new ObjectConstCalcer( new IntImp( i ) );
     args.push_back( d );
 
     ret.push_back( new ObjectHolder( new ObjectTypeCalcer(
     CubicLineTwoIntersectionType::instance(), args
      ) ) );
    args.clear();
  }
    return ret;
  }
  for ( int i = 1; i <= 3; i += 1 )
  {
    ObjectConstCalcer* d = new ObjectConstCalcer( new IntImp( i ) );
    std::vector<ObjectCalcer*> args( parents );
    args.push_back( d );

    ret.push_back( new ObjectHolder( new ObjectTypeCalcer(
      mtype_std, args
      ) ) );
  }
  return ret;
}

void ThreeTwoOneIntersectionConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool ThreeTwoOneIntersectionConstructor::isTransform() const
{
  return false;
}

/*
 * conic-conic intersection
 */

class ConicConicIntersectionConstructor
  : public StandardConstructorBase
{
protected:
  ArgsParser mparser;
public:
  ConicConicIntersectionConstructor();
  ~ConicConicIntersectionConstructor();

  void drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                   const KigDocument& ) const Q_DECL_OVERRIDE;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const Q_DECL_OVERRIDE;
  void plug( KigPart* doc, KigGUIAction* kact ) Q_DECL_OVERRIDE;

  bool isTransform() const Q_DECL_OVERRIDE;
};

class ConicLineIntersectionConstructor
  : public MultiObjectTypeConstructor
{
public:
  ConicLineIntersectionConstructor();
  ~ConicLineIntersectionConstructor();
};

class ArcLineIntersectionConstructor
  : public MultiObjectTypeConstructor
{
public:
  ArcLineIntersectionConstructor();
  ~ArcLineIntersectionConstructor();
};

ConicRadicalConstructor::ConicRadicalConstructor()
  : StandardConstructorBase(
    I18N_NOOP( "Radical Lines for Conics" ),
    I18N_NOOP( "The lines constructed through the intersections "
               "of two conics.  This is also defined for "
               "non-intersecting conics." ),
    "conicsradicalline", mparser ),
    mtype( ConicRadicalType::instance() ),
    mparser( mtype->argsParser().without( IntImp::stype() ) )
{
}

ConicRadicalConstructor::~ConicRadicalConstructor()
{
}

void ConicRadicalConstructor::drawprelim(
  const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& doc ) const
{
  if ( parents.size() == 2 && parents[0]->imp()->inherits( ConicImp::stype() ) &&
       parents[1]->imp()->inherits( ConicImp::stype() ) )
  {
    Args args;
    std::transform( parents.begin(), parents.end(),
                    std::back_inserter( args ), std::mem_fun( &ObjectCalcer::imp ) );
    for ( int i = -1; i < 2; i += 2 )
    {
      IntImp root( i );
      IntImp zeroindex( 1 );
      args.push_back( &root );
      args.push_back( &zeroindex );
      ObjectImp* data = mtype->calc( args, doc );
      drawer.draw( *data, p, true );
      delete data; data = 0;
      args.pop_back();
      args.pop_back();
    };
  };
}

std::vector<ObjectHolder*> ConicRadicalConstructor::build( const std::vector<ObjectCalcer*>& os, KigDocument&, KigWidget& ) const
{
  using namespace std;
  std::vector<ObjectHolder*> ret;
  ObjectCalcer* zeroindexcalcer = new ObjectConstCalcer( new IntImp( 1 ) );
  for ( int i = -1; i < 2; i += 2 )
  {
    std::vector<ObjectCalcer*> args;
    std::copy( os.begin(), os.end(), back_inserter( args ) );
    args.push_back( new ObjectConstCalcer( new IntImp( i ) ) );
    // we use only one zeroindex dataobject, so that if you switch one
    // radical line around, then the other switches along..
    args.push_back( zeroindexcalcer );
    ret.push_back(
      new ObjectHolder( new ObjectTypeCalcer( mtype, args ) ) );
  };
  return ret;
}

static const struct ArgsParser::spec argsspecpp[] =
{
  { PointImp::stype(), I18N_NOOP( "Moving Point" ),
    I18N_NOOP( "Select the moving point, which will be moved around while drawing the locus..." ), false },
  { PointImp::stype(), I18N_NOOP( "Following Point" ),
    I18N_NOOP( "Select the following point, whose locations the locus will be drawn through..." ), true }
};

LocusConstructor::LocusConstructor()
  : StandardConstructorBase( I18N_NOOP( "Locus" ), I18N_NOOP( "A locus" ),
                             "locus", margsparser ),
    margsparser( argsspecpp, 2 )
{
}

LocusConstructor::~LocusConstructor()
{
}

void LocusConstructor::drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                                   const KigDocument& ) const
{
  // this function is rather ugly, but it is necessary to do it this
  // way in order to play nice with Kig's design..

  if ( parents.size() != 2 ) return;
  const ObjectTypeCalcer* constrained = dynamic_cast<ObjectTypeCalcer*>( parents.front() );
  const ObjectCalcer* moving = parents.back();
  if ( ! constrained || ! constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
  {
    // moving is in fact the constrained point.. swap them..
    moving = parents.front();
    constrained = dynamic_cast<const ObjectTypeCalcer*>( parents.back() );
    assert( constrained );
  };
  assert( constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) );

  const ObjectImp* oimp = constrained->parents().back()->imp();
  if( !oimp->inherits( CurveImp::stype() ) )
    oimp = constrained->parents().front()->imp();
  assert( oimp->inherits( CurveImp::stype() ) );
  const CurveImp* cimp = static_cast<const CurveImp*>( oimp );

  ObjectHierarchy hier( constrained, moving );

  LocusImp limp( cimp->copy(), hier );
  drawer.draw( limp, p, true );
}

int LocusConstructor::wantArgs(
 const std::vector<ObjectCalcer*>& os, const KigDocument&, const KigWidget&
 ) const
{
  int ret = margsparser.check( os );
  if ( ret == ArgsParser::Invalid ) return ret;
  else if ( os.size() != 2 ) return ret;
  if ( dynamic_cast<ObjectTypeCalcer*>( os.front() ) &&
       static_cast<ObjectTypeCalcer*>( os.front() )->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
  {
    std::set<ObjectCalcer*> children = getAllChildren( os.front() );
    return children.find( os.back() ) != children.end() ? ret : ArgsParser::Invalid;
  }
  if ( dynamic_cast<ObjectTypeCalcer*>( os.back() ) &&
       static_cast<ObjectTypeCalcer*>( os.back() )->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
  {
    std::set<ObjectCalcer*> children = getAllChildren( os.back() );
    return children.find( os.front() ) != children.end() ? ret : ArgsParser::Invalid;
  }
  return ArgsParser::Invalid;
}

std::vector<ObjectHolder*> LocusConstructor::build( const std::vector<ObjectCalcer*>& parents, KigDocument&, KigWidget& ) const
{
  std::vector<ObjectHolder*> ret;
  assert( parents.size() == 2 );

  ObjectTypeCalcer* constrained = dynamic_cast<ObjectTypeCalcer*>( parents.front() );
  ObjectCalcer* moving = parents.back();
  if ( ! constrained || ! constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
  {
    // moving is in fact the constrained point.. swap them..
    moving = parents.front();
    constrained = dynamic_cast<ObjectTypeCalcer*>( parents.back() );
    assert( constrained );
  };
  assert( constrained->type()->inherits( ObjectType::ID_ConstrainedPointType ) );

  ret.push_back( ObjectFactory::instance()->locus( constrained, moving ) );
  return ret;
}

QString LocusConstructor::useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& os,
                                   const KigDocument&, const KigWidget& ) const
{
  if ( dynamic_cast<const ObjectTypeCalcer*>( &o ) &&
       static_cast<const ObjectTypeCalcer&>( o ).type()->inherits( ObjectType::ID_ConstrainedPointType ) &&
       ( os.empty() || !dynamic_cast<ObjectTypeCalcer*>( os[0] ) ||
         !static_cast<const ObjectTypeCalcer*>( os[0] )->type()->inherits( ObjectType::ID_ConstrainedPointType ) )
    ) return i18n( "Moving Point" );
  else return i18n( "Dependent Point" );
}

void ConicRadicalConstructor::plug( KigPart*, KigGUIAction* )
{
}

void LocusConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool ConicRadicalConstructor::isTransform() const
{
  return mtype->isTransform();
}

bool LocusConstructor::isTransform() const
{
  return false;
}

/*
 * generic sequence of points constructor
 */

PointSequenceConstructor::PointSequenceConstructor(
  const char* descname,
  const char* desc,
  const char* iconfile,
  const ObjectType* type )
  : mdescname( descname ),
    mdesc( desc ),
    miconfile( iconfile ),
    mtype( type )
{
}

const QString PointSequenceConstructor::descriptiveName() const
{
  return i18n( mdescname );
}

const QString PointSequenceConstructor::description() const
{
  return i18n( mdesc );
}

const QByteArray PointSequenceConstructor::iconFileName( const bool ) const
{
  return miconfile;
}

void PointSequenceConstructor::handleArgs(
  const std::vector<ObjectCalcer*>& os, KigPart& d,
  KigWidget& v ) const
{
  std::vector<ObjectHolder*> bos = build( os, d.document(), v );
  for ( std::vector<ObjectHolder*>::iterator i = bos.begin();
        i != bos.end(); ++i )
  {
    (*i)->calc( d.document() );
  }

  d.addObjects( bos );
}

void PointSequenceConstructor::handlePrelim(
  KigPainter& p, const std::vector<ObjectCalcer*>& os,
  const KigDocument& d, const KigWidget&
  ) const
{
  uint count = os.size();
  if ( count < 2 ) return;

  for ( uint i = 0; i < count; i++ )
  {
    assert ( os[i]->imp()->inherits( PointImp::stype() ) );
  }

  std::vector<ObjectCalcer*> args = os;
  p.setBrushStyle( Qt::NoBrush );
  p.setBrushColor( Qt::red );
  p.setPen( QPen ( Qt::red,  1) );
  p.setWidth( -1 ); // -1 means the default width for the object being
                    // drawn..

  ObjectDrawer drawer( Qt::red );
  drawprelim( drawer, p, args, d );
}

std::vector<ObjectHolder*> PointSequenceConstructor::build( const std::vector<ObjectCalcer*>& parents, KigDocument&, KigWidget& ) const
{
  uint count = parents.size() - 1;
  assert ( count >= 3 );
  std::vector<ObjectCalcer*> args;
  for ( uint i = 0; i < count; ++i ) args.push_back( parents[i] );
  ObjectTypeCalcer* calcer = new ObjectTypeCalcer( mtype, args );
  ObjectHolder* h = new ObjectHolder( calcer );
  std::vector<ObjectHolder*> ret;
  ret.push_back( h );
  return ret;
}

void PointSequenceConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool PointSequenceConstructor::isTransform() const
{
  return false;
}

/*
 * generic polygon constructor
 */

PolygonBNPTypeConstructor::PolygonBNPTypeConstructor()
  : PointSequenceConstructor(
    I18N_NOOP( "Polygon by Its Vertices" ),
    I18N_NOOP( "Construct a polygon by giving its vertices" ),
    "kig_polygon",
    PolygonBNPType::instance() )
{
}

PolygonBNPTypeConstructor::~PolygonBNPTypeConstructor()
{
}

bool PolygonBNPTypeConstructor::isAlreadySelectedOK(
 const std::vector<ObjectCalcer*>& os, const uint& pos ) const
{
  if ( pos == 0 && os.size() >= 3 ) return true;
  return false;
}

int PolygonBNPTypeConstructor::wantArgs( const std::vector<ObjectCalcer*>& os,
                                         const KigDocument&,
                                         const KigWidget& ) const
{
  int count=os.size() - 1;

  for ( int i = 0; i <= count; i++ )
  {
    if ( ! ( os[i]->imp()->inherits( PointImp::stype() ) ) ) return ArgsParser::Invalid;
  }
  if ( count < 3 ) return ArgsParser::Valid;
  if ( os[0] == os[count] ) return ArgsParser::Complete;
  return ArgsParser::Valid;
}

QString PolygonBNPTypeConstructor::useText( const ObjectCalcer&, const std::vector<ObjectCalcer*>& os,
                                          const KigDocument&, const KigWidget& ) const
{
  if ( os.size() > 3 )
    return i18n("... with this vertex (click on the first vertex to terminate construction)");
  else return i18n("Construct a polygon with this vertex");
}

QString PolygonBNPTypeConstructor::selectStatement(
  const std::vector<ObjectCalcer*>&, const KigDocument&,
  const KigWidget& ) const
{
  return i18n("Select a point to be a vertex of the new polygon...");
}

void PolygonBNPTypeConstructor::drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                                   const KigDocument& ) const
{
  if ( parents.size() < 2 ) return;

  std::vector<Coordinate> points;

  for ( uint i = 0; i < parents.size(); ++i )
  {
    const Coordinate vertex =
        static_cast<const PointImp*>( parents[i]->imp() )->coordinate();
    points.push_back( vertex );
  }

  if ( parents.size() == 2 )
  {
    SegmentImp segment = SegmentImp( points[0], points[1] );
    drawer.draw( segment, p, true );
  } else {
    FilledPolygonImp polygon = FilledPolygonImp( points );
    drawer.draw( polygon, p, true );
  }
}


/*
 * open polygon (polyline) constructor
 */

OpenPolygonTypeConstructor::OpenPolygonTypeConstructor()
  : PointSequenceConstructor(
    I18N_NOOP( "Open Polygon (Polygonal Line)" ),
    I18N_NOOP( "Construct an open polygon" ),
    "openpolygon",
    OpenPolygonType::instance() )
{
}

OpenPolygonTypeConstructor::~OpenPolygonTypeConstructor()
{
}

bool OpenPolygonTypeConstructor::isAlreadySelectedOK(
  const std::vector<ObjectCalcer*>& os, const uint& pos ) const
{
  if ( pos == os.size() - 1 && os.size() >= 2 ) return true;
  return false;
}

int OpenPolygonTypeConstructor::wantArgs( const std::vector<ObjectCalcer*>& os,
                                         const KigDocument&,
                                         const KigWidget& ) const
{
  int count=os.size() - 1;

  for ( int i = 0; i <= count; i++ )
  {
    if ( ! ( os[i]->imp()->inherits( PointImp::stype() ) ) ) return ArgsParser::Invalid;
  }
  if ( count < 2 ) return ArgsParser::Valid;
  if ( os[count] == os[count - 1] ) return ArgsParser::Complete;
  return ArgsParser::Valid;
}

QString OpenPolygonTypeConstructor::useText( const ObjectCalcer&, const std::vector<ObjectCalcer*>& os,
                                          const KigDocument&, const KigWidget& ) const
{
  if ( os.size() > 2 )
    return i18n("... with this vertex (click again on the last vertex to terminate construction)");
  else return i18n("Construct a polygonal line with this vertex");
}

QString OpenPolygonTypeConstructor::selectStatement(
  const std::vector<ObjectCalcer*>&, const KigDocument&,
  const KigWidget& ) const
{
  return i18n("Select a point to be a vertex of the new polygonal line...");
}

void OpenPolygonTypeConstructor::drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                                   const KigDocument& ) const
{
  if ( parents.size() < 2 ) return;

  std::vector<Coordinate> points;

  for ( uint i = 0; i < parents.size(); ++i )
  {
    const Coordinate vertex =
        static_cast<const PointImp*>( parents[i]->imp() )->coordinate();
    points.push_back( vertex );
  }

  if ( parents.size() == 2 )
  {
    SegmentImp segment = SegmentImp( points[0], points[1] );
    drawer.draw( segment, p, true );
  } else {
    OpenPolygonalImp polygon = OpenPolygonalImp( points );
    drawer.draw( polygon, p, true );
  }
}

/*
 * construction of polygon vertices
 */

static const struct ArgsParser::spec argsspecpv[] =
{
  { FilledPolygonImp::stype(), I18N_NOOP( "Polygon" ),
    I18N_NOOP( "Construct the vertices of this polygon..." ), true }
};

PolygonVertexTypeConstructor::PolygonVertexTypeConstructor()
  : StandardConstructorBase( I18N_NOOP( "Vertices of a Polygon" ),
       I18N_NOOP( "The vertices of a polygon." ),
       "polygonvertices", margsparser ),
    mtype( PolygonVertexType::instance() ),
    margsparser( argsspecpv, 1 )
{
}

PolygonVertexTypeConstructor::~PolygonVertexTypeConstructor()
{
}

void PolygonVertexTypeConstructor::drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                                   const KigDocument& ) const
{
  if ( parents.size() != 1 ) return;

  const FilledPolygonImp* polygon = dynamic_cast<const FilledPolygonImp*>( parents.front()->imp() );
  const std::vector<Coordinate> points = polygon->points();

  int sides = points.size();
  for ( int i = 0; i < sides; ++i )
  {
    PointImp point = PointImp( points[i] );
    drawer.draw( point, p, true );
  }
}

std::vector<ObjectHolder*> PolygonVertexTypeConstructor::build( const std::vector<ObjectCalcer*>& parents, KigDocument&, KigWidget& ) const
{
  std::vector<ObjectHolder*> ret;
  assert( parents.size() == 1 );
  const FilledPolygonImp* polygon = dynamic_cast<const FilledPolygonImp*>( parents.front()->imp() );
  const std::vector<Coordinate> points = polygon->points();

  int sides = points.size();

  for ( int i = 0; i < sides; ++i )
  {
    ObjectConstCalcer* d = new ObjectConstCalcer( new IntImp( i ) );
    std::vector<ObjectCalcer*> args( parents );
    args.push_back( d );
    ret.push_back( new ObjectHolder( new ObjectTypeCalcer( mtype, args ) ) );
  }
  return ret;
}

void PolygonVertexTypeConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool PolygonVertexTypeConstructor::isTransform() const
{
  return false;
}

/*
 * construction of polygon sides
 */

static const struct ArgsParser::spec argsspecps[] =
{
  { FilledPolygonImp::stype(), I18N_NOOP( "Polygon" ),
    I18N_NOOP( "Construct the sides of this polygon..." ), false }
};

PolygonSideTypeConstructor::PolygonSideTypeConstructor()
  : StandardConstructorBase( I18N_NOOP( "Sides of a Polygon" ),
       I18N_NOOP( "The sides of a polygon." ),
       "polygonsides", margsparser ),
    mtype( PolygonSideType::instance() ),
    margsparser( argsspecps, 1 )
{
}

PolygonSideTypeConstructor::~PolygonSideTypeConstructor()
{
}

void PolygonSideTypeConstructor::drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                                   const KigDocument& ) const
{
  if ( parents.size() != 1 ) return;

  const FilledPolygonImp* polygon = dynamic_cast<const FilledPolygonImp*>( parents.front()->imp() );
  const std::vector<Coordinate> points = polygon->points();

  uint sides = points.size();
  for ( uint i = 0; i < sides; ++i )
  {
    uint nexti = ( i + 1 < sides )?(i + 1):0;
    SegmentImp segment = SegmentImp( points[i], points[nexti] );
    drawer.draw( segment, p, true );
  }
}

std::vector<ObjectHolder*> PolygonSideTypeConstructor::build( const std::vector<ObjectCalcer*>& parents, KigDocument&, KigWidget& ) const
{
  std::vector<ObjectHolder*> ret;
  assert( parents.size() == 1 );
  const FilledPolygonImp* polygon = dynamic_cast<const FilledPolygonImp*>( parents.front()->imp() );
  const std::vector<Coordinate> points = polygon->points();

  uint sides = points.size();

  for ( uint i = 0; i < sides; ++i )
  {
    ObjectConstCalcer* d = new ObjectConstCalcer( new IntImp( i ) );
    std::vector<ObjectCalcer*> args( parents );
    args.push_back( d );
    ret.push_back( new ObjectHolder( new ObjectTypeCalcer( mtype, args ) ) );
  }
  return ret;
}

void PolygonSideTypeConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool PolygonSideTypeConstructor::isTransform() const
{
  return false;
}

/*
 * polygon by center and vertex
 */

PolygonBCVConstructor::PolygonBCVConstructor()
  : mtype( PolygonBCVType::instance() )
{
}

PolygonBCVConstructor::~PolygonBCVConstructor()
{
}

const QString PolygonBCVConstructor::descriptiveName() const
{
  return i18n("Regular Polygon with Given Center");
}

const QString PolygonBCVConstructor::description() const
{
  return i18n("Construct a regular polygon with a given center and vertex");
}

const QByteArray PolygonBCVConstructor::iconFileName( const bool ) const
{
  return "hexagonbcv";
}

bool PolygonBCVConstructor::isAlreadySelectedOK(
 const std::vector<ObjectCalcer*>&, const uint& ) const
{
  return false;
}

int PolygonBCVConstructor::wantArgs( const std::vector<ObjectCalcer*>& os,
                                     const KigDocument&,
                                     const KigWidget& ) const
{
  if ( os.size() > 3 ) return ArgsParser::Invalid;

  uint imax = ( os.size() <= 2) ? os.size() : 2;
  for ( uint i = 0; i < imax; ++i )
    if ( ! ( os[i]->imp()->inherits( PointImp::stype() ) ) ) return ArgsParser::Invalid;

  if ( os.size() < 3 ) return ArgsParser::Valid;

  if ( ! ( os[2]->imp()->inherits( BogusPointImp::stype() ) ) )
    return ArgsParser::Invalid;

  return ArgsParser::Complete;
}

void PolygonBCVConstructor::handleArgs(
  const std::vector<ObjectCalcer*>& os, KigPart& d,
  KigWidget& v ) const
{
  std::vector<ObjectHolder*> bos = build( os, d.document(), v );
  for ( std::vector<ObjectHolder*>::iterator i = bos.begin();
        i != bos.end(); ++i )
  {
    (*i)->calc( d.document() );
  }

  d.addObjects( bos );
}

void PolygonBCVConstructor::handlePrelim(
  KigPainter& p, const std::vector<ObjectCalcer*>& os,
  const KigDocument& d, const KigWidget&
  ) const
{
  if ( os.size() < 2 ) return;

  for ( uint i = 0; i < 2; i++ )
  {
    assert ( os[i]->imp()->inherits( PointImp::stype() ) );
  }

  Coordinate c = static_cast<const PointImp*>( os[0]->imp() )->coordinate();
  Coordinate v = static_cast<const PointImp*>( os[1]->imp() )->coordinate();

  int nsides = 6;
  bool moreinfo = false;
  int winding = 0;    // 0 means allow winding > 1
  if ( os.size() == 3 )
  {
    assert ( os[2]->imp()->inherits( BogusPointImp::stype() ) );
    Coordinate cntrl = static_cast<const PointImp*>( os[2]->imp() )->coordinate();
    nsides = computeNsides( c, v, cntrl, winding );
    moreinfo = true;
  }

  std::vector<ObjectCalcer*> args;
  args.push_back( os[0] );
  args.push_back( os[1] );
  ObjectConstCalcer* ns = new ObjectConstCalcer( new IntImp( nsides ) );
  args.push_back( ns );
  if ( winding > 1 )
  {
    ns = new ObjectConstCalcer( new IntImp( winding ) );
    args.push_back( ns );
  }

  p.setBrushStyle( Qt::NoBrush );
  p.setBrushColor( Qt::red );
  p.setPen( QPen ( Qt::red,  1) );
  p.setWidth( -1 ); // -1 means the default width for the object being
                    // drawn..

  ObjectDrawer drawer( Qt::red );
  drawprelim( drawer, p, args, d );
  if ( moreinfo )
  {
    p.setPointStyle( Kig::RoundEmpty );
    p.setWidth( 6 );
    double ro = 1.0/(2.5);
    Coordinate where = getRotatedCoord( c, (1-ro)*c+ro*v, 4*M_PI/5.0 );
    PointImp ptn = PointImp( where );
    TextImp text = TextImp( QStringLiteral("(5,2)"), where, false );
    ptn.draw( p );
    text.draw( p );
    for ( int i = 3; i < 9; ++i )
    {
      where = getRotatedCoord( c, v, 2.0*M_PI/i );
      ptn = PointImp( where );
      ptn.draw( p );
      if ( i > 5 ) continue;
      text = TextImp( QStringLiteral( "(%1)" ).arg(i), where, false );
      text.draw( p );
    }
    p.setStyle( Qt::DotLine );
    p.setWidth( 1 );
    double radius = ( v - c ).length();
    CircleImp circle = CircleImp( c, radius );
    circle.draw( p );
    for ( int i = 2; i < 5; i++ )
    {
      ro = 1.0/(i+0.5);
      CircleImp circle = CircleImp( c, ro*radius );
      circle.draw( p );
    }
  }
  delete_all( args.begin() + 2, args.end() );
}

std::vector<ObjectHolder*> PolygonBCVConstructor::build( const std::vector<ObjectCalcer*>& parents, KigDocument&, KigWidget& ) const
{
  assert ( parents.size() == 3 );
  std::vector<ObjectCalcer*> args;

  Coordinate c = static_cast<const PointImp*>( parents[0]->imp() )->coordinate();
  Coordinate v = static_cast<const PointImp*>( parents[1]->imp() )->coordinate();
  Coordinate cntrl = static_cast<const PointImp*>( parents[2]->imp() )->coordinate();

  args.push_back( parents[0] );
  args.push_back( parents[1] );
  int winding = 0;
  int nsides = computeNsides( c, v, cntrl, winding );
  ObjectConstCalcer* d = new ObjectConstCalcer( new IntImp( nsides ) );
  args.push_back( d );
  if ( winding > 1 )
  {
    d = new ObjectConstCalcer( new IntImp( winding ) );
    args.push_back( d );
  }

  ObjectTypeCalcer* calcer = new ObjectTypeCalcer( mtype, args );
  ObjectHolder* h = new ObjectHolder( calcer );
  std::vector<ObjectHolder*> ret;
  ret.push_back( h );
  return ret;
}

QString PolygonBCVConstructor::useText( const ObjectCalcer&, const std::vector<ObjectCalcer*>& os,
                                          const KigDocument&, const KigWidget& ) const
{
  switch ( os.size() )
  {
    case 1:
    return i18n( "Construct a regular polygon with this center" );
    break;

    case 2:
    return i18n( "Construct a regular polygon with this vertex" );
    break;

    case 3:
    Coordinate c = static_cast<const PointImp*>( os[0]->imp() )->coordinate();
    Coordinate v = static_cast<const PointImp*>( os[1]->imp() )->coordinate();
    Coordinate cntrl = static_cast<const PointImp*>( os[2]->imp() )->coordinate();
    int winding = 0;
    int nsides = computeNsides( c, v, cntrl, winding );

    if ( winding > 1 )
    {
      QString result = i18n( "Adjust the number of sides (%1/%2)", nsides, winding );
      return result;
    } else
    {
      QString result = i18n( "Adjust the number of sides (%1)", nsides );
      return result;
    }
    break;
  }

  return QLatin1String("");
}

QString PolygonBCVConstructor::selectStatement(
  const std::vector<ObjectCalcer*>& os, const KigDocument&,
  const KigWidget& ) const
{
  switch ( os.size() )
  {
    case 1:
    return i18n( "Select the center of the new polygon..." );
    break;

    case 2:
    return i18n( "Select a vertex for the new polygon..." );
    break;

    case 3:
    return i18n( "Move the cursor to get the desired number of sides..." );
    break;
  }

  return QLatin1String("");
}

void PolygonBCVConstructor::drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                                   const KigDocument& doc ) const
{
  if ( parents.size() < 3 || parents.size() > 4 ) return;

  assert ( parents[0]->imp()->inherits( PointImp::stype() ) &&
           parents[1]->imp()->inherits( PointImp::stype() ) &&
           parents[2]->imp()->inherits( IntImp::stype() ) );

  if ( parents.size() == 4 )
    assert ( parents[3]->imp()->inherits( IntImp::stype() ) );

  Args args;
  std::transform( parents.begin(), parents.end(),
               std::back_inserter( args ), std::mem_fun( &ObjectCalcer::imp ) );

  ObjectImp* data = mtype->calc( args, doc );
  drawer.draw( *data, p, true );
  delete data;
  data = 0;
}

void PolygonBCVConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool PolygonBCVConstructor::isTransform() const
{
  return false;
}

Coordinate PolygonBCVConstructor::getRotatedCoord( const Coordinate& c, 
               const Coordinate& v, double alpha ) const
{
  double cosalpha = cos(alpha);
  double sinalpha = sin(alpha);
  double dx = v.x - c.x;
  double dy = v.y - c.y;
  return c + Coordinate( cosalpha*dx - sinalpha*dy, sinalpha*dx + cosalpha*dy );
}

int PolygonBCVConstructor::computeNsides ( const Coordinate& c,
        const Coordinate& v, const Coordinate& cntrl, int& winding ) const
{
  Coordinate lvect = v - c;
  Coordinate rvect = cntrl - c;

  double angle = atan2( rvect.y, rvect.x ) - atan2( lvect.y, lvect.x );
  angle = fabs( angle/(2*M_PI) );
  while ( angle > 1 ) angle -= 1;
  if ( angle > 0.5 ) angle = 1 - angle;

  double realsides = 1.0/angle;    // this is bigger that 2
  if ( angle == 0. ) realsides = 3;
  if ( winding <= 0 )              // free to compute winding
  {
    winding = 1;
    double ratio = lvect.length()/rvect.length();
    winding = int ( ratio );
    if ( winding < 1 ) winding = 1;
    if ( winding > 50 ) winding = 50;
  }
  int nsides = int( winding*realsides + 0.5 );  // nsides/winding should be reduced!
  if ( nsides > 100 ) nsides = 100;     // well, 100 seems large enough!
  if ( nsides < 3 ) nsides = 3;
  while ( !relativePrimes ( nsides, winding ) ) ++nsides;
  return nsides;
}

/*
 * generic Bézier curve constructor
 */

BezierCurveTypeConstructor::BezierCurveTypeConstructor()
  : PointSequenceConstructor(
    I18N_NOOP( "Bézier Curve by its Control Points" ),
    I18N_NOOP( "Construct a Bézier curve by giving its control points" ),
    "bezierN",
    BezierCurveType::instance() )
{
}

BezierCurveTypeConstructor::~BezierCurveTypeConstructor()
{
}

bool BezierCurveTypeConstructor::isAlreadySelectedOK(
 const std::vector<ObjectCalcer*>& os, const uint& pos ) const
{
  if ( pos == os.size() - 1 && os.size() >= 3 ) return true;
  return false;
}

int BezierCurveTypeConstructor::wantArgs( const std::vector<ObjectCalcer*>& os,
                                          const KigDocument&,
                                          const KigWidget& ) const
{
  int count=os.size() - 1;

  for ( int i = 0; i <= count; i++ )
  {
    if ( ! ( os[i]->imp()->inherits( PointImp::stype() ) ) ) return ArgsParser::Invalid;
  }
  if ( count < 3 ) return ArgsParser::Valid;
  if ( os[count] == os[count - 1] ) return ArgsParser::Complete;
  return ArgsParser::Valid;
}

QString BezierCurveTypeConstructor::useText( const ObjectCalcer&, const std::vector<ObjectCalcer*>& os,
                                             const KigDocument&, const KigWidget& ) const
{
  if ( os.size() > 3 )
    return i18n("... with this control point (click again on the last control point to terminate construction)");
  else return i18n("Construct a Bézier curve with this control point");
}

QString BezierCurveTypeConstructor::selectStatement(
  const std::vector<ObjectCalcer*>&, const KigDocument&,
  const KigWidget& ) const
{
  return i18n("Select a point to be a control point of the new Bézier curve...");
}

void BezierCurveTypeConstructor::drawprelim( const ObjectDrawer& , 
                                             KigPainter& p, 
                                             const std::vector<ObjectCalcer*>& parents,
                                             const KigDocument& ) const
{
  if ( parents.size() < 2 ) return;

  std::vector<Coordinate> points;

  for ( uint i = 0; i < parents.size(); ++i )
  {
    const Coordinate vertex =
        static_cast<const PointImp*>( parents[i]->imp() )->coordinate();
    points.push_back( vertex );
  }

  BezierImp B = BezierImp( points );
  B.draw( p );
}

/*
 * generic rational Bézier curve constructor
 */

RationalBezierCurveTypeConstructor::RationalBezierCurveTypeConstructor()
{
}

RationalBezierCurveTypeConstructor::~RationalBezierCurveTypeConstructor()
{
}

const QString RationalBezierCurveTypeConstructor::descriptiveName() const
{
  return i18n( "Rational Bézier Curve by its Control Points" );
}

const QString RationalBezierCurveTypeConstructor::description() const
{
  return i18n( "Construct a Bézier curve by giving its control points and positive weights" );
}

const QByteArray RationalBezierCurveTypeConstructor::iconFileName( const bool ) const
{
  return "rbezierN";
}

bool RationalBezierCurveTypeConstructor::isAlreadySelectedOK(
 const std::vector<ObjectCalcer*>& os, const uint& pos ) const
{
  if ( pos % 2 == 1 ) return true;
  if ( pos == os.size() - 2 && os.size() >= 3 ) return true;
  return false;
}

int RationalBezierCurveTypeConstructor::wantArgs( const std::vector<ObjectCalcer*>& os,
                                                  const KigDocument&,
                                                  const KigWidget& ) const
{
  int count=os.size() - 1;

  for ( int i = 0; i <= count; i++ )
  {
    if ( ! ( os[i]->imp()->inherits( i % 2 == 0 ? PointImp::stype() : &weightimptypeinstance ) ) ) 
      return ArgsParser::Invalid;
  }
  if ( count < 6 ) return ArgsParser::Valid;
  if ( count % 2 == 0 && ( os[count] == os[count - 2] ) ) return ArgsParser::Complete;
  return ArgsParser::Valid;
}

std::vector<ObjectHolder*> RationalBezierCurveTypeConstructor::build( const std::vector<ObjectCalcer*>& parents, KigDocument&, KigWidget& ) const
{
  uint count = parents.size() - 1;
  assert ( count >= 3 );
  std::vector<ObjectCalcer*> args;
  for ( uint i = 0; i < count; ++i ) args.push_back( parents[i] );
  ObjectTypeCalcer* calcer = new ObjectTypeCalcer( RationalBezierCurveType::instance(), args );
  ObjectHolder* h = new ObjectHolder( calcer );
  std::vector<ObjectHolder*> ret;
  ret.push_back( h );
  return ret;
}

void RationalBezierCurveTypeConstructor::handleArgs( const std::vector<ObjectCalcer*>& os, 
                                                     KigPart& d,
                                                     KigWidget& v ) const
{
  std::vector<ObjectHolder*> bos = build( os, d.document(), v );
  for ( std::vector<ObjectHolder*>::iterator i = bos.begin();
        i != bos.end(); ++i )
  {
    (*i)->calc( d.document() );
  }

  d.addObjects( bos );
}

QString RationalBezierCurveTypeConstructor::useText( const ObjectCalcer&, 
                                                     const std::vector<ObjectCalcer*>& os,
                                                     const KigDocument&, 
                                                     const KigWidget& ) const
{
  if ( os.size() % 2 == 0 )
    return i18n("... assign this weight to last selected control point");

  if ( os.size() > 6 )
    return i18n("... with this control point (click again on the last control point or weight to terminate construction)");
  else return i18n("Construct a rational Bézier curve with this control point");
}

QString RationalBezierCurveTypeConstructor::selectStatement(
  const std::vector<ObjectCalcer*>& os, const KigDocument&,
  const KigWidget& ) const
{
  if ( os.size() % 2 == 0 )
    return i18n("Select a point to be a control point of the new rational Bézier curve...");
  else
    return i18n("Select a numeric label to be a weight of last selected point...");
}

void RationalBezierCurveTypeConstructor::drawprelim( const ObjectDrawer& , 
                                                     KigPainter& p, 
                                                     const std::vector<ObjectCalcer*>& parents,
                                                     const KigDocument& ) const
{
  if ( parents.size() < 5 ) return;

  std::vector<Coordinate> points;
  std::vector<double> weights;

  uint count = parents.size();
  for ( uint i = 0; i < count; i += 2 )
  {
    bool valid;
    assert ( parents[i]->imp()->inherits( PointImp::stype() ) );
    const Coordinate vertex =
        static_cast<const PointImp*>( parents[i]->imp() )->coordinate();
    points.push_back( vertex );
    if ( i+1 >= count ) break;
    assert ( parents[i+1]->imp()->inherits( &weightimptypeinstance ) );
    const double weight =
        getDoubleFromImp( parents[i+1]->imp(), valid );
    assert ( valid );
    weights.push_back( weight );
  }

  if ( count % 2 == 1 )
  {
                                            // point was selected, we
    weights.push_back( 1 );                 // don't have its weight so far
  }

  assert ( points.size() == weights.size() );

  RationalBezierImp rB = RationalBezierImp( points, weights );
  rB.draw( p );
}

void RationalBezierCurveTypeConstructor::handlePrelim(
  KigPainter& p, const std::vector<ObjectCalcer*>& os,
  const KigDocument& d, const KigWidget&
  ) const
{
  uint count = os.size();
  if ( count < 5 ) return;

  for ( uint i = 0; i < count; i += 2 )
  {
    assert ( os[i]->imp()->inherits( PointImp::stype() ) );
    if ( i+1 >= count ) break;
    assert ( os[i+1]->imp()->inherits( &weightimptypeinstance ) );
  }

  std::vector<ObjectCalcer*> args = os;
  p.setBrushStyle( Qt::NoBrush );
  p.setBrushColor( Qt::red );
  p.setPen( QPen ( Qt::red,  1) );
  p.setWidth( -1 ); // -1 means the default width for the object being
                    // drawn..

  ObjectDrawer drawer( Qt::red );
  drawprelim( drawer, p, args, d );
}

void RationalBezierCurveTypeConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool RationalBezierCurveTypeConstructor::isTransform() const
{
  return false;
}


/*
 * ConicConic intersection...
 */

static const ArgsParser::spec argsspectc[] = {
  { ConicImp::stype(), "SHOULD NOT BE SEEN", "SHOULD NOT BE SEEN", true },
  { ConicImp::stype(), "SHOULD NOT BE SEEN", "SHOULD NOT BE SEEN", true }
};

ConicConicIntersectionConstructor::ConicConicIntersectionConstructor()
  : StandardConstructorBase( "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
                             "curvelineintersection", mparser ),
    mparser( argsspectc, 2 )
{
}

ConicConicIntersectionConstructor::~ConicConicIntersectionConstructor()
{
}

void ConicConicIntersectionConstructor::drawprelim( const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                                                    const KigDocument& ) const
{
  if ( parents.size() != 2 ) return;
  assert ( parents[0]->imp()->inherits( ConicImp::stype() ) &&
           parents[1]->imp()->inherits( ConicImp::stype() ) );
  const ConicCartesianData conica =
    static_cast<const ConicImp*>( parents[0]->imp() )->cartesianData();
  const ConicCartesianData conicb =
    static_cast<const ConicImp*>( parents[1]->imp() )->cartesianData();
  bool ok = true;
  for ( int wr = -1; wr < 2; wr += 2 )
  {
    LineData radical = calcConicRadical( conica, conicb, wr, 1, ok );
    if ( ok )
    {
      for ( int wi = -1; wi < 2; wi += 2 )
      {
        Coordinate c = calcConicLineIntersect( conica, radical, 0.0, wi );
        if ( c.valid() ) {
          PointImp pi( c );
          drawer.draw( pi, p, true );
        }
      };
    };
  };
}

std::vector<ObjectHolder*> ConicConicIntersectionConstructor::build(
  const std::vector<ObjectCalcer*>& os, KigDocument& doc, KigWidget& ) const
{
  assert( os.size() == 2 );
  std::vector<ObjectHolder*> ret;
  ObjectCalcer* conica = os[0];
  ObjectConstCalcer* zeroindexdo = new ObjectConstCalcer( new IntImp( 1 ) );

  for ( int wr = -1; wr < 2; wr += 2 )
  {
    std::vector<ObjectCalcer*> args = os;
    args.push_back( new ObjectConstCalcer( new IntImp( wr ) ) );
    args.push_back( zeroindexdo );
    ObjectTypeCalcer* radical =
      new ObjectTypeCalcer( ConicRadicalType::instance(), args );
    radical->calc( doc );
    for ( int wi = -1; wi < 2; wi += 2 )
    {
      args.clear();
      args.push_back( conica );
      args.push_back( radical );
      args.push_back( new ObjectConstCalcer( new IntImp( wi ) ) );
      ret.push_back(
        new ObjectHolder(
          new ObjectTypeCalcer(
            ConicLineIntersectionType::instance(), args ) ) );
    };
  };
  return ret;
}

void ConicConicIntersectionConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool ConicConicIntersectionConstructor::isTransform() const
{
  return false;
}

ConicLineIntersectionConstructor::ConicLineIntersectionConstructor()
  : MultiObjectTypeConstructor(
    ConicLineIntersectionType::instance(),
    "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
    "curvelineintersection", -1, 1 )
{
}

ConicLineIntersectionConstructor::~ConicLineIntersectionConstructor()
{
}

ArcLineIntersectionConstructor::ArcLineIntersectionConstructor()
  : MultiObjectTypeConstructor(
    ArcLineIntersectionType::instance(),
    "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
    "curvelineintersection", -1, 1 )
{
}

ArcLineIntersectionConstructor::~ArcLineIntersectionConstructor()
{
}

QString ConicRadicalConstructor::useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>&,
                                          const KigDocument&, const KigWidget& ) const
{
  if ( o.imp()->inherits( CircleImp::stype() ) )
    return i18n( "Construct the Radical Lines of This Circle" );
  else
    return i18n( "Construct the Radical Lines of This Conic" );
}

/*
 * generic affinity and generic projectivity.  A unique affinity can be
 * obtained by specifying the image of three points (four for projectivity)
 * in the end we need, besides the object to be transformed, a total of
 * six point or (alternatively) two triangles; our affinity will map the
 * first triangle onto the second with corresponding ordering of their
 * vertices.  Since we allow for two different ways of specifying the six
 * points we shall use a Generic constructor, like that for intersections.
 */

GenericAffinityConstructor::GenericAffinityConstructor()
  : MergeObjectConstructor(
    I18N_NOOP( "Generic Affinity" ),
    I18N_NOOP( "The unique affinity that maps three points (or a triangle) onto three other points (or a triangle)" ),
    "genericaffinity" )
{
  SimpleObjectTypeConstructor* b2tr =
     new SimpleObjectTypeConstructor(
      AffinityB2TrType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "genericaffinity" );

  SimpleObjectTypeConstructor* gi3p =
     new SimpleObjectTypeConstructor(
      AffinityGI3PType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "genericaffinity" );

  merge( b2tr );
  merge( gi3p );
}

GenericAffinityConstructor::~GenericAffinityConstructor() {}

bool GenericAffinityConstructor::isAlreadySelectedOK(const std::vector< ObjectCalcer* >& , const uint& ) const
{
  return true;
}

GenericProjectivityConstructor::GenericProjectivityConstructor()
  : MergeObjectConstructor(
    I18N_NOOP( "Generic Projective Transformation" ),
    I18N_NOOP( "The unique projective transformation that maps four points (or a quadrilateral) onto four other points (or a quadrilateral)" ),
    "genericprojectivity" )
{
  SimpleObjectTypeConstructor* b2qu =
     new SimpleObjectTypeConstructor(
      ProjectivityB2QuType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "genericprojectivity" );

  SimpleObjectTypeConstructor* gi4p =
     new SimpleObjectTypeConstructor(
      ProjectivityGI4PType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "genericprojectivity" );

  merge( b2qu );
  merge( gi4p );
}

GenericProjectivityConstructor::~GenericProjectivityConstructor() {}

bool GenericProjectivityConstructor::isAlreadySelectedOK(const std::vector< ObjectCalcer* >& , const uint& ) const
{
  return true;
}

/*
 * inversion of points, lines with respect to a circle
 */

InversionConstructor::InversionConstructor()
  : MergeObjectConstructor(
    I18N_NOOP( "Inversion of Point, Line or Circle" ),
    I18N_NOOP( "The inversion of a point, line or circle with respect to a circle" ),
    "inversion" )
{
  SimpleObjectTypeConstructor* pointobj =
     new SimpleObjectTypeConstructor(
      InvertPointType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "inversion" );

  SimpleObjectTypeConstructor* curveobj =
     new SimpleObjectTypeConstructor(
      CircularInversionType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "inversion" );

//  SimpleObjectTypeConstructor* lineobj =
//     new SimpleObjectTypeConstructor(
//      InvertLineType::instance(),
//      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
//      "inversion" );
//
//  SimpleObjectTypeConstructor* segmentobj =
//     new SimpleObjectTypeConstructor(
//      InvertSegmentType::instance(),
//      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
//      "inversion" );
//
//  SimpleObjectTypeConstructor* circleobj =
//     new SimpleObjectTypeConstructor(
//      InvertCircleType::instance(),
//      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
//      "inversion" );
//
//  SimpleObjectTypeConstructor* arcobj =
//     new SimpleObjectTypeConstructor(
//      InvertArcType::instance(),
//      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
//      "inversion" );

//  merge( arcobj );
//  merge( circleobj );
  merge( curveobj );
  merge( pointobj );
//  merge( segmentobj );
//  merge( lineobj );
}

InversionConstructor::~InversionConstructor() {}

/*
 * Transport of Measure
 */

MeasureTransportConstructor::MeasureTransportConstructor()
  : mtype( MeasureTransportType::instance() )
{
}

MeasureTransportConstructor::~MeasureTransportConstructor()
{
}

const QString MeasureTransportConstructor::descriptiveName() const
{
  return i18n("Measure Transport");
}

const QString MeasureTransportConstructor::description() const
{
  return i18n("Transport the measure of a segment or arc over a line or circle.");
}

const QByteArray MeasureTransportConstructor::iconFileName( const bool ) const
{
  return "measuretransport";
}

bool MeasureTransportConstructor::isAlreadySelectedOK(
 const std::vector<ObjectCalcer*>&, const uint& ) const
{
  return false;
}

/*
 * we want the arguments in the exact order, this makes
 * the code simpler, but I guess it is also less confusing
 * to the user
 */

int MeasureTransportConstructor::wantArgs(
                                const std::vector<ObjectCalcer*>& os,
                                const KigDocument& doc,
                                const KigWidget& ) const
{
  if ( os.size() == 0 ) return ArgsParser::Valid;

  if ( ! os[0]->imp()->inherits( &lengthimptypeinstance ) )
    return ArgsParser::Invalid;

  if ( os.size() == 1 ) return ArgsParser::Valid;

  if ( ! os[1]->imp()->inherits( LineImp::stype() ) &&
       ! os[1]->imp()->inherits( CircleImp::stype() ) )
    return ArgsParser::Invalid;
  const CurveImp* c = static_cast<const CurveImp*>( os[1]->imp() );

  if ( os.size() == 2 ) return ArgsParser::Valid;

  if ( ! os[2]->imp()->inherits( PointImp::stype() ) )
    return ArgsParser::Invalid;
  const PointImp* p = static_cast<const PointImp*>( os[2]->imp() );

  // we have two choices:
  // - using "isPointOnCurve" produces a "by construction" incidence
  //   test.  This would be fine, but doesn't always work; e.g. if we
  //   have two points A, B, the segment s = AB and we construct the
  //   support line of the segment (property of segments), then kig
  //   is not able to understand that A is "by construction" on the
  //   constructed line.
  //   Moreover there are problems when hovering the cursor over points
  //   that are on both a segment and its support line.
  //      if ( ! isPointOnCurve( os[2], os[1] ) )
  // - using "containsPoint", which is actually the test performed
  //   when calc-ing the TransportOfMeasure; the risk here is to
  //   be able to select points that are only coincidentally on the line.

  if ( ! c->containsPoint( p->coordinate(), doc ) )
    return ArgsParser::Invalid;

  if ( os.size() == 3 ) return ArgsParser::Complete;

  return ArgsParser::Invalid;
}

void MeasureTransportConstructor::handleArgs(
  const std::vector<ObjectCalcer*>& os, KigPart& d,
  KigWidget& v ) const
{
  std::vector<ObjectHolder*> bos = build( os, d.document(), v );
  for ( std::vector<ObjectHolder*>::iterator i = bos.begin();
        i != bos.end(); ++i )
  {
    (*i)->calc( d.document() );
  }

  d.addObjects( bos );
}

void MeasureTransportConstructor::handlePrelim(
  KigPainter& p, const std::vector<ObjectCalcer*>& os,
  const KigDocument& d, const KigWidget&
  ) const
{
  p.setBrushStyle( Qt::NoBrush );
  p.setBrushColor( Qt::red );
  p.setPen( QPen ( Qt::red,  1) );
  p.setWidth( -1 ); // -1 means the default width for the object being
                    // drawn..

  ObjectDrawer drawer( Qt::red );
  drawprelim( drawer, p, os, d );
}

void MeasureTransportConstructor::drawprelim( const ObjectDrawer& drawer,
                         KigPainter& p,
                         const std::vector<ObjectCalcer*>& parents,
                         const KigDocument& doc ) const
{
  Args args;
  using namespace std;
  transform( parents.begin(), parents.end(),
             back_inserter( args ), mem_fun( &ObjectCalcer::imp ) );
  ObjectImp* data = mtype->calc( args, doc );
  drawer.draw( *data, p, true );
  delete data;
}

QString MeasureTransportConstructor::useText( const ObjectCalcer& o,
                        const std::vector<ObjectCalcer*>& os,
                        const KigDocument&, const KigWidget& ) const
{
  if ( o.imp()->inherits( SegmentImp::stype() ) )
    return i18n("Segment to transport");
  if ( o.imp()->inherits( ArcImp::stype() ) )
    return i18n("Arc to transport");
  if ( o.imp()->inherits( NumericTextImp::stype() ) )
    return i18n("Value to transport");
  if ( o.imp()->inherits( LineImp::stype() ) )
    return i18n("Transport a measure on this line");
  if ( o.imp()->inherits( CircleImp::stype() ) )
    return i18n("Transport a measure on this circle");
  if ( o.imp()->inherits( PointImp::stype() ) )
  {
    if ( os[1]->imp()->inherits( CircleImp::stype() ) )
      return i18n("Start transport from this point of the circle");
    if ( os[1]->imp()->inherits( LineImp::stype() ) )
      return i18n("Start transport from this point of the line");
    else
      return i18n("Start transport from this point of the curve");
      // well, this isn't impemented yet, should never get here
  }
  return QLatin1String("");
}

QString MeasureTransportConstructor::selectStatement(
  const std::vector<ObjectCalcer*>& os, const KigDocument&,
  const KigWidget& ) const
{
  switch ( os.size() )
  {
    case 0:
    return i18n( "Select a segment, arc or numeric label to be transported..." );
    break;

    case 1:
    return i18n( "Select a destination line or circle..." );
    break;

    case 2:
    return i18n( "Choose a starting point on the line/circle..." );
    break;
  }

  return QLatin1String("");
}

std::vector<ObjectHolder*> MeasureTransportConstructor::build(
    const std::vector<ObjectCalcer*>& parents,
    KigDocument&, KigWidget& ) const
{
  assert ( parents.size() == 3 );
//  std::vector<ObjectCalcer*> args;
//  for ( uint i = 0; i < count; ++i ) args.push_back( parents[i] );
  ObjectTypeCalcer* calcer = new ObjectTypeCalcer( mtype, parents );
  ObjectHolder* h = new ObjectHolder( calcer );
  std::vector<ObjectHolder*> ret;
  ret.push_back( h );
  return ret;
}

void MeasureTransportConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool MeasureTransportConstructor::isTransform() const
{
  return false;
}

/*
 * Generic intersection
 */

/*
 * these two argsparser spec vectors are used for the special
 * construction of conic-line and circle-circle constructions
 */

static const struct ArgsParser::spec argsspeccli[] =
{
  { ConicImp::stype(), I18N_NOOP( "Intersect with this conic" ),
    "SHOULD NOT BE SEEN", true },
  { AbstractLineImp::stype(), I18N_NOOP( "Intersect with this line" ),
    "SHOULD NOT BE SEEN", true }
};


static const struct ArgsParser::spec argsspeccbli[] =
{
  { CubicImp::stype(), I18N_NOOP( "Intersect with this cubic" ),
    "SHOULD NOT BE SEEN", true },
  { AbstractLineImp::stype(), I18N_NOOP( "Intersect with this line" ),
    "SHOULD NOT BE SEEN", true }
};


static const struct ArgsParser::spec argsspeccci[] =
{
  { CircleImp::stype(), I18N_NOOP( "Intersect with this circle" ),
    "SHOULD NOT BE SEEN", true },
  { CircleImp::stype(), I18N_NOOP( "Intersect with this circle" ),
    "SHOULD NOT BE SEEN", true }
};

GenericIntersectionConstructor::GenericIntersectionConstructor()
  : MergeObjectConstructor(
    I18N_NOOP( "Intersect" ),
    I18N_NOOP( "The intersection of two objects" ),
    "curvelineintersection" )
{
  // intersection type..
  // There is one "toplevel" object_constructor, that is composed
  // of multiple subconstructors..  First we build the
  // subconstructors:
  SimpleObjectTypeConstructor* lineline =
    new SimpleObjectTypeConstructor(
      LineLineIntersectionType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "curvelineintersection" );

  ObjectConstructor* lineconic =
//    new ConicLineIntersectionConstructor();
    new TwoOrOneIntersectionConstructor(
          ConicLineIntersectionType::instance(),
          ConicLineOtherIntersectionType::instance(),
          "curvelineintersection",
          argsspeccli);

  ObjectConstructor* arcline =
    new ArcLineIntersectionConstructor();

  ObjectConstructor* linecubic =
    new ThreeTwoOneIntersectionConstructor(
          CubicLineIntersectionType::instance(),
          CubicLineOtherIntersectionType::instance(),
          "curvelineintersection",
          argsspeccbli);

  ObjectConstructor* conicconic =
    new ConicConicIntersectionConstructor();

//  MultiObjectTypeConstructor* circlecircle =
//    new MultiObjectTypeConstructor(
//      CircleCircleIntersectionType::instance(),
//      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
//      "circlecircleintersection", -1, 1 );
  ObjectConstructor* circlecircle =
    new TwoOrOneIntersectionConstructor(
          CircleCircleIntersectionType::instance(),
          CircleCircleOtherIntersectionType::instance(),
          "circlecircleintersection",
          argsspeccci);

  SimpleObjectTypeConstructor* polygonline =
    new SimpleObjectTypeConstructor(
      PolygonLineIntersectionType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "curvelineintersection" );

  SimpleObjectTypeConstructor* polygonpolygon =
    new SimpleObjectTypeConstructor(
      PolygonPolygonIntersectionType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "curvelineintersection" );

  MultiObjectTypeConstructor* opolygonalline =
    new MultiObjectTypeConstructor(
      OPolygonalLineIntersectionType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "curvelineintersection", -1, 1 );

  MultiObjectTypeConstructor* cpolygonalline =
    new MultiObjectTypeConstructor(
      CPolygonalLineIntersectionType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "curvelineintersection", -1, 1 );

  merge( lineline );
  merge( circlecircle );
  merge( lineconic );
  merge( linecubic );
  merge( conicconic );
  merge( arcline );
  merge( polygonline );
  merge( opolygonalline );
  merge( cpolygonalline );
  merge( polygonpolygon );
}

GenericIntersectionConstructor::~GenericIntersectionConstructor()
{
}

bool GenericIntersectionConstructor::isIntersection() const
{
  return true;
}

QString GenericIntersectionConstructor::useText(
  const ObjectCalcer& o, const std::vector<ObjectCalcer*>& os,
  const KigDocument&, const KigWidget& ) const
{
  QString preamble;
  switch (os.size())
  {
    case 1:
      if ( o.imp()->inherits( CircleImp::stype() ) )
        return i18n( "Intersect this Circle" );
      else if ( o.imp()->inherits( ConicImp::stype() ) )
        return i18n( "Intersect this Conic" );
      else if ( o.imp()->inherits( SegmentImp::stype() ) )
        return i18n( "Intersect this Segment" );
      else if ( o.imp()->inherits( RayImp::stype() ) )
        return i18n( "Intersect this Half-line" );
      else if ( o.imp()->inherits( LineImp::stype() ) )
        return i18n( "Intersect this Line" );
      else if ( o.imp()->inherits( CubicImp::stype() ) )
        return i18n( "Intersect this Cubic Curve" );
      else if ( o.imp()->inherits( ArcImp::stype() ) )
        return i18n( "Intersect this Arc" );
      else if ( o.imp()->inherits( FilledPolygonImp::stype() ) )
        return i18n( "Intersect this Polygon" );
      else if ( o.imp()->inherits( AbstractPolygonImp::stype() ) )
        return i18n( "Intersect this Polygonal" );
      else assert( false );
      break;
    case 2:
      if ( o.imp()->inherits( CircleImp::stype() ) )
        return i18n( "with this Circle" );
      else if ( o.imp()->inherits( ConicImp::stype() ) )
        return i18n( "with this Conic" );
      else if ( o.imp()->inherits( SegmentImp::stype() ) )
        return i18n( "with this Segment" );
      else if ( o.imp()->inherits( RayImp::stype() ) )
        return i18n( "with this Half-line" );
      else if ( o.imp()->inherits( LineImp::stype() ) )
        return i18n( "with this Line" );
      else if ( o.imp()->inherits( CubicImp::stype() ) )
        return i18n( "with this Cubic Curve" );
      else if ( o.imp()->inherits( ArcImp::stype() ) )
        return i18n( "with this Arc" );
      else if ( o.imp()->inherits( FilledPolygonImp::stype() ) )
        return i18n( "with this Polygon" );
      else if ( o.imp()->inherits( AbstractPolygonImp::stype() ) )
        return i18n( "with this Polygonal" );
      else assert( false );
      break;
  }

  return QString();
}

static const ArgsParser::spec argsspecMidPointOfTwoPoints[] =
{
  { PointImp::stype(), I18N_NOOP( "Construct midpoint of this point and another one" ),
    I18N_NOOP( "Select the first of the points of which you want to construct the midpoint..." ), false },
  { PointImp::stype(), I18N_NOOP( "Construct the midpoint of this point and another one" ),
    I18N_NOOP( "Select the other of the points of which to construct the midpoint..." ), false }
};

MidPointOfTwoPointsConstructor::MidPointOfTwoPointsConstructor()
  : StandardConstructorBase( "Mid Point",
                             "Construct the midpoint of two points",
                             "bisection", mparser ),
    mparser( argsspecMidPointOfTwoPoints, 2 )
{
}

MidPointOfTwoPointsConstructor::~MidPointOfTwoPointsConstructor()
{
}

void MidPointOfTwoPointsConstructor::drawprelim(
  const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
  const KigDocument& ) const
{
  if ( parents.size() != 2 ) return;
  assert( parents[0]->imp()->inherits( PointImp::stype() ) );
  assert( parents[1]->imp()->inherits( PointImp::stype() ) );
  const Coordinate m =
    ( static_cast<const PointImp*>( parents[0]->imp() )->coordinate() +
      static_cast<const PointImp*>( parents[1]->imp() )->coordinate() ) / 2;
  drawer.draw( PointImp( m ), p, true );
}

std::vector<ObjectHolder*> MidPointOfTwoPointsConstructor::build(
  const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& ) const
{
  ObjectTypeCalcer* seg = new ObjectTypeCalcer( SegmentABType::instance(), os );
  seg->calc( d );
//  int index = seg->imp()->propertiesInternalNames().indexOf( "mid-point" );
//  assert( index != -1 );
  ObjectPropertyCalcer* prop = new ObjectPropertyCalcer( seg, "mid-point" );
  prop->calc( d );
  std::vector<ObjectHolder*> ret;
  ret.push_back( new ObjectHolder( prop ) );
  return ret;
}

void MidPointOfTwoPointsConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool MidPointOfTwoPointsConstructor::isTransform() const
{
  return false;
}

static const ArgsParser::spec argsspecGoldenPointOfTwoPoints[] =
{
  { PointImp::stype(), I18N_NOOP( "Construct golden ratio point of this point and another one" ),
    I18N_NOOP( "Select the first of the points of which you want to construct the golden ratio point..." ), false },
  { PointImp::stype(), I18N_NOOP( "Construct the golden ratio point of this point and another one" ),
    I18N_NOOP( "Select the other of the points of which to construct the golden ratio point..." ), false }
};

GoldenPointOfTwoPointsConstructor::GoldenPointOfTwoPointsConstructor()
  : StandardConstructorBase( "Golden Ratio Point",
                             "Construct the golden ratio point of two points",
                             "bisection", mparser ),
    mparser( argsspecGoldenPointOfTwoPoints, 2 )
{
}

GoldenPointOfTwoPointsConstructor::~GoldenPointOfTwoPointsConstructor()
{
}

void GoldenPointOfTwoPointsConstructor::drawprelim(
  const ObjectDrawer& drawer, KigPainter& p, const std::vector<ObjectCalcer*>& parents,
  const KigDocument& ) const
{
  if ( parents.size() != 2 ) return;
  assert( parents[0]->imp()->inherits( PointImp::stype() ) );
  assert( parents[1]->imp()->inherits( PointImp::stype() ) );
  const Coordinate m =
    ( static_cast<const PointImp*>( parents[0]->imp() )->coordinate() +
      (sqrt(5) - 1) / 2 *
      ( static_cast<const PointImp*>( parents[1]->imp() )->coordinate() -
        static_cast<const PointImp*>( parents[0]->imp() )->coordinate()
      )
    );
  drawer.draw( PointImp( m ), p, true );
}

std::vector<ObjectHolder*> GoldenPointOfTwoPointsConstructor::build(
  const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& ) const
{
  ObjectTypeCalcer* seg = new ObjectTypeCalcer( SegmentABType::instance(), os );
  seg->calc( d );
//  int index = seg->imp()->propertiesInternalNames().indexOf( "golden-point" );
//  assert( index != -1 );
  ObjectPropertyCalcer* prop = new ObjectPropertyCalcer( seg, "golden-point" );
  prop->calc( d );
  std::vector<ObjectHolder*> ret;
  ret.push_back( new ObjectHolder( prop ) );
  return ret;
}

void GoldenPointOfTwoPointsConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool GoldenPointOfTwoPointsConstructor::isTransform() const
{
  return false;
}

TestConstructor::TestConstructor( const ArgsParserObjectType* type, const char* descname,
                                  const char* desc, const char* iconfile )
  : StandardConstructorBase( descname, desc, iconfile, type->argsParser() ),
    mtype( type )
{
}

TestConstructor::~TestConstructor()
{
}

void TestConstructor::drawprelim( const ObjectDrawer&, KigPainter&, const std::vector<ObjectCalcer*>&,
                                  const KigDocument& ) const
{
  // not used, only here because of the wrong
  // ObjectConstructor-GUIAction design.  See the TODO
}

std::vector<ObjectHolder*> TestConstructor::build( const std::vector<ObjectCalcer*>&, KigDocument&,
                                                   KigWidget& ) const
{
  // not used, only here because of the wrong
  // ObjectConstructor-GUIAction design.  See the TODO
  std::vector<ObjectHolder*> ret;
  return ret;
}

void TestConstructor::plug( KigPart*, KigGUIAction* )
{
}

bool TestConstructor::isTransform() const
{
  return false;
}

bool TestConstructor::isTest() const
{
  return true;
}

BaseConstructMode* TestConstructor::constructMode( KigPart& doc )
{
  return new TestConstructMode( doc, mtype );
}

int TestConstructor::wantArgs( const std::vector<ObjectCalcer*>& os,
                                     const KigDocument& d, const KigWidget& v ) const
{
  int ret = StandardConstructorBase::wantArgs( os, d, v );
  if ( ret == ArgsParser::Complete ) ret = ArgsParser::Valid;
  return ret;
}

QString GenericIntersectionConstructor::selectStatement(
  const std::vector<ObjectCalcer*>& sel, const KigDocument&,
  const KigWidget& ) const
{
  if ( sel.size() == 0 )
    return i18n( "Select the first object to intersect..." );
  else
    return i18n( "Select the second object to intersect..." );
}

TangentConstructor::TangentConstructor()
  : MergeObjectConstructor(
    I18N_NOOP( "Tangent" ),
    I18N_NOOP( "The line tangent to a curve" ),
    "tangent" )
{
  SimpleObjectTypeConstructor* conic =
    new SimpleObjectTypeConstructor(
      TangentConicType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "tangentconic" );

  SimpleObjectTypeConstructor* arc =
    new SimpleObjectTypeConstructor(
      TangentArcType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "tangentarc" );

  SimpleObjectTypeConstructor* cubic =
    new SimpleObjectTypeConstructor(
      TangentCubicType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "tangentcubic" );

  SimpleObjectTypeConstructor* curve =
    new SimpleObjectTypeConstructor(
      TangentCurveType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "tangentcurve" );

  merge( conic );
  merge( arc );
  merge( cubic );
  merge( curve );
}

TangentConstructor::~TangentConstructor()
{
}

QString TangentConstructor::useText(
  const ObjectCalcer& o, const std::vector<ObjectCalcer*>&,
  const KigDocument&, const KigWidget& ) const
{
  if ( o.imp()->inherits( CircleImp::stype() ) )
    return i18n( "Tangent to This Circle" );
  else if ( o.imp()->inherits( ConicImp::stype() ) )
    return i18n( "Tangent to This Conic" );
  else if ( o.imp()->inherits( ArcImp::stype() ) )
    return i18n( "Tangent to This Arc" );
  else if ( o.imp()->inherits( CubicImp::stype() ) )
    return i18n( "Tangent to This Cubic Curve" );
  else if ( o.imp()->inherits( CurveImp::stype() ) )
    return i18n( "Tangent to This Curve" );
  else if ( o.imp()->inherits( PointImp::stype() ) )
    return i18n( "Tangent at This Point" );
//  else assert( false );
  return QString();
}

//QString TangentConstructor::selectStatement(
//  const std::vector<ObjectCalcer*>& sel, const KigDocument&,
//  const KigWidget& ) const
//{
//  if ( sel.size() == 0 )
//    return i18n( "Select the object..." );
//  else
//    return i18n( "Select the point for the tangent to go through..." );
//}

/*
 * center of curvature of a curve
 */

CocConstructor::CocConstructor()
  : MergeObjectConstructor(
    I18N_NOOP( "Center Of Curvature" ),
    I18N_NOOP( "The center of the osculating circle to a curve" ),
    "centerofcurvature" )
{
  SimpleObjectTypeConstructor* conic =
    new SimpleObjectTypeConstructor(
      CocConicType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "cocconic" );

  SimpleObjectTypeConstructor* cubic =
    new SimpleObjectTypeConstructor(
      CocCubicType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "coccubic" );

  SimpleObjectTypeConstructor* curve =
    new SimpleObjectTypeConstructor(
      CocCurveType::instance(),
      "SHOULDNOTBESEEN", "SHOULDNOTBESEEN",
      "coccurve" );

  merge( conic );
  merge( cubic );
  merge( curve );
}

CocConstructor::~CocConstructor()
{
}

QString CocConstructor::useText(
  const ObjectCalcer& o, const std::vector<ObjectCalcer*>&,
  const KigDocument&, const KigWidget& ) const
{
  if ( o.imp()->inherits( ConicImp::stype() ) )
    return i18n( "Center of Curvature of This Conic" );
  else if ( o.imp()->inherits( CubicImp::stype() ) )
    return i18n( "Center of Curvature of This Cubic Curve" );
  else if ( o.imp()->inherits( CurveImp::stype() ) )
    return i18n( "Center of Curvature of This Curve" );
  else if ( o.imp()->inherits( PointImp::stype() ) )
    return i18n( "Center of Curvature at This Point" );
  return QString();
}

bool relativePrimes( int n, int p )
{
  if ( p > n ) return relativePrimes( p, n );
  assert ( p >= 0 );
  if ( p == 0 ) return false;
  if ( p == 1 ) return true;
  int d = int( n/p );
  return relativePrimes( p, n-d*p );
}

//QString CocConstructor::selectStatement(
//  const std::vector<ObjectCalcer*>& sel, const KigDocument&,
//  const KigWidget& ) const
//{
//  if ( sel.size() == 0 )
//    return i18n( "Select the object..." );
//  else
//    return i18n( "Select the point where to compute the center of curvature..." );
//}
