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
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.

#include "object_hierarchy.h"

#include "../objects/object_holder.h"
#include "../objects/other_type.h"
#include "../objects/object_imp.h"
#include "../objects/object_imp_factory.h"
#include "../objects/object_type_factory.h"
#include "../objects/bogus_imp.h"
#include "../objects/transform_types.h"
#include "../objects/object_type.h"

#include <kglobal.h>
#include <qdom.h>

class ObjectHierarchy::Node
{
public:
  enum { ID_PushStack, ID_ApplyType, ID_FetchProp };
  virtual int id() const = 0;

  virtual ~Node();
  virtual Node* copy() const = 0;

  virtual void apply( std::vector<const ObjectImp*>& stack, int loc,
                      const KigDocument& ) const = 0;

  virtual void apply( std::vector<ObjectCalcer*>& stack, int loc ) const = 0;

  // this function is used to check whether the final objects depend
  // on the given objects.  The dependsstack contains a set of
  // booleans telling which parts of the hierarchy certainly depend on
  // the given objects.  In this function, the node should check
  // whether any of its parents have true set, and if so, set its own
  // value to true.
  virtual void checkDependsOnGiven( std::vector<bool>& dependsstack, int loc ) const = 0;
  // this function is used to check whether the given objects are all
  // used by one or more of the final objects.  The usedstack contains
  // a set of booleans telling which parts of the hierarchy are
  // certainly ancestors of the final objects.  In this function, the
  // node should set all of its parents' booleans to true.
  virtual void checkArgumentsUsed( std::vector<bool>& usedstack ) const = 0;
};

ObjectHierarchy::Node::~Node()
{
}

class PushStackNode
  : public ObjectHierarchy::Node
{
  ObjectImp* mimp;
public:
  PushStackNode( ObjectImp* imp ) : mimp( imp ) {}
  ~PushStackNode();

  const ObjectImp* imp() const { return mimp; }

  int id() const;
  Node* copy() const;
  void apply( std::vector<const ObjectImp*>& stack,
              int loc, const KigDocument& ) const;
  void apply( std::vector<ObjectCalcer*>& stack, int loc ) const;

  void checkDependsOnGiven( std::vector<bool>& dependsstack, int loc ) const;
  void checkArgumentsUsed( std::vector<bool>& usedstack ) const;
};

void PushStackNode::checkArgumentsUsed( std::vector<bool>& ) const
{
}

void PushStackNode::apply( std::vector<ObjectCalcer*>& stack, int loc ) const
{
  stack[loc] = new ObjectConstCalcer( mimp->copy() );
}

void PushStackNode::checkDependsOnGiven( std::vector<bool>&, int ) const {
  // pushstacknode depends on nothing..
  return;
}

int PushStackNode::id() const { return ID_PushStack; }

PushStackNode::~PushStackNode()
{
  delete mimp;
}

ObjectHierarchy::Node* PushStackNode::copy() const
{
  return new PushStackNode( mimp->copy() );
}

void PushStackNode::apply( std::vector<const ObjectImp*>& stack,
                           int loc, const KigDocument& ) const
{
  stack[loc] = mimp->copy();
}

class ApplyTypeNode
  : public ObjectHierarchy::Node
{
  const ObjectType* mtype;
  std::vector<int> mparents;
public:
  ApplyTypeNode( const ObjectType* type, const std::vector<int>& parents )
    : mtype( type ), mparents( parents ) {}
  ~ApplyTypeNode();
  Node* copy() const;

  const ObjectType* type() const { return mtype; }
  const std::vector<int>& parents() const { return mparents; }

  int id() const;
  void apply( std::vector<const ObjectImp*>& stack,
              int loc, const KigDocument& ) const;
  void apply( std::vector<ObjectCalcer*>& stack, int loc ) const;

  void checkDependsOnGiven( std::vector<bool>& dependsstack, int loc ) const;
  void checkArgumentsUsed( std::vector<bool>& usedstack ) const;
};

int ApplyTypeNode::id() const { return ID_ApplyType; }

