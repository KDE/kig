// python_scripter.cc
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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

#include "python_scripter.h"

#include <Python.h>
#include <iostream>
#include <string>
#include <boost/python.hpp>
#include <boost/mpl/bool.hpp>

#include "../misc/coordinate.h"
#include "../misc/kigtransform.h"
#include "../misc/common.h"
#include "../objects/common.h"
#include "../objects/bogus_imp.h"
#include "../objects/point_imp.h"
#include "../objects/line_imp.h"
#include "../objects/circle_imp.h"

using namespace boost::python;

BOOST_PYTHON_MODULE_INIT( kig )
{
  class_<Coordinate>( "Coordinate" )
    .def( init<const double, const double>() )
    .def( init<const Coordinate&>() )
    .def( "squareLength", &Coordinate::squareLength )
    .def( "orthogonal", &Coordinate::orthogonal )
    .def( "round", &Coordinate::round )
    .def( "normalize", &Coordinate::normalize )
    .def( -self )
//    .def( self = self )
    .def( self += self )
    .def( self -= self )
    .def( self *= other<const double>() )
    .def( self *= other<const int>() )
    .def( self /= other<const double>() )
    .def( self / other<const double>() )
    .def( self + self )
    .def( self - self )
    .def( self * other<const double>() )
    .def( other<const double>() * self )
    .def_readwrite( "x", &Coordinate::x )
    .def_readwrite( "y", &Coordinate::y )
    ;

  class_<LineData>( "LineData" )
    .def( init<Coordinate, Coordinate>() )
    .def( "dir", &LineData::dir )
    .def( "length", &LineData::length )
    .def_readwrite( "a", &LineData::a )
    .def_readwrite( "b", &LineData::a )
    ;

  // we need this cause Transformation::apply is overloaded and
  // otherwise using Transformation::apply would be ambiguous..
  const Coordinate (Transformation::*transformapplyfunc)( const Coordinate&, bool& ) const = &Transformation::apply;
  class_<Transformation>( "Transformation", no_init )
    .def( "apply", transformapplyfunc )
    .def( "isHomothetic", &Transformation::isHomothetic )
    .def( "inverse", &Transformation::inverse )
    .def( "translation", &Transformation::translation )
    .def( "rotation", &Transformation::rotation )
    .def( "pointReflection", &Transformation::pointReflection )
    .def( "lineReflection", &Transformation::lineReflection )
    .def( "castShadow", &Transformation::castShadow )
    .def( "projectiveRotation", &Transformation::projectiveRotation )
    .def( self * self )
    .staticmethod( "translation" )
    .staticmethod( "rotation" )
    .staticmethod( "pointReflection" )
    .staticmethod( "lineReflection" )
    .staticmethod( "castShadow" )
    .staticmethod( "projectiveRotation" )
    ;

  class_<ObjectImp, boost::noncopyable>( "Object", no_init )
    .def( "transform", &ObjectImp::transform,
          return_value_policy<manage_new_object>() )
    .def( "valid", &ObjectImp::valid )
    .def( "copy", &ObjectImp::copy,
          return_value_policy<manage_new_object>() )
    ;

  class_<CurveImp, bases<ObjectImp>, boost::noncopyable>( "Curve", no_init )
//    .def( "getParam", &CurveImp::getParam )
//    .def( "getPoint", &CurveImp::getPoint );
    ;
  class_<PointImp, bases<ObjectImp> >( "Point", init<Coordinate>() )
    .def( "coordinate", &PointImp::coordinate,
          return_internal_reference<1>() )
    .def( "setCoordinate", &PointImp::setCoordinate )
    ;

  class_<AbstractLineImp, bases<CurveImp>, boost::noncopyable >( "AbstractLine", no_init )
    .def( "slope", &AbstractLineImp::slope )
    .def( "equationString", &AbstractLineImp::equationString )
    .def( "data", &AbstractLineImp::data )
    ;

  class_<SegmentImp, bases<AbstractLineImp> >( "Segment", init<Coordinate, Coordinate>() )
    .def( init<LineData>() )
    .def( "length", &SegmentImp::length )
    ;

  class_<RayImp, bases<AbstractLineImp> >( "Ray", init<Coordinate, Coordinate>() )
    .def( init<LineData>() )
    ;

  class_<LineImp, bases<AbstractLineImp> >( "Line", init<Coordinate, Coordinate>() )
    .def( init<LineData>() )
    ;

  class_<ConicCartesianData>( "ConicCartesianData", init<double,double,double,double,double,double>() )
    .def( init<ConicPolarData>() )
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
    .def( "center", &CircleImp::center )
    .def( "radius", &CircleImp::radius )
    .def( "squareRadius", &CircleImp::squareRadius )
    .def( "surface", &CircleImp::surface )
    .def( "circumference", &CircleImp::circumference )
    ;
};

