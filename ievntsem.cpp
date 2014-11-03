/*******************************************************************************
* FILE NAME: IEVNTSEM.CPP
*
* DESCRIPTION:
*   This file contains implementation of the class IEventSem.
*
* COPYRIGHT:
*   Licensed Materials - Property of IBM
*   (C) Copyright IBM Corporation 1995
*   All Rights Reserved
*   US Government Users Restricted Rights - Use, duplication, or disclosure
*   restricted by GSA ADP Schedule Contract with IBM Corp.
*
*******************************************************************************/

 #define INCL_DOSSEMAPHORES
 #define INCL_DOSERRORS
 #define INCL_DOSPROCESS
 #include <os2.h>
 #include <stdio.h>
 #include <string.h>

 #include <iexcept.hpp>
 #include <istring.hpp>
 #include <ihandle.hpp>
 #include <ievntsem.hpp>

// Define the functions and static data members to be exported.
// Ordinals 150 through 199 are reserved for use by IEventSem.
#pragma export(IEventSem::handle(),, 150)
#pragma export(IEventSem::IEventSem(const IString&,IEventSem::SemOperation),, 151)
#pragma export(IEventSem::IEventSem(),, 152)
#pragma export(IEventSem::IEventSem(ISemaphoreHandle&),, 153)
#pragma export(IEventSem::name(),, 154)
#pragma export(IEventSem::post(),, 155)
#pragma export(IEventSem::postCount(),, 156)
#pragma export(IEventSem::reset(),, 157)
#pragma export(IEventSem::type(),, 158)
#pragma export(IEventSem::wait(long),, 159)
#pragma export(IEventSem::~IEventSem(),, 160)

// It's possible for the caller of the external entry points to have a
// different C library environment.  Make sure the exception handler for
// our library environment is registered on entry and deregistered on exit.
#pragma handler(IEventSem::handle())
#pragma handler(IEventSem::IEventSem(const IString&,IEventSem::SemOperation))
#pragma handler(IEventSem::IEventSem())
#pragma handler(IEventSem::IEventSem(ISemaphoreHandle&))
#pragma handler(IEventSem::name())
#pragma handler(IEventSem::post())
#pragma handler(IEventSem::postCount())
#pragma handler(IEventSem::reset())
#pragma handler(IEventSem::type())
#pragma handler(IEventSem::wait())
#pragma handler(IEventSem::wait(long))
#pragma handler(IEventSem::~IEventSem())


IEventSem :: IEventSem( const IString& semName, SemOperation semOp):
               szName( *(new IString("\\SEM32\\" + semName)) )
   {
    long      rc = 0;
    HEV       handle = 0;          // no handle yet.

    if ( semOp == createSem )
       {
         // Semaphore is public (i.e. SHARED)
         // and initial semaphore state is "set" (i.e. FALSE)
         // If a name was specified, then the semaphore is named.
         semType = created;
         if ( semName.length() != 0)
          {
           rc = (long) DosCreateEventSem( (PSZ)szName,
                                          &handle,
                                          DC_SEM_SHARED,
                                          FALSE );
          }
         else
          {
           rc = (long) DosCreateEventSem( (PSZ)0,
                                          &handle,
                                          DC_SEM_SHARED,
                                          FALSE );
           IString nullStr;
           szName = nullStr;
          }

        if ( rc != 0 )
          {
           ITHROWSYSTEMERROR( rc,
                              "DosCreateEventSem",
                              IErrorInfo::accessError,
                              IException::recoverable) ;
          }
       }  //end if Creating semaphore
    else
       {
        // Open an existing semaphore.
        semType = opened;
        rc = (long) DosOpenEventSem( (PSZ)szName,
                                      &handle );
        if ( rc != 0 )
          {
           ITHROWSYSTEMERROR( rc,
                              "DosOpenEventSem",
                              IErrorInfo::accessError,
                              IException::recoverable) ;
          }
       }  //end else Opening semaphore

    hndlSem = new ISemaphoreHandle( handle);
    return;
   }

 /*----------------------------*/
