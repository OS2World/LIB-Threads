/*******************************************************************************
* FILE NAME: iasyngui.cpp
*
* DESCRIPTION:
*   Functions to implement the class(es):
*     IAsyncNotifierGUIThread
*
* COPYRIGHT:
*   Licensed Materials - Property of IBM
*   (C) Copyright IBM Corporation 1995
*   All Rights Reserved
*   US Government Users Restricted Rights - Use, duplication, or disclosure
*   restricted by GSA ADP Schedule Contract with IBM Corp.
*
*******************************************************************************/
#include <iasyngui.hpp>

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

#ifndef _IHANDLER_
  #include <ihandler.hpp>
#endif

#ifndef _IOBJWIN_
  #include <iobjwin.hpp>
#endif

#ifndef _ISEQ_H
  #include <iseq.h>
#endif

#define INCL_WINMESSAGEMGR
#include <os2.h>


//------------------------------------------------------------------------------
// Declare the event notification handler for the object window.
//------------------------------------------------------------------------------
class IAsyncNotificationHandler : public IHandler
{
public:
  IAsyncNotificationHandler ( );
  virtual ~IAsyncNotificationHandler ( );

  virtual IBoolean dispatchHandlerEvent ( IEvent & event );

private:
  // Private copy constructor and assignment operator are not implemented.
  IAsyncNotificationHandler ( const IAsyncNotificationHandler & );
  IAsyncNotificationHandler & operator = ( const IAsyncNotificationHandler & );
};


/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierGUIThread :: IAsyncNotifierGUIThread
|
| Implementation:
|   Initialize the base class then create our object window and handler.
|-----------------------------------------------------------------------------*/
IAsyncNotifierGUIThread :: IAsyncNotifierGUIThread ( ) :
                   IAsyncNotifierThread ( ),
                   objectWindow ( new IObjectWindow ),
                   asyncNotificationHandler ( new IAsyncNotificationHandler ),
                   objectWindowKey ( )
{
  objectWindow->setAutoDeleteObject ( true );
  asyncNotificationHandler->handleEventsFor ( objectWindow );
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierGUIThread :: ~IAsyncNotifierGUIThread
|
| Implementation:
|   If the object window is still around, close the object window (it is
|     auto deleted).
|   Delete the handler.
|-----------------------------------------------------------------------------*/
IAsyncNotifierGUIThread :: ~IAsyncNotifierGUIThread ( )
{
  if ( objectWindow != NULL )
  {
    asyncNotificationHandler->stopHandlingEventsFor ( objectWindow );
    objectWindow->close();
  }

  delete asyncNotificationHandler;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierGUIThread :: removeRef
|
| Implementation:
|   Decrement the async notifier count.
|   If the count is now zero, close the object window (it is auto deleted).
|     Make sure the destructor does not try to clean this up by clearing the
|     object window pointer.
|-----------------------------------------------------------------------------*/
unsigned long IAsyncNotifierGUIThread :: removeRef ( )
{
  unsigned long count = IAsyncNotifierThread::removeRef();
  if ( count == 0 )
  {
    asyncNotificationHandler->stopHandlingEventsFor ( objectWindow );
    objectWindow->close();
    objectWindow = NULL;
  }

  return count;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierGUIThread :: enqueueNotification
|
| Implementation:
|   Copy and enqueue the notification.
|-----------------------------------------------------------------------------*/
IAsyncNotifierGUIThread & IAsyncNotifierGUIThread :: enqueueNotification (
                            const INotificationEvent & anEvent )
{
  IResourceLock objectWindowLock ( objectWindowKey );

  INotificationEvent * copiedEvent = new INotificationEvent ( anEvent );
  objectWindow->postEvent ( WM_USER, IEventData ( copiedEvent ) );

  return *this;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierGUIThread :: processMsgs
|
| Implementation:
|   Pass the call on to ICurrentThread.
|-----------------------------------------------------------------------------*/
IAsyncNotifierGUIThread & IAsyncNotifierGUIThread :: processMsgs ( )
{
  IASSERTSTATE ( threadId() == IThread::currentId() );

  setIsRunning ( true );
  IThread::current().processMsgs();
  setIsRunning ( false );

  return *this;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotifierGUIThread :: deleteNotificationsFor
|
| Implementation:
|   Peek at all notifcation events coming to our object window.
|   If it is for the passed async notifier, delete it.
|   Else, save it and requeue it at the end.
|-----------------------------------------------------------------------------*/
IAsyncNotifierGUIThread & IAsyncNotifierGUIThread
                                   :: deleteNotificationsFor (
                                        const IAsyncNotifier & asyncNotifier )
{
  IASSERTSTATE ( threadId() == IThread::currentId() );

  IResourceLock objectWindowLock ( objectWindowKey );

  ISequence<INotificationEvent *> savedEvents;

  HAB hab = IThread::current().anchorBlock();
  QMSG msg;
  HWND handle = objectWindow->handle();

  INotificationEvent * nextEvent = NULL;
  const IAsyncNotifier * nextNotifier = NULL;

  IBoolean msgFound = true;
  while ( msgFound )
  {
    msgFound = WinPeekMsg ( hab, &msg, handle, WM_USER, WM_USER, PM_REMOVE );

    if ( msgFound )
    {
      nextEvent = (INotificationEvent *)(msg.mp1);
      nextNotifier = (const IAsyncNotifier *)(&(nextEvent->notifier()));
      if ( nextNotifier == &asyncNotifier )
      {
        nextNotifier->notificationCleanUp ( *nextEvent );
        delete nextEvent;
      }
      else
      {
        savedEvents.addAsLast ( nextEvent );
      }
    }
  }

  while ( ! ( savedEvents.isEmpty() ) )
  {
    nextEvent = savedEvents.firstElement();
    savedEvents.removeFirst();
    objectWindow->postEvent ( WM_USER, IEventData ( nextEvent ) );
  }

  return *this;
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotificationHandler :: IAsyncNotificationHandler
|
| Implementation:
|   Initialize the base class.
|-----------------------------------------------------------------------------*/
IAsyncNotificationHandler :: IAsyncNotificationHandler ( ) :
                   IHandler ( )
{
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotificationHandler :: ~IAsyncNotificationHandler
|
| Implementation:
|   Nothing to delete.
|-----------------------------------------------------------------------------*/
IAsyncNotificationHandler :: ~IAsyncNotificationHandler ( )
{
}

/*------------------------------------------------------------------------------
| Function Name: IAsyncNotificationHandler :: dispatchHandlerEvent
|
| Implementation:
|   If the event is one of our notifications, notify the observers of the
|   notifier on this thread and delete the event.
|-----------------------------------------------------------------------------*/
IBoolean IAsyncNotificationHandler :: dispatchHandlerEvent ( IEvent & event )
{
  IBoolean handledEvent = false;

  if ( event.eventId() == WM_USER )
  {
    INotificationEvent * theEvent = (INotificationEvent *)
                                       ((char *)(event.parameter1()));

    IAsyncNotifier * theNotifier = (IAsyncNotifier *)(&(theEvent->notifier()));
    if ( theEvent->notificationId() == IAsyncNotifierThread::deleteThisId )
    {
      delete theNotifier;
    }
    else
    {
      theNotifier->IStandardNotifier::notifyObservers ( *theEvent );
      theNotifier->notificationCleanUp ( *theEvent );
    }

    delete theEvent;

    handledEvent = true;
  }

  return handledEvent;
}
