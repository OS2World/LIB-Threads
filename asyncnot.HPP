

/* 
 * This file was created for George DeCandio 
 * by Project Smarts on 13 Sep 1995. 
 */

//-------------------------------------------
// asyncnot.h - Header file for a C++ DLL
//-------------------------------------------


/*-----------------------------------------------------------------*/
/* This file defines 3 classes, area, rectangle, and triangle      */
/* whose members are explicitly exported using the #pragma export  */
/* directive.  With #pragma export you can assign ordinals to the  */
/* symbols so that they are maintained when you update the DLL.    */
/* #pragma export also lets you specify the external name of the   */
/* symbol.  To export your classes in this manner, you must add a  */
/* #pragma export directive for each member data, function,        */
/* constructor and destructor you want to export.  You must always */
/* export constructors and destructors.  See the end of this file  */
/* to see how it is done.                                          */
/*-----------------------------------------------------------------*/


/*--------------------------*/
/* Define a base class      */
/*--------------------------*/
class area
   {
   private:
      double dim1, dim2;
   public:
      static int   objectCount;
      void setarea(double d1 , double d2);
      void getDim(double &d1, double &d2);
      virtual double getArea();
      area::area(void);
   };


/*-----------------------------*/
/* Define a derived class      */
/*-----------------------------*/
class rectangle : public area
   {
   public:
      static int objectCount ;
      double getArea();
      rectangle::rectangle(void);
   };


/*------------------------------*/
/* Define another derived class */
/*------------------------------*/
class triangle : public area
   {
   public:
      static int objectCount;
      double getArea();
      triangle::triangle(void);
   };


/*--------------------------------------------------------*/
/* Export symbols from the declared classes               */
/*--------------------------------------------------------*/
#pragma export(area::objectCount, "countArea", 1)
#pragma export(area::getDim, "getAreaDimensions", 2)
#pragma export(area::getArea, "getAreaArea", 3)
#pragma export(area::area(), "createArea", 4)
// Note:  area::setarea() is not exported because it is not required

#pragma export(triangle::objectCount, "countTriangle", 5)
#pragma export(triangle::getArea, "getTriangleArea", 6)
#pragma export(triangle::triangle(void), "createTriangle", 7)

#pragma export(rectangle::objectCount, "countRectangle", 8)
#pragma export(rectangle::getArea, "getRectangleArea", 9)
#pragma export(rectangle::rectangle(void), "createRectangle" , 10)


/*--------------------------------------------------------*/
/* Considerations: Ensure you export all the required     */
/* member functions.  If an inlined or exported function  */
/* uses private or protected members, you must also export*/
/* those members.  In addition, you should also export    */
/* all static members otherwise clients of your class will*/
/* not be able to debug their code because the reference  */
/* to the static data members cannot be resolved.         */
/*--------------------------------------------------------*/


