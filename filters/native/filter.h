/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese <devriese@kde.org>

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


#ifndef KIG_FILTER_NATIVE_H
#define KIG_FILTER_NATIVE_H

#include "../filter.h"

// Kig's native format is an xml format, it uses the same interface as
// the other filters cause that seemed cool to me..

class KigFilterNative
  : public KigFilter
{
public:
  KigFilterNative();
  ~KigFilterNative();
  virtual bool supportMime ( const QString mime );
  virtual Result load ( const QString from, Objects& to );
  virtual Result save ( const Objects& os, const QString to );
};

#endif
