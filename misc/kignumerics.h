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

#ifndef KIG_MISC_KIGNUMERICS_H
#define KIG_MISC_KIGNUMERICS_H

#include <cmath>

double calcCubicRoot ( double xmin, double xmax, double a,
      double b, double c, double d, int root, bool& valid, int& numroots );

int calcCubicVariations (double x, double a, double b, double c,
      double d, double p1a, double p1b, double p0a);

double calcCubicRootwithNewton ( double ymin, double ymax, double a, 
    double b, double c, double d, double tol );

void GaussianElimination( double *matrix[], int numrows, int numcols,
    int scambio[] );

void BackwardSubstitution( double *matrix[], int numrows, int numcols,
    int scambio[], double solution[] );

#endif // KIG_MISC_KIGNUMERICN_H
