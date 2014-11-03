/*******************************************************************************
* FILE NAME: iasyntfy.cpp
*
* DESCRIPTION:
*   Functions to implement the class(es):
*     IAsyncNotifier
*
* COPYRIGHT:
*   Licensed Materials - Property of IBM
*   (C) Copyright IBM Corporation 1995
*   All Rights Reserved
*   US Government Users Restricted Rights - Use, duplication, or disclosure
*   restricted by GSA ADP Schedule Contract with IBM Corp.
*
*******************************************************************************/
#include <iasyntfy.hpp>

#ifndef _IASYNTHR_
  #include <iasynthr.hpp>
#endif

#ifndef _INOTIFEV_
  #include <inotifev.hpp>
#endif

#ifndef _ITHREAD_
  #include <ithread.hpp>
#endif

#ifndef _IEVNTSEM_
  #include <ievntsem.hpp>
#endif

#ifndef _IKEYSET_H
  #include <ikeyset.h>
#endif

// Define the functions and static data members to be exported.
// Ordinals 200 through 249 are reserved for use by IAsyncNotifier.
#pragma export(IAsyncNotifier::IAsyncNotifier(),, 200)
#pragma export(IAsyncNotifier::IAsyncNotifier(const IAsyncNotifier&),, 201)
#pragma export(IAsyncNotifier::~IAsyncNotifier(),, 202)
#pragma export(IAsyncNotifier::deleteThis(),, 203)
#pragma export(IAsyncNotifier::operator=(const IAsyncNotifier&),, 204)
#pragma export(IAsyncNotifier::run(),, 205)
#pragma export(IAsyncNotifier::notifyObservers(const INotificationEvent&),, 206)
#pragma export(IAsyncNotifier::dispatchThread() const,, 207)
#pragma export(IAsyncNotifier::notificationCleanUp(                    \
                 const INotificationEvent&) const,, 208)
#pragma export(IAsyncNotifier::notifyObservers(const INotificationId&),, 209)
#pragma export(IAsyncNotifier::dispatchThreadId,, 210)
// *********** TEMPORARY *************
#pragma export(IAsyncNotifier::thisRefId,, 211)

// It's possible for the caller of the external entry points to have a
// different C library environment.  Make sure the exception handler for
// our library environment is registered on entry and deregistered on exit.
#pragma handler(IAsyncNotifier::IAsyncNotifier())
#pragma handler(IAsyncNotifier::IAsyncNotifier(const IAsyncNotifier&))
#pragma handler(IAsyncNotifier::~IAsyncNotifier())
#pragma handler(IAsyncNotifier::deleteThis())
#pragma handler(IAsyncNotifier::operator=(const IAsyncNotifier&))
#pragma handler(IAsyncNotifier::run())
#pragma handler(IAsyncNotifier::notifyObservers(const INotificationEvent&))
#pragma handler(IAsyncNotifier::dispatchThread() const)
#pragma handler(IAsyncNotifier::notificationCleanUp(                   \
                  const INotificationEvent&) const)
#pragma handler(IAsyncNotifier::notifyObservers(const INotificationId&))

// Initialize class static members.
IKeySet<IAsyncNotifierThread *, IThreadId> * IAsyncNotifier::threads
                               = new IKeySet<IAsyncNotifierThread *, IThreadId>;
IPrivateResource IAsyncNotifier::threadsKey;
INotificationId const IAsyncNotifier::dispatchThreadId
                                        = "IAsyncNotifier::dispatchThread";
// *********** TEMPORARY *************
INotificationId const IAsyncNotifier::thisRefId = "IAsyncNotifier::thisRef";


