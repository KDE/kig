#ifndef VECTOR_H
#define VECTOR_H

class Vector
: public Object
{
 public:
  Vector() : a(0), b(0){};
  ~Vector(){};
  const char* type() { return I18N_NOOP("vector");};
  void draw (QPainter& p, bool showSelect = true);
  bool inRect (const QRect& r) const;
  QRect getSpan() const;
  const char* wantArument (const char* arg);
  bool selectArgument (Object* which)
    {
      if (a) { a = which; if (b) constructed = true;};
      else if (b) { b = which; if (a) constructed = true;}
      else cerr << "vector::selectArgument called on a vector which is ready";
    };
  void unselectArgument(Object* which)
    {
      if (a == which) { a = 0; constructed = false;}
      else if (b == which) { b = 0; constructed = false;}
      else cerr << "vector::unselectArgument called for an argument which hasn't been selected";
    };
  
 protected:
  Point a, b;
};

#endif
