// object_hierarchy.cc
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

#include "object_hierarchy.h"

#include "../objects/object.h"
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

  virtual void apply( Objects& stack, int loc ) const = 0;
};

ObjectHierarchy::Node::~Node()
{
};

class PushStackNode
  : public ObjectHierarchy::Node
{
  ObjectImp* mimp;
public:
  PushStackNode( ObjectImp* imp ) : mimp( imp ) {};
  ~PushStackNode();

  const ObjectImp* imp() const { return mimp; };

  int id() const;
  Node* copy() const;
  void apply( std::vector<const ObjectImp*>& stack,
              int loc, const KigDocument& ) const;
  void apply( Objects& stack, int loc ) const;
};

void PushStackNode::apply( Objects& stack, int loc ) const
{
  stack[loc] = new DataObject( mimp->copy() );
};

int PushStackNode::id() const { return ID_PushStack; };

PushStackNode::~PushStackNode()
{
  delete mimp;
};

ObjectHierarchy::Node* PushStackNode::copy() const
{
  return new PushStackNode( mimp->copy() );
};

void PushStackNode::apply( std::vector<const ObjectImp*>& stack,
                           int loc, const KigDocument& ) const
{
  stack[loc] = mimp->copy();
};

class ApplyTypeNode
  : public ObjectHierarchy::Node
{
  const ObjectType* mtype;
  std::vector<int> mparents;
public:
  ApplyTypeNode( const ObjectType* type, const std::vector<int>& parents )
    : mtype( type ), mparents( parents ) {};
  ~ApplyTypeNode();
  Node* copy() const;

  const ObjectType* type() const { return mtype; };
  const std::vector<int>& parents() const { return mparents; };

  int id() const;
  void apply( std::vector<const ObjectImp*>& stack,
              int loc, const KigDocument& ) const;
  void apply( Objects& stack, int loc ) const;
};

int ApplyTypeNode::id() const { return ID_ApplyType; };

ApplyTypeNode::~ApplyTypeNode()
{
}

ObjectHierarchy::Node* ApplyTypeNode::copy() const
{
  return new ApplyTypeNode( mtype, mparents );
};

void ApplyTypeNode::apply( Objects& stack, int loc ) const
{
  Objects parents;
  for ( uint i = 0; i < mparents.size(); ++i )
    parents.push_back( stack[ mparents[i] ] );
  stack[loc] = new RealObject( mtype, parents );
};

void ApplyTypeNode::apply( std::vector<const ObjectImp*>& stack,
                           int loc, const KigDocument& doc ) const
{
  Args args;
  for ( uint i = 0; i < mparents.size(); ++i )
  {
    args.push_back( stack[mparents[i]] );
  };
  stack[loc] = mtype->calc( args, doc );
};

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
    : mpropid( propid ), mparent( parent ), mname( name ) {};
  ~FetchPropertyNode();
  Node* copy() const;

  int parent() const { return mparent; };
  const QCString& propinternalname() const { return mname; };

  int id() const;
  void apply( std::vector<const ObjectImp*>& stack,
              int loc, const KigDocument& ) const;
  void apply( Objects& stack, int loc ) const;
};

FetchPropertyNode::~FetchPropertyNode()
{
};

ObjectHierarchy::Node* FetchPropertyNode::copy() const
{
  return new FetchPropertyNode( mparent, mname, mpropid );
};

int FetchPropertyNode::id() const
{
  return ID_FetchProp;
};

void FetchPropertyNode::apply( std::vector<const ObjectImp*>& stack,
                               int loc, const KigDocument& d ) const
{
  assert( stack[mparent] );
  if ( mpropid == -1 ) mpropid = stack[mparent]->propertiesInternalNames().findIndex( mname );
  assert( mpropid != -1 );
  stack[loc] = stack[mparent]->property( mpropid, d );
};

void FetchPropertyNode::apply( Objects& stack, int loc ) const
{
  if ( mpropid == -1 ) mpropid = stack[mparent]->propertiesInternalNames().findIndex( mname );
  assert( mpropid != -1 );
  stack[loc] = new PropertyObject( stack[mparent], mpropid );
};

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

