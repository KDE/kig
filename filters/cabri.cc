#include "cabri.h"

KigFilterCabri::KigFilterCabri()
{
    
}

KigFilterCabri::~KigFilterCabri()
{
    
}

bool KigFilterCabri::supportMime( const QString mime )
{
  if (mime == QString::fromLatin1( "application/x-cabri" ) )
    return true;
  return false;
}

struct ObjectData
{
  Object* o;
  std::vector<int> p;
  int id;
  bool valid;
  operator bool() { return valid; };
};

ObjectData readObject( QFile& f )
{
  ObjectData n;
  n.valid = false;
  bool eof;
  QCString l = readLine( f, eof );
  // n.valid == false, so this is cool...
  if( eof ) return n;

  // first a number and a colon to indicate the id:
  int colonPos = l.find(':') 
  QCString idS = l.left( colonPos );
  bool ok = true;
  int id = idS.toInt( ok );
  if( ! ok ) return n;
  n.id = id;

  // we skip the colon, and the space after it...
  l = l.mid( colonPos + 2 );

  // next the object Type..
  int commaPos = l.find(',');
  QCString idT = l.left( commaPos );

  // TODO...
  n.valid = true;
  return n;  
};

KigFilter::Result KigFilterCabri::convert( const QString from, KTempFile& to)
{
  // we fill up objs with the objects from the file, and then save
  // those objects...
  std::vector<ObjectData> objs;
  
  QFile f( sFrom );
  f.open( IO_ReadOnly );

  // this code will look more like C than C++, but that's cause i
  // think parsing text is not something that C++ is good at...

  bool eof;

  QCString s = readLine( f, eof );
  QCString a = s.left( 21 );
  QCString b = s.mid( 22 );
  if( eof || ( a != "Figure CabriII vers. " ) || ( b != "DOS 1.0" && b != "MS-Windows 1.0" ) )
    return NotSupported;
  // next we have:
  // line 2: empty line
  // line 3: window dimensions -> we don't need/use that...
  // line 4: empty line
  // line 5 through 8: center point
  // line 9 through 12: axes
  // so we skip 11 lines...
  for( int i = 0; i != 11; ++i)
    (void) readLine( f, eof );

  // all Cabri files seem to at least have these center and axes...
  if( eof ) return NotSupported;

  // next, we get the objects we want...
  ObjectData o;
  // fetch the objects...
  while( ( o = readObject( f ) ) ) objs.push_back(o);
  // get the dependencies right...
  for( std::vector<ObjectData>::iterator i = objs.begin(); i != objs.end(); ++i )
    {
      for( std::vector<int>::iterator j = i->p.begin(); j != i->p.end(); ++j )
	{
	  objs[*i].o->addChild( j->o );
	};
    };

  // the output file...
  QFile* fTo = new QFile (to.name());
  fTo->open(IO_WriteOnly);

  // start building our file...
  QTextStream stream (fTo);

  // cf. KigDocument::saveFile()...
  QDomDocument doc ("KigDocument");
  QDomElement elem = doc.createElement ("KigDocument");
  elem.setAttribute( "Version", "2.0.000");

  Objects go;
  Objects fo;
  for (std::vector<Object*>::iterator i = objs.begin(); i != objs.end(); ++i)
    {
      fo.add(*i);
    };
  ObjectHierarchy hier ( go, fo, 0);
  hier.calc();
  hier.saveXML ( doc, elem );
  doc.appendChild(elem);

  stream << doc.toCString() << endl;
  kdDebug() << k_funcinfo << endl << doc.toCString() << endl;
  fTo->close();
  delete fTo;
  return OK;
}

std::string KigFilterCabri::readLine( QFile& f, bool& eof )
{
  std::string s;
  char r;
  while()
    {
      r = f.getch();
      if( r == 0 || r == -1 ) { eof = true; break; };
      if( r == '\n' ) { eof = false; break; }
      if( r == '\r' )
	{
	  r = f.getch();
	  if( r == '\n' ) { eof = false; break; }
	  f.ungetch( r );
	  r = '\r';
	};
      s += r;
    };
  s += '\n';
  return s;
}