void ApplyTypeNode::checkArgumentsUsed( std::vector<bool>& usedstack ) const
{
  for ( uint i = 0; i < mparents.size(); ++i )
  {
    usedstack[mparents[i]] = true;
  }
}

void ApplyTypeNode::checkDependsOnGiven( std::vector<bool>& dependsstack, int loc ) const
{
  bool result = false;
  for ( uint i = 0; i < mparents.size(); ++i )
    if ( dependsstack[mparents[i]] == true ) result = true;
  dependsstack[loc] = result;
}

ApplyTypeNode::~ApplyTypeNode()
{
}

ObjectHierarchy::Node* ApplyTypeNode::copy() const
{
  return new ApplyTypeNode( mtype, mparents );
}

void ApplyTypeNode::apply( std::vector<ObjectCalcer*>& stack, int loc ) const
{
  std::vector<ObjectCalcer*> parents;
  for ( uint i = 0; i < mparents.size(); ++i )
    parents.push_back( stack[ mparents[i] ] );
  stack[loc] = new ObjectTypeCalcer( mtype, parents );
}

void ApplyTypeNode::apply( std::vector<const ObjectImp*>& stack,
                           int loc, const KigDocument& doc ) const
{
  Args args;
  for ( uint i = 0; i < mparents.size(); ++i )
    args.push_back( stack[mparents[i]] );
  args = mtype->sortArgs( args );
  stack[loc] = mtype->calc( args, doc );
}

class FetchPropertyNode
  : public ObjectHierarchy::Node
{
  mutable int mpropid;
  int mparent;
  const QCString mname;
public:
  // propid is a cache of the location of name in the parent's
  // propertiesInternalNames(), just as it is in PropertyObject.  We
  // don't want to ever save this value, since we cannot guarantee it
  // remains consistent if we add properties some place..
  FetchPropertyNode( const int parent, const QCString& name, const int propid = -1 )
    : mpropid( propid ), mparent( parent ), mname( name ) {}
  ~FetchPropertyNode();
  Node* copy() const;

  void checkDependsOnGiven( std::vector<bool>& dependsstack, int loc ) const;
  void checkArgumentsUsed( std::vector<bool>& usedstack ) const;
  int parent() const { return mparent; }
  const QCString& propinternalname() const { return mname; }

  int id() const;
  void apply( std::vector<const ObjectImp*>& stack,
              int loc, const KigDocument& ) const;
  void apply( std::vector<ObjectCalcer*>& stack, int loc ) const;
};

FetchPropertyNode::~FetchPropertyNode()
{
}

void FetchPropertyNode::checkArgumentsUsed( std::vector<bool>& usedstack ) const
{
  usedstack[mparent] = true;
}

void FetchPropertyNode::checkDependsOnGiven( std::vector<bool>& dependsstack, int loc ) const
{
  dependsstack[loc] = dependsstack[mparent];
}

ObjectHierarchy::Node* FetchPropertyNode::copy() const
{
  return new FetchPropertyNode( mparent, mname, mpropid );
}

int FetchPropertyNode::id() const
{
  return ID_FetchProp;
}

void FetchPropertyNode::apply( std::vector<const ObjectImp*>& stack,
                               int loc, const KigDocument& d ) const
{
  assert( stack[mparent] );
  if ( mpropid == -1 ) mpropid = stack[mparent]->propertiesInternalNames().findIndex( mname );
  if ( mpropid != -1 )
    stack[loc] = stack[mparent]->property( mpropid, d );
  else
    stack[loc] = new InvalidImp();
}

void FetchPropertyNode::apply( std::vector<ObjectCalcer*>& stack, int loc ) const
{
  if ( mpropid == -1 )
    mpropid = stack[mparent]->imp()->propertiesInternalNames().findIndex( mname );
  assert( mpropid != -1 );
  stack[loc] = new ObjectPropertyCalcer( stack[mparent], mpropid );
}