int ObjectHierarchy::visit( const Object* o, std::map<const Object*, int>& seenmap,
                            bool isresult )
{
  using namespace std;

  std::map<const Object*, int>::iterator smi = seenmap.find( o );
  if ( smi != seenmap.end() )
  {
    if ( isresult )
    {
      // isresult means that this object is one of the resultant
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

  Objects p( o->parents() );
  // we check if o descends from the given objects..
  bool descendsfromgiven = false;
  std::vector<int> parents;
  parents.resize( p.size(), -1 );
  for ( uint i = 0; i < p.size(); ++i )
  {
    int v = visit( p[i], seenmap );
    parents[i] = v;
    descendsfromgiven |= (v != -1);
  };

  if ( ! descendsfromgiven )
  {
    if ( isresult )
    {
      // o is a result object that does not descend from the given
      // objects..  We have to just save its value here, I guess..
      mnodes.push_back( new PushStackNode( o->imp()->copy() ) );
      return mnodes.size() + mnumberofargs - 1;
    }
    else
      return -1;
  };

  for ( uint i = 0; i < p.size(); ++i )
  {
    if ( parents[i] == -1 )
    {
      mnodes.push_back( new PushStackNode( p[i]->imp()->copy() ) );
      parents[i] = mnumberofargs + mnodes.size() - 1;
    }
    else if ( (uint) parents[i] < mnumberofargs )
    {
      Object* parent = o->parents()[i];
      Objects oparents = o->parents();

      margrequirements[parents[i]] = kMax( margrequirements[parents[i]],
                                           o->impRequirement( parent, oparents ) );
    };
  };
  if ( o->inherits( Object::ID_RealObject ) )
    mnodes.push_back( new ApplyTypeNode( static_cast<const RealObject*>( o )->type(), parents ) );
  else if ( o->inherits( Object::ID_PropertyObject ) )
  {
    assert( parents.size() == 1 );
    int parent = parents.front();
    Object* op = p.front();
    assert( op );
    uint propid = static_cast<const PropertyObject*>( o )->propId();
    assert( propid < op->propertiesInternalNames().size() );
    mnodes.push_back( new FetchPropertyNode( parent, op->propertiesInternalNames()[propid], propid ) );
  }
  else
    assert( false );
  seenmap[o] = mnumberofargs + mnodes.size() - 1;
  return mnumberofargs + mnodes.size() - 1;
}

ObjectHierarchy::~ObjectHierarchy()
{
  for ( uint i = 0; i < mnodes.size(); ++i ) delete mnodes[i];
}

ObjectHierarchy::ObjectHierarchy( const ObjectHierarchy& h )
  : mnumberofargs( h.mnumberofargs ), mnumberofresults( h.mnumberofresults ),
    margrequirements( h.margrequirements )
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
    *newnodesiter++ = new PushStackNode( a[i]->copy() );
  std::copy( ret.mnodes.begin(), ret.mnodes.end(), newnodesiter );
  ret.mnodes = newnodes;

  return ret;
}

ObjectHierarchy::ObjectHierarchy( const Objects& from, const Object* to )
  : mnumberofargs( from.size() ), mnumberofresults( 1 )
{
  margrequirements.resize( from.size(), ObjectImp::ID_AnyImp );
  std::map<const Object*, int> seenmap;
  for ( uint i = 0; i < from.size(); ++i )
    seenmap[from[i]] = i;
  Objects parents = to->parents();
  for ( Objects::const_iterator i = parents.begin();
        i != parents.end(); ++i )
    visit( *i, seenmap );
  visit( to, seenmap, true );
}

ObjectHierarchy::ObjectHierarchy( const Objects& from, const Objects& to )
  : mnumberofargs( from.size() ), mnumberofresults( to.size() )
{
  margrequirements.resize( from.size(), ObjectImp::ID_AnyImp );
  std::map<const Object*, int> seenmap;
  for ( uint i = 0; i < from.size(); ++i )
    seenmap[from[i]] = i;
  for ( Objects::const_iterator i = to.begin(); i != to.end(); ++i )
  {
    Objects parents = (*i)->parents();
    for ( Objects::const_iterator j = parents.begin();
          j != parents.end(); ++j )
      visit( *j, seenmap );
  }
  for ( Objects::const_iterator i = to.begin(); i != to.end(); ++i )
    visit( *i, seenmap, true );
}

void ObjectHierarchy::serialize( QDomElement& parent, QDomDocument& doc ) const
{
  int id = 1;
  for ( uint i = 0; i < mnumberofargs; ++i )
  {
    QDomElement e = doc.createElement( "input" );
    e.setAttribute( "id", id++ );
    e.setAttribute( "requirement", ObjectImp::idToString( margrequirements[i] ) );
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

ObjectHierarchy::ObjectHierarchy( const QDomElement& parent )
  : mnumberofargs( 0 ), mnumberofresults( 0 )
{
  bool ok = true;
  QString tmp;
  QDomElement e = parent.firstChild().toElement();
  for (; !e.isNull(); e = e.nextSibling().toElement() )
  {
    if ( e.tagName() != "input" ) break;

    tmp = e.attribute( "id" );
    uint id = tmp.toInt( &ok );
    assert( ok );

    mnumberofargs = kMax( id, mnumberofargs );

    tmp = e.attribute( "requirement" );
    int req = ObjectImp::stringToID( tmp.latin1() );
    if ( req == -1 ) req = ObjectImp::ID_AnyImp; // sucks, i know..
    margrequirements.resize( mnumberofargs, ObjectImp::ID_AnyImp );
    margrequirements[id - 1] = req;
  }
  for (; !e.isNull(); e = e.nextSibling().toElement() )
  {
    bool result = e.tagName() == "result";
    if ( result ) ++mnumberofresults;

    tmp = e.attribute( "id" );
    int id = tmp.toInt( &ok );
    assert( ok );

    tmp = e.attribute( "action" );
    Node* newnode = 0;
    if ( tmp == "calc" )
    {
      // ApplyTypeNode
      QCString typen = e.attribute( "type" ).latin1();
      const ObjectType* type = ObjectTypeFactory::instance()->find( typen );
      assert( type );
      std::vector<int> parents;
      for ( QDomNode p = e.firstChild(); !p.isNull(); p = p.nextSibling() )
      {
        QDomElement q = p.toElement();
        assert( !q.isNull() ); // see above
        assert ( q.tagName() == "arg" );
        int pid = q.text().toInt(&ok );
        assert( ok );
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
      assert( ok );
      newnode = new FetchPropertyNode( parent - 1, propname );
    }
    else
    {
      // PushStackNode
      assert( e.attribute( "action" ) == "push" );
      QString typen = e.attribute( "type" );
      assert ( !typen.isNull() );
      ObjectImp* imp = ObjectImpFactory::instance()->deserialize( typen, e );
      newnode = new PushStackNode( imp );
    };
    mnodes.resize( kMax( id - mnumberofargs, mnodes.size() ) );
    mnodes[id - mnumberofargs - 1] = newnode;
  };
}

ArgParser ObjectHierarchy::argParser() const
{
  std::vector<ArgParser::spec> specs;
  for ( uint i = 0; i < margrequirements.size(); ++i )
  {
    int req = margrequirements[i];
    ArgParser::spec spec;
    spec.type = req;
    spec.usetext = ObjectImp::selectStatement( req );
    assert( spec.usetext );
    specs.push_back( spec );
  };
  return ArgParser( specs );
}

Objects ObjectHierarchy::buildObjects( const Objects& os, const KigDocument& doc ) const
{
  assert( os.size() == mnumberofargs );
  for ( uint i = 0; i < os.size(); ++i )
    assert( os[i]->hasimp( margrequirements[i] ) );

  Objects stack;
  stack.resize( mnodes.size() + mnumberofargs, 0 );
  std::copy( os.begin(), os.end(), stack.begin() );

  for( uint i = 0; i < mnodes.size(); ++i )
  {
    mnodes[i]->apply( stack, mnumberofargs + i );
    stack[mnumberofargs + i]->calc( doc );
  };

  for ( uint i = mnumberofargs; i < stack.size() - mnumberofresults; ++i )
    stack[i]->setShown( false );

  Objects ret( stack.begin() + mnumberofargs, stack.end() );

  return ret;
}

int ObjectHierarchy::idOfLastResult() const
{
  const Node* n = mnodes.back();
  if ( n->id() == Node::ID_PushStack )
    return static_cast<const PushStackNode*>( n )->imp()->id();
  else if ( n->id() == Node::ID_FetchProp )
    return ObjectImp::ID_AnyImp;
  else
    return static_cast<const ApplyTypeNode*>( n )->type()->resultId();
}

ObjectHierarchy ObjectHierarchy::transformFinalObject( const Transformation& t ) const
{
  assert( mnumberofresults == 1 );
  ObjectHierarchy ret( *this );
  ret.mnodes.push_back( new PushStackNode( new TransformationImp( t ) ) );
  std::vector<int> parents;
  parents.push_back( ret.mnodes.size() - 1 );
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
};

bool ObjectHierarchy::resultDoesNotDependOnGiven() const
{
  for ( uint i = mnodes.size() - mnumberofresults; i < mnodes.size(); ++i )
  {
    if ( mnodes[i]->id() == Node::ID_PushStack )
      return true;
  };
  return false;
}
