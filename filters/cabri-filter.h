// This file is part of Kig, a KDE program for Interactive Geometry...
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>

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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#ifndef KIG_FILTERS_CABRI_FILTER_H
#define KIG_FILTERS_CABRI_FILTER_H

#include "filter.h"

struct CabriObject;

class QFile;

/**
 * This is an import filter for the output of the commercial program
 * Cabri ("CAhier de BRouillon Interactif" or something like that),
 * which is being pushed by Texas Instruments, but only exists for
 * the Winblows(tm) platform and some TI scientific calculator...
 *
 * \note
 * This is completely free code, i have not looked at any Cabri source
 * code, and have implemented this implementation of the Cabri file
 * format from zero, by just looking at the input and output from a
 * (properly licensed) Cabri program...
 */
class KigFilterCabri
  : public KigFilter
{
  KigFilterCabri();
  ~KigFilterCabri();
public:
  static KigFilterCabri* instance();

  bool supportMime ( const QString& mime );
  KigDocument* load ( const QString& fromfile );
private:
  bool readObject( QFile& f, CabriObject& myobj );

};

#endif
