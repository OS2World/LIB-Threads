/* NOSHIP */
#ifndef _IASYNTHR_
#define _IASYNTHR_
/*******************************************************************************
* FILE NAME: iasynthr.hpp
*
* DESCRIPTION:
*   Declaration of the class(es):
*     IAsyncNotifierThread - Base class for asynchronous notifier threads.
*
* COPYRIGHT:
*   Licensed Materials - Property of IBM
*   (C) Copyright IBM Corporation 1995
*   All Rights Reserved
*   US Government Users Restricted Rights - Use, duplication, or disclosure
*   restricted by GSA ADP Schedule Contract with IBM Corp.
*
*******************************************************************************/
#ifndef _IVBASE_
  #include <ivbase.hpp>
#endif

// Other dependency classes.
#ifndef  _IHANDLE_
  #include <ihandle.hpp>
#endif

#ifndef  _INOTIFY_
  #include <inotify.hpp>
#endif

class INotificationEvent;
class IAsyncNotifier;

// Align classes on four byte boundary.
#pragma pack(4)

class IAsyncNotifierThread : public IVBase {
/*******************************************************************************
*
* This abstract base class defines the interface for asynchronous notifier
* thread objects.  Subclasses define the actual queue mechanism to be used
* for this thread.
*
*******************************************************************************/

public:
/*------------------------------ Constructors ----------------------------------
| You can not directly construct an object of this abstract base class.        |
| Subclasses can initialize this class as follows:                             |
|   - With the default constructor.  This object takes on the identity of the  |
|     current thread.  The reference count is initially zero.                  |
|   - Use the make constructor to create the appropriate subclass for the      |
|     current thread.  By default it will make one for any thread.  Of true is |
|     passed and the current thread is not a GUI thread, an invalid request    |
|     exception is thrown.                                                     |
|-----------------------------------------------------------------------------*/
IAsyncNotifierThread ( );

static IAsyncNotifierThread * make ( IBoolean guiOnly = false );

virtual ~IAsyncNotifierThread ( );

/*-------------------------------- Thread Id -----------------------------------
| Use this function to query the thread Id for this thread.                    |
|   threadId - Returns the thread Id.                                          |
|-----------------------------------------------------------------------------*/
const IThreadId & threadId ( ) const;

/*--------------------------- Reference Counting -------------------------------
| Used by IAsyncNotifier objects to add and remove references to this object.  |
|   addRef    - Bumps and returns the reference count.                         |
|   removeRef - Decrements and returns the reference count.                    |
|-----------------------------------------------------------------------------*/
virtual unsigned long addRef ( );
virtual unsigned long removeRef ( );

/*-------------------------- Enqueue Notification ------------------------------
| Used by IAsyncNotifier objects to enque notifications.                       |
|   enqueueNotification - Places the notification on this thread's queue.      |
|-----------------------------------------------------------------------------*/
virtual IAsyncNotifierThread & enqueueNotification (
                                 const INotificationEvent & anEvent ) = 0;

/*----------------------------- Process Messages -------------------------------
| Use this to start dispatching notifications for this thread.                 |
|   processMsgs - Throws an invalid request exception if the current thread is |
|                 not this thread.  This function does not return until        |
|                 notification processing is done for this thread.  This       |
|                 function should be called once by IAsyncNotifier::run and    |
|                 this object should be deleted after this function returns.   |
|   isRunning   - Returns true if messages are being processed.                |
|-----------------------------------------------------------------------------*/
virtual IAsyncNotifierThread & processMsgs ( ) = 0;
IBoolean isRunning ( ) const;

/*-------------------------- Delete Notifications ------------------------------
| IAsyncNotifier calls this from its destructor to have all pending            |
| notifications deleted.                                                       |
|   deleteNotificationsFor - Ensures that all notifications for the passed     |
|                            object are never dispatched.  Throws an invalid   |
|                            request exception if the current thread is not    |
|                            this thread.                                      |
|   deleteThisId           - Used by IAsyncNotifier and this class to signal   |
|                            async deletion of an IAsyncNotifier.              |
|-----------------------------------------------------------------------------*/
virtual IAsyncNotifierThread & deleteNotificationsFor (
                                 const IAsyncNotifier & asyncNotifier ) = 0;
static INotificationId const deleteThisId;


protected:
unsigned long refCount ( ) const;
IAsyncNotifierThread & setIsRunning ( IBoolean running );


private:
// The private copy constructor and assignment operator are not implemented.
IAsyncNotifierThread ( const IAsyncNotifierThread & rhs );
IAsyncNotifierThread & operator = ( const IAsyncNotifierThread & rhs );

/*--------------------------- Private State Data -----------------------------*/
unsigned long asyncNotifierCount;
IThreadId     theThreadId;
IBoolean      bRunning;

}; // IAsyncNotifierThread


/*---------------------- Collection Class Key Function -------------------------
| Used by collections of pointers to objects of this class,                    |
|   key - Returns a reference to the thread Id.                                |
|-----------------------------------------------------------------------------*/
const IThreadId & key ( IAsyncNotifierThread * const & object );

// Resume compiler default packing.
#pragma pack()

#endif // _IASYNTHR_
