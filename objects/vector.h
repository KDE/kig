/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/


#ifndef VECTOR_H
#define VECTOR_H

class Vector
: public Object
{
 public:
  Vector() : a(0), b(0){};
  ~Vector(){};
  const char* type() { return I18N_NOOP("vector");};
  void draw (KigPainter& p, bool showSelect = true);
  bool inRect (const Rect& r) const;
  Rect getSpan() const;
  const char* wantArument (const char* arg);
  bool selectArgument (Object* which)
    {
      if (a) { a = which; if (b) constructed = true;};
      else if (b) { b = which; if (a) constructed = true;}
      else cerr << "vector::selectArgument called on a vector which is ready";
    };
  void unselectArgument(Object* which)
    {
      if (a == which) { a = 0; constructed = false;}
      else if (b == which) { b = 0; constructed = false;}
      else cerr << "vector::unselectArgument called for an argument which hasn't been selected";
    };
  
 protected:
  Point a, b;
};

#endif
