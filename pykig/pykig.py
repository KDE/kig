#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# SPDX-FileCopyrightText: 2005 Maurizio Paolini-Daniele Zambelli
# SPDX-FileCopyrightText: 2011 Bartosz Dziewoński
# (licensed under GPL)

version="0.2.11"

#####
# Type constants
#####
TI=type(0)
TF=type(0.)
TS=type("")
TT=type((0, 0))

#####
# Constants: Point Style, Line Style, default values...
#####
PS=("Round", "RoundEmpty", "Rectangular", "RectangularEmpty", "Cross")
LS=("SolidLine", "DashLine", "DashDotLine", "DashDotDotLine", "DotLine")
KIGTRUE="true"
KIGFALSE="false"
DEFWIDTH=-1
DEFCOLOR="#0000ff"
DEFNAME="none"
PROPERTY_INI="Property which"
OBJECT_INI="Object type"
PROPERTY_END="Property"
OBJECT_END="Object"

#
# this is a trick to allow definitions like "p=Point(0,0,HIDDEN)"
# 
HIDDEN=KIGFALSE
VISIBLE=KIGTRUE

#####
# Validating parameters
#####

def parameter(val, defval):
  if val==None: return defval
  else:         return val
  
def validshown(shown):
  if shown==KIGTRUE or shown==KIGFALSE: return shown

def validwidth(width):
  if type(width)==TI: return width

def validpointstyle(ps):
  if ps in PS: return ps

def validname(name):
  if type(name)==TS: return name

def validlinestyle(ls):
  if ls in LS: return ls

def validcolor(color):
  if type(color)==TS: return color

#####
# if as a function
#####

def rif(condition, val1, val2):
  """Return val1 if condition is True; else return val2."""
  if condition: return val1
  else:         return val2

#####
# Force some Python variables as kig variables
#####

def kig_double(val):
  tp=type(val)
  if tp==TI or tp==TF: return Double(val)
  else:                return val

def kig_int(val):
  tp=type(val)
  if tp==TI:           return Int(val)
  else:                return val

def kig_string(val):
  tp=type(val)
  if tp==TS:           return String(val)
  else:                return val

def kig_point(val):
  tp=type(val)
  if tp==TT:
    x, y = val
    return Point(x, y, internal=True)
  else:
    return val

def kig_relpoint(obj, displ):
  x, y = displ
  return RelativePoint(x, y, obj, internal=True)

#####
# base classes
#####

#####
# KigDocument class
#####

class KigDocument(object):
  """A class representing a Kig document.

    ancestor chain:
      KigDocument <- object

    class attributes:

    attributes:
      axes
      grid
      outfilename
      outfile
      callkig
      of
      viewkig
      hierarchy
      internal
      width
      pointstyle
      name
      linestyle
      shown
      color
      
    methods:
      viewappend
      hierarchyappend
      setcallkig
      setof
      str_open
      close
      noaxes
      nogrid
      hideobjects 
      showobjects 
      setwidth 
      setpointstyle 
      setname 
      setlinestyle 
      setshown 
      setcolor 
      setinternal 
  """

  def __init__(self, outfilename, callkig=True, of=False):
#    print "KigDocument.__init__()"
    self.axes = "1"
    self.grid = "1"
    self.outfilename=outfilename
    self.callkig=callkig
    self.of=of
    try:
      self.outfile = open(outfilename, 'w')
    except IOError as value:
#      print >> sys.stderr, outfilename, 'unwritable'
      print(value,  file=sys.stderr)
      sys.exit(2)
#    KigOut._kigdocument=self
    KigDOP._kd=self
    KigView._kd=self
    self.viewkig=[]
    self.hierarchy=[]
