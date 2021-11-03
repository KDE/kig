// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_MODES_NORMAL_H
#define KIG_MODES_NORMAL_H

#include "base_mode.h"

#include <QPoint>
#include <set>

class NormalMode
  : public BaseMode
{
public:
  explicit NormalMode( KigPart& );
  ~NormalMode();

  using BaseMode::midClicked;
  using BaseMode::rightClicked;
  using BaseMode::mouseMoved;

protected:
  void dragRect( const QPoint& p, KigWidget& w ) override;
  void dragObject( const std::vector<ObjectHolder*>& os, const QPoint& pointClickedOn,
                   KigWidget& w, bool ctrlOrShiftDown ) override;
  void leftClickedObject( ObjectHolder* o, const QPoint& p,
                          KigWidget& w, bool ctrlOrShiftDown ) override;
  void midClicked( const QPoint& p, KigWidget& w ) override;
  void rightClicked( const std::vector<ObjectHolder*>& os, const QPoint& p, KigWidget& w ) override;
  void mouseMoved( const std::vector<ObjectHolder*>& os, const QPoint& p, KigWidget& w,
                   bool shiftpressed ) override;
  void selectAll() override;
  void deselectAll() override;
  void invertSelection() override;

protected:
  /**
   * Objects were added.
   */
  void redrawScreen( KigWidget* ) override;

  void enableActions() override;

  void deleteObjects() override;
  void showHidden() override;
  void newMacro() override;
  void editTypes() override;
  void browseHistory() override;

public:
  void selectObject( ObjectHolder* o );
  void selectObjects( const std::vector<ObjectHolder*>& os );
  void unselectObject( ObjectHolder* o );
  void clearSelection();

//   KigObjectsPopup* popup( const Objects& os );
//   KigDocumentPopup* popup( KigDocument* );
protected:
  /**
   * selected objects...
   */
  std::set<ObjectHolder*> sos;
};

#endif
