// SPDX-FileCopyrightText: 2004 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_OBJECTS_SPECIAL_CALCERS_H
#define KIG_OBJECTS_SPECIAL_CALCERS_H

class MeasureTransportCalcer
  : public ObjectCalcer
{
  ObjectCalcer* mcircle;
  ObjectCalcer* mpoint;
  ObjectCalcer* msegment;
  ObjectImp* mimp;
public:
  MeasureTransportCalcer(ObjectCalcer* circle, ObjectCalcer* point, ObjectCalcer* segment );
  ~MeasureTransportCalcer();

  std::vector<ObjectCalcer*> parents() const;
  void calc( const KigDocument& );
  const ObjectImpType* impRequirement(
    ObjectCalcer* o, const std::vector<ObjectCalcer*>& os ) const;
};

#endif