# Default values  
    self.internal=False
    self.width=DEFWIDTH
    self.pointstyle=PS[0]
    self.name=DEFNAME
    self.linestyle=LS[0]
    self.shown=VISIBLE
    self.color=DEFCOLOR


  def viewappend(self, e):          self.viewkig.append(e)
  def hierarchyappend(self, e):     self.hierarchy.append(e)
  def setcallkig(v):                self.callkig=v
  def setof(v):                     self.of=v
  
  def str_open(self):
    return """<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE KigDocument>
<KigDocument axes="%s" grid="%s" CompatibilityVersion="0.7.0" Version="0.9.1" >
 <CoordinateSystem>Euclidean</CoordinateSystem>
 <Hierarchy>
""" % (self.axes, self.grid)

  def close(self):
    try:
      self.outfile.write(self.str_open())
      self.outfile.writelines(self.hierarchy)
      self.outfile.write(" </Hierarchy>\n <View>\n")
      for f in self.viewkig:
        self.outfile.write(f.str_view())
      self.outfile.write(" </View>\n</KigDocument>\n")
      if self.outfile != sys.stdout:
        self.outfile.close()
    except IOError as value:
      print(value, file=sys.stderr)
      sys.exit(2)
    try:
      if self.callkig:
        err = os.system('kig ' + self.outfilename)
    except Exception as value:
      print(value, file=sys.stderr)
    if not self.of:
      os.system('rm ' + self.outfilename)

  def noaxes(self):            self.axes="0"
  def nogrid(self):            self.grid="0"
  def hideobjects(self):       self.shown=HIDDEN
  def showobjects(self):       self.shown=VISIBLE
  def setwidth(self, w):       self.width=w
  def setpointstyle(self, ps): self.pointstyle=ps
  def setname(self, n):        self.name=n
  def setlinestyle(self, ls):  self.linestyle=ls
  def setshown(self, s):       self.shown=s
  def setcolor(self, c):       self.color=c
  def setinternal(self, v):    self.internal=v

#####
# KigDOP class
#####

#class KigDOP(KigOut):
class KigDOP(object):
  """An class from which all elements having an id (Data, Object, Property) inherit.

    ancestor chain:
      KigDOP <- object

    class attributes:
      id-counter

    attributes:
      id
      type

    methods:
      getid
      str_hierarchy
  """
  _kd=None
  _id_counter=0

  def __init__(self, type):
    KigDOP._id_counter+=1
    self.id=KigDOP._id_counter
    self._type=type
#    self.getkigdocument().hierarchyappend(self.str_hierarchy())
    KigDOP._kd.hierarchyappend(self.str_hierarchy())

  def getid(self):          return str(self.id)
  def str_hierarchy(self):  pass

#####
# KigView class
#####

#class KigView(KigOut):
class KigView(object):
  """A class containing visualization data.

    ancestor chain:
      KigView <- object

    class attributes:
      _kd
      
    attributes:
      shown
      width
      style
      color
      name
      pointstyle

    methods:
      str_view
      show
      hide
  """
  _kd=None
  
  def __init__(self, object, shown, name, width, pointstyle, linestyle, color):
    self.object=object
    self.shown      = parameter(shown, KigView._kd.shown)
    self.width      = parameter(width, KigView._kd.width)
    self.pointstyle = parameter(pointstyle, KigView._kd.pointstyle)
    self.linestyle  = parameter(linestyle, KigView._kd.linestyle)
    self.color      = parameter(color, KigView._kd.color)
    self.name       = parameter(name, KigView._kd.name)
    KigView._kd.viewappend(self)

  def str_view(self):
    """Returns a string which can be placed inside <View> tags.

    example:
  <Draw width="-1" point-style="Round" namecalcer="none"
style="SolidLine" shown=None color="#0000ff" object="3" />
"""

    return '  <Draw width="%s" point-style="%s" namecalcer="%s"\
 style="%s" shown="%s" color="%s" object="%s" />\n' %\
           (self.width, self.pointstyle, self.name,
            self.linestyle, self.shown, self.color, self.object.getid())

#####
# Data class
#####

