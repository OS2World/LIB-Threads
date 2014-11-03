#ifndef _IASYNTFY_
#define _IASYNTFY_
/*******************************************************************************
* FILE NAME: iasyntfy.hpp
*
* DESCRIPTION:
*   Declaration of the class(es):
*     IAsyncNotifier - Base class for asynchronous notifiers.
*
* COPYRIGHT:
*   Licensed Materials - Property of IBM
*   (C) Copyright IBM Corporation 1995
*   All Rights Reserved
*   US Government Users Restricted Rights - Use, duplication, or disclosure
*   restricted by GSA ADP Schedule Contract with IBM Corp.
*
*******************************************************************************/
#ifndef _ISTDNTFY_
  #include <istdntfy.hpp>
#endif

// Other dependency classes.
#ifndef  _IHANDLE_
  #include <ihandle.hpp>
#endif

#ifndef _IRESLOCK_
  #include <ireslock.hpp>
#endif

#pragma library("asyncnot.lib")

class IAsyncNotifierThread;
template <class Element, class Key> class IKeySet;

// Align classes on four byte boundary.
#pragma pack(4)

class IAsyncNotifier : public IStandardNotifier {
/*******************************************************************************
*
* This abstract base class implements an asynchronous notification mechanism.
* When a notification is sent via IAsyncNotifier::notifyObservers, it is not
* sent synchronously as with IStandardNotifier, but is queued for later
* dispatch.  In addition, if the asynchronous notifier has multiple internal
* threads, the notification will be dispatched on the thread on which the
* notifier was created regardless of which internal thread called
* IAsyncNotifier::notifyObservers.  This class works for notifiers created
* on GUI and non-GUI threads.
*
* If the current thread is going to be initialized for GUI, make sure it is
* initialized for GUI (directly or indirectly) before any IAsyncNotifier
* objects are created on the current thread.
*
* It is recommended that IAsyncNotifier objects be either enabled or disabled
* for notification throughout their entire lifetime.  Since notifications are
* generated and dispatched asynchronously, it is impossible to predict
* exactly which notifications will not be dispatched if notification is
* temporarily disabled.
*
* It is recommended that an IAsyncNotifier object not be deleted until a
* notification has been processed that indicates that notification activity
* for the object becomes quiescent.  For example, an IConversation::statusId
* notification that indicates the conversation has closed.  In addition, if
* you want to delete an IAsyncNotifier in response to one of its notifications,
* do not use the delete operator.  In this case, you must use the deleteThis
* member function.  If you are deleting the IAsyncNotifier at some other
* time, for example as the thread it was created on terminates, use the
* delete operator.
*
* An IAsyncNotifier object must be deleted on the same thread on which it was
* created.  The deleteThis function may be used from any thread and this
* condition will be met.  When an IAsyncNotifier object is deleted, all pending
* notifications are destroyed and are never dispatched.  This is to ensure that
* notifications are not sent for objects that do not exist.  The delete
* notification is sent on the same thread on which the object is deleted.  When
* deleteThis is used, the delete notification will occur on the same thread
* on which the object was created.
*
* Subclass destructors must ensure that all internal threads are stopped.
*
*******************************************************************************/

public:
/*------------------------------ Constructors ----------------------------------
| You can not directly construct an object of this abstract base class.        |
| An IAsyncNotifier object must be deleted on the same thread on which it was  |
| created.  An invalid request exception is thrown if a delete is attempted    |
| a different thread and the object will be in an undefined state.             |
| Subclasses can initialize this class as follows:                             |
|   - With the default constructor.                                            |
|   - With the copy constructor.  The created object's dispatch thread is the  |
|     current thread, not the dispatch thread of the parameter.                |
|-----------------------------------------------------------------------------*/
IAsyncNotifier ( );

IAsyncNotifier ( const IAsyncNotifier & asyncNotifier );

virtual ~IAsyncNotifier ( ) = 0;

/*----------------------------- Async Deletion ---------------------------------
| Use this function rather than the delete operator to asynchronously delete   |
| this object.                                                                 |
|   deleteThis - Posts a notification to itself to safely delete itself.  This |
|                function should be used instead of the delete operator when   |
|                you wish to delete this object in response to one of its      |
|                notifications.                                                |
|-----------------------------------------------------------------------------*/
IAsyncNotifier & deleteThis ( );

/*------------------------------- Assignment -----------------------------------
| The assignment for this class is the standard assignment operator.           |
|   operator = - The assignment operator must be used by subclasses to         |
|                implement their own assignment operators.                     |
|                The target object's dispatch thread is not changed.           |
|-----------------------------------------------------------------------------*/
IAsyncNotifier & operator = ( const IAsyncNotifier & rhs );

/*----------------------------------- Run --------------------------------------
| Use this to start dispatching notifications for the current thread.          |
|   run - If this thread is initialized for GUI,                               |
|         IThread::current().processMsgs() is called and this function         |
|         returns when that function returns.  If it is not initialized for    |
|         GUI, it starts dispatching notification events.  This call does not  |
|         return until there are no more objects with this thread as their     |
|         dispatch thread.  If this is not a GUI thread and no IAsyncNotifier  |
|         objects have been created on this thread, an invalid request         |
|         exception is thrown.                                                 |
|-----------------------------------------------------------------------------*/
static void run ( );

/*-------------------------- Observer Notification -----------------------------
| Use this function to asynchronously notify observers of an event.            |
|   notifyObservers - If notification is enabled, queues notification for      |
|                     dispatch and returns.                                    |
|-----------------------------------------------------------------------------*/
virtual IAsyncNotifier & notifyObservers ( const INotificationEvent & anEvent );

/*----------------------------- Dispatch Thread --------------------------------
| Use this function to query the dispatch thread.                              |
|   dispatchThread    - Returns the thread id for the dispatch thread.         |
|-----------------------------------------------------------------------------*/
const IThreadId & dispatchThread ( ) const;

/*-------------------------- Notification Clean Up -----------------------------
| This function is called on the dispatch thread after each event from this    |
| IAsyncNotifier is dispatched, but before the event is deleted.               |
|   notificationCleanUp - The default implementation does nothing.  Subclasses |
|                         must use this function to clean up objects that were |
|                         created as event data.  Each subclass implementation |
|                         must look like this:                                 |
|                             if ( it is one of my events )                    |
|                               delete the event data, if any                  |
|                             else                                             |
|                               BaseClass::notificationCleanUp ( anEvent );    |
|-----------------------------------------------------------------------------*/
virtual const IAsyncNotifier & notificationCleanUp (
                                 const INotificationEvent & anEvent ) const;

/*---------------------------- Event Identifiers -------------------------------
| Event Ids for notification purposes.                                         |
|   dispatchThreadId - Id for the dispatch thread read only attribute.         |
|-----------------------------------------------------------------------------*/
static INotificationId const dispatchThreadId;

//******************* TEMPORARY ************************************************
// The Visual Builder does not properly dereference pointers for reference
// parameters on connections to actions and we would like to pass objects of
// the class as reference parameters.  This will be fixed in the next
// release.  In part by providing a "*this" attribute for all parts in addition
// to the "this" attribute.  The work around to use a custom logic connection
// to dereference and set the parameter does not work in the GA version due to
// a code generation bug.  A fix for the bug will not be made until the first
// CSD.  As the next best work around, I am temporarily adding a "*this"
// attribute to the class.  These should be able to be deleted before GA
// of the next release of VisualAge C++.
//******************************************************************************
IAsyncNotifier & thisRef () { return *this; }
static INotificationId const thisRefId;


protected:
/*-------------------------- Observer Notification -----------------------------
| Use this function to asynchronously notify observers of an event.            |
|   notifyObservers - If notification is enabled, queues notification for      |
|                     dispatch and returns.                                    |
|-----------------------------------------------------------------------------*/
virtual IAsyncNotifier & notifyObservers ( const INotificationId & nId );


private:
IAsyncNotifier & findOrCreateDispatchThread ( );

/*--------------------------- Private State Data -----------------------------*/
IAsyncNotifierThread * theDispatchThread;

static IKeySet<IAsyncNotifierThread *, IThreadId> * threads;
static IPrivateResource                             threadsKey;

}; // IAsyncNotifier

// Resume compiler default packing.
#pragma pack()

#endif // _IASYNTFY_
