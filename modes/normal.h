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
  void dragRect( const QPoint& p, KigWidget& w ) Q_DECL_OVERRIDE;
  void dragObject( const std::vector<ObjectHolder*>& os, const QPoint& pointClickedOn,
                   KigWidget& w, bool ctrlOrShiftDown ) Q_DECL_OVERRIDE;
  void leftClickedObject( ObjectHolder* o, const QPoint& p,
                          KigWidget& w, bool ctrlOrShiftDown ) Q_DECL_OVERRIDE;
  void midClicked( const QPoint& p, KigWidget& w ) Q_DECL_OVERRIDE;
  void rightClicked( const std::vector<ObjectHolder*>& os, const QPoint& p, KigWidget& w ) Q_DECL_OVERRIDE;
  void mouseMoved( const std::vector<ObjectHolder*>& os, const QPoint& p, KigWidget& w,
                   bool shiftpressed ) Q_DECL_OVERRIDE;
  void selectAll() Q_DECL_OVERRIDE;
  void deselectAll() Q_DECL_OVERRIDE;
  void invertSelection() Q_DECL_OVERRIDE;

protected:
  /**
   * Objects were added.
   */
  void redrawScreen( KigWidget* ) Q_DECL_OVERRIDE;

  void enableActions() Q_DECL_OVERRIDE;

  void deleteObjects() Q_DECL_OVERRIDE;
  void showHidden() Q_DECL_OVERRIDE;
  void newMacro() Q_DECL_OVERRIDE;
  void editTypes() Q_DECL_OVERRIDE;
  void browseHistory() Q_DECL_OVERRIDE;

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
