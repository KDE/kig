// This file is part of Kig, a KDE program for Interactive Geometry...
// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_FILTERS_CABRI_FILTER_H
#define KIG_FILTERS_CABRI_FILTER_H

#include "filter.h"

class CabriReader;

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

  bool supportMime ( const QString& mime ) Q_DECL_OVERRIDE;
  KigDocument* load ( const QString& fromfile ) Q_DECL_OVERRIDE;
  
  friend class CabriReader;
};

#endif
