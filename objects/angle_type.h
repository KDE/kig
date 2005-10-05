// Copyright (C) 2003-2004  Dominique Devriese <devriese@kde.org>
// Copyright (C) 2004       Pino Toscano <toscano.pino@tiscali.it>

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

#ifndef KIG_MISC_ANGLE_TYPE_H
#define KIG_MISC_ANGLE_TYPE_H

#include "base_type.h"

class AngleType
  : public ArgsParserObjectType
{
  AngleType();
  ~AngleType();
public:
  static const AngleType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;

  QStringList specialActions() const;
  void executeAction( int i, ObjectHolder& o, ObjectTypeCalcer& c,
                      KigPart& d, KigWidget& w, NormalMode& m ) const;
};

class HalfAngleType
  : public ArgsParserObjectType
{
  HalfAngleType();
  ~HalfAngleType();
public:
  static const HalfAngleType* instance();
  ObjectImp* calc( const Args& args, const KigDocument& ) const;
  const ObjectImpType* resultId() const;
};

#endif
