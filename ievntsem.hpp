/* NOSHIP */
#ifndef _IEVNTSEM_
#define _IEVNTSEM_
/*******************************************************************************
* FILE NAME: IEVNTSEM.HPP
*
* DESCRIPTION:
*   Declaration of the class(es):
*     IEventSem - Provides a signaling mechanism among threads or processes.
*
* COPYRIGHT:
*   Licensed Materials - Property of IBM
*   (C) Copyright IBM Corporation 1995
*   All Rights Reserved
*   US Government Users Restricted Rights - Use, duplication, or disclosure
*   restricted by GSA ADP Schedule Contract with IBM Corp.
*
*******************************************************************************/
#ifndef _IBASE_
  #include <ibase.hpp>
#endif

// Forward declarations for other classes:
class IString;
class ISemaphoreHandle;

// Align classes on four byte boundary.
#pragma pack(4)

class IEventSem : public IBase {
/*******************************************************************************
* The IEventSem class provides a signaling mechanism among threads or          *
* processes.  An IEventSem instance is used by one thread to signal other      *
* threads that an event has occurred.  An application can use objects of       *
* this class to block a thread or process until the event has occurred.        *
*                                                                              *
* An IEventSem object has two states, reset and posted.  When an IEventSem     *
* object is in the reset state, the operating system blocks any thread or      *
* process that is waiting on this IEventSem object (or any other IEventSem     *
* object with the same semaphore handle).  When an IEventSem object is in the  *
* posted state, all threads or processes waiting on the event semaphore resume *
* execution.                                                                   *
*                                                                              *
*******************************************************************************/
public:

/*------------------------------- Enumerators ----------------------------------
| Two enumerations are defined.                                                |
|   SemOperation  - enumerator to distinguish between a global semaphore       |
|                   that is being opened verses one that is being created.     |
|                   createSem - create the event semaphore.                    |
|                   openSem   - open the event semaphore.  The semaphore       |
|                               must already exists.                           |
|                                                                              |
|   EventSemType  - enumerator to determine the type of semaphore              |
|                   managed by this object:                                    |
|                   created   - a global semaphore created by this IEventSem   |
|                               object.                                        |
|                   opened    - a global semaphore opened by this IEventSem    |
|                               object.                                        |
|                   localRam  - a private semaphore.                           |
|-----------------------------------------------------------------------------*/
 enum SemOperation { createSem, openSem };
 enum EventSemType { created, opened, localRam };


/*------------------------------- Constructors ---------------------------------
| You can construct an object of this class in the following ways:             |
|   -  Construct a private, unnamed event semaphore.                           |
|      The initial state of the semaphore (on a successful                     |
|      return from this constructor) is "reset".                               |
|                                                                              |
|   -  Construct a public, named event semaphore. The SemOperation value       |
|      indicates whether the semaphore is being created or opened.             |
|      For create, the initial state of the semaphore (on a                    |
|      successful return from this constructor) is "reset".                    |
|      The semaphore name is constructed from semName by adding                |
|      the prefix '\SEM32'. For example, semName == 'CS\SIGNAL.SEM'            |
|      would result in an actual semaphore name '\SEM32\CS\SIGNAL.SEM'.        |
|      If the semaphore is being created and the semName argument is an        |
|      empty string, an unnamed event semaphore is constructed.                |
|                                                                              |
|   -  Construct an IEventSem object from a previously created public          |
|      semaphore.  The ISemaphoreHandle augument is used to open the           |
|      semaphore for this process.                                             |
|                                                                              |
| The destructor closes the event semaphore.                                   |
|-----------------------------------------------------------------------------*/
 IEventSem( );                     /* local, unnamed semaphore */
                                   /* global, named semaphore  */
 IEventSem( const IString& semName,
            SemOperation semOp = IEventSem::createSem);

 IEventSem( ISemaphoreHandle& handle);  /* global, unnamed semphore open */

 ~IEventSem( ) ;

/*------------------------- Implementation -------------------------------------
| Use functions in this group to do event signaling and to wait for an         |
| event to be signaled.                                                        |
|                                                                              |
|   post   - Posts the event semaphore, causing all threads that were          |
|            blocked, via IEventSem::wait on this object, to execute.          |
|                                                                              |
|   reset  - Resets the event semaphore, causing all threads that subsequently |
|            call IEventSem::wait for this semaphore object to be blocked.     |
|            The number of post calls since the last reset is returned.        |
|                                                                              |
|   wait   - Enables a thread to wait for this semaphore to be posted.         |
|            The wait times out after the specified timeOut value.  If no      |
|            timeout argument is specified on the wait() call, the timeOut     |
|            value defaults to forever.                                        |
|                                                                              |
|-----------------------------------------------------------------------------*/
 IEventSem & post( );
 unsigned long reset( );
 IEventSem & wait( long timeOut=-1);

/*------------------------- Accessors ------------------------------------------
| These functions are used to query the characteristics of an IEventSem        |
| object.                                                                      |
|                                                                              |
|   name      - Returns the name (if any) associated with this semaphore.      |
|   postCount - Returns the current post count for this object.                |
|   type      - Returns the type of semaphore this object represents.          |
|   handle    - Returns the handle that is associated with this semaphore.     |
|                                                                              |
|-----------------------------------------------------------------------------*/
 const IString & name( );
 unsigned long postCount( );
 IEventSem::EventSemType type( );
 const ISemaphoreHandle &handle( );

protected:

private:
 // data
 IString & szName;                 // semaphore name
 ISemaphoreHandle * hndlSem;       // semaphore handle
 EventSemType semType;             // type of semaphore.

};  // IEventSem

// Resume compiler default packing.
#pragma pack()

#endif // _IEVNTSEM_
