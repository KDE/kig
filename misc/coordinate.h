#ifndef COORDINATE_H
#define COORDINATE_H

#include <qpoint.h>
#include <kdebug.h>

// including cmath doesn't work properly here...
extern "C"
{
  double sqrt( double );
  double round( double );
}

struct Coordinate
{
public:
  inline Coordinate( const double x, const double y );
  inline Coordinate( const Coordinate& p );
  inline Coordinate();
  ~Coordinate() {};
  
  inline Coordinate operator+ ( const Coordinate& c ) const;
  inline Coordinate operator- ( const Coordinate& c ) const;
  inline Coordinate operator* ( const double r ) const;
  inline Coordinate operator/ ( const double r ) const;
  inline Coordinate operator- () const;
  inline double distance ( const Coordinate& p ) const;
  inline double length () const;
  inline bool operator== ( const Coordinate& p ) const;
  inline bool operator!=( const Coordinate& p ) const;
  inline Coordinate round() const;
  inline Coordinate normalize() const;
  inline QPoint toQPoint() const;

  inline Coordinate& operator= ( const Coordinate& c );
  inline Coordinate& operator+= ( const Coordinate& c );
  inline Coordinate& operator-= ( const Coordinate& c );
  inline Coordinate& operator*= ( const double r );
  inline Coordinate& operator/= ( const double r );
public:
  double x;
  double y;
  friend kdbgstream& operator<<( kdbgstream& s, const Coordinate& t );
};

inline kdbgstream& operator<<( kdbgstream& s, const Coordinate& t )
{
  s << "x: " << t.x << "  y: " << t.y << endl;
  return s;
};


Coordinate::Coordinate()
  : x(0),
    y(0)
{
};

Coordinate::Coordinate( const double nx, const double ny )
  : x( nx ),
    y( ny )
{
};

Coordinate::Coordinate( const Coordinate& p )
  : x( p.x ),
    y( p.y )
{
};

Coordinate Coordinate::operator+( const Coordinate& p ) const
{
  return Coordinate ( x + p.x, y + p.y );
};

Coordinate Coordinate::operator-( const Coordinate& p ) const
{
  return Coordinate ( x - p.x, y - p.y );
};

Coordinate Coordinate::operator*( const double r ) const
{
  return Coordinate ( r*x, r*y );
};

Coordinate Coordinate::operator/( const double r ) const
{
  return Coordinate ( x/r, y/r );
};

Coordinate Coordinate::operator-() const
{
  return Coordinate ( -x, -y );
};

Coordinate& Coordinate::operator=( const Coordinate& p )
{
  x = p.x;
  y = p.y;
  return *this;
};

Coordinate& Coordinate::operator+=( const Coordinate& p )
{
  x += p.x;
  y += p.y;
  return *this;
};

Coordinate& Coordinate::operator-=( const Coordinate& p )
{
  x -= p.x;
  y -= p.y;
  return *this;
};

Coordinate& Coordinate::operator*=( const double r )
{
  x *= r;
  y *= r;
  return *this;
};

Coordinate& Coordinate::operator/=( const double r )
{
  x /= r;
  y /= r;
  return *this;
};

double Coordinate::distance( const Coordinate& p ) const
{
  return (p - *this).length();
};

double Coordinate::length() const
{
  return sqrt(x*x+y*y);
};

bool Coordinate::operator==( const Coordinate& p ) const
{
  return p.x == x && p.y == y;
};

bool Coordinate::operator!=( const Coordinate& p ) const
{
  return !operator==(p);
};

Coordinate Coordinate::normalize() const
{
  return operator/(length());
};

Coordinate Coordinate::round() const
{
  return Coordinate( ::round( x ), ::round( y ) );
};

QPoint Coordinate::toQPoint() const
{
  Coordinate t = round();
  return QPoint( t.x, t.y );
};

#endif