class Data(KigDOP):
  """An class from which all Data elements inherit.

    ancestor chain:
      Data <- KigDOP <- object

    attributes:
      val

    methods:
      str_hierarchy
"""
  def __init__(self, type, val):
    self.val=val
    KigDOP.__init__(self, type)

  def str_hierarchy(self):
    """Returns a string which can be placed inside <Hierarchy> tags.

    example:
  <Data type="double" id="170" >0.1</Data>
"""
    return '  <Data type="%s" id="%s" >%s</Data>\n' % \
          (self._type, self.getid(), self.val)

#####
# PropObj class
#####

class PropObj(KigDOP):
  """A class from which all visible elements inherit.

    ancestor chain:
      PropObj <- KigDOP <- object

    class attributes:

    attributes:
      prop
      objvec
      view

    methods:
      str_hierarchy
      showname(self, n)
      show(self)
      hide(self)
      setwidth(self, width)
      setcolor(self, color)
      setlinestyle(self, linestyle)
      setpointstyle(self, pointstyle)
      setname(self, n)
      setshown(self, s)
      getwidth(self)
      getcolor(self)
      getlinestyle(self)
      getpointstyle(self)
  """
  
  def __init__(self, prop, type, objvec, shown, name, internal,
               width, pointstyle, linestyle, color):
    self.prop=prop
    self.objvec=objvec
    self.n_lb=None
    KigDOP.__init__(self, type)
    internal=parameter(internal, KigDOP._kd.internal)
    if internal:
      self.view = None
    else:
# Here we assume that, if we're given a name of an object,
# we want to visualize the name as well
      if name: n_id=self.showname(name, shown, width, pointstyle, linestyle,
                                  color)
      else:    n_id=None
      self.view = KigView(self, shown, n_id, width, pointstyle, linestyle,
                          color)

  def str_hierarchy(self):
    """Returns a string which can be placed inside <Hierarchy> tags.

    example:
  <Property which="mid-point" id="170" >
   <Parent id="..." />
  </Property>

    example:
  <Object type="ConstrainedPoint" id="14" >
   <Parent id="13" />
   <Parent id="10" />
  </Object>
"""
    retstring = '  <%s="%s" id="%s" >' %\
                ((self.prop and PROPERTY_INI or OBJECT_INI),
                 self._type, self.getid())
    for p in self.objvec:
      retstring = retstring + '\n   <Parent id="%s" />' % p.getid()
    retstring = retstring + '\n  </%s>\n' % (self.prop and PROPERTY_END or
                                                         OBJECT_END)
    return retstring

  def showname(self, name, shown, width, pointstyle, linestyle, color):
    n=String(name)
    self.n_lb=Label(self, (0, 0), n, 0, shown, None, False,
                    width, pointstyle, linestyle, color)
    return n.getid()
    
  def show(self):
    if self.view:                  self.view.shown=None
  def hide(self):
    if self.view:                  self.view.shown=KIGFALSE
  def setwidth(self, width):       self.view.width=width
  def setcolor(self, color):       self.view.color=color
  def setlinestyle(self, linestyle):
    if linestyle in LS:            self.view.linestyle=linestyle
  def setpointstyle(self, pointstyle):
    if pointstyle in PS:           self.view.pointstyle=pointstyle
  def type(self):                  return Type(self)
  def setname(self, n):
    v=self.view
    v.name=self.showname(n, v.shown, v.width, v.pointstyle, v.linestyle,
                         v.color)
  def setshown(self, s):           self.view.shown=s

#####
# Property class
#####

class Property(PropObj):
  """A class from which all Property elements inherit.

    ancestor chain:
      Property <- PropObj <- KigDOP <- object
  """
  def __init__(self, type, parent, shown, name, internal,
               width, pointstyle, linestyle, color):
    PropObj.__init__(self, True, type, (parent,), shown, name, internal,
               width, pointstyle, linestyle, color)
#    print shown

#####
# Object class
#####

