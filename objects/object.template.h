#ifndef segment_h
#define segment_h

class X
    : public Object
{
public:
  X(){};
  ~X() {};
  QCString type() const { return "point"; };
  bool contains (const QPoint& o, bool strict) const;
  void draw (QPainter& p, bool selected, bool showSelection);
  bool inRect (const QRect&) const;
  QRect getSpan(void) const;

    // arguments
    QString wantArg ( const Object* ) const;
    bool selectArg (Object* which);
    void unselectArg (Object* which);

    // moving
    void startMove(const QPoint&);
    void moveTo(const QPoint&);
    void stopMove();
    void cancelMove();

    void moved(Object* o);

protected:
}

#endif
