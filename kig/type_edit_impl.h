#ifndef KIGTYPEEDITIMPL_H
#define KIGTYPEEDITIMPL_H

#include "type_edit.h"

#include "../misc/type.h"

#include <klistbox.h>

class TypeListElement
  : public QListBoxText
{
  Type* type;
public:
  TypeListElement(Type* inType) : QListBoxText(inType->getActionName()), type(inType) {};
  Type* getType() { return type; };
};

class KigDocument;
class Types;

class KigTypeEditImpl : public TypeEditDialogUI
{ 
  Q_OBJECT

  KigDocument* doc;
  
  Types* types;

public:
    KigTypeEditImpl( KigDocument* inDoc );
    ~KigTypeEditImpl();

public slots:
    void helpSlot();
    void okSlot();

protected slots:
    void deleteType();
    void exportType();
    void importTypes();

};

#endif // KIGTYPEEDITIMPL_H
