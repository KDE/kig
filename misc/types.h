#ifndef types_h
#define types_h

#include <qcstring.h>
#include <list>

#include "type.h"

class Object;

class Types
  : public list<Type*>
{
public:
  Types() {};
  Types( KigDocument* inDoc, const QString& file_name);
  ~Types() {};
  void deleteAll();
  void saveToDir(const QString& dir_name);
  void saveToFile(const QString& file_name);
  Type* findType(const QCString& type) const;
  Object* newObject( const QCString& type );
  void add ( Type* t ) { push_back( t ); };
};

#endif
