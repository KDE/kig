// constructible.h
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

#ifndef KIG_MISC_CONSTRUCTIBLE_H
#define KIG_MISC_CONSTRUCTIBLE_H

/**
 * This class represents a way to construct a set of objects from a
 * set of other objects.  There are some important child classes, like
 * MacroConstructor, StandardObjectConstructor etc. ( see below )
 * Actually, it is more generic than that, it provides a way to do
 * _something_ with a set of objects, but for now, i only use it to
 * construct objects.  Maybe i'll find something more interesting to
 * do with it, who knows... ;)
 */
class ObjectConstructor
{
public:
  virtual const QString descriptiveName() const = 0;
  virtual const QString description() const = 0;
  virtual const QCString iconFileName() const = 0;

  virtual const int wantArgs( const Objects& os,
                                         const KigDocument& d,
                                         const KigView& v,
                                         const KigMode& curmode
    ) const = 0;

  virtual void handleArgs( const Object& os,
                           const KigDocument& d,
                           const KigView& v,
                           const KigMode& curmode
    ) const = 0;

  virtual void handlePrelim( const Object& os,
                             const KigDocument& d,
                             const KigView& v,
                             const KigMode& curmode
    ) const = 0;
};

class StandardConstructorBase
  : public ObjectConstructor
{
  const char[] mdescname;
  const char[] mdesc;
  const char[] miconfile;
  const ArgParser margsparser;
public:
  StandardConstructorBase( const char[] descname,
                           const char[] desc,
                           const char[] iconfile,
                           const ArgsParser::spec specs[] );
  const QString descriptiveName() const;
  const QString description() const;
  const QCString iconFileName() const;

  const int wantArgs(
    const Objects& os, const KigDocument& d,
    const KigView& v, const KigMode& curmode
    ) const;

  virtual void handleArgs(
    const Object& os, const KigDocument& d,
    const KigView& v, const KigMode& curmode
    ) const;

  virtual void handlePrelim(
    const Objects& ouc, const Objects& sel,
    const KigDocument& d, const KigView& v,
    const KigMode& curmode
    ) const;

  virtual Objects build(
    const Objects& os,
    KigDocument& d,
    KigWidget& w
    ) const = 0;
};

#endif