std::vector<ObjectImp*> ObjectHierarchy::calc( const Args& a, const KigDocument& doc ) const
{
  assert( a.size() == mnumberofargs );
  for ( uint i = 0; i < a.size(); ++i )
    assert( a[i]->inherits( margrequirements[i] ) );

  std::vector<const ObjectImp*> stack;
  stack.resize( mnodes.size() + mnumberofargs, 0 );
  std::copy( a.begin(), a.end(), stack.begin() );
  for( uint i = 0; i < mnodes.size(); ++i )
  {
    mnodes[i]->apply( stack, mnumberofargs + i, doc );
  };
  for ( uint i = mnumberofargs; i < stack.size() - mnumberofresults; ++i )
    delete stack[i];
  if ( stack.size() < mnumberofargs + mnumberofresults )
  {
    std::vector<ObjectImp*> ret;
    ret.push_back( new InvalidImp );
    return ret;
  }
  else
  {
    std::vector<ObjectImp*> ret;
    for ( uint i = stack.size() - mnumberofresults; i < stack.size(); ++i )
      ret.push_back( const_cast<ObjectImp*>( stack[i] ) );
    return ret;
  };
}

int ObjectHierarchy::visit( const ObjectCalcer* o, std::map<const ObjectCalcer*, int>& seenmap,
                            bool needed, bool neededatend )
{
  using namespace std;

  std::map<const ObjectCalcer*, int>::iterator smi = seenmap.find( o );
  if ( smi != seenmap.end() )
  {
    if ( neededatend )
    {
      // neededatend means that this object is one of the resultant
      // objects.  Therefore, its node has to appear at the end,
      // because that's where we expect it..  We therefore copy it
      // there using CopyObjectType..
      int ret = mnumberofargs + mnodes.size();
      std::vector<int> parents;
      parents.push_back( smi->second );
      mnodes.push_back( new ApplyTypeNode( CopyObjectType::instance(), parents ) );
      return ret;
    }
    else return smi->second;
  }

  std::vector<ObjectCalcer*> p( o->parents() );
  // we check if o descends from the given objects..
  bool descendsfromgiven = false;
  std::vector<int> parents;
  parents.resize( p.size(), -1 );
  for ( uint i = 0; i < p.size(); ++i )
  {
    int v = visit( p[i], seenmap, false );
    parents[i] = v;
    descendsfromgiven |= (v != -1);
  };

  if ( ! descendsfromgiven && ! ( needed && o->imp()->isCache() ) )
  {
    if ( needed )
    {
      assert( ! o->imp()->isCache() );
      // o is an object that does not depend on the given objects, but
      // is needed by other objects, so we just have to just save its
      // current value here.
      Node* node = new PushStackNode( o->imp()->copy() );
      mnodes.push_back( node );
      int ret = mnodes.size() + mnumberofargs - 1;
      seenmap[o] = ret;
      return ret;
    }
    else
      return -1;
  };

  return storeObject( o, p, parents, seenmap );
}

ObjectHierarchy::~ObjectHierarchy()
{
  for ( uint i = 0; i < mnodes.size(); ++i ) delete mnodes[i];
}

ObjectHierarchy::ObjectHierarchy( const ObjectHierarchy& h )
  : mnumberofargs( h.mnumberofargs ), mnumberofresults( h.mnumberofresults ),
    margrequirements( h.margrequirements ), musetexts( h.musetexts ),
    mselectstatements( h.mselectstatements )
{
  mnodes.reserve( h.mnodes.size() );
  for ( uint i = 0; i < h.mnodes.size(); ++i )
    mnodes.push_back( h.mnodes[i]->copy() );
}

ObjectHierarchy ObjectHierarchy::withFixedArgs( const Args& a ) const
{
  assert( a.size() <= mnumberofargs );
  ObjectHierarchy ret( *this );

  ret.mnumberofargs -= a.size();
  ret.margrequirements.resize( ret.mnumberofargs );

  std::vector<Node*> newnodes( mnodes.size() + a.size() );
  std::vector<Node*>::iterator newnodesiter = newnodes.begin();
  for ( uint i = 0; i < a.size(); ++i )
  {
    assert( ! a[i]->isCache() );
    *newnodesiter++ = new PushStackNode( a[i]->copy() );
  };
  std::copy( ret.mnodes.begin(), ret.mnodes.end(), newnodesiter );
  ret.mnodes = newnodes;

  return ret;
}

