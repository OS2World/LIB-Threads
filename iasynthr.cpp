/*******************************************************************************
* FILE NAME: iasynthr.cpp
*
* DESCRIPTION:
*   Functions to implement the class(es):
*     IAsyncNotifierThread
*
* COPYRIGHT:
*   Licensed Materials - Property of IBM
*   (C) Copyright IBM Corporation 1995
*   All Rights Reserved
*   US Government Users Restricted Rights - Use, duplication, or disclosure
*   restricted by GSA ADP Schedule Contract with IBM Corp.
*
*******************************************************************************/
#include <iasynthr.hpp>

#ifndef _ITHREAD_
  #include <ithread.hpp>
#endif

#ifndef _IASYNTFY_
  #include <iasyntfy.hpp>
#endif

#ifndef _IASYNGUI_
  #include <iasyngui.hpp>
#endif

#ifndef _IASYNBKG_
  #include <iasynbkg.hpp>
#endif

#ifndef _IEXCEPT_
  #include <iexcept.hpp>
#endif

INotificationId const IAsyncNotifierThread::deleteThisId
                        = "IAsyncNotifierThread::deleteThis";


/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierThread :: IAsyncNotifierThread
|
| Implementation:
|   Initialize the base class then find our thread id.
|-----------------------------------------------------------------------------*/
IAsyncNotifierThread :: IAsyncNotifierThread ( ) :
                   IVBase ( ),
                   asyncNotifierCount ( 0 ),
                   theThreadId ( IThread::currentId() ),
                   bRunning ( false )
{
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierThread :: make
|
| Implementation:
|   Get the current thread and see if it is a GUI thread or not.
|   Create the appropriate subclass object and return it.
|-----------------------------------------------------------------------------*/
IAsyncNotifierThread * IAsyncNotifierThread :: make ( IBoolean guiOnly )
{
  IAsyncNotifierThread * result = NULL;

  IBoolean isGUI = IThread::current().isGUIInitialized();

  IASSERTSTATE ( ( isGUI ) || ( !guiOnly ) );

  if ( isGUI )
    result = new IAsyncNotifierGUIThread;
  else
    result = new IAsyncNotifierBackgroundThread;

  return result;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierThread :: ~IAsyncNotifierThread
|
| Implementation:
|   Nothing to delete.
|-----------------------------------------------------------------------------*/
IAsyncNotifierThread :: ~IAsyncNotifierThread ( )
{
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierThread :: threadId
|
| Implementation:
|   Return the thread id.
|-----------------------------------------------------------------------------*/
const IThreadId & IAsyncNotifierThread :: threadId ( ) const
{
  return theThreadId;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierThread :: addRef
|
| Implementation:
|   Bump and return the reference count.
|-----------------------------------------------------------------------------*/
unsigned long IAsyncNotifierThread :: addRef ( )
{
  asyncNotifierCount++;
  return asyncNotifierCount;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierThread :: removeRef
|
| Implementation:
|   Decrement and return the reference count.
|-----------------------------------------------------------------------------*/
unsigned long IAsyncNotifierThread :: removeRef ( )
{
  asyncNotifierCount--;
  return asyncNotifierCount;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierThread :: refCount
|
| Implementation:
|   Return the current reference count.
|-----------------------------------------------------------------------------*/
unsigned long IAsyncNotifierThread :: refCount ( ) const
{
  return asyncNotifierCount;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierThread :: isRunning
|
| Implementation:
|   Return the running flag.
|-----------------------------------------------------------------------------*/
IBoolean IAsyncNotifierThread :: isRunning ( ) const
{
  return bRunning;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierThread :: setIsRunning
|
| Implementation:
|   Set the running flag.
|-----------------------------------------------------------------------------*/
IAsyncNotifierThread & IAsyncNotifierThread :: setIsRunning ( IBoolean running )
{
  bRunning = running;
  return *this;
}

/*------------------------------------------------------------------------------
| Function Name: key
|
| Implementation:
|   Return the thread id for the referenced object.
|-----------------------------------------------------------------------------*/
const IThreadId & key ( IAsyncNotifierThread * const & object )
{
  return ( object->threadId() );
}
