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
#include "../objects/object_type.h"

class ObjectHierarchy::Node
{
public:
  virtual ~Node();
  virtual void apply( std::vector<const ObjectImp*>& stack, const KigWidget&,
                      int loc ) const = 0;
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
  void apply( std::vector<const ObjectImp*>& stack,
              const KigWidget&, int loc ) const;
};

PushStackNode::~PushStackNode()
{
  delete mimp;
};

void PushStackNode::apply( std::vector<const ObjectImp*>& stack,
                           const KigWidget&, int loc ) const
{
  stack[loc] = mimp->copy();
};

class ApplyTypeNode
  : public ObjectHierarchy::Node
{
  ObjectType* mtype;
  std::vector<int> mparents;
public:
  ApplyTypeNode( ObjectType* type, std::vector<int>& parents )
    : mtype( type ), mparents( parents ) {};
  ~ApplyTypeNode();
  void apply( std::vector<const ObjectImp*>& stack, const KigWidget&,
              int loc ) const;
};

ApplyTypeNode::~ApplyTypeNode()
{
  delete mtype;
}

void ApplyTypeNode::apply( std::vector<const ObjectImp*>& stack,
                           const KigWidget& w,int loc ) const
{
  Args args;
  for ( uint i = 0; i < mparents.size(); ++i )
  {
    args.push_back( stack[mparents[i]] );
  };
  stack[loc] = mtype->calc( args, w );
};

ObjectImp* ObjectHierarchy::calc( const Args& a, const KigWidget& w ) const
{
  std::vector<const ObjectImp*> stack;
  stack.resize( mnodes.size() + mnumberofargs, 0 );
  std::copy( a.begin(), a.end(), stack.begin() );
  for( uint i = 0; i < mnodes.size(); ++i )
  {
    mnodes[i]->apply( stack, w, mnumberofargs + i );
  };
  for ( uint i = 0; i < stack.size() - 1; ++i )
    delete stack[i];
  if ( stack.empty() ) return 0;
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
  bool neednode = false;
  std::vector<int> parents;
  parents.resize( p.size(), -1 );
  for ( uint i = 0; i < p.size(); ++i )
  {
    parents[i] = visit( p[i], from );
    neednode |= parents[i] != -1;
  };
  if ( ! neednode ) return -1;

  for ( uint i = 0; i < parents.size(); ++i )
  {
    if ( parents[i] == -1 )
    {
      mnodes.push_back( new PushStackNode( p[i]->imp()->copy() ) );
      parents[i] = mnumberofargs + mnodes.size() - 1;
    };
  };
  mnodes.push_back( new ApplyTypeNode( o->type()->copy(), parents ) );
  return mnumberofargs + mnodes.size() - 1;
}

ObjectHierarchy::~ObjectHierarchy()
{
  for ( uint i = 0; i < mnodes.size(); ++i ) delete mnodes[i];
}
