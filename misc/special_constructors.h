// special_constructors.h
// Copyright (C)  2003  Dominique Devriese <devriese@kde.org>

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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef KIG_MISC_SPECIAL_CONSTRUCTORS_H
#define KIG_MISC_SPECIAL_CONSTRUCTORS_H

#include "object_constructor.h"

class ConicRadicalConstructor
  : public StandardConstructorBase
{
  const ArgsParserObjectType* mtype;
  const ArgsParser mparser;
public:
  ConicRadicalConstructor();
  ~ConicRadicalConstructor();
  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
                   const KigWidget& v ) const;
  void drawprelim( KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigDocument* doc, KigGUIAction* kact );

  bool isTransform() const;
};

class LocusConstructor
  : public StandardConstructorBase
{
  ArgsParser margsparser;
public:
  LocusConstructor();
  ~LocusConstructor();
  // we override the wantArgs() function, since we need to see
  // something about the objects that an ArgsParser can't know about,
  // namely, whether the first point is a constrained point...
  const int wantArgs(
    const std::vector<ObjectCalcer*>& os, const KigDocument& d,
    const KigWidget& v
    ) const;
  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
                   const KigWidget& v ) const;

  void drawprelim( KigPainter& p, const std::vector<ObjectCalcer*>& parents, const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigDocument* doc, KigGUIAction* kact );

  bool isTransform() const;
};

class ConicConicIntersectionConstructor
  : public StandardConstructorBase
{
protected:
  ArgsParser mparser;
public:
  ConicConicIntersectionConstructor();
  ~ConicConicIntersectionConstructor();

  void drawprelim( KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                   const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d, KigWidget& w ) const;
  void plug( KigDocument* doc, KigGUIAction* kact );

  bool isTransform() const;
};

class ConicLineIntersectionConstructor
  : public MultiObjectTypeConstructor
{
public:
  ConicLineIntersectionConstructor();
  ~ConicLineIntersectionConstructor();
};

class GenericIntersectionConstructor
  : public MergeObjectConstructor
{
public:
  GenericIntersectionConstructor();
  ~GenericIntersectionConstructor();

  bool isIntersection() const;

  QString useText( const ObjectCalcer& o, const std::vector<ObjectCalcer*>& sel, const KigDocument& d,
                   const KigWidget& v ) const;
};

class MidPointOfTwoPointsConstructor
  : public StandardConstructorBase
{
  ArgsParser mparser;
public:
  MidPointOfTwoPointsConstructor();
  ~MidPointOfTwoPointsConstructor();
  void drawprelim( KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                   const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d,
                 KigWidget& w ) const;
  void plug( KigDocument* doc, KigGUIAction* kact );
  bool isTransform() const;
};

class TestConstructor
  : public StandardConstructorBase
{
  const ArgsParserObjectType* mtype;
public:
  TestConstructor( const ArgsParserObjectType* type, const char* descname,
    const char* desc, const char* iconfile );
  ~TestConstructor();
  void drawprelim( KigPainter& p, const std::vector<ObjectCalcer*>& parents,
                   const KigDocument& ) const;
  std::vector<ObjectHolder*> build( const std::vector<ObjectCalcer*>& os, KigDocument& d,
                 KigWidget& w ) const;
  const int wantArgs( const std::vector<ObjectCalcer*>& os,
                      const KigDocument& d, const KigWidget& v ) const;
  void plug( KigDocument* doc, KigGUIAction* kact );
  bool isTransform() const;
  bool isTest() const;

  BaseConstructMode* constructMode( KigDocument& doc );
};

#endif
