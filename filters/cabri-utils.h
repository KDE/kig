// This file is part of Kig, a KDE program for Interactive Geometry...
// Copyright (C)  2002  Dominique Devriese <devriese@kde.org>
// Copyright (C)  2006  Pino Toscano       <toscano.pino@tiscali.it>

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

#ifndef KIG_FILTERS_CABRI_UTILS_H
#define KIG_FILTERS_CABRI_UTILS_H

#undef CABRI_DEBUG

#include <qbytearray.h>
#include <qcolor.h>

#include "../misc/rect.h"
#include "../objects/object_drawer.h"

#include <map>
#include <vector>

class QFile;
class KigFilterCabri;

namespace CabriNS
{
  /**
   * Cabri versions we try to read from.
   */
  enum CabriVersion
  {
    CV_1_0,
    CV_1_2
  };

  enum CabriGonio
  {
    CG_Deg,
    CG_Rad
    // TODO
  };

  /**
   * Read a line from a Cabri file, stripping the \\n and \\r characters.
   */
  QString readLine( QFile& file );

  QString readText( QFile& f, const QString& s, const QString& sep = QStringLiteral( "\n" ) );
}

/**
 * Base class representing a Cabri object.
 *
 * Particular features of a particular version are added in the
 * inherited classes.
 */
class CabriObject
{
public:
  CabriObject();

  uint id;
  QByteArray type;
  int specification;
  QColor color;
  QColor fillColor;
  int thick;
  int lineSegLength;
  int lineSegSplit;
//  int specialAppearanceSwitch;
  bool visible;
  int intersectionId;
  int ticks;
  int side;
//  bool fixed;
  std::vector<int> parents;
  std::vector<double> data;
  QString name;
  QString text;
  Rect textRect;
  std::vector<int> incs;
  CabriNS::CabriGonio gonio;
};

class CabriObject_v10
  : public CabriObject
{
public:
  CabriObject_v10();

  int specialAppearanceSwitch;
  bool fixed;
};

class CabriObject_v12
  : public CabriObject
{
public:
  CabriObject_v12();

  int pointStyle;
};


/**
 * Base reader for a Cabri figure. It basically reads all the things
 * it can from a Cabri figure, even if they are not used.
 */
class CabriReader
{
protected:
  const KigFilterCabri* m_filter;

  CabriReader( const KigFilterCabri* filter );

  static void initColorMap();
  void parseError( const QString& explanation = QString() ) const;

public:
  virtual ~CabriReader();

  /**
   * Translate a color from a 1 to 3 character sequence.
   */
  static QColor translateColor( const QString& s );

  virtual bool readWindowMetrics( QFile& f ) = 0;
  virtual CabriObject* readObject( QFile& f ) = 0;
  virtual void decodeStyle( CabriObject* obj, Qt::PenStyle& ps, Kig::PointStyle& pointType ) = 0;
};

class CabriReader_v10
  : public CabriReader
{
public:
  explicit CabriReader_v10( const KigFilterCabri* filter );
  virtual ~CabriReader_v10();

  bool readWindowMetrics( QFile& f ) Q_DECL_OVERRIDE;
  CabriObject* readObject( QFile& f ) Q_DECL_OVERRIDE;
  void decodeStyle( CabriObject* obj, Qt::PenStyle& ps, Kig::PointStyle& pointType ) Q_DECL_OVERRIDE;
};

class CabriReader_v12
  : public CabriReader
{
private:
  static void initColorMap();
  bool readStyles( const QString& s, CabriObject_v12* myobj );

public:
  explicit CabriReader_v12( const KigFilterCabri* filter );
  virtual ~CabriReader_v12();

  static QColor translateColor( const QString& s );

  bool readWindowMetrics( QFile& f ) Q_DECL_OVERRIDE;
  CabriObject* readObject( QFile& f ) Q_DECL_OVERRIDE;
  void decodeStyle( CabriObject* obj, Qt::PenStyle& ps, Kig::PointStyle& pointType ) Q_DECL_OVERRIDE;
};

#endif
