/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <dominique.devriese@student.kuleuven.ac.be>

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


/*
NOTE:
This is completely free code, i have not looked at any Cabri source
code, and have implemented this implementation of the Cabri file
format from zero, by just looking at the input and output from a
(properly licensed) Cabri program... I have the right to do this,
because of the democratic right we all have to reverse-engineer any
lawfully owned product...
*/

#ifndef KIG_CABRI_H
#define KIG_CABRI_H

#include "../filter.h"

//  This is an import filter for the output of the commercial program
//  Cabri ("CAhier de BRouillon Interactif" or something like that),
//  which is being pushed by Texas Instruments, but only exists for
//  the Winblows(tm) platform and some TI scientific calculator...

// THIS IS PRE-ALPHA CODE

class QFile;

class KigFilterCabri
  : public KigFilter
{
public:
  KigFilterCabri();
  ~KigFilterCabri();
  virtual bool supportMime ( const QString mime );
  virtual Result load ( const QString from, Objects& to );

private:
  QCString readLine( QFile& f, bool& eof );
  struct ObjectData
  {
    Object* o;
    std::vector<int> p;
    int id;
    bool valid;
    operator bool() { return valid; };
  };

  ObjectData readObject( QFile& f );

};

#endif
