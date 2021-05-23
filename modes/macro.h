// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_MODES_MACRO_H
#define KIG_MODES_MACRO_H

#include "base_mode.h"

#include <QObject>

class MacroWizard;

class DefineMacroMode
  : public BaseMode
{
public:
  explicit DefineMacroMode( KigPart& );
  ~DefineMacroMode();

  void dragRect( const QPoint& p, KigWidget& w ) Q_DECL_OVERRIDE;
  void leftClickedObject( ObjectHolder* o, const QPoint& p,
                          KigWidget& w, bool ctrlOrShiftDown ) Q_DECL_OVERRIDE;
  using BaseMode::midClicked;
  using BaseMode::rightClicked;
  using BaseMode::mouseMoved;

  // called by MacroWizard class
  void givenPageEntered();
  void finalPageEntered();
  bool validateObjects();
  void finishPressed();
  void cancelPressed();

  bool hasGivenArgs() const;
  bool hasFinalArgs() const;

protected:
  void rightClicked( const std::vector<ObjectHolder*>& oco, const QPoint& p, KigWidget& w ) Q_DECL_OVERRIDE;
  void midClicked( const QPoint& p, KigWidget& w ) Q_DECL_OVERRIDE;
  void mouseMoved( const std::vector<ObjectHolder*>& os, const QPoint& p, KigWidget& w, bool shiftpressed ) Q_DECL_OVERRIDE;
  void enableActions() Q_DECL_OVERRIDE;
  /**
   * quit this mode...
   */
  void abandonMacro();

  QPoint plc;
  MacroWizard* mwizard;

  // we can't use a set for this because the order is important
  std::vector<ObjectHolder*> mgiven;
  std::vector<ObjectHolder*> mfinal;
};

#endif
