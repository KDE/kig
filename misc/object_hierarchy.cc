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

#include "../objects/object_imp.h"
#include "../objects/object_type.h"

class ObjectHierarchy::Node
{
protected:
  virtual ~Node();
public:
  virtual void apply( std::vector<const ObjectImp*>& stack, const KigWidget&,
                      int loc ) = 0;
};

ObjectHierarchy::Node::~Node()
{
};

class PushStackNode
  : public ObjectHierarchy::Node
{
  ObjectImp* mimp;
public:
  void apply( std::vector<const ObjectImp*>& stack, const KigWidget&, int loc );
};

class ApplyTypeNode
  : public ObjectHierarchy::Node
{
  ObjectType* mtype;
  std::vector<int> mparents;
public:
  void apply( std::vector<const ObjectImp*>& stack, const KigWidget&, int loc );
};

void PushStackNode::apply( std::vector<const ObjectImp*>& stack, const KigWidget&,
                           int loc )
{
  stack[loc] = mimp->copy();
};

void ApplyTypeNode::apply( std::vector<const ObjectImp*>& stack, const KigWidget& w,
                           int loc )
{
  Args args;
  for ( uint i = 0; i < mparents.size(); ++i )
  {
    args.push_back( stack[mparents[i]] );
  };
  stack[loc] = mtype->calc( args, w );
};

ObjectImp* ObjectHierarchy::calc( const Args& a, const KigWidget& w )
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
