/* NOSHIP */
#ifndef _IASYNBKG_
#define _IASYNBKG_
/*******************************************************************************
* FILE NAME: iasynbkg.hpp
*
* DESCRIPTION:
*   Declaration of the class(es):
*     IAsyncNotifierBackgroundThread - Class for asynchronous notifier
*                                      background threads.
*
* COPYRIGHT:
*   Licensed Materials - Property of IBM
*   (C) Copyright IBM Corporation 1995
*   All Rights Reserved
*   US Government Users Restricted Rights - Use, duplication, or disclosure
*   restricted by GSA ADP Schedule Contract with IBM Corp.
*
*******************************************************************************/
#ifndef _IASYNTHR_
  #include <iasynthr.hpp>
#endif

// Other dependency classes:
#ifndef _IRESLOCK_
  #include <ireslock.hpp>
#endif

#ifndef _IEVNTSEM_
  #include <ievntsem.hpp>
#endif

class INotificationEvent;
template <class Element> class ISequence;

// Align classes on four byte boundary.
#pragma pack(4)

class IAsyncNotifierBackgroundThread : public IAsyncNotifierThread {
/*******************************************************************************
*
* This class implements the interface for asynchronous notifier background
* thread objects.
*
*******************************************************************************/

public:
/*------------------------------ Constructors ----------------------------------
| You can construct an object of this class as follows:                        |
|   - With the default constructor.                                            |
|-----------------------------------------------------------------------------*/
IAsyncNotifierBackgroundThread ( );

virtual ~IAsyncNotifierBackgroundThread ( );

/*--------------------------- Reference Counting -------------------------------
| Used by IAsyncNotifier objects to remove references to this object.          |
|   removeRef - Calls base class implementation.  Then if the count is zero    |
|               and the queue is empty, the queueEventSem is posted so that    |
|               processMsgs will see that it is time to exit.                  |
|-----------------------------------------------------------------------------*/
virtual unsigned long removeRef ( );

/*-------------------------- Enqueue Notification ------------------------------
| Used by IAsyncNotifier objects to enque notifications.                       |
|   enqueueNotification - Places the notification on this thread's queue.      |
|-----------------------------------------------------------------------------*/
virtual IAsyncNotifierBackgroundThread & enqueueNotification (
                                           const INotificationEvent & anEvent );

/*---------------------------- Process Messages --------------------------------
| Use this to start dispatching notifications for this thread.                 |
|   processMsgs - Throws an invalid request exception if the current thread is |
|                 not this thread.  This function does not return until        |
|                 asyncNotifierCount reaches zero.  This function should be    |
|                 called once by IAsyncNotifier::run and this object should be |
|                 deleted after this function returns.                         |
|-----------------------------------------------------------------------------*/
virtual IAsyncNotifierBackgroundThread & processMsgs ( );

/*-------------------------- Delete Notifications ------------------------------
| IAsyncNotifier calls this from its destructor to have all pending            |
| notifications deleted.                                                       |
|   deleteNotificationsFor - Ensures that all notifications for the passed     |
|                            object are never dispatched.  Throws an invalid   |
|                            request exception if the current thread is not    |
|                            this thread.                                      |
|-----------------------------------------------------------------------------*/
virtual IAsyncNotifierBackgroundThread & deleteNotificationsFor (
                                         const IAsyncNotifier & asyncNotifier );


private:
// The private copy constructor and assignment operator are not implemented.
IAsyncNotifierBackgroundThread ( const IAsyncNotifierBackgroundThread & rhs );
IAsyncNotifierBackgroundThread & operator = (
                                   const IAsyncNotifierBackgroundThread & rhs );

/*--------------------------- Private State Data -----------------------------*/
ISequence<INotificationEvent> * queue;
IPrivateResource                queueKey;
IEventSem                       queueEventSem;

}; // IAsyncNotifierBackgroundThread

// Resume compiler default packing.
#pragma pack()

#endif // _IASYNBKG_
