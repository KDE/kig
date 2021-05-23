// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef KIG_MISC_GUIACTION_H
#define KIG_MISC_GUIACTION_H

#include <config-kig.h>

#ifdef KIG_ENABLE_PYTHON_SCRIPTING
#include "../scripting/script-common.h"
#endif

#include <QString>
#include <QByteArray>
#include <QAction>

class GUIAction;
class KigPart;

class KigGUIAction
  : public QAction
{
  Q_OBJECT
  GUIAction* mact;
  KigPart& mdoc;
public:
  KigGUIAction( GUIAction* act,
                KigPart& doc );

  GUIAction* guiAction();

  void plug( KigPart* doc );
private slots:
  void slotActivated();
};

class GUIAction
{
public:
  virtual ~GUIAction();

  virtual QString description() const = 0;
  virtual QByteArray iconFileName( const bool canBeNull = false ) const = 0;
  virtual QString descriptiveName() const = 0;
  virtual const char* actionName() const = 0;
  virtual int shortcut() const = 0;
  virtual void act( KigPart& ) = 0;

  virtual void plug( KigPart* doc, KigGUIAction* kact );
};

class ObjectConstructor;

class ConstructibleAction
  : public GUIAction
{
  ObjectConstructor* mctor;
  QByteArray mactionname;
  int mshortcut;
public:
  ConstructibleAction( ObjectConstructor* ctor, const QByteArray& actionname,
                       int shortcut = 0 );
  ~ConstructibleAction();
  QString description() const Q_DECL_OVERRIDE;
  QByteArray iconFileName( const bool canBeNull = false ) const Q_DECL_OVERRIDE;
  QString descriptiveName() const Q_DECL_OVERRIDE;
  const char* actionName() const Q_DECL_OVERRIDE;
  int shortcut() const Q_DECL_OVERRIDE;
  void act( KigPart& ) Q_DECL_OVERRIDE;
  void plug( KigPart* doc, KigGUIAction* kact ) Q_DECL_OVERRIDE;
};

class ConstructPointAction
  : public GUIAction
{
  const char* mactionname;
public:
  explicit ConstructPointAction( const char* actionname );
  ~ConstructPointAction();

  QString description() const Q_DECL_OVERRIDE;
  QByteArray iconFileName( const bool canBeNull = false ) const Q_DECL_OVERRIDE;
  QString descriptiveName() const Q_DECL_OVERRIDE;
  const char* actionName() const Q_DECL_OVERRIDE;
  int shortcut() const Q_DECL_OVERRIDE;
  void act( KigPart& ) Q_DECL_OVERRIDE;
};

class ConstructTextLabelAction
  : public GUIAction
{
  const char* mactionname;
public:
  explicit ConstructTextLabelAction( const char* actionname );

  QString description() const Q_DECL_OVERRIDE;
  QByteArray iconFileName( const bool canBeNull = false ) const Q_DECL_OVERRIDE;
  QString descriptiveName() const Q_DECL_OVERRIDE;
  const char* actionName() const Q_DECL_OVERRIDE;
  int shortcut() const Q_DECL_OVERRIDE;
  void act( KigPart& ) Q_DECL_OVERRIDE;
};

class AddFixedPointAction
  : public GUIAction
{
  const char* mactionname;
public:
  explicit AddFixedPointAction( const char* actionname );
  ~AddFixedPointAction();
  QString description() const Q_DECL_OVERRIDE;
  QByteArray iconFileName( const bool canBeNull = false ) const Q_DECL_OVERRIDE;
  QString descriptiveName() const Q_DECL_OVERRIDE;
  const char* actionName() const Q_DECL_OVERRIDE;
  int shortcut() const Q_DECL_OVERRIDE;
  void act( KigPart& ) Q_DECL_OVERRIDE;
};

class ConstructNumericLabelAction
  : public GUIAction
{
  const char* mactionname;
public:
  explicit ConstructNumericLabelAction( const char* actionname );
  ~ConstructNumericLabelAction();
  QString description() const Q_DECL_OVERRIDE;
  QByteArray iconFileName( const bool canBeNull = false ) const Q_DECL_OVERRIDE;
  QString descriptiveName() const Q_DECL_OVERRIDE;
  const char* actionName() const Q_DECL_OVERRIDE;
  int shortcut() const Q_DECL_OVERRIDE;
  void act( KigPart& ) Q_DECL_OVERRIDE;
};

#if 0
class TestAction
  : public GUIAction
{
  const char* mactionname;
public:
  TestAction( const char* actionname );
  ~TestAction();
  QString description() const;
  QByteArray iconFileName( const bool canBeNull = false ) const;
  QString descriptiveName() const;
  const char* actionName() const;
  void act( KigPart& );
};
#endif

#ifdef KIG_ENABLE_PYTHON_SCRIPTING

class NewScriptAction
  : public GUIAction
{
  const char* mactionname;
  const char* mdescname;
  const char* mdescription;
  const char* micon;
  const ScriptType::Type mtype;
public:
  NewScriptAction( const char* descname, const char* description,
                   const char* actionname, const ScriptType::Type type,
                   const char* icon = "" );
  ~NewScriptAction();
  QString description() const override;
  QByteArray iconFileName( const bool canBeNull = false ) const override;
  QString descriptiveName() const override;
  const char* actionName() const override;
  void act( KigPart& ) override;
  int shortcut() const override;
};

#endif // KIG_ENABLE_PYTHON_SCRIPTING

#endif
