/**
 This file is part of Kig, a KDE program for Interactive Geometry...
 Copyright (C) 2002  Dominique Devriese
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 USA
**/


#ifndef MACROWIZARDIMPL_H
#define MACROWIZARDIMPL_H
#include "macrowizard.h"

#include "../misc/objects.h"

#include <klineedit.h>
class KigDocument;

class MacroWizardImpl : public MacroWizard
{ 
  Q_OBJECT

protected:
  KigDocument* document;

public:
  MacroWizardImpl( KigDocument* parent);
  ~MacroWizardImpl();

  Objects givenObjs;
  Objects finalObjs;
  
public slots:
  void setFinishable(const QString&) { updateNexts(); };

  /** 
   * en-/disable the next and finish buttons...
   * 
   */
  void updateNexts();

  virtual void reject();
  virtual void next();
  virtual void back();
  virtual void accept();

};

#endif // MACROWIZARDIMPL_H
