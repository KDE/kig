#include "segment.h"

#include <klocale.h>

bool Segment::contains(const Coordinate& o, bool strict) const
{
  return false;
}

void Segment::draw(KigPainter& p, bool selected, bool showSelection)
{
    if ( !shown ) return;
}

bool Segment::inRect(const Rect& p) const
{
}

Rect Segment::getSpan(void) const
{
}

QString Segment::wantArg( const Object* o) const
{
  return 0;
}

bool Segment::selectArg(Object* which)
{
  return true; // if we're complete (don't need any more args), we return true
}

void Segment::unselectArg(Object* which)
{
}

void Segment::startMove(const Coordinate& p)
{
}

void Segment::moveTo(const Coordinate& p)
{
}

void Segment::stopMove()
{
}

void Segment::cancelMove()
{
}

void Segment::moved(Object*)
{
}
