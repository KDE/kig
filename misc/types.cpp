#include "types.h"

#include <kdebug.h>
#include <kmessagebox.h>

#include <qfile.h>
#include <qtextstream.h>

Object* Types::newObject( const QCString& type )
{
  Type* theType = findType(type);
  if (theType) return theType->newObject();
  else return 0;
}

Type* Types::findType(const QCString& type) const
{
  for ( const_iterator i = begin(); i != end(); ++i )
    if ( type == ( *i )->fullTypeName() )
      return ( *i );
  kdError() << "Types::newObject: no TypeRepr found for typename \"" << type << "\"." << endl;
  return 0;
}
void Types::deleteAll()
{
  for ( iterator i = begin(); i != end();++i ) delete *i;
}

void Types::saveToFile(const QString& file_name)
{
  QFile file(file_name);
  if (!file.open(IO_WriteOnly)) {
    KMessageBox::sorry
      (0,
       i18n("Couldn't open file %1 for writing...").arg(file_name),
       i18n("Could not save file"));
    return;
  };
  QTextStream stream(&file);
  QDomDocument doc ("KigMacroFile");
  QDomElement el = doc.createElement("Types");
  for (iterator i = begin(); i != end(); ++i) {
    MTypeOne* appel = (*i)->toMTypeOne();
    if (appel) appel->saveXML(doc, el);
  };
  doc.appendChild(el);
  stream << doc.toCString();
  file.close();
}

Types::Types( KigDocument* doc, const QString& file_name)
{
  QFile file(file_name);
  if (!file.open(IO_ReadOnly)) {
    KMessageBox::sorry
      (0,
       i18n("Couldn't open file %1 for reading...").arg(file_name),
       i18n("Could not open file"));
    return;
  };
  QDomDocument qdoc;
  qdoc.setContent(&file);
  QDomElement el = qdoc.documentElement();
  for (QDomNode n = el.firstChild(); !n.isNull(); n = n.nextSibling())
    {
      QDomElement e = n.toElement();
      if (e.isNull()) continue;
      if (e.tagName() != "MTypeOne") continue;
      add(new MTypeOne ( e, doc ) );
    };
}
