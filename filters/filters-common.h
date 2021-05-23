// SPDX-FileCopyrightText: 2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef FILTERS_COMMON_H
#define FILTERS_COMMON_H

class ObjectTypeCalcer;
class Coordinate;
class ObjectCalcer;
class QByteArray;
class KigDocument;

/**
 * constructs a text object with text "%1", location \p c, and variable
 * parts given by the argument \p arg of obj \p o.
 */
ObjectTypeCalcer* filtersConstructTextObject(
  const Coordinate& c, ObjectCalcer* o,
  const QByteArray& arg, const KigDocument& doc, bool needframe );

#endif  // FILTERS_COMMON_H
