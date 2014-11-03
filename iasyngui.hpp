/* NOSHIP */
#ifndef _IASYNGUI_
#define _IASYNGUI_
/*******************************************************************************
* FILE NAME: iasyngui.hpp
*
* DESCRIPTION:
*   Declaration of the class(es):
*     IAsyncNotifierGUIThread - Class for asynchronous notifier GUI threads.
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

class INotificationEvent;
class IObjectWindow;
class IAsyncNotificationHandler;

// Align classes on four byte boundary.
#pragma pack(4)

class IAsyncNotifierGUIThread : public IAsyncNotifierThread {
/*******************************************************************************
*
* This class implements the interface for asynchronous notifier GUI thread
* objects.
*
*******************************************************************************/

public:
/*------------------------------ Constructors ----------------------------------
| You can construct an object of this class as follows:                        |
|   - With the default constructor.                                            |
|-----------------------------------------------------------------------------*/
IAsyncNotifierGUIThread ( );

virtual ~IAsyncNotifierGUIThread ( );

/*--------------------------- Reference Counting -------------------------------
| Used by IAsyncNotifier objects to remove references to this object.          |
|   removeRef - Calls base class implementation.  Then if the count is zero    |
|               the object window is closed.                                   |
|-----------------------------------------------------------------------------*/
virtual unsigned long removeRef ( );

/*-------------------------- Enqueue Notification ------------------------------
| Used by IAsyncNotifier objects to enque notifications.                       |
|   enqueueNotification - Places the notification on this thread's queue.      |
|-----------------------------------------------------------------------------*/
virtual IAsyncNotifierGUIThread & enqueueNotification (
                                    const INotificationEvent & anEvent );

/*---------------------------- Process Messages --------------------------------
| Use this to start dispatching notifications for this thread.                 |
|   processMsgs - Throws an invalid request exception if the current thread is |
|                 not this thread.  This function does not return until        |
|                 IThread::current()::processMsgs() returns.  This function    |
|                 should be called once by IAsyncNotifier::run and this object |
|                 should be deleted after this function returns.               |
|-----------------------------------------------------------------------------*/
virtual IAsyncNotifierGUIThread & processMsgs ( );

/*-------------------------- Delete Notifications ------------------------------
| IAsyncNotifier calls this from its destructor to have all pending            |
| notifications deleted.                                                       |
|   deleteNotificationsFor - Ensures that all notifications for the passed     |
|                            object are never dispatched.  Throws an invalid   |
|                            request exception if the current thread is not    |
|                            this thread.                                      |
|-----------------------------------------------------------------------------*/
virtual IAsyncNotifierGUIThread & deleteNotificationsFor (
                                    const IAsyncNotifier & asyncNotifier );


private:
// The private copy constructor and assignment operator are not implemented.
IAsyncNotifierGUIThread ( const IAsyncNotifierGUIThread & rhs );
IAsyncNotifierGUIThread & operator = ( const IAsyncNotifierGUIThread & rhs );

/*--------------------------- Private State Data -----------------------------*/
IObjectWindow             * objectWindow;
IAsyncNotificationHandler * asyncNotificationHandler;
IPrivateResource            objectWindowKey;

}; // IAsyncNotifierGUIThread

// Resume compiler default packing.
#pragma pack()

#endif // _IASYNGUI_
