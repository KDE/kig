// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "objects/common.h"
#include <config-kig.h>

#ifdef KIG_ENABLE_PYTHON_SCRIPTING
#include "../scripting/script-common.h"
#endif

#include <QAction>
#include <QByteArray>
#include <QString>

class GUIAction;
class KigPart;

class KigGUIAction : public QAction
{
    Q_OBJECT
    GUIAction *mact;
    KigPart &mdoc;

public:
    KigGUIAction(GUIAction *act, KigPart &doc);

    GUIAction *guiAction();

    void plug(KigPart *doc);
private Q_SLOTS:
    void slotActivated();
};

class GUIAction
{
public:
    virtual ~GUIAction();

    virtual QString description() const = 0;
    virtual QString iconFileName(const bool canBeNull = false) const = 0;
    virtual QString descriptiveName() const = 0;
    virtual QString actionName() const = 0;
    virtual int shortcut() const = 0;
    virtual void act(KigPart &) = 0;

    virtual void plug(KigPart *doc, KigGUIAction *kact);
};

class ObjectConstructor;

class ConstructibleAction : public GUIAction
{
    ObjectConstructor *mctor;
    QString mactionname;
    int mshortcut;

public:
    ConstructibleAction(ObjectConstructor *ctor, const QString &actionname, int shortcut = 0);
    ~ConstructibleAction();
    QString description() const override;
    QString iconFileName(const bool canBeNull = false) const override;
    QString descriptiveName() const override;
    QString actionName() const override;
    int shortcut() const override;
    void act(KigPart &) override;
    void plug(KigPart *doc, KigGUIAction *kact) override;
};

class ConstructPointAction : public GUIAction
{
    QString mactionname;

public:
    explicit ConstructPointAction(const QString &actionname);
    ~ConstructPointAction();

    QString description() const override;
    QString iconFileName(const bool canBeNull = false) const override;
    QString descriptiveName() const override;
    QString actionName() const override;
    int shortcut() const override;
    void act(KigPart &) override;
};

class ConstructTextLabelAction : public GUIAction
{
    QString mactionname;

public:
    explicit ConstructTextLabelAction(const QString &actionname);

    QString description() const override;
    QString iconFileName(const bool canBeNull = false) const override;
    QString descriptiveName() const override;
    QString actionName() const override;
    int shortcut() const override;
    void act(KigPart &) override;
};

class AddFixedPointAction : public GUIAction
{
    QString mactionname;

public:
    explicit AddFixedPointAction(const QString &actionname);
    ~AddFixedPointAction();
    QString description() const override;
    QString iconFileName(const bool canBeNull = false) const override;
    QString descriptiveName() const override;
    QString actionName() const override;
    int shortcut() const override;
    void act(KigPart &) override;
};

class ConstructNumericLabelAction : public GUIAction
{
    QString mactionname;

public:
    explicit ConstructNumericLabelAction(const QString &actionname);
    ~ConstructNumericLabelAction();
    QString description() const override;
    QString iconFileName(const bool canBeNull = false) const override;
    QString descriptiveName() const override;
    QString actionName() const override;
    int shortcut() const override;
    void act(KigPart &) override;
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
  QString iconFileName( const bool canBeNull = false ) const;
  QString descriptiveName() const;
  const char* actionName() const;
  void act( KigPart& );
};
#endif

#ifdef KIG_ENABLE_PYTHON_SCRIPTING

class NewScriptAction : public GUIAction
{
    QString mactionname;
    QString mdescname;
    QString mdescription;
    QString micon;
    const ScriptType::Type mtype;

public:
    NewScriptAction(const QString &descname,
                    const QString &description,
                    const QString &actionname,
                    const ScriptType::Type type,
                    const QString &icon = "");
    ~NewScriptAction();
    QString description() const override;
    QString iconFileName(const bool canBeNull = false) const override;
    QString descriptiveName() const override;
    QString actionName() const override;
    void act(KigPart &) override;
    int shortcut() const override;
};

#endif // KIG_ENABLE_PYTHON_SCRIPTING