void ObjectHierarchy::init( const std::vector<ObjectCalcer*>& from, const std::vector<ObjectCalcer*>& to )
{
  mnumberofargs = from.size();
  mnumberofresults = to.size();
  margrequirements.resize( from.size(), ObjectImp::stype() );
  musetexts.resize( margrequirements.size(), "" );
  std::map<const ObjectCalcer*, int> seenmap;
  for ( uint i = 0; i < from.size(); ++i )
    seenmap[from[i]] = i;
  for ( std::vector<ObjectCalcer*>::const_iterator i = to.begin(); i != to.end(); ++i )
  {
    std::vector<ObjectCalcer*> parents = (*i)->parents();
    for ( std::vector<ObjectCalcer*>::const_iterator j = parents.begin();
          j != parents.end(); ++j )
      visit( *j, seenmap, true );
  }
  for ( std::vector<ObjectCalcer*>::const_iterator i = to.begin(); i != to.end(); ++i )
    visit( *i, seenmap, true, true );

  mselectstatements.resize( margrequirements.size(), "" );
}

ObjectHierarchy::ObjectHierarchy( const std::vector<ObjectCalcer*>& from, const ObjectCalcer* to )
{
  std::vector<ObjectCalcer*> tov;
  tov.push_back( const_cast<ObjectCalcer*>( to ) );
  init( from, tov );
}

ObjectHierarchy::ObjectHierarchy( const std::vector<ObjectCalcer*>& from, const std::vector<ObjectCalcer*>& to )
{
  init( from, to );
}

void ObjectHierarchy::serialize( QDomElement& parent, QDomDocument& doc ) const
{
  int id = 1;
  for ( uint i = 0; i < mnumberofargs; ++i )
  {
    QDomElement e = doc.createElement( "input" );
    e.setAttribute( "id", id++ );
    e.setAttribute( "requirement", margrequirements[i]->internalName() );
    // we don't save these atm, since the user can't define them.
    // we only load them from builtin macro's.
//     QDomElement ut = doc.createElement( "UseText" );
//     ut.appendChild( doc.createTextNode( QString::fromLatin1(musetexts[i].c_str() ) ) );
//     e.appendChild( ut );
//     QDomElement ss = doc.createElement( "SelectStatement" );
//     ss.appendChild( doc.createTextNode( QString::fromLatin1(mselectstatements[i].c_str() ) ) );
//     e.appendChild( ss );
    parent.appendChild( e );
  }

  for ( uint i = 0; i < mnodes.size(); ++i )
  {
    bool result = mnodes.size() - ( id - mnumberofargs - 1 ) <= mnumberofresults;
    QDomElement e = doc.createElement( result ? "result" : "intermediate" );
    e.setAttribute( "id", id++ );

    if ( mnodes[i]->id() == Node::ID_ApplyType )
    {
      const ApplyTypeNode* node = static_cast<const ApplyTypeNode*>( mnodes[i] );
      e.setAttribute( "action", "calc" );
      e.setAttribute( "type", QString::fromLatin1( node->type()->fullName() ) );
      for ( uint i = 0; i < node->parents().size(); ++i )
      {
        int parent = node->parents()[i] + 1;
        QDomElement arge = doc.createElement( "arg" );
        arge.appendChild( doc.createTextNode( QString::number( parent ) ) );
        e.appendChild( arge );
      };
    }
    else if ( mnodes[i]->id() == Node::ID_FetchProp )
    {
      const FetchPropertyNode* node = static_cast<const FetchPropertyNode*>( mnodes[i] );
      e.setAttribute( "action", "fetch-property" );
      e.setAttribute( "property", node->propinternalname() );
      QDomElement arge = doc.createElement( "arg" );
      arge.appendChild( doc.createTextNode( QString::number( node->parent() + 1 ) ) );
      e.appendChild( arge );
    }
    else
    {
      assert( mnodes[i]->id() == ObjectHierarchy::Node::ID_PushStack );
      const PushStackNode* node = static_cast<const PushStackNode*>( mnodes[i] );
      e.setAttribute( "action", "push" );
      QString type = ObjectImpFactory::instance()->serialize( *node->imp(), e, doc );
      e.setAttribute( "type", type );
    };

    parent.appendChild( e );
  };
}

