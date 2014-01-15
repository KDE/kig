/*
 * GeoGebra Filter for Kig
 * Copyright 2013  David E. Narvaez <david.narvaez@computer.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef KIGFILTERGEOGEBRA_H
#define KIGFILTERGEOGEBRA_H

#include "filter.h"

#include <QMap>
#include <QAbstractXmlReceiver>
#include <QXmlNamePool>

class ObjectCalcer;
class ObjectType;

class KigFilterGeogebra : public KigFilter, public QAbstractXmlReceiver
{
public:
  static KigFilterGeogebra* instance();
  virtual KigDocument* load(const QString& fromfile);
  virtual bool supportMime(const QString& mime);
  
  // QAbstractXmlReceiver implementation
  virtual void atomicValue(const QVariant &);
  virtual void attribute(const QXmlName & name, const QStringRef & value);
  virtual void characters(const QStringRef &);
  virtual void comment(const QString &);
  virtual void endDocument();
  virtual void endElement();
  virtual void endOfSequence();
  virtual void namespaceBinding(const QXmlName &);
  virtual void processingInstruction(const QXmlName &, const QString &);
  virtual void startDocument();
  virtual void startElement(const QXmlName & name);
  virtual void startOfSequence();
  
protected:
  KigFilterGeogebra() {}
  ~KigFilterGeogebra() {}
  
private:
  enum State
  {
    ReadingDouble,
    ReadingObject,
    ReadingArguments
  };
  
  State m_currentState;
  QMap<QByteArray, ObjectCalcer *> m_objectMap;
  std::vector<ObjectCalcer *> m_currentArgStack;
  const ObjectType * m_currentObject;
  QByteArray m_currentObjectLabel;
  KigDocument * m_document;
  QXmlNamePool m_np;
};

#endif // KIGFILTERGEOGEBRA_H
