#include "segment.h"

#include <klocale.h>

bool Segment::contains(const QPoint& o, bool strict) const
{
  return false;
}

void Segment::draw(QPainter& p, bool selected, bool showSelection)
{
    if ( !shown ) return;
}

bool Segment::inRect(const QRect& p) const
{
}

QRect Segment::getSpan(void) const
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

void Segment::startMove(const QPoint& p)
{
}

void Segment::moveTo(const QPoint& p)
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

Point Segment::getPoint(double /*param*/) const
{
    return Point( 0,0 );
}

double Segment::getParam(const Point&) const
{
    return 0;
}
