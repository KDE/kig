// newtype.h
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.


#ifndef KIG_MISC_NEWTYPE_H
#define KIG_MISC_NEWTYPE_H

class ObjectType
{
  const char[] mbasetypename;
  const char[] mfulltypename;
public:
  ObjectType( const char[] basetypename, const char[] fulltypename );
  virtual ~ObjectType();

  typedef std::map<QCString, QString> ParamMap;

  virtual Object* build( const Objects& parents,
                         const ParamMap& params = ParamMap()
    ) const = 0;

  const QCString baseName();
  const QString translatedBaseName();
  const QCString fullName();
};

template<typename Obj>
class TObjectType
  : public ObjectType
{
  TObjectType( const char[] basetypename, const char[] fulltypename )
    : ObjectType( basetypename, fulltypename );
    {
    }
  Object* build( const Objects& parents,
                 const ParamMap& params = ParamMap()
    ) const
    {
      Obj* o = new Obj( parents );
      o->setParams( params );
      return o;
    };
};

#endif
