// SPDX-FileCopyrightText: 2003 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

// mp: following a kind suggestion by David Faure the Python.h include has
// been moved before all qt includes in order to avoid a clash related to
// the "slots" identifier
// krazy:excludeall=includes
#undef _XOPEN_SOURCE // it will be defined inside Python
#include <Python.h>
#include "python_scripter.h"

#include <iostream>
#include <string>

#include <boost/python.hpp>
#include <boost/mpl/bool.hpp>

#include "../misc/common.h"
#include "../misc/coordinate.h"
#include "../misc/cubic-common.h"
#include "../misc/kigtransform.h"
#include "../objects/bogus_imp.h"
#include "../objects/common.h"
#include "../objects/circle_imp.h"
#include "../objects/cubic_imp.h"
#include "../objects/line_imp.h"
#include "../objects/other_imp.h"
#include "../objects/point_imp.h"
#include "../objects/text_imp.h"
#include "../objects/polygon_imp.h"

using namespace boost::python;

BOOST_PYTHON_MODULE_INIT( kig )
{
  class_<Coordinate>( "Coordinate" )
    .def( init<double, double>() )
    .def( init<const Coordinate&>() )
    .def( "invalidCoord", &Coordinate::invalidCoord )
    .staticmethod( "invalidCoord" )
    .def( "valid", &Coordinate::valid )
    .def( "distance", &Coordinate::distance )
    .def( "length", &Coordinate::length )
    .def( "squareLength", &Coordinate::squareLength )
    .def( "orthogonal", &Coordinate::orthogonal )
    .def( "round", &Coordinate::round )
    .def( "normalize", &Coordinate::normalize )
    .def( -self )
//    .def( self = self )
    .def( self += self )
    .def( self -= self )
    .def( self *= other<double>() )
    .def( self *= other<int>() )
    .def( self /= other<double>() )
    .def( self / other<double>() )
    .def( self + self )
    .def( self - self )
    .def( self * other<double>() )
    .def( other<double>() * self )
    .def( self * self )
    .def_readwrite( "x", &Coordinate::x )
    .def_readwrite( "y", &Coordinate::y )
    ;

  class_<LineData>( "LineData" )
    .def( init<Coordinate, Coordinate>() )
    .def( "dir", &LineData::dir )
    .def( "length", &LineData::length )
    .def( "isParallelTo", &LineData::isParallelTo )
    .def_readwrite( "a", &LineData::a )
    .def_readwrite( "b", &LineData::b )
    ;

  // we need this cause Transformation::apply is overloaded and
  // otherwise using Transformation::apply would be ambiguous..
  const Coordinate (Transformation::*transformapplyfunc)( const Coordinate& ) const = &Transformation::apply;
  class_<Transformation>( "Transformation", no_init )
    .def( "apply", transformapplyfunc )
    .def( "isHomothetic", &Transformation::isHomothetic )
    .def( "inverse", &Transformation::inverse )
    .def( "identity", &Transformation::identity )
    .def( "translation", &Transformation::translation )
    .def( "rotation", &Transformation::rotation )
    .def( "pointReflection", &Transformation::pointReflection )
    .def( "lineReflection", &Transformation::lineReflection )
    .def( "castShadow", &Transformation::castShadow )
    .def( "projectiveRotation", &Transformation::projectiveRotation )
    .def( "scalingOverPoint", &Transformation::scalingOverPoint )
    .def( "scalingOverLine", &Transformation::scalingOverLine )
    .def( self * self )
    .def( self == self )
    .staticmethod( "identity" )
    .staticmethod( "translation" )
    .staticmethod( "rotation" )
    .staticmethod( "pointReflection" )
    .staticmethod( "lineReflection" )
    .staticmethod( "castShadow" )
    .staticmethod( "projectiveRotation" )
    .staticmethod( "scalingOverPoint" )
    .staticmethod( "scalingOverLine" )
    ;

  class_<ObjectImpType, boost::noncopyable>( "ObjectType", no_init )
    .def( "fromInternalName", &ObjectImpType::typeFromInternalName,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "fromInternalName" )
    .def( "inherits", &ObjectImpType::inherits )
    .def( "internalName", &ObjectImpType::internalName )
    .def( "translatedName", &ObjectImpType::translatedName )
    .def( "selectStatement", &ObjectImpType::selectStatement )
    .def( "removeAStatement", &ObjectImpType::removeAStatement )
    .def( "addAStatement", &ObjectImpType::addAStatement )
    .def( "moveAStatement", &ObjectImpType::moveAStatement )
    .def( "attachToThisStatement", &ObjectImpType::attachToThisStatement )
    ;

  class_<ObjectImp, boost::noncopyable>( "Object", no_init )
    .def( "stype", &ObjectImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    .def( "inherits", &ObjectImp::inherits )
    .def( "transform", &ObjectImp::transform,
          return_value_policy<manage_new_object>() )
    .def( "valid", &ObjectImp::valid )
    .def( "copy", &ObjectImp::copy,
          return_value_policy<manage_new_object>() )
    .def( "equals", &ObjectImp::equals )
    ;

  class_<CurveImp, bases<ObjectImp>, boost::noncopyable>( "Curve", no_init )
    .def( "stype", &CurveImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
//    .def( "getParam", &CurveImp::getParam )
//    .def( "getPoint", &CurveImp::getPoint );
    ;
  class_<PointImp, bases<ObjectImp> >( "Point", init<Coordinate>() )
    .def( "stype", &PointImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    .def( "coordinate", &PointImp::coordinate,
          return_internal_reference<1>() )
    .def( "setCoordinate", &PointImp::setCoordinate )
    ;

  class_<AbstractLineImp, bases<CurveImp>, boost::noncopyable >( "AbstractLine", no_init )
    .def( "stype", &AbstractLineImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    .def( "slope", &AbstractLineImp::slope )
    .def( "equationString", &AbstractLineImp::equationString )
    .def( "data", &AbstractLineImp::data )
    ;

  class_<SegmentImp, bases<AbstractLineImp> >( "Segment", init<Coordinate, Coordinate>() )
    .def( "stype", &SegmentImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    .def( init<LineData>() )
    .def( "length", &SegmentImp::length )
    ;

  class_<RayImp, bases<AbstractLineImp> >( "Ray", init<Coordinate, Coordinate>() )
    .def( "stype", &RayImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    .def( init<LineData>() )
    ;

  class_<LineImp, bases<AbstractLineImp> >( "Line", init<Coordinate, Coordinate>() )
    .def( "stype", &LineImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    .def( init<LineData>() )
    ;

  class_<ConicCartesianData>( "ConicCartesianData", init<double,double,double,double,double,double>() )
    .def( init<ConicPolarData>() )
    .def( "invalidData", &ConicCartesianData::invalidData )
    .staticmethod( "invalidData" )
    .def( "valid", &ConicCartesianData::valid )
//    .def( init<double[6]>() )
//    .def_readwrite( "coeffs", &ConicCartesianData::coeffs )
    ;

  class_<ConicPolarData>( "ConicPolarData", init<Coordinate, double, double, double>() )
    .def( init<ConicCartesianData>() )
    .def_readwrite( "focus1", &ConicPolarData::focus1 )
    .def_readwrite( "pdimen", &ConicPolarData::pdimen )
    .def_readwrite( "ecostheta0", &ConicPolarData::ecostheta0 )
    .def_readwrite( "esintheta0", &ConicPolarData::esintheta0 )
    ;

  class_<ConicImp, bases<CurveImp>, boost::noncopyable >( "Conic", no_init )
    .def( "stype", &ConicImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    .def( "conicType", &ConicImp::conicType )
//    .def( "conicTypeString", &ConicImp::conicTypeString )
//    .def( "cartesianEquationString", &ConicImp::cartesianEquationString )
//    .def( "polarEquationString", &ConicImp::polarEquationString )
    .def( "cartesianData", &ConicImp::cartesianData )
    .def( "polarData", &ConicImp::polarData )
    .def( "focus1", &ConicImp::focus1 )
    .def( "focus2", &ConicImp::focus2 )
    ;

  class_<ConicImpCart, bases<ConicImp> >( "CartesianConic", init<ConicCartesianData>() )
    ;
  class_<ConicImpPolar, bases<ConicImp> >( "PolarConic", init<ConicPolarData>() )
    ;

  class_<CircleImp, bases<ConicImp> >( "Circle", init<Coordinate, double>() )
    .def( "stype", &CircleImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    .def( "center", &CircleImp::center )
    .def( "radius", &CircleImp::radius )
    .def( "squareRadius", &CircleImp::squareRadius )
    .def( "surface", &CircleImp::surface )
    .def( "circumference", &CircleImp::circumference )
    ;

  class_<FilledPolygonImp, bases<ObjectImp>, boost::noncopyable>( "Polygon", no_init )
    .def( "stype", &FilledPolygonImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    .def( "npoints", &FilledPolygonImp::npoints )
    .def( "perimeter", &FilledPolygonImp::cperimeter )
    .def( "area", &FilledPolygonImp::area )
    .def( "windingNumber", &FilledPolygonImp::windingNumber )
    ;

  class_<VectorImp, bases<CurveImp> >( "Vector", init<Coordinate, Coordinate>() )
    .def( "stype", &VectorImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    .def( "length", &VectorImp::length )
    .def( "dir", &VectorImp::dir )
    .def( "data", &VectorImp::data )
    ;

  //TODO find how to mark default
  class_<AngleImp, bases<ObjectImp> >( "Angle", init<Coordinate, double, double, bool>() )
    .def( "stype", &AngleImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    .def( "size", &AngleImp::size )
    .def( "point", &AngleImp::point )
    .def( "startAngle", &AngleImp::startAngle )
    .def( "angle", &AngleImp::angle )
    ;

  class_<ArcImp, bases<ObjectImp> >( "Arc", init<Coordinate, double, double, double>() )
    .def( "stype", &ArcImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    .def( "startAngle", &ArcImp::startAngle )
    .def( "angle", &ArcImp::angle )
    .def( "radius", &ArcImp::radius )
    .def( "center", &ArcImp::center )
    .def( "firstEndPoint", &ArcImp::firstEndPoint )
    .def( "secondEndPoint", &ArcImp::secondEndPoint )
    .def( "sectorSurface", &ArcImp::sectorSurface )
    ;

  class_<BogusImp, bases<ObjectImp>, boost::noncopyable >( "BogusObject", no_init )
    .def( "stype", &BogusImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    ;

  class_<InvalidImp, bases<BogusImp> >( "InvalidObject", init<>() )
    .def( "stype", &InvalidImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    ;

  class_<DoubleImp, bases<BogusImp> >( "DoubleObject", init<double>() )
    .def( "stype", &DoubleImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    .def( "data", &DoubleImp::data )
    .def( "setData", &DoubleImp::setData )
    ;

  class_<IntImp, bases<BogusImp> >( "IntObject", init<int>() )
    .def( "stype", &IntImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    .def( "data", &IntImp::data )
    .def( "setData", &IntImp::setData )
    ;

  class_<StringImp, bases<BogusImp> >( "StringObject", init<char*>() )
    .def( "stype", &StringImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
//     .def( "data", &StringImp::data )
//     .def( "setData", &StringImp::setData )
    ;

  class_<TestResultImp, bases<BogusImp> >( "TestResultObject", no_init )
    .def( "stype", &TestResultImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
//     .def( "data", &TestResultImp::data )
    ;

//  class_<TextImp, bases<ObjectImp> >( "Text", init<string, Coordinate, bool>() )
//    .def( "stype", &TextImp::stype,
//          return_value_policy<reference_existing_object>() )
//    .staticmethod( "stype" )
//    .def( "text", &TextImp::text )
//    .def( "coordinate", &TextImp::coordinate )
//    .def( "hasFrame", &TextImp::hasFrame )
//    ;

  class_<NumericTextImp, bases<ObjectImp> >( "NumericObject", no_init )
    .def( "stype", &NumericTextImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    .def( "value", &NumericTextImp::getValue )
    ;

  class_<BoolTextImp, bases<ObjectImp> >( "BooleanObject", no_init )
    .def( "stype", &BoolTextImp::stype,
          return_value_policy<reference_existing_object>() )
    .staticmethod( "stype" )
    .def( "value", &BoolTextImp::getValue )
    ;

  class_<CubicCartesianData>( "CubicCartesianData", init<double,double,double,double,double,double,double,double,double,double>() )
    .def( "invalidData", &CubicCartesianData::invalidData )
    .staticmethod( "invalidData" )
    .def( "valid", &CubicCartesianData::valid )
//    .def( init<double[10]>() )
//    .def_readwrite( "coeffs", &CubicCartesianData::coeffs )
    ;

   class_<CubicImp, bases<CurveImp> >( "Cubic", init<CubicCartesianData>() )
     .def( "stype", &CubicImp::stype,
           return_value_policy<reference_existing_object>() )
     .staticmethod( "stype" )
     .def( "data", &CubicImp::data )
     ;

}

// helper class to initialize Python in a constructor;
// this way, PythonScripter::Private inherits from it and thus
// already has Python initialized before the class members
// (like 'mainnamespace') are initialized
class PythonInitializer
{
public:
  PythonInitializer();
};

PythonInitializer::PythonInitializer()
{
  // tell the python interpreter about our API..

  PyImport_AppendInittab( "kig", PyInit_kig );

  Py_Initialize();

  PyRun_SimpleString( "import math; from math import *;" );
  PyRun_SimpleString( "import kig; from kig import *;" );
  PyRun_SimpleString( "import traceback;" );
}

PythonScripter* PythonScripter::instance()
{
  static PythonScripter t;
  return &t;
}

class PythonScripter::Private : private PythonInitializer
{
public:
  dict mainnamespace;
};

PythonScripter::PythonScripter()
{
  d = new Private;

  // find the main namespace..

  handle<> main_module( borrowed( PyImport_AddModule( "__main__" ) ) );

  handle<> mnh(borrowed( PyModule_GetDict(main_module.get()) ));
  d->mainnamespace = extract<dict>( mnh.get() );
}

PythonScripter::~PythonScripter()
{
  PyErr_Clear();
  delete d;
  // Py_FinalizeEx();
  Py_Finalize();  // maintained for compatibility reasons with python2
}

class CompiledPythonScript::Private
{
public:
  int ref;
  object calcfunc;
  // TODO
//  object movefunc;
};

ObjectImp* CompiledPythonScript::calc( const Args& args, const KigDocument& )
{
  return PythonScripter::instance()->calc( *this, args );
}

CompiledPythonScript::~CompiledPythonScript()
{
  --d->ref;
  if ( d->ref == 0 )
    delete d;
}

CompiledPythonScript::CompiledPythonScript( Private* ind )
  : d( ind )
{
  ++d->ref;
}

CompiledPythonScript PythonScripter::compile( const char* code )
{
  clearErrors();
  dict retdict;
  bool error = false;
  try
  {
    (void) PyRun_String( const_cast<char*>( code ), Py_file_input,
                         d->mainnamespace.ptr(), retdict.ptr() );
  }
  catch( ... )
  {
    error = true;
  };
  error |= static_cast<bool>( PyErr_Occurred() );
  if ( error )
  {
    saveErrors();
    retdict.clear();
  }

  // debugging stuff, removed.
//  std::string dictstring = extract<std::string>( str( retdict ) );

  CompiledPythonScript::Private* ret = new CompiledPythonScript::Private;
  ret->ref = 0;
  ret->calcfunc = retdict.get( "calc" );
  return CompiledPythonScript( ret );
}

CompiledPythonScript::CompiledPythonScript( const CompiledPythonScript& s )
  : d( s.d )
{
  ++d->ref;
}

std::string PythonScripter::lastErrorExceptionType() const
{
  return lastexceptiontype;
}

std::string PythonScripter::lastErrorExceptionValue() const
{
  return lastexceptionvalue;
}

std::string PythonScripter::lastErrorExceptionTraceback() const
{
  return lastexceptiontraceback;
}

ObjectImp* PythonScripter::calc( CompiledPythonScript& script, const Args& args )
{
  clearErrors();
  object calcfunc = script.d->calcfunc;
  try
  {
    std::vector<object> objectvect;
    objectvect.reserve( args.size() );

    for ( int i = 0; i < (int) args.size(); ++i )
    {
      object o( boost::ref( *args[i] ) );
      objectvect.push_back( o );
    }

    handle<> argstuph( PyTuple_New( args.size() ) );
    for ( int i = 0; i < (int) objectvect.size(); ++i )
    {
      /*
       * this fixes bug https://bugs.kde.org/show_bug.cgi?id=401512
       *
       * it isn't completely clear whether we need XINCREF (test for null pointer)
       * instead of INCREF.  However I think that the arguments should never be
       * null pointers
       *    mp
       */
      Py_INCREF((objectvect.begin() +i)->ptr());
      PyTuple_SetItem( argstuph.get(), i, (objectvect.begin() +i)->ptr() );
    };
    tuple argstup( argstuph );

    handle<> reth( PyObject_CallObject( calcfunc.ptr(), argstup.ptr() ) );
//    object resulto = calcfunc( argstup );
//    handle<> reth( PyObject_CallObject( calcfunc.ptr(), args ) );
    object resulto( reth );

    extract<ObjectImp&> result( resulto );
    if( ! result.check() ) return new InvalidImp;
    else
    {
      ObjectImp& ret = result();
      return ret.copy();
    };
  }
  catch( ... )
  {
    saveErrors();

    return new InvalidImp;
  };
}

void PythonScripter::saveErrors()
{
  erroroccurred = true;
  PyObject* poexctype;
  PyObject* poexcvalue;
  PyObject* poexctraceback;
  PyErr_Fetch( &poexctype, &poexcvalue, &poexctraceback );
  PyErr_NormalizeException( &poexctype, &poexcvalue, &poexctraceback );
  handle<> exctypeh( poexctype );
  handle<> excvalueh( poexcvalue );

  object exctype( exctypeh );
  object excvalue( excvalueh );
  object exctraceback;
  if ( poexctraceback )
  {
    handle<> exctracebackh( poexctraceback );
    exctraceback = object( exctracebackh );
  }

  lastexceptiontype = extract<std::string>( str( exctype ) )();
  lastexceptionvalue = extract<std::string>( str( excvalue ) )();

  object printexcfunc = d->mainnamespace[ "traceback" ].attr( "format_exception" );

  list tracebacklist = extract<list>( printexcfunc( exctype, excvalue, exctraceback ) )();
  str tracebackstr( "" );
  while ( true )
  {
    try {
      str s = extract<str>( tracebacklist.pop() );
      tracebackstr += s;
    }
    catch( ... )
    {
      break;
    }
  }

  lastexceptiontraceback = extract<std::string>( tracebackstr )();
  PyErr_Clear();
}

void PythonScripter::clearErrors()
{
  PyErr_Clear();
  lastexceptiontype.clear();
  lastexceptionvalue.clear();
  lastexceptiontraceback.clear();
  erroroccurred = false;
}

bool CompiledPythonScript::valid()
{
  return !!d->calcfunc;
}

bool PythonScripter::errorOccurred() const
{
  return erroroccurred;
}

