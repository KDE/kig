/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Maurizio Paolini <paolini@dmf.unicatt.it>

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

#ifndef KIG_MISC_KIGTRANSFORM_H
#define KIG_MISC_KIGTRANSFORM_H

#include <cmath>
#include "../objects/object.h"

enum tWantArgsResult { tComplete, tNotComplete, tNotGood };

bool getProjectiveTransformation ( int transformationsnum, 
   Object *mtransformations[], double transformation[3][3] );

bool getProjectiveTransformFromSimple ( Object *transform[], int& ipt,
		    int transformsnum,
                    double ltransformation[3][3] );

tWantArgsResult WantTransformation ( Objects::const_iterator& i,
         const Objects& os );

QString getTransformMessage ( const Objects& os, const Object *o );

bool isHomoteticTransformation ( double transformation[3][3] );

const Coordinate calcTransformedPoint ( Coordinate p, 
                  double transformation[3][3], bool& valid );

#endif // KIG_MISC_KIGTRANSFORM_H