IEventSem :: IEventSem( ):
               szName( *(new IString("")) )
   {
                                   // an unnamed semaphore here.
    long      rc = 0;
    HEV       handle = 0;          // no handle yet.

     // Semaphore is unnamed, private
     // and initial semaphore state is "set" (i.e. FALSE)
     semType = localRam;
     rc = (long) DosCreateEventSem( (PSZ)0,
                                    &handle,
                                    0,
                                    FALSE );
     if ( rc != 0 )
       {
        ITHROWSYSTEMERROR( rc,
                           "DosCreateEventSem",
                           IErrorInfo::accessError,
                           IException::recoverable) ;
       }

    hndlSem = new ISemaphoreHandle( handle);
    return;
   }


 /*----------------------------*/
IEventSem :: IEventSem( ISemaphoreHandle& handle):
               szName( *(new IString("")) )
   {
                                   // a previously created, unnammed semaphore.
    long     rc;
    HEV      hand = handle;        // handle as passed in.

    // Semaphore is unnamed.
    // and initial semaphore state is "set" (i.e. FALSE)
    semType = opened;
    rc = (long) DosOpenEventSem( (PSZ)0,
                                 &hand );
    if ( rc != 0 )
      {
       ITHROWSYSTEMERROR( rc,
                          "DosOpenEventSem",
                          IErrorInfo::accessError,
                          IException::recoverable) ;
      }

    hndlSem = new ISemaphoreHandle( hand);
    return;
   }

 /*----------------------------*/
IEventSem :: ~IEventSem( )
   {
    long      rc = 0;
    delete  &szName;
    // if this semaphore is currently created/opened, close it.
    rc = (long) DosCloseEventSem( (HEV)(*hndlSem) );
    if ( rc)
     {
      ITHROWSYSTEMERROR( rc,
                         "DosCloseEventSem",
                         IErrorInfo::accessError,
                         IException::recoverable) ;
     }
    delete hndlSem;
   }

/*----------------------------*/
IEventSem & IEventSem :: post( )
   {
    long      rc = 0;
    rc = (long) DosPostEventSem( (HEV)(*hndlSem) );
    if ( rc)
     {
      // Treat the case of already-posted as OK.
      if ( rc != ERROR_ALREADY_POSTED)
        ITHROWSYSTEMERROR( rc,
                           "DosPostEventSem",
                           IErrorInfo::accessError,
                           IException::recoverable) ;
     }
    return (*this);
   }

 /*----------------------------*/
unsigned long IEventSem :: postCount( )
   {
    long      rc = 0;
    unsigned long count = 0;

    rc = (long) DosQueryEventSem( (HEV)(*hndlSem), &count);
    if ( rc)
     {
      ITHROWSYSTEMERROR( rc,
                         "DosQueryEventSem",
                         IErrorInfo::accessError,
                         IException::recoverable) ;
     }
    return (count);
   }

 /*----------------------------*/
unsigned long IEventSem :: reset( )
   {
    long      rc = 0;
    unsigned long ulPostCount = 0;

    rc = (long) DosResetEventSem( (HEV)(*hndlSem), &ulPostCount );
    if ( rc)
     {
      // Treat the case of already-reset as OK.
      if ( rc != ERROR_ALREADY_RESET)
        ITHROWSYSTEMERROR( rc,
                           "DosResetEventSem",
                           IErrorInfo::accessError,
                           IException::recoverable) ;
     }
    return( ulPostCount );
   }

 /*----------------------------*/
IEventSem & IEventSem :: wait( long timeOut)
   {
    long      rc = 0;
    rc = (long) DosWaitEventSem( (HEV)(*hndlSem), timeOut );
    if ( rc)
     {
      IErrorInfo::ExceptionType type;
      if ( rc == ERROR_TIMEOUT)
        type = IErrorInfo::resourceExhausted;
      else
        type = IErrorInfo::accessError;

      ITHROWSYSTEMERROR( rc,
                         "DosWaitEventSem",
                         type,
                         IException::recoverable) ;
     } /* endif */
    return (*this);
   }

 /*----------------------------*/
 const IString & IEventSem :: name( )
   {
    return szName;
   }

 /*----------------------------*/
 IEventSem::EventSemType IEventSem :: type( )
   {
    return semType;
   }

 /*----------------------------*/
 const ISemaphoreHandle &IEventSem :: handle( )
   {
    return( *hndlSem );
   }