ObjectHierarchy::ObjectHierarchy()
  : mnumberofargs( 0 ), mnumberofresults( 0 )
{
}

ObjectHierarchy* ObjectHierarchy::buildSafeObjectHierarchy( const QDomElement& parent, QString& error )
{
#define KIG_GENERIC_PARSE_ERROR \
  { \
    error = i18n( "An error was encountered at line %1 in file %2." ) \
            .arg( __LINE__ ).arg( __FILE__ ); \
    return 0; \
  }

  ObjectHierarchy* obhi = new ObjectHierarchy();

  bool ok = true;
  QString tmp;
  QDomElement e = parent.firstChild().toElement();
  for (; !e.isNull(); e = e.nextSibling().toElement() )
  {
    if ( e.tagName() != "input" ) break;

    tmp = e.attribute( "id" );
    uint id = tmp.toInt( &ok );
    if ( !ok ) KIG_GENERIC_PARSE_ERROR;

    obhi->mnumberofargs = kMax( id, obhi->mnumberofargs );

    tmp = e.attribute( "requirement" );
    const ObjectImpType* req = ObjectImpType::typeFromInternalName( tmp.latin1() );
    if ( req == 0 ) req = ObjectImp::stype(); // sucks, i know..
    obhi->margrequirements.resize( obhi->mnumberofargs, ObjectImp::stype() );
    obhi->musetexts.resize( obhi->mnumberofargs, "" );
    obhi->mselectstatements.resize( obhi->mnumberofargs, "" );
    obhi->margrequirements[id - 1] = req;
    obhi->musetexts[id - 1] = req->selectStatement();
    QDomElement esub = e.firstChild().toElement();
    for ( ; !esub.isNull(); esub = esub.nextSibling().toElement() )
    {
      if ( esub.tagName() == "UseText" )
      {
        obhi->musetexts[id - 1] = esub.text().latin1();
      }
      else if ( esub.tagName() == "SelectStatement" )
      {
        obhi->mselectstatements[id - 1] = esub.text().latin1();
      }
      else
      {
        // broken file ? ignore...
      }
    }
  }
  for (; !e.isNull(); e = e.nextSibling().toElement() )
  {
    bool result = e.tagName() == "result";
    if ( result ) ++obhi->mnumberofresults;

    tmp = e.attribute( "id" );
    int id = tmp.toInt( &ok );
    if ( !ok ) KIG_GENERIC_PARSE_ERROR;

    tmp = e.attribute( "action" );
    Node* newnode = 0;
    if ( tmp == "calc" )
    {
      // ApplyTypeNode
      QCString typen = e.attribute( "type" ).latin1();
      const ObjectType* type = ObjectTypeFactory::instance()->find( typen );
      if ( ! type )
      {
        error = i18n( "This Kig file uses an object of type \"%1\", "
                      "which this Kig version does not support."
                      "Perhaps you have compiled Kig without support "
                      "for this object type,"
                      "or perhaps you are using an older Kig version." ).arg( typen );
        return 0;
      }

      std::vector<int> parents;
      for ( QDomNode p = e.firstChild(); !p.isNull(); p = p.nextSibling() )
      {
        QDomElement q = p.toElement();
        if ( q.isNull() ) KIG_GENERIC_PARSE_ERROR; // see above
        if ( q.tagName() != "arg" ) KIG_GENERIC_PARSE_ERROR;
        int pid = q.text().toInt(&ok );
        if ( !ok ) KIG_GENERIC_PARSE_ERROR;
        parents.push_back( pid - 1 );
      };
      newnode = new ApplyTypeNode( type, parents );
    }
    else if ( tmp == "fetch-property" )
    {
      // FetchPropertyNode
      QCString propname = e.attribute( "property" ).latin1();
      QDomElement arge = e.firstChild().toElement();
      int parent = arge.text().toInt( &ok );
      if ( !ok ) KIG_GENERIC_PARSE_ERROR;
      newnode = new FetchPropertyNode( parent - 1, propname );
    }
    else
    {
      // PushStackNode
      if ( e.attribute( "action" ) != "push" ) KIG_GENERIC_PARSE_ERROR;
      QString typen = e.attribute( "type" );
      if ( typen.isNull() ) KIG_GENERIC_PARSE_ERROR;
      ObjectImp* imp = ObjectImpFactory::instance()->deserialize( typen, e, error );
      if ( ( ! imp ) && !error.isEmpty() ) return 0;
      newnode = new PushStackNode( imp );
    };
    obhi->mnodes.resize( kMax( size_t(id - obhi->mnumberofargs), obhi->mnodes.size() ) );
    obhi->mnodes[id - obhi->mnumberofargs - 1] = newnode;
  };

  // if we are here, all went fine
  return obhi;
}

