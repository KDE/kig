// macro.cc
// Copyright (C)  2002  Dominique Devriese <fritmebufstek@pandora.be>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

// #include "macro.h"
// #include "macro.moc"

// #include "macrowizard.h"
// #include "../kig/kig_part.h"

// #include <klineedit.h>

// DefineMacroMode::DefineMacroMode( KigDocument* d, KigMode* p )
//   : KigMode( d ), mPrev( p ), cur( &mGiven )
// {
//   mWizard = new MacroWizard( d->widget(), "macro wizard" );
//   mWizard->show();
// }

// // void DefineMacroMode::rejected()
// // {
// //   KigMode* m = mPrev;
// //   KigDocument* d = mDoc;
// //   delete this;
// //   d->setMode( m );
// // }

// // DefineMacroMode::~DefineMacroMode()
// // {
// //   delete mWizard;
// // }

// // void DefineMacroMode::updateNexts()
// // {
// //   mWizard->setNextEnabled( mWizard->mpgiven,
// //                            !mGiven.empty() );
// //   mWizard->setNextEnabled( mWizard->mpfinal,
// //                            !mFinal.empty() );
// //   mWizard->setFinishEnabled( mWizard->mpname,
// //                              !mWizard->KLineEdit1->text().isEmpty()
// //     );
// // }

// // void DefineMacroMode::enteredGosPage()
// // {
// //   cur = &mGiven;
// // }

// // void DefineMacroMode::enteredFosPage()
// // {
// //   cur = &mFinal;
// // }
