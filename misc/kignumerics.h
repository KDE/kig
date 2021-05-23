/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Maurizio Paolini <paolini@dmf.unicatt.it>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KIG_MISC_KIGNUMERICS_H
#define KIG_MISC_KIGNUMERICS_H

#include <cmath>

double calcCubicRoot ( double xmin, double xmax, double a,
      double b, double c, double d, int root, bool& valid, int& numroots );

int calcCubicVariations (double x, double a, double b, double c,
      double d, double p1a, double p1b, double p0a);

double calcCubicRootwithNewton ( double ymin, double ymax, double a,
    double b, double c, double d, double tol );

/**
 * Gaussian Elimination.  We return false if the matrix is singular,
 * and can't be usefully eliminated..
 */
bool GaussianElimination( double *matrix[], int numrows, int numcols,
                          int scambio[] );

void BackwardSubstitution( double *matrix[], int numrows, int numcols,
                           int scambio[], double solution[] );

bool Invert3by3matrix ( const double m[3][3], double inv[3][3] );

#endif // KIG_MISC_KIGNUMERICS_H
