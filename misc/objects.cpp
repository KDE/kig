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

#include "objects.h"

#include "../objects/object.h"

#include "screeninfo.h"
#include "rect.h"
#include <qrect.h>

// this used to be a bit more filled up :)

Objects Objects::with( Object* o ) const
{
  Objects os( *this );
  os.push_back( o );
  return os;
}

void Objects::calc( const KigDocument& d ) const
{
  for ( const_iterator i = begin(); i != end(); ++i )
    (*i)->calc( d );
}

void Objects::setSelected( bool sel )
{
  for ( iterator i = begin(); i != end(); ++i )
    (*i)->setSelected( sel );
}

Objects Objects::getAllChildren() const
{
  Objects ret;
  for ( const_iterator i = begin(); i != end(); ++i )
    ret |= (*i)->getAllChildren();
  return ret;
}
