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

class ObjectHierarchy::Node
{
public:
  virtual void apply( std::stack<ObjectImp*>& stack, const KigWidget& ) = 0;
};

class PushStackNode
  : public ObjectHierarchy::Node
{
  ObjectImp* mimp;
public:
  virtual void apply( std::stack<ObjectImp*>& stack, const KigWidget& );
};

class ApplyTypeNode
  : public ObjectHierarchy::Node
{
  ObjectType* mtype;
  int mnumbertopop;
public:
  virtual void apply( std::stack<ObjectImp*>& stack, const KigWidget& );
};

void PushStackNode::apply( std::stack<ObjectImp*>& stack, const KigWidget& )
{
  stack.push( mimp->copy() );
};

void ApplyTypeNode::apply( std::stack<ObjectImp*>& stack, const KigWidget& w )
{
  Args args;
  for ( int i = 0; i < mnumbertopop; ++i )
  {
    if ( stack.empty() ) return;
    args.push_back( stack.top() );
    stack.pop();
  };
  stack.push( mtype->calc( args, w ) );
  for ( Args::iterator i = args.begin(); i != args.end(); ++i )
    delete *i;
};

ObjectImp* ObjectHierarchy::calc( const Args& a, const KigWidget& w )
{
  std::stack<ObjectImp*> stack;
  std::copy( a.begin(), a.end(), std::back_inserter( stack ) );
  for( std::vector<Node*>::iterator i = mnodes.begin(); i != mnodes.end(); ++i )
  {
    (*i)->apply( stack );
  };
  if ( stack.size() == 1 ) return stack.top();
  else
  {
    while ( ! stack.empty() )
    {
      delete stack.top();
      stack.pop();
    };
    return new InvalidImp;
  };
}