class Object(PropObj):
  """A class from which all Object elements inherit.

    ancestor chain:
      Object <- PropObj <- KigDOP <- object
  """

  def __init__(self, type, objvec, shown, name, internal,
               width, pointstyle, linestyle, color):
    PropObj.__init__(self, False, type, objvec, shown, name, internal,
               width, pointstyle, linestyle, color)
 
#####
# Data
#####

data=(\
("Int",    "int",    "val"),
("Double", "double", "val"),
("String", "string", "xml.sax.saxutils.escape(val)"),
)

def databuild(nomeclasse, nomekig, v="val"):
  """Create string with a Data class definition."""
  return """class %s(Data):

  def __init__(self, val):
    Data.__init__(self, "%s", %s)
""" % (nomeclasse, nomekig, v)

for d in data:
  p1, p2, p3 = d
  exec (databuild(p1, p2, p3))

#####
# Objects
#####
"""To add:
("ConvexHull", "ConvexHull", "polygon,", "(polygon,),"),
("EllipseByFocusFocusPoint", "EllipseBFFP", "p1, p2, p3,", "(p1, p2, p3,),"),
("HyperbolaByFocusFocusPoint", "HyperbolaBFFP", "p1, p2, p3,", "(p1, p2, p3,),"),
("ConicBy5Points", "ConicB5P", "p1, p2, p3, p4, p5,", "(p1, p2, p3, p4, p5),"),
("ParabolaBy3Points", "ParabolaBTP", "p1, p2, p3,", "(p1, p2, p3,),"),
("CocCurve", "CocCurve", "line, point,", "(line, point,),"),
"""
objects=(\
###### Points class
("Point",           "FixedPoint", "x, y,", "(kig_double(x), kig_double(y)),"),
("ConstrainedPoint", "ConstrainedPoint",
                    "t, curve,", "(kig_double(t), curve),"),
("RelativePoint",   "RelativePoint",
                    "x, y, p,", "(kig_double(x), kig_double(y), p),"),
###### Segments, rays, lines
("Line",            "LineAB", "p1, p2,", "(p1, p2),"),
("Segment",         "SegmentAB", "p1, p2,", "(p1, p2),"),
("Ray",             "RayAB", "p1, p2,", "(p1, p2),"),
("Orthogonal",      "LinePerpend", "line, point,", "(line, point,),"),
("Parallel",        "LineParallel", "line, point,", "(line, point,),"),
###### Circles, arcs, ...
("Circle",          "CircleBCP", "center, point,", "(center, point,),"),
("CircleByCenterPoint",          "CircleBCP", "center, point,", "(center, point,),"),
("CircleByCenterRadius", "CircleBPR", "center, radius,", "(center, radius,),"),
("CircleBy3Points",  "CircleBTP", "p1, p2, p3,", "(p1, p2, p3,),"),
("ArcBy3Points",    "ArcBTP", "p1, p2, p3,", "(p1, p2, p3,),"),
("ArcByCenterPointAngle", "ArcBCPA",
                    "center, point, angle,", "(center, point, angle),"),
###### Conics...
("ParabolaByDirectrixFocus", "ParabolaBDP", "line, point,", "(line, point,),"),
("VerticalCubic", "VerticalCubicB4P", "p1, p2, p3, p4,", "(p1, p2, p3, p4,),"),
("ConicArc", "ConicArcBTPC", "p1, p2, p3, center,", "(p1, p2, p3, center),"),
("ConicBy5Points", "ConicB5P", "p1, p2, p3, p4, p5,", "(p1, p2, p3, p4, p5,),"),
("EllipseByFocusFocusPoint", "EllipseBFFP", "p1, p2, p3,", "(p1, p2, p3,),"),
("ParabolaBy3Points", "ParabolaBTP", "p1, p2, p3,", "(p1, p2, p3,),"),
("HyperbolaByFocusFocusPoint", "HyperbolaBFFP", "p1, p2, p3,", "(p1, p2, p3,),"),
#### Cubics
("CubicBy9Points", "CubicB9P", "p1, p2, p3, p4, p5, p6, p7, p8, p9,","(p1, p2, p3, p4, p5, p6, p7, p8, p9,)," ),
#####
# Intersections.  The only standard object is the intersection
# of two lines, which always gives one single point.
#####
("LineLineIntersection", "LineLineIntersection", "l1, l2,", "(l1, l2),"),
#####
# CircleCircleIntersection and ConicLineIntersection classes
# The "which" argument takes values of 1 or -1, to indicate
# which of the intersections you want to get.
#####
("CircleCircleIntersection", "CircleCircleIntersection",
                    "c1, c2, which,", "(c1, c2, Int(which),),"),
("ConicLineIntersection", "ConicLineIntersection",
                    "conic, line, which,", "(conic, line, Int(which),),"),
("ConicLineOtherIntersection", "ConicLineOtherIntersection",
           	    "conic, line, p,", "(conic, line, p),"),
("LineCubicIntersection", "CubicLineIntersection",
                    "cubic, line, which,", "(cubic, line, Int(which),),"),
("CubicLineIntersection", "CubicLineIntersection",
                    "cubic, line, which,", "(cubic, line, Int(which),),"),
("CubicLineOtherIntersection", "CubicLineOtherIntersection", "cubic, line, p1, p2,", "(cubic, line, p1, p2),"),
###### Triangle class
("Triangle",        "TriangleB3P", "p1, p2, p3,", "(p1, p2, p3),"),
###### Polygon class   (the only argument is a points vect)
("Polygon",         "PolygonBNP", "pvec,", "pvec,"),
###### PolygonBCV class
# Regular polygon data - center and a vertex. The third argument
# is an integer signifying the number of sides.
("PolygonBCV",      "PoligonBCV",
                    "center, vertex, n,", "(center, vertex, Int(n)),"),
##### PolygonVertex class    (polygon, integer >= 0)
("PolygonVertex",   "PolygonVertex",
                    "polygon, i,", "(polygon, Int(i)),"),
##### PolygonSide class    (polygon, integer >= 0)
("PolygonSide",     "PolygonSide",
                    "polygon, i,", "(polygon, Int(i)),"),
###### Vector, angle,...
("Vector",          "Vector", "p1, p2,", "(p1, p2),"),
("Angle",           "Angle", "p1, v, p2,", "(p1, v, p2),"),
###### Transformations
("Translate",       "Translation", "obj, vector,", "(obj, vector),"),
("CentralSymmetry", "PointReflection", "obj, center,", "(obj, center),"),
("AxialSymmetry",   "LineReflection", "obj, center,", "(obj, center),"),
("Rotate",          "Rotation",
                    "obj, center, angle,", "(obj, center, angle),"),
("Scale",           "ScalingOverCenter",
                    "obj, center, segment,", "(obj, center, segment),"),
("Scale2",          "ScalingOverCenter2",
                    "obj, center, s1, s2,", "(obj, center, s1, s2),"),
("Stretch",         "ScalingOverLine",
                    "obj, line, segment,", "(obj, line, segment),"),
("Stretch2",        "ScalingOverLine2",
                    "obj, line, s1, s2,", "(obj, line, s1, s2),"),
("InvertPoint",     "InvertPoint",
                    "point, circle,", "(point, circle),"),
("CircularInversion", "CircularInversion",
                    "objecttoinvert, circle,", "(objecttoinvert, circle),"),
("InvertLine",      "InvertLine",
                    "line, circle,", "(line, circle),"),
("InvertCircle",    "InvertCircle",
                    "circletoinvert, circle,", "(circletoinvert, circle),"),
("InvertArc",       "InvertArc",
                    "arctoinvert, circle,", "(arctoinvert, circle),"),
("InvertSegment",   "InvertSegment",
                    "segment, circle,", "(segment, circle),"),
###### Text, Label, ...
("Text",            "Label",
                    "point, string, boxed=0,",
                    "(Int(boxed), kig_point(point), kig_string(string)),"),
("Label",           "Label",
                    "obj, displ, string, boxed=0,",
                    "(Int(boxed),kig_relpoint(obj, displ),kig_string(string)),"),
("VarText",         "Label",
                    "point, string, vars, boxed=0,",
                    "(Int(boxed), kig_point(point), \
                    kig_string(string))+tuple(vars),"),
("VarLabel",        "Label",
                    "obj, displ, string, vars, boxed=0,",
                    "(Int(boxed), kig_relpoint(obj, displ), \
                    kig_string(string))+tuple(vars),"),
###### Python scripting... we need some work here...
("PythonScript",    "PythonExecuteType",
                    "script, argvec,",
                    '(Object("PythonCompileType", (kig_string(script),), shown,\
                             name, internal, width, pointstyle, linestyle,\
                             color),)+tuple(argvec),'),
)