ArgsParser ObjectHierarchy::argParser() const
{
  std::vector<ArgsParser::spec> specs;
  for ( uint i = 0; i < margrequirements.size(); ++i )
  {
    const ObjectImpType* req = margrequirements[i];
    ArgsParser::spec spec;
    spec.type = req;
    spec.usetext = musetexts[i];
    spec.selectstat = mselectstatements[i];
    specs.push_back( spec );
  };
  return ArgsParser( specs );
}

std::vector<ObjectCalcer*> ObjectHierarchy::buildObjects( const std::vector<ObjectCalcer*>& os, const KigDocument& doc ) const
{
  assert( os.size() == mnumberofargs );
  for ( uint i = 0; i < os.size(); ++i )
    assert( os[i]->imp()->inherits( margrequirements[i] ) );

  std::vector<ObjectCalcer*> stack;
  stack.resize( mnodes.size() + mnumberofargs, 0 );
  std::copy( os.begin(), os.end(), stack.begin() );

  for( uint i = 0; i < mnodes.size(); ++i )
  {
    mnodes[i]->apply( stack, mnumberofargs + i );
    stack[mnumberofargs + i]->calc( doc );
  };

  std::vector<ObjectCalcer*> ret( stack.end() - mnumberofresults, stack.end() );

  return ret;
}

const ObjectImpType* ObjectHierarchy::idOfLastResult() const
{
  const Node* n = mnodes.back();
  if ( n->id() == Node::ID_PushStack )
    return static_cast<const PushStackNode*>( n )->imp()->type();
  else if ( n->id() == Node::ID_FetchProp )
    return ObjectImp::stype();
  else
    return static_cast<const ApplyTypeNode*>( n )->type()->resultId();
}

ObjectHierarchy ObjectHierarchy::transformFinalObject( const Transformation& t ) const
{
  assert( mnumberofresults == 1 );
  ObjectHierarchy ret( *this );
  ret.mnodes.push_back( new PushStackNode( new TransformationImp( t ) ) );

  std::vector<int> parents;
  parents.push_back( ret.mnodes.size() - 1);
  parents.push_back( ret.mnodes.size() );
  const ObjectType* type = ApplyTransformationObjectType::instance();
  ret.mnodes.push_back( new ApplyTypeNode( type, parents ) );
  return ret;
}

bool operator==( const ObjectHierarchy& lhs, const ObjectHierarchy& rhs )
{
  if ( ! ( lhs.mnumberofargs == rhs.mnumberofargs &&
           lhs.mnumberofresults == rhs.mnumberofresults &&
           lhs.margrequirements == rhs.margrequirements &&
           lhs.mnodes.size() == rhs.mnodes.size() ) )
    return false;

  // this isn't entirely correct, but it will do, because we don't
  // really want to know whether the hierarchies are different, but
  // whether rhs has changed with regard to lhs..
  for ( uint i = 0; i < lhs.mnodes.size(); ++i )
    if ( lhs.mnodes[i] != lhs.mnodes[i] )
      return false;

  return true;
}

bool ObjectHierarchy::resultDoesNotDependOnGiven() const
{
  std::vector<bool> dependsstack( mnodes.size() + mnumberofargs, false );

  for ( uint i = 0; i < mnumberofargs; ++i )
    dependsstack[i] = true;
  for ( uint i = 0; i < mnodes.size(); ++i )
    mnodes[i]->checkDependsOnGiven( dependsstack, i + mnumberofargs );
  for ( uint i = dependsstack.size() - mnumberofresults; i < dependsstack.size(); ++i )
    if ( !dependsstack[i] )
      return true;
  return false;
}

