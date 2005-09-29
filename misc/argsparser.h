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

#ifndef KIG_MISC_ARGSPARSER_H
#define KIG_MISC_ARGSPARSER_H

#include "../objects/common.h"

#include <string>

class ObjectImpType;

/**
 * This class is meant to take care of checking the types of the
 * parents to ObjectCalcer's, and to put them in the correct order.
 * An ObjectType should construct an ArgsParser with a specification
 * of the arguments it wants.  This specification is given as an array
 * of ArgsParser::spec structs.  This struct contains a pointer to an
 * ObjectImpType ( which is the type you want the argument to have ),
 * a string ( which is an I18N_NOOP'd string describing what you will
 * be using the argument for ) and a boolean ( which says whether the
 * constructed object is by construction on the curve argument ( if
 * the constructed object is a point ), or whether the constructed
 * object is by construction through the point argument ( if the
 * constructed object is a curve ) ).
 *
 * An ObjectType using an ArgsParser to take care of the various
 * things that it can handle ( impRequirement, the sortArgs functions
 * and the isDefinedOnOrThrough stuff ), should inherit from
 * ArgsParserObjectType, which takes care of calling the ArgsParser
 * for these things...  It also allows you to use a convenient
 * ObjectConstructor for your type.
 *
 * E.g., let's see what CircleBCPType has for its arguments spec:
 * here's some code:
 * \code
 * static const ArgsParser::spec argsspecTranslation[] =
 * {
 *   { ObjectImp::stype(), I18N_NOOP("Translate this object"), false },
 *   { VectorImp::stype(), I18N_NOOP("Translate by this vector"), false }
 * };
 *
 * TranslatedType::TranslatedType()
 *   : ArgsParserObjectType( "Translation", argsspecTranslation, 2 )
 * {
 * }
 *
 * ObjectImp* TranslatedType::calc( const Args& args, const KigDocument& ) const
 * {
 *   if ( ! margsparser.checkArgs( args ) ) return new InvalidImp;
 *
 *   Coordinate dir = static_cast<const VectorImp*>( args[1] )->dir();
 *   Transformation t = Transformation::translation( dir );
 *
 *   return args[0]->transform( t );
 * }
 * \endcode
 *
 * As you can see above, the argsspec can be declared right in the
 * cpp-file.  The usetexts explain to the user what the argument in
 * question will be used for.  The boolean says that in this case, the
 * constructed object is not by construction on or through one of its
 * arguments. In the constructor, you simply call the
 * ArgsParserObjectType with the argsspec struct you defined, and the
 * number of arguments in the argsspec ( in this case 2 ).
 *
 * In the calc function, you can rely on the arguments already being
 * in the correct order ( the same order as you put them in in the
 * arguments spec.  You should use the checkArgs function to check if
 * all the arguments are valid, and if they aren't return a
 * InvalidImp.  All objects can always become invalid ( e.g. an
 * intersection point of two non-intersecting conics can become valid
 * again when the conics move ), and you should always check for this.
 *
 * An interesting to note here is that the first argument is of a more
 * general type than the second.  A VectorImp is *also* an ObjectImp.
 * In general, when this happens, you should put the more general type
 * first, as in general this produces the results that the user
 * expects.  I have no formal proof for this, just talking from
 * experience.  It might be that you experience different things, but
 * unless you're sure of the results, put the more general type first.
 *
 * This class uses a pretty basic algorithm for doing the parsing (
 * e.g. if a match fails in one order, it does not try a different
 * order, which could perhaps be necessary in the case of having more
 * general argument types in the same argument spec ).  However, the
 * current algorithm works in all the situation where I've tested it,
 * and I don't feel the need to change it.  Feel free to do so if you
 * like, but even if you do, I'm not sure if I will include it in
 * mainline Kig.
 */
class ArgsParser
{
public:
  /**
   * this are some enum values that we return from some functions.
   */
  enum { Invalid = 0, Valid = 1, Complete = 2 };
  struct spec { const ObjectImpType* type; std::string usetext; std::string selectstat; bool onOrThrough;};
private:
  /**
   * the args spec..
   */
  std::vector<spec> margs;

  spec findSpec( const ObjectImp* o, const Args& parents ) const;
public:
  ArgsParser( const struct spec* args, int n );
  ArgsParser( const std::vector<spec>& args );
  ArgsParser();
  ~ArgsParser();

  void initialize( const std::vector<spec>& args );
  void initialize( const struct spec* args, int n );

  /**
   * returns a new ArgsParser that wants the same args, except for the
   * ones of the given type..
   */
  ArgsParser without( const ObjectImpType* type ) const;
  // checks if os matches the argument list this parser should parse..
  int check( const Args& os ) const;
  int check( const std::vector<ObjectCalcer*>& os ) const;
  /**
   * returns the usetext for the argument that o would be used for,
   * if sel were used as parents..
   * \p o should be in \p sel ...
   */
  std::string usetext( const ObjectImp* o, const Args& sel ) const;

  /**
   * returns the select statement for the next selectable argument
   * when the given args are selected.
   */
  std::string selectStatement( const Args& sel ) const;

  // this reorders the objects or args so that they are in the same
  // order as the requested arguments..
  Args parse( const Args& os ) const;
  std::vector<ObjectCalcer*> parse( const std::vector<ObjectCalcer*>& os ) const;

  /**
   * returns the minimal ObjectImp ID that \p o needs to inherit in order
   * to be useful..  \p o should be part of \p parents .
   */
  const ObjectImpType* impRequirement( const ObjectImp* o, const Args& parents ) const;

  /**
   * Supposing that \p parents would be given as parents, this function
   * returns whether the returned ObjectImp will be, by construction,
   * on \p o ( if \p o is a curve ), or through \p o ( if \p o is a point ).
   */
  bool isDefinedOnOrThrough( const ObjectImp* o, const Args& parents ) const;

  // Checks the args according to this args specification.  If the
  // objects should never have occurred, then an assertion failure
  // will happen, if one of the args is invalid, then false will be
  // returned, if all is fine, then true is returned..
  // assert that the objects are of the right types, and in the right
  // order as what would be returned by parse( os )..  If minobjects
  // is provided, then not all objects are needed, and it is enough if
  // at least minobjects are available..  Use this for object types
  // that can calc a temporary example object using less than the
  // required args.  These args need to be at the end of argsspec +
  // anyobjsspec.  If minobjects is not provided, then it is assumed
  // that all args are necessary.
  bool checkArgs( const std::vector<ObjectCalcer*>& os ) const;
  bool checkArgs( const std::vector<ObjectCalcer*>& os, uint minobjects ) const;
  bool checkArgs( const Args& os ) const;
  bool checkArgs( const Args& os, uint minobjects ) const;
};

#endif
