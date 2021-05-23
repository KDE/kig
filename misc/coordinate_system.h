/*
    This file is part of Kig, a KDE program for Interactive Geometry...
    SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef KIG_MISC_COORDINATE_SYSTEM_H
#define KIG_MISC_COORDINATE_SYSTEM_H

#include <qnamespace.h>

class KigPainter;
class KigDocument;
class KigWidget;
class CoordinateSystem;
class QValidator;
class Coordinate;
class QString;
class QStringList;

/**
 * a factory to build a CoordinateSystem and a small handle to the
 * existent CoordinateSystem's...
 */
class CoordinateSystemFactory
{
public:
  enum { Euclidean = 0, Polar = 1 };

  static QStringList names();
  static QString setCoordinateSystemStatement( int id );
  static CoordinateSystem* build( int which );
  static CoordinateSystem* build( const char* type );
};

/**
 * a CoordinateSystem is what the user sees: it is kept by KigPart to
 * show the user a grid, and to show the coordinates of points... it
 * allows for weird CoordinateSystem's like homogeneous or
 * projective...
 * internally, it does nothing, it could almost have been an ordinary
 * Object..., mapping coordinates from and to the screen to and from
 * the internal coordinates is done elsewhere ( KigPainter and
 * KigWidget... )
 */
class CoordinateSystem
//  : public Qt
{
public:
  CoordinateSystem();
  virtual ~CoordinateSystem();

  virtual QString fromScreen ( const Coordinate& pt, const KigDocument& w ) const = 0;
  /**
   * This returns a notice to say in which format coordinates should
   * be entered.  This should be something like:
   * i18n( "Enter coordinates in the following form: \"(x,y)\", where
   * x is the x coordinate, and y is the y coordinate." );
   */
  virtual QString coordinateFormatNotice() const = 0;
  /**
   * Like \ref coordinateFormatNotice(), but with HTML tags useful to
   * have a rich text...
   */
  virtual QString coordinateFormatNoticeMarkup() const = 0;
  virtual Coordinate toScreen (const QString& pt, bool& ok) const = 0;
  virtual void drawGrid ( KigPainter& p, bool showgrid = true,
                          bool showaxes = true ) const = 0;
  virtual QValidator* coordinateValidator() const = 0;
  virtual Coordinate snapToGrid( const Coordinate& c,
                                 const KigWidget& w ) const = 0;

  virtual const char* type() const = 0;
  virtual int id() const = 0;
};

class EuclideanCoords
  : public CoordinateSystem
{
public:
  EuclideanCoords();
  ~EuclideanCoords();
  QString fromScreen( const Coordinate& pt, const KigDocument& w ) const Q_DECL_OVERRIDE;
  QString coordinateFormatNotice() const Q_DECL_OVERRIDE;
  QString coordinateFormatNoticeMarkup() const Q_DECL_OVERRIDE;
  Coordinate toScreen (const QString& pt, bool& ok) const Q_DECL_OVERRIDE;
  void drawGrid ( KigPainter& p, bool showgrid = true,
                  bool showaxes = true ) const Q_DECL_OVERRIDE;
  QValidator* coordinateValidator() const Q_DECL_OVERRIDE;
  Coordinate snapToGrid( const Coordinate& c,
                         const KigWidget& w ) const Q_DECL_OVERRIDE;

  const char* type() const Q_DECL_OVERRIDE;
  int id() const Q_DECL_OVERRIDE;
};

class PolarCoords
  : public CoordinateSystem
{
  void drawGridLine( KigPainter& p, const Coordinate& center,
                     double radius ) const;
public:
  PolarCoords();
  ~PolarCoords();
  QString fromScreen( const Coordinate& pt, const KigDocument& w ) const Q_DECL_OVERRIDE;
  QString coordinateFormatNotice() const Q_DECL_OVERRIDE;
  QString coordinateFormatNoticeMarkup() const Q_DECL_OVERRIDE;
  Coordinate toScreen (const QString& pt, bool& ok) const Q_DECL_OVERRIDE;
  void drawGrid ( KigPainter& p, bool showgrid = true,
                  bool showaxes = true ) const Q_DECL_OVERRIDE;
  QValidator* coordinateValidator() const Q_DECL_OVERRIDE;
  Coordinate snapToGrid( const Coordinate& c,
                         const KigWidget& w ) const Q_DECL_OVERRIDE;

  const char* type() const Q_DECL_OVERRIDE;
  int id() const Q_DECL_OVERRIDE;
};

#endif