// returns the "minimum" of a and b ( in the partially ordered set of
// ObjectImpType's, using the inherits member function as comparison,
// if you for some reason like this sort of non-sense ;) ).  This
// basically means: return the type that inherits the other type,
// because if another type inherits the lowermost type, then it will
// also inherit the other..
const ObjectImpType* lowermost( const ObjectImpType* a, const ObjectImpType* b )
{
  if ( a->inherits( b ) ) return a;
  assert( b->inherits( a ) );
  return b;
}

// this function is part of the visit procedure really.  It is
// factored out, because it recurses for cache ObjectImp's.  What this
// does is, it makes sure that object o is calcable, by putting
// appropriate Node's in mnodes..  po is o->parents() and pl contains
// the location of objects that are already in mnodes and -1
// otherwise..  -1 means we have to store their ObjectImp, unless
// they're cache ObjectImp's etc.
int ObjectHierarchy::storeObject( const ObjectCalcer* o, const std::vector<ObjectCalcer*>& po, std::vector<int>& pl,
                                  std::map<const ObjectCalcer*, int>& seenmap )
{
  for ( uint i = 0; i < po.size(); ++i )
  {
    if ( pl[i] == -1 )
    {
      // we can't store cache ObjectImp's..
      if ( po[i]->imp()->isCache() )
      {
        pl[i] = visit( po[i], seenmap, true, false );
      }
      else
      {
        Node* argnode = new PushStackNode( po[i]->imp()->copy() );
        mnodes.push_back( argnode );
        int argloc = mnumberofargs + mnodes.size() - 1;
        seenmap[po[i]] = argloc;
        pl[i] = argloc;
      };
    }
    else if ( (uint) pl[i] < mnumberofargs )
    {
      ObjectCalcer* parent = o->parents()[i];
      std::vector<ObjectCalcer*> opl = o->parents();

      margrequirements[pl[i]] =
        lowermost( margrequirements[pl[i]],
                   o->impRequirement( parent, opl ) );
      musetexts[pl[i]] = margrequirements[pl[i]]->selectStatement();
    };
  };
  if ( dynamic_cast<const ObjectTypeCalcer*>( o ) )
    mnodes.push_back( new ApplyTypeNode( static_cast<const ObjectTypeCalcer*>( o )->type(), pl ) );
  else if ( dynamic_cast<const ObjectPropertyCalcer*>( o ) )
  {
    assert( pl.size() == 1 );
    int parent = pl.front();
    ObjectCalcer* op = po.front();
    assert( op );
    uint propid = static_cast<const ObjectPropertyCalcer*>( o )->propId();
    assert( propid < op->imp()->propertiesInternalNames().size() );
    mnodes.push_back( new FetchPropertyNode( parent, op->imp()->propertiesInternalNames()[propid], propid ) );
  }
  else
    assert( false );
  seenmap[o] = mnumberofargs + mnodes.size() - 1;
  return mnumberofargs + mnodes.size() - 1;
}

ObjectHierarchy::ObjectHierarchy( const ObjectCalcer* from, const ObjectCalcer* to )
{
  std::vector<ObjectCalcer*> fromv;
  fromv.push_back( const_cast<ObjectCalcer*>( from ) );
  std::vector<ObjectCalcer*> tov;
  tov.push_back( const_cast<ObjectCalcer*>( to ) );
  init( fromv, tov );
}

bool ObjectHierarchy::allGivenObjectsUsed() const
{
  std::vector<bool> usedstack( mnodes.size() + mnumberofargs, false );
  for ( uint i = mnodes.size() - mnumberofresults; i < mnodes.size(); ++i )
    usedstack[i + mnumberofargs] = true;
  for ( int i = mnodes.size() - 1; i >= 0; --i )
    if ( usedstack[i + mnumberofargs] )
      mnodes[i]->checkArgumentsUsed( usedstack );
  for ( uint i = 0; i < mnumberofargs; ++i )
    if ( ! usedstack[i] ) return false;
  return true;
}