/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifier :: IAsyncNotifier
|
| Implementation:
|   Initialize the base class then find or create the dispatch thread.
|-----------------------------------------------------------------------------*/
IAsyncNotifier :: IAsyncNotifier ( ) :
                   IStandardNotifier ( ),
                   theDispatchThread ( NULL )
{
  findOrCreateDispatchThread();
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifier :: IAsyncNotifier
|
| Implementation:
|   Initialize the base class then find or create the dispatch thread.
|   Use the base class default constructor so notification will always be
|   disabled for a new object.
|-----------------------------------------------------------------------------*/
IAsyncNotifier :: IAsyncNotifier ( const IAsyncNotifier & asyncNotifier ) :
                   IStandardNotifier ( ),
                   theDispatchThread ( NULL )
{
  findOrCreateDispatchThread();
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifier :: ~IAsyncNotifier
|
| Implementation:
|   Delete all pending notifications for this object.
|   Remove our reference to the thread.
|   If the reference count is zero, remove the thread from the collection.
|     If it is not running, delete it.  If it is running,
|     IAsyncNotifier::run will delete it.
|-----------------------------------------------------------------------------*/
IAsyncNotifier :: ~IAsyncNotifier ( )
{
  theDispatchThread->deleteNotificationsFor ( *this );
  if ( theDispatchThread->removeRef() == 0 )
  {
    threads->removeElementWithKey ( theDispatchThread->threadId() );

    if ( ! ( theDispatchThread->isRunning() ) )
      delete theDispatchThread;
  }
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifier :: deleteThis
|
| Implementation:
|   Post our secret notification for async delete.
|-----------------------------------------------------------------------------*/
IAsyncNotifier & IAsyncNotifier :: deleteThis ( )
{
  theDispatchThread->enqueueNotification ( INotificationEvent (
                                             IAsyncNotifierThread::deleteThisId,
                                             *this ) );
  return *this;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifier :: operator =
|
| Implementation:
|   Currently, there is nothing to do here.
|   Do not change the dispatch thread.
|   Do not call IStandardNotifier::operator=.  It kills notification.
|-----------------------------------------------------------------------------*/
IAsyncNotifier & IAsyncNotifier :: operator = ( const IAsyncNotifier & rhs )
{
  return *this;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifier :: run
|
| Implementation:
|   Find the current thread in the list of threads.
|   If not found try to create one, but only for GUI.
|   Call the thread's run function.
|   Remove the thread from the collection and delete it.
|-----------------------------------------------------------------------------*/
void IAsyncNotifier :: run ( )
{
  IAsyncNotifierThread * anAsyncNotifierThread = NULL;
  IThreadId threadId = IThread::currentId();

  {
    IResourceLock threadsLock ( threadsKey );

    if ( threads->containsElementWithKey ( threadId ) )
    {
      anAsyncNotifierThread = threads->elementWithKey ( threadId );
    }
    else
    {
      anAsyncNotifierThread = IAsyncNotifierThread::make ( true );
      threads->add ( anAsyncNotifierThread );
    }
  }

  anAsyncNotifierThread->processMsgs();

  threads->removeElementWithKey ( threadId );
  delete anAsyncNotifierThread;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifier :: notifyObservers
|
| Implementation:
|   If enabled for notification, enqueue the event.
|-----------------------------------------------------------------------------*/
IAsyncNotifier & IAsyncNotifier :: notifyObservers (
                                     const INotificationEvent & anEvent )
{
  if ( isEnabledForNotification() )
    theDispatchThread->enqueueNotification ( anEvent );

  return *this;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifier :: dispatchThread
|
| Implementation:
|   Return the dispatch thread for this object.
|-----------------------------------------------------------------------------*/
const IThreadId & IAsyncNotifier :: dispatchThread ( ) const
{
  return ( theDispatchThread->threadId() );
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifier :: notificationCleanUp
|
| Implementation:
|   This default implementation does nothing.
|-----------------------------------------------------------------------------*/
const IAsyncNotifier & IAsyncNotifier :: notificationCleanUp (
                         const INotificationEvent & anEvent ) const
{
  return *this;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifier :: notifyObservers
|
| Implementation:
|   If enabled for notification, create an event and enqueue.
|-----------------------------------------------------------------------------*/
IAsyncNotifier & IAsyncNotifier :: notifyObservers (
                                     const INotificationId & nId )
{
  if ( isEnabledForNotification() )
  {
    INotificationEvent anEvent ( nId, *this );
    theDispatchThread->enqueueNotification ( anEvent );
  }

  return *this;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifier :: findOrCreateDispatchThread
|
| Implementation:
|   Find or create the dispatch thread.
|-----------------------------------------------------------------------------*/
IAsyncNotifier & IAsyncNotifier :: findOrCreateDispatchThread ( )
{
  IThreadId threadId = IThread::currentId();

  IResourceLock threadsLock ( threadsKey );

  if ( threads->containsElementWithKey ( threadId ) )
  {
    theDispatchThread = threads->elementWithKey ( threadId );
  }
  else
  {
    theDispatchThread = IAsyncNotifierThread::make();
    threads->add ( theDispatchThread );
  }

  theDispatchThread->addRef();

  return *this;
}
