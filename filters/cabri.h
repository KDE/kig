/*
NOTE:
This is completely free code, i have not looked at any Cabri source
code, and have implemented this implementation of the Cabri file
format from zero, by just looking at the input and output from a
(properly licensed) Cabri program... I have the right to do this,
because of the democratic right we all have to reverse-engineer any
lawfully owned product...
*/

#ifndef CABRI_H
#define CABRI_H

//  This is an import filter for the output of the commercial program
//  Cabri ("CAhier de BRouillon Interactif" or something like that),
//  which is being pushed by Texas Instruments, but only exists for
//  the Winblows platform and some TI scientific calculator...

// THIS IS PRE-ALPHA CODE

class KigFilterCabri
  : public KigFilter
{
public:
  KigFilterCabri();
  ~KigFilterCabri();
  virtual bool supportMime ( const QString mime );
  virtual Result convert ( const QString from, KTempFile& to);
};

#endif