PythonScripter* PythonScripter::instance()
{
  static PythonScripter t;
  return &t;
}

class PythonScripter::Private
{
public:
  dict mainnamespace;
};

// allocates a new string using new [], and copies contents into it..
static char* newstring( const char* contents )
{
  char* ret = new char[strlen( contents ) + 1];
  strcpy( ret, contents );
  return ret;
};

PythonScripter::PythonScripter()
{
  d = new Private;

  // tell the python interpreter about our API..

  // the newstring stuff is to prevent warnings about conversion from
  // const char* to char*..
  char* s = newstring( "kig" );
  PyImport_AppendInittab( s, initkig );
  //  we can't delete this yet, since python keeps a pointer to it..
  // This means we have a small but harmless memory leak here, but it
  // doesn't hurt at all, since it could only be freed at the end of
  // the program, at which time it is freed by the system anyway if we
  // don't do it..
  //delete [] s;

  Py_Initialize();

  s = newstring( "import kig;" );
  PyRun_SimpleString( s );
  delete [] s;

  s = newstring( "from kig import *;" );
  PyRun_SimpleString( s );
  delete [] s;

  // find the main namespace..

  s = newstring( "__main__" );
  handle<> main_module( borrowed( PyImport_AddModule( s ) ) );
  delete [] s;

  handle<> mnh(borrowed( PyModule_GetDict(main_module.get()) ));
  d->mainnamespace = extract<dict>( mnh.get() );
}

PythonScripter::~PythonScripter()
{
  Py_Finalize();
  delete d;
}

class CompiledPythonScript::Private
{
public:
  int ref;
  dict mainnamespace;
  object calcfunc;
  // TODO
//  object movefunc;
};

ObjectImp* CompiledPythonScript::calc( const Args& args, const KigDocument& )
{
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
      PyTuple_SetItem( argstuph.get(), i, (objectvect.begin() +i)->ptr() );
    };
    tuple argstup( argstuph );

    handle<> reth( PyEval_CallObject( d->calcfunc.ptr(), argstup.ptr() ) );
//    object resulto = d->calcfunc( args );
    object resulto( reth );

//    handle<> reth( PyEval_CallObject( d->calcfunc.ptr(), args ) );
    extract<ObjectImp&> result( resulto );
    if( ! result.check() ) return new InvalidImp;
    else
    {
      ObjectImp& ret = result();
      return ret.copy();
    };
  }
  catch( error_already_set e )
  {
    kdDebug() << k_funcinfo << "error_already_set" << endl;

    PyObject* poexctype;
    PyObject* poexcvalue;
    PyObject* poexctraceback;
    PyErr_Fetch( &poexctype, &poexcvalue, &poexctraceback );
    kdDebug() << poexctype << poexcvalue << poexctraceback << endl;
    handle<> exctype( poexctype );
    handle<> excvalue( poexcvalue );
//    handle<> exctraceback( poexctraceback );
    kdDebug() << k_funcinfo
              << "exctype: " << extract<std::string>( str( exctype ) )().c_str() << endl
              << "excvalue: " << extract<std::string>( str( excvalue ) )().c_str() << endl;
    return new InvalidImp;
  }
  catch( ... )
  {
    kdDebug() << k_funcinfo << "other error" << endl;
//    throw;
    return new InvalidImp;
  };
}

CompiledPythonScript::~CompiledPythonScript()
{
  --d->ref;
  if ( d->ref == 0 )
    delete d;
}

CompiledPythonScript::CompiledPythonScript( Private* ind )
{
  d = ind;
  ++d->ref;
}

CompiledPythonScript PythonScripter::compile( const char* code )
{
  dict retdict;
  try
  {
    (void) PyRun_String( const_cast<char*>( code ), Py_file_input,
                         d->mainnamespace.ptr(), retdict.ptr() );
  }
  catch( ... )
  {
    retdict.clear();
  };
  std::string dictstring = extract<std::string>( str( retdict ) );

  CompiledPythonScript::Private* ret = new CompiledPythonScript::Private;
  ret->ref = 0;
  ret->calcfunc = retdict.get( "calc" );
  ret->mainnamespace = d->mainnamespace;
  return CompiledPythonScript( ret );
}

CompiledPythonScript::CompiledPythonScript( const CompiledPythonScript& s )
  : d( s.d )
{
  ++d->ref;
}