def objectbuild(nameclass, namekig, params, objparams):
  """Create string with a Object class definition."""
  return """class %s(Object):

  def __init__(self, %s shown=None, name=None, internal=None,
               width=None, pointstyle=None, linestyle=None, color=None):
    Object.__init__(self, "%s", %s shown, name, internal,
                    width, pointstyle, linestyle, color)
""" % (nameclass, params, namekig, objparams)

for o in objects:
  p1, p2, p3, p4 = o
  exec (objectbuild(p1, p2, p3, p4))

#####
# Properties
#####

property=(\
("Type",            "base-object-type", "o,", "o,"),
("Coordinate",      "coordinate", "point,", "point,"),
("XCoord",          "coordinate-x", "point,", "point,"),
("YCoord",          "coordinate-y", "point,", "point,"),
("MidPoints",       "mid-point", "a, b,", "Segment(a, b, internal=True),"),
("MidPoint",        "mid-point", "segment,", "segment,"),
("MidPointAB",      "mid-point", "a, b,", "Segment(a, b, internal=True),"),
("MidPointSegment", "mid-point", "segment,", "segment,"),
("EndPointA",       "end-point-A", "segment,", "segment,"),
("EndPointB",       "end-point-B", "segment,", "segment,"),
("Length",          "length", "segment,", "segment,"),
("Equation",        "equation", "segment,", "segment,"),
("Slope",           "slope", "segment,", "segment,"),
("NumOfSides",      "polygon-number-of-sides", "poly,", "poly,"),
("Perimeter",       "polygon-perimeter", "poly,", "poly,"),
("Surface",         "polygon-surface", "poly,", "poly,"),
("CenterOfMass",    "polygon-center-of-mass", "poly,", "poly,"),
("WindingNumber",   "polygon-winding-number", "poly,", "poly,"),
("Radius",          "radius", "circle,", "circle,"),
("Center",          "center", "circle,", "circle,"),
("Bisector",        "angle-bisector", "angle,", "angle,"),
("Support",         "support", "object,", "object,"),
)

