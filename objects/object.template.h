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


#ifndef segment_h
#define segment_h

class X
    : public Object
{
public:
  X(){};
  ~X() {};
  QCString type() const { return "point"; };
  bool contains (const Coordinate& o, bool strict) const;
  void draw (KigPainter& p, bool selected, bool showSelection);
  bool inRect (const Rect&) const;
  Rect getSpan(void) const;

    // arguments
    QString wantArg ( const Object* ) const;
    bool selectArg (Object* which);
    void unselectArg (Object* which);

    // moving
    void startMove(const Coordinate&);
    void moveTo(const Coordinate&);
    void stopMove();
    void cancelMove();

    void moved(Object* o);

protected:
}

#endif
