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
#include "../objects/object_imp.h"
#include "../objects/object_imp_factory.h"
#include "../objects/object_type_factory.h"
#include "../objects/bogus_imp.h"
#include "../objects/object_type.h"

#include <kglobal.h>
#include <qdom.h>

class ObjectHierarchy::Node
{
public:
  enum { ID_PushStack, ID_ApplyType };
  virtual int id() const = 0;

  virtual ~Node();
  virtual Node* copy() const = 0;
  virtual void apply( std::vector<const ObjectImp*>& stack, int loc ) const = 0;
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
              int loc ) const;
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
                           int loc ) const
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
              int loc ) const;
};

int ApplyTypeNode::id() const { return ID_ApplyType; };

ApplyTypeNode::~ApplyTypeNode()
{
}

ObjectHierarchy::Node* ApplyTypeNode::copy() const
{
  return new ApplyTypeNode( mtype, mparents );
};

void ApplyTypeNode::apply( std::vector<const ObjectImp*>& stack,
                           int loc ) const
{
  Args args;
  for ( uint i = 0; i < mparents.size(); ++i )
  {
    args.push_back( stack[mparents[i]] );
  };
  stack[loc] = mtype->calc( args );
};

std::vector<ObjectImp*> ObjectHierarchy::calc( const Args& a ) const
{
  std::vector<const ObjectImp*> stack;
  stack.resize( mnodes.size() + mnumberofargs, 0 );
  std::copy( a.begin(), a.end(), stack.begin() );
  for( uint i = 0; i < mnodes.size(); ++i )
  {
    mnodes[i]->apply( stack, mnumberofargs + i );
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

ObjectHierarchy::ObjectHierarchy( const Objects& from, const Object* to )
  : mnumberofargs( from.size() ), mnumberofresults( 1 )
{
  visit( to, from );
}

int ObjectHierarchy::visit( const Object* o, const Objects& from )
{
  for ( uint i = 0; i < from.size(); ++i )
    if ( from[i] == o ) return i;
  Objects p( o->parents() );
  if ( p.empty() ) return -1;

  assert( o->inherits( Object::ID_RealObject ) );

  bool neednode = false;
  std::vector<int> parents;
  parents.resize( p.size(), -1 );
  for ( uint i = 0; i < p.size(); ++i )
  {
    int v = visit( p[i], from );
    parents[i] = v;
    neednode |= (v != -1);
  };
  if ( ! neednode ) return -1;

  for ( uint i = 0; i < p.size(); ++i )
  {
    if ( parents[i] == -1 )
    {
      mnodes.push_back( new PushStackNode( p[i]->imp()->copy() ) );
      parents[i] = mnumberofargs + mnodes.size() - 1;
    };
  };
  mnodes.push_back( new ApplyTypeNode( static_cast<const RealObject*>( o )->type(), parents ) );
  return mnumberofargs + mnodes.size() - 1;
}

ObjectHierarchy::~ObjectHierarchy()
{
  for ( uint i = 0; i < mnodes.size(); ++i ) delete mnodes[i];
}

ObjectHierarchy::ObjectHierarchy( const ObjectHierarchy& h )
  : mnumberofargs( h.mnumberofargs ), mnumberofresults( h.mnumberofresults )
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
  std::vector<Node*> newnodes( mnodes.size() + a.size() );
  std::vector<Node*>::iterator newnodesiter = newnodes.begin();
  for ( uint i = 0; i < a.size(); ++i )
    *newnodesiter++ = new PushStackNode( a[i]->copy() );
  std::copy( ret.mnodes.begin(), ret.mnodes.end(), newnodesiter );
  ret.mnodes = newnodes;
  return ret;
}

ObjectHierarchy::ObjectHierarchy( const Objects& from, const Objects& to )
  : mnumberofargs( from.size() ), mnumberofresults( to.size() )
{
  for ( Objects::const_iterator i = to.begin(); i != to.end(); ++i )
    visit( *i, from );
}

void ObjectHierarchy::serialize( QDomElement& parent, QDomDocument& doc ) const
{
  int id = 1;
  for ( uint i = 0; i < mnumberofargs; ++i )
  {
    QDomElement e = doc.createElement( "input" );
    e.setAttribute( "id", id++ );
    parent.appendChild( e );
  }
  for ( uint i = 0; i < mnodes.size(); ++i )
  {
    bool result = mnodes.size() - ( id - mnumberofargs - 1 ) <= mnumberofresults;
    QDomElement e = doc.createElement( result ? "result" : "intermediate" );
    e.setAttribute( "id", id++ );

    if ( mnodes[i]->id() == ObjectHierarchy::Node::ID_ApplyType )
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

ObjectHierarchy::ObjectHierarchy( QDomElement& parent )
  : mnumberofargs( 0 ), mnumberofresults( 0 )
{
  bool ok = true;
  QDomElement e = parent.firstChild().toElement();
  for (; !e.isNull(); e = e.nextSibling().toElement() )
  {
    if ( e.tagName() != "input" ) break;
    else ++mnumberofargs;
  }
  for (; !e.isNull(); e = e.nextSibling().toElement() )
  {
    bool result = e.tagName() == "result";
    if ( result ) ++mnumberofresults;

    QString tmp = e.attribute( "id" );
    int id = tmp.toInt( &ok );
    if ( ! ok ) continue;       // could be better :(

    tmp = e.attribute( "action" );
    Node* newnode = 0;
    if ( tmp == "calc" )
    {
      // ApplyTypeNode
      QCString typen = e.attribute( "type" ).latin1();
      const ObjectType* type = ObjectTypeFactory::instance()->find( typen );
      if ( ! type ) continue;   // anyone got a better idea on
                                // reportin errors here ?
      std::vector<int> parents;
      for ( QDomNode p = e.firstChild(); !p.isNull(); p = p.nextSibling() )
      {
        QDomElement q = p.toElement();
        if ( q.isNull() ) continue; // see above
        if ( q.tagName() != "arg" ) continue;
        int pid = q.text().toInt(&ok );
        if ( ! ok ) continue;
        parents.push_back( pid - 1 );
      };
      newnode = new ApplyTypeNode( type, parents );
    }
    else
    {
      // PushStackNode
      if ( e.attribute( "action" ) != "push" ) continue;
      QString typen = e.attribute( "type" );
      if ( typen.isNull() ) continue;
      ObjectImp* imp = ObjectImpFactory::instance()->deserialize( typen, e );
      newnode = new PushStackNode( imp );
    };
    mnodes.resize( kMax( id - mnumberofargs, mnodes.size() ) );
    mnodes[id - mnumberofargs - 1] = newnode;
  };
}