def propertybuild(nameclass, namekig, params, objparams):
  """Create string with a Property class definition."""
  return """class %s(Property):

  def __init__(self, %s shown=None, name=None, internal=False,
               width=None, pointstyle=None, linestyle=None, color=None):
    Property.__init__(self, "%s", %s shown, name, internal,
                    width, pointstyle, linestyle, color)
""" % (nameclass, params, namekig, objparams)

for p in property:
  p1, p2, p3, p4 = p
  exec (propertybuild(p1, p2, p3, p4))

#####
# Start of properties definitions as Object's methods
#####
# to be cleaned up!
points  =(Point, ConstrainedPoint, RelativePoint, PolygonVertex)
lines=(Segment, Ray, Vector, InvertLine)
segments=(Segment, Vector, PolygonSide, InvertSegment)
circles =(CircleByCenterPoint, CircleBy3Points, CircularInversion, ArcBy3Points,
          ArcByCenterPointAngle, InvertCircle)
polygons=(Polygon, PolygonBCV, Triangle)
angles  =(Angle,)
supp    = circles+lines

methods=(\
("coordinate", "coordinate", points),
("coordinate-x", "xcoord", points),
("coordinate-y", "ycoord", points),
("mid-point", "midpoint", segments),
("end-point-A", "endpointA", segments),
("end-point-B", "endpointB", segments),
("length", "length", segments),
("equation", "equation", lines),
("slope", "slope", lines),
("polygon-number-of-sides", "numofsides", polygons),
("polygon-perimeter", "perimeter", polygons),
("polygon-surface", "surface", polygons),
("polygon-center-of-mass", "centerofmass", polygons),
("polygon-winding-number", "windingnumber", polygons),
("center", "center", polygons),
("center", "center", circles),
("angle-bisector", "bisector", angles),
("support", "support",  supp),
)

