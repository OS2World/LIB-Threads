
/* 
 * This file was created for George DeCandio 
 * by Project Smarts on 13 Sep 1995. 
 */

//----------------------------------------------
// asyncnot.c - Source file for a C++ DLL
//----------------------------------------------

#include "asyncnot.hpp"

/***************************/
/* Defining Static Objects */
/***************************/

int area::objectCount = 0;
int rectangle::objectCount = 0;
int triangle::objectCount = 0;

/****************************/
/* Defining the Base Class  */
/****************************/

area::area(void)
   {
   ++area::objectCount;
   }

void area::setarea(double d1 , double d2)
         {
         dim1 = d1;
         dim2 = d2;
         }

void area::getDim(double &d1, double &d2)
         {
         d1 = dim1;
         d2 = dim2;
         }

double area::getArea()
         {
         return -1 ;    /* not override */
         }

/*******************************/
/* Defining a   Derived class  */
/*******************************/

double rectangle::getArea()
         {
         double d1,d2;
         getDim (d1,d2);
         return d1*d2;
         }

rectangle::rectangle(void)
   {
   ++rectangle::objectCount;
   }

/*******************************/
/* Defining a   Derived class  */
/*******************************/


triangle::triangle()
   {
   ++triangle::objectCount;
   }

double triangle::getArea()
   {
   double d1,d2;
   getDim (d1,d2);
   return .5*d1*d2;
   }

