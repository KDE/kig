/** \mainpage Kig Python Scripting API Documentation
 * \section Introduction
 * This is the documentation of the Kig Python Scripting API.  It is
 * intended as a reference for people using the Kig Python Scripting
 * System.  The Documentation is generated from the C++ sources for
 * the classes, and unfortunately, this means that the documentation
 * uses a C++ syntax.  However, if you ignore this fact, and simply
 * look at the API as a set of classes with member and static
 * functions, then you should be fine.
 *
 * \section The API
 * This API is mostly meant to be used from user-defined calc()
 * functions.  In these functions, it is necessary to
 * \li access the information of the argument \ref ObjectImp's
 * \li construct and return a new ObjectImp
 *
 * Both uses require the ObjectImp API to be exported to Python, and
 * this is the major part of this API.  Some more classes are also
 * exported, but mostly because they are used from one of the
 * ObjectImp's APIs.
 *
 * \section Links
 *
 * Next suggested reading is the
 * \ref ObjectImp "documentation of the ObjectImp class".
 */