def methodsbuild(namekig):
  """Create string with a method class definition."""
  return """def method(self,shown=None, name=None, internal=False,
               width=None, pointstyle=None, linestyle=None, color=None):
    return Property("%s", self, shown, name, internal,
                    width, pointstyle, linestyle, color)
""" % (namekig, )

for p in methods:
  p1, p2, cl = p
  exec (methodsbuild(p1))
  for c in cl:
    setattr(c, p2, method)

#####
# Usage
#####

def usage(codexit):
  print ("""
usage: pykig.py [options...] file ...

Options:
    -h, --help          Show this text.
    -o <kig_file>
    --output <kig_file> Output to <kig_file> and don't call Kig.
    -v, --version       Output version and exit.
    -n, --nokig         Don't call Kig.

examples:
    $ pykig.py my_file.kpy
    $ pykig.py -o output_file.kig my_file.kpy
    $ ...
""", file=sys.stderr)
  sys.exit(codexit)

#####
# Main body
#####

import sys, traceback, os
#from math import *    # for user's programs
import math            # for user's programs
import getopt
import atexit
import xml.sax.saxutils

def prog():
  try:
    _opts, _args = getopt.getopt(sys.argv[1:], "hvno:",\
                                 ["help", "version", "nokig", "output="])
  except getopt.GetoptError:
    print ("GetoptError")
    usage(2)
  _callKig=True
  _of=False
  for _opt, _arg in _opts:
    if _opt in ("-h", "--help"):
      usage(0)
    if _opt in ("-v", "--version"):
      print ("Version:", version)
      sys.exit(0)
    if _opt in ("-n", "--nokig"):
      _callKig=False
    elif _opt in ("-o", "--output"):
      _outfilename=_arg
      _of=True
      _callKig=False   # if there's an output file, don't call Kig
  if len(_args)==0:
    _infilename=input("Input file name: ")
    if not _infilename:
      print ("No input file name.")
      usage(2)
  elif len(_args)==1:
    _infilename=_args[0]
  else:
    print ("No input file name.")
    usage(2)
  try:
    _infile = open(_infilename, 'r')
  except:
    print (_infilename, "input file can't be read.", file=sys.stderr)
    sys.exit(2)
  if _of:
    if _outfilename=="-":
      _n, _e = os.path.splitext(_infilename)
      _outfilename=_n+'.kig'
  else:
    _outfilename="/tmp/pykig" + str(os.getpid()) + ".kig"
  global kigdocument
  kigdocument=KigDocument(_outfilename, _callKig, _of)
  kd=kigdocument
  try:
    exec(open(_infilename).read(), globals())
  except:
    print ('Syntax error in', _infilename, file=sys.stderr)
    _info = sys.exc_info()    # print out the traceback
    traceback.print_exc()
    sys.exit(3)
  kigdocument.close()

  if _infile != sys.stdin:
    _infile.close()

def lib():
  _outfilename="/tmp/pykig" + str(os.getpid()) + ".kig"
  global kigdocument
  kigdocument=KigDocument(_outfilename)
  kd=kigdocument
  atexit.register(kigdocument.close)

if __name__ == "__main__":
  prog()
else:
  lib()
