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
#include "../objects/bogus_imp.h"
#include "../objects/object_type.h"

class ObjectHierarchy::Node
{
public:
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
  Node* copy() const;
  void apply( std::vector<const ObjectImp*>& stack,
              int loc ) const;
};

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
  ObjectType* mtype;
  std::vector<int> mparents;
public:
  ApplyTypeNode( ObjectType* type, const std::vector<int>& parents )
    : mtype( type ), mparents( parents ) {};
  ~ApplyTypeNode();
  Node* copy() const;
  void apply( std::vector<const ObjectImp*>& stack,
              int loc ) const;
};

ApplyTypeNode::~ApplyTypeNode()
{
  delete mtype;
}

ObjectHierarchy::Node* ApplyTypeNode::copy() const
{
  return new ApplyTypeNode( mtype->copy(), mparents );
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

ObjectImp* ObjectHierarchy::calc( const Args& a ) const
{
  std::vector<const ObjectImp*> stack;
  stack.resize( mnodes.size() + mnumberofargs, 0 );
  std::copy( a.begin(), a.end(), stack.begin() );
  for( uint i = 0; i < mnodes.size(); ++i )
  {
    mnodes[i]->apply( stack, mnumberofargs + i );
  };
  for ( uint i = mnumberofargs; i < stack.size() - 1; ++i )
    delete stack[i];
  if ( stack.size() <= mnumberofargs ) return new InvalidImp;
  else return const_cast<ObjectImp*>( stack.back() );
}

ObjectHierarchy::ObjectHierarchy( const Objects& from, const Object* to )
  : mnumberofargs( from.size() )
{
  visit( to, from );
}

int ObjectHierarchy::visit( const Object* o, const Objects& from )
{
  for ( uint i = 0; i < from.size(); ++i )
    if ( from[i] == o ) return i;
  Objects p( o->parents() );
  Args args = o->fixedArgs();
  bool neednode = false;
  std::vector<int> parents;
  parents.resize( p.size() + args.size(), -1 );
  for ( uint i = 0; i < p.size(); ++i )
  {
    int v = visit( p[i], from );
    parents[args.size() + i] = v;
    neednode |= (v != -1);
  };
  if ( ! neednode ) return -1;

  for ( uint i = 0; i < args.size(); ++i )
  {
    mnodes.push_back( new PushStackNode( args[i]->copy() ) );
    parents[i] = mnumberofargs + mnodes.size() - 1;
  };
  for ( uint i = 0; i < p.size(); ++i )
  {
    if ( parents[args.size() + i] == -1 )
    {
      mnodes.push_back( new PushStackNode( p[i]->imp()->copy() ) );
      parents[args.size() + i] = mnumberofargs + mnodes.size() - 1;
    };
  };
  mnodes.push_back( new ApplyTypeNode( o->type()->copy(), parents ) );
  return mnumberofargs + mnodes.size() - 1;
}

ObjectHierarchy::~ObjectHierarchy()
{
  for ( uint i = 0; i < mnodes.size(); ++i ) delete mnodes[i];
}

ObjectHierarchy::ObjectHierarchy( const ObjectHierarchy& h )
  : mnumberofargs( h.mnumberofargs )
{
  mnodes.reserve( h.mnodes.size() );
  for ( uint i = 0; i < h.mnodes.size(); ++i )
    mnodes.push_back( h.mnodes[i]->copy() );
}
