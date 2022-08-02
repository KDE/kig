// SPDX-FileCopyrightText: 2002 Dominique Devriese <devriese@kde.org>

// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

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
private slots:
    void slotActivated();
};

class GUIAction
{
public:
    virtual ~GUIAction();

    virtual QString description() const = 0;
    virtual QString iconFileName(const bool canBeNull = false) const = 0;
    virtual QString descriptiveName() const = 0;
    virtual const char *actionName() const = 0;
    virtual int shortcut() const = 0;
    virtual void act(KigPart &) = 0;

    virtual void plug(KigPart *doc, KigGUIAction *kact);
};

class ObjectConstructor;

class ConstructibleAction : public GUIAction
{
    ObjectConstructor *mctor;
    QByteArray mactionname;
    int mshortcut;

public:
    ConstructibleAction(ObjectConstructor *ctor, const QByteArray &actionname, int shortcut = 0);
    ~ConstructibleAction();
    QString description() const override;
    QString iconFileName(const bool canBeNull = false) const override;
    QString descriptiveName() const override;
    const char *actionName() const override;
    int shortcut() const override;
    void act(KigPart &) override;
    void plug(KigPart *doc, KigGUIAction *kact) override;
};

class ConstructPointAction : public GUIAction
{
    const char *mactionname;

public:
    explicit ConstructPointAction(const char *actionname);
    ~ConstructPointAction();

    QString description() const override;
    QString iconFileName(const bool canBeNull = false) const override;
    QString descriptiveName() const override;
    const char *actionName() const override;
    int shortcut() const override;
    void act(KigPart &) override;
};

class ConstructTextLabelAction : public GUIAction
{
    const char *mactionname;

public:
    explicit ConstructTextLabelAction(const char *actionname);

    QString description() const override;
    QString iconFileName(const bool canBeNull = false) const override;
    QString descriptiveName() const override;
    const char *actionName() const override;
    int shortcut() const override;
    void act(KigPart &) override;
};

class AddFixedPointAction : public GUIAction
{
    const char *mactionname;

public:
    explicit AddFixedPointAction(const char *actionname);
    ~AddFixedPointAction();
    QString description() const override;
    QString iconFileName(const bool canBeNull = false) const override;
    QString descriptiveName() const override;
    const char *actionName() const override;
    int shortcut() const override;
    void act(KigPart &) override;
};

class ConstructNumericLabelAction : public GUIAction
{
    const char *mactionname;

public:
    explicit ConstructNumericLabelAction(const char *actionname);
    ~ConstructNumericLabelAction();
    QString description() const override;
    QString iconFileName(const bool canBeNull = false) const override;
    QString descriptiveName() const override;
    const char *actionName() const override;
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
    const char *mactionname;
    const char *mdescname;
    const char *mdescription;
    const char *micon;
    const ScriptType::Type mtype;

public:
    NewScriptAction(const char *descname, const char *description, const char *actionname, const ScriptType::Type type, const char *icon = "");
    ~NewScriptAction();
    QString description() const override;
    QString iconFileName(const bool canBeNull = false) const override;
    QString descriptiveName() const override;
    const char *actionName() const override;
    void act(KigPart &) override;
    int shortcut() const override;
};

#endif // KIG_ENABLE_PYTHON_SCRIPTING
