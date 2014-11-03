/*******************************************************************************
* FILE NAME: iasynbkg.cpp
*
* DESCRIPTION:
*   Functions to implement the class(es):
*     IAsyncNotifierBackgroundThread
*
* COPYRIGHT:
*   Licensed Materials - Property of IBM
*   (C) Copyright IBM Corporation 1995
*   All Rights Reserved
*   US Government Users Restricted Rights - Use, duplication, or disclosure
*   restricted by GSA ADP Schedule Contract with IBM Corp.
*
*******************************************************************************/
#include <iasynbkg.hpp>

#ifndef _IASYNTFY_
  #include <iasyntfy.hpp>
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

#ifndef _IEXCEPT_
  #include <iexcept.hpp>
#endif

#ifndef _ISEQ_H
  #include <iseq.h>
#endif


/*------------------------------------------------------------------------------
| Function Name: removeFor
|
| Implementation:
|   Used by IAsyncNotifierBackgroundThread::deleteNotificationsFor as a
|   parameter to ISequence<INotificationEvent>::removeAll.
|-----------------------------------------------------------------------------*/
IBoolean removeFor ( const INotificationEvent & anEvent, void * asyncNotifier )
{
  IBoolean remove = false;

  if ( (&(anEvent.notifier())) == ((INotifier *)asyncNotifier) )
  {
    remove = true;
    ((const IAsyncNotifier *)asyncNotifier)->notificationCleanUp ( anEvent );
  }

  return remove;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierBackgroundThread :: IAsyncNotifierBackgroundThread
|
| Implementation:
|   Initialize the base class and create our queue.
|-----------------------------------------------------------------------------*/
IAsyncNotifierBackgroundThread :: IAsyncNotifierBackgroundThread ( ) :
                   IAsyncNotifierThread ( ),
                   queue ( new ISequence<INotificationEvent> ),
                   queueKey ( ),
                   queueEventSem ( )
{
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierBackgroundThread :: ~IAsyncNotifierBackgroundThread
|
| Implementation:
|   Delete the queue.
|-----------------------------------------------------------------------------*/
IAsyncNotifierBackgroundThread :: ~IAsyncNotifierBackgroundThread ( )
{
  delete queue;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierBackgroundThread :: removeRef
|
| Implementation:
|   If the count is now zero and the queue is empty let processMsgs know it
|     is time to exit.
|-----------------------------------------------------------------------------*/
unsigned long IAsyncNotifierBackgroundThread :: removeRef ( )
{
  IResourceLock queueLock ( queueKey );

  unsigned long count = IAsyncNotifierThread::removeRef();
  if ( ( count == 0 ) && ( queue->isEmpty() ) )
    queueEventSem.post();

  return count;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierBackgroundThread :: enqueueNotification
|
| Implementation:
|   Enqueue the notification.  The queue will make a copy of the event.
|   If the dispatch thread may be waiting, post the semaphore.
|-----------------------------------------------------------------------------*/
IAsyncNotifierBackgroundThread & IAsyncNotifierBackgroundThread
                                   :: enqueueNotification (
                                        const INotificationEvent & anEvent )
{
  IResourceLock queueLock ( queueKey );

  queue->addAsLast ( anEvent );

  if ( queue->numberOfElements() == 1 )
    queueEventSem.post();

  return *this;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierBackgroundThread :: processMsgs
|
| Implementation:
|   Lock the queue
|   While there are async notifiers on this thread:
|     If queue is empty:
|       Reset event sem
|       Unlock the queue
|       Wait on the event sem
|       Lock the queue
|     If the queue is not empty
|       Dequeue the next event
|       Unlock the queue
|       Check for deleteThisId and notify observers
|       Lock the queue
|   Unlock the queue
|-----------------------------------------------------------------------------*/
IAsyncNotifierBackgroundThread & IAsyncNotifierBackgroundThread
                                   :: processMsgs ( )
{
  IASSERTSTATE ( threadId() == IThread::currentId() );

  setIsRunning ( true );

  // Lock the queue
  queueKey.lock();
  IBoolean lockedHere = true;

  try
  {
    // While there are async notifiers on this thread:
    while ( refCount() != 0 )
    {
      // If queue is empty:
      if ( queue->isEmpty() )
      {
        // Reset event sem
        queueEventSem.reset();

        // Unlock the queue
        queueKey.unlock();
        lockedHere = false;

        // Wait on the event sem
        queueEventSem.wait();

        // Lock the queue
        queueKey.lock();
        lockedHere = true;
      }

      // If the queue is not empty
      if ( ! (queue->isEmpty()) )
      {
        // Dequeue the next event
        INotificationEvent nextEvent ( queue->firstElement() );
        queue->removeFirst();

        // Unlock the queue
        queueKey.unlock();
        lockedHere = false;

        // Check for deleteThisId and notify observers
        IAsyncNotifier * theNotifier
                           = (IAsyncNotifier *)(&(nextEvent.notifier()));
        if ( nextEvent.notificationId() == deleteThisId )
        {
          delete theNotifier;
        }
        else
        {
          if ( theNotifier->isEnabledForNotification() )
            theNotifier->IStandardNotifier::notifyObservers ( nextEvent );
          theNotifier->notificationCleanUp ( nextEvent );
        }

        // Lock the queue
        queueKey.lock();
        lockedHere = true;
      }
    }
  }
  catch ( IException & exc )
  {
    // Unlock the queue and rethrow.
    if ( lockedHere )
      queueKey.unlock();
    throw;
  }

  // Unlock the queue
  queueKey.unlock();

  setIsRunning ( false );

  return *this;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierBackgroundThread :: deleteNotificationsFor
|
| Implementation:
|   Remove all pending notifications for the passed async notifier.
|-----------------------------------------------------------------------------*/
IAsyncNotifierBackgroundThread & IAsyncNotifierBackgroundThread
                                   :: deleteNotificationsFor (
                                        const IAsyncNotifier & asyncNotifier )
{
  IASSERTSTATE ( threadId() == IThread::currentId() );

  IResourceLock queueLock ( queueKey );

  queue->removeAll ( removeFor, (void *)(&asyncNotifier) );

  return *this;
}
