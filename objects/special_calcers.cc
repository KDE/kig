// Copyright (C)  2004  Dominique Devriese <devriese@kde.org>

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

#include "special_calcers.h"

static const ArgsParser::spec argsspecMeasureTransport[] =
{
  { CircleImp::stype(), I18N_NOOP( "Transport a measure on this circle" ) },
  { PointImp::stype(), I18N_NOOP( "Project this point onto the circle" ) },
  { SegmentImp::stype(), I18N_NOOP( "Segment to transport" ) }
};

static ArgsParser measuretransportargsparser( argsspecMeasureTransport, 3 );

std::vector<ObjectCalcer*> MeasureTransportCalcer::parents() const
{
  std::vector<ObjectCalcer*> ret;
  ret.push_back( mcircle );
  ret.push_back( mpoint );
  ret.push_back( msegment );
  return ret;
}

MeasureTransportCalcer::MeasureTransportCalcer(ObjectCalcer* circle, ObjectCalcer* point, ObjectCalcer* segment )
  : mcircle( circle ), mpoint( point ), msegment( segment ), mimp( 0 )
{
}

MeasureTransportCalcer::~MeasureTransportCalcer()
{
}

void MeasureTransportCalcer::calc( const KigDocument& )
{
  if ( ! measuretransportargsparser.checkArgs( parents() ) )
    return new InvalidImp();

  if ( ! isPointOnCurve( mpoint, mcircle ) )
    return new InvalidImp();

  const CircleImp* c = static_cast<const CircleImp*>( mcircle->imp() );
  const PointImp* p = static_cast<const PointImp*>( mpoint->imp() );
  const SegmentImp* s = static_cast<const SegmentImp*>( msegment->imp() );
  double param = c->getParam( p->coordinate(), doc );
  double measure = s->length();
  measure /= 2*c->radius()*M_PI;
  param += measure;
  while (param > 1) param -= 1;

  const Coordinate nc = c->getPoint( param, doc );
  if ( nc.valid() ) return new PointImp( nc );
  else return new InvalidImp;
}

const ObjectImpType* MeasureTransportCalcer::impRequirement(
  ObjectCalcer* o, const std::vector<ObjectCalcer*>& os ) const
{
  if ( o->imp()->inherits( CircleImp::stype() ) )
    return CircleImp::stype();
  else if ( o->imp()->inherits( PointImp::stype() ) )
    return PointImp::stype();
  else if ( o->imp()->inherits( SegmentImp::stype() ) )
    return SegmentImp::stype();
  else
  {
    assert( false );
    return 0;
  }
}

