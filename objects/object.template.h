#ifndef segment_h
#define segment_h

class X
    : public Object
{
public:
  X(){};
  ~X() {};
  QCString type() const { return "point"; };
  bool contains (const Coordinate& o, bool strict) const;
  void draw (KigPainter& p, bool selected, bool showSelection);
  bool inRect (const Rect&) const;
  Rect getSpan(void) const;

    // arguments
    QString wantArg ( const Object* ) const;
    bool selectArg (Object* which);
    void unselectArg (Object* which);

    // moving
    void startMove(const Coordinate&);
    void moveTo(const Coordinate&);
    void stopMove();
    void cancelMove();

    void moved(Object* o);

protected:
}

#endif
