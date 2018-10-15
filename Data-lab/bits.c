/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implent floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
/* Copyright (C) 1991-2016 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 8.0.0.  Version 8.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2014, plus Amendment 1 (published
   2015-05-15).  */
/* We do not support C11 <threads.h>.  */
//1
/* 
 * thirdBits - return word with every third bit (starting from the LSB) set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 1
 */
int thirdBits(void) {
	int c1=(0x9<<6)|0x9;//001001001001
	int c2=(c1<<12)|c1;//001001001001001001001001
  return (c2<<9)|c1;
}
/*
 * isTmin - returns 1 if x is the minimum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmin(int x) {
  return !(((~x+1)^x)|(!x));//if x is Tmin then ~x+1==x (but x!=0)
}
//2
/* 
 * isNotEqual - return 0 if x == y, and 1 otherwise 
 *   Examples: isNotEqual(5,5) = 0, isNotEqual(4,5) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int isNotEqual(int x, int y) {
  return !(!(x^y));// if x==y then x^y==000...0
}
/* 
 * anyOddBit - return 1 if any odd-numbered bit in word set to 1
 *   Examples anyOddBit(0x5) = 0, anyOddBit(0x7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int anyOddBit(int x) {
	int c1=0xaa;//10101010
	int c2=(c1<<8)|c1;//0xaaaa
	int c3=(c2<<16)|c2;//0xaaaaaaaa
    return !(!(x&c3));
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return ~x+1;
}
//3
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
	int a=~(!x)+1;
	//if x==0 then a=111...1
	//if x!=0 then a=000...0	
  	return (z&a)|(y&(~a));
}
/* 
 * subOK - Determine if can compute x-y without overflow
 *   Example: subOK(0x80000000,0x80000000) = 1,
 *            subOK(0x80000000,0x70000000) = 0, 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int subOK(int x, int y) {
	int c1=1<<31;
	int c2=~c1;
	int y1=~y;
	int x_val=x&c2;
	int y_val=(y1&c2)+1;
	int x_sign=x>>31;
	int y_sign=y1>>31;
	int val=x_val+y_val;
	int sign=x_sign+y_sign+((val>>31)&1);
	int result=!(((sign>>1)^sign)&1);
  	//if the most sig bit and the flowed sig bit of (x-y) is the same,then subOK
  	return result;
}
/* 
 * isGreater - if x > y  then return 1, else return 0 
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isGreater(int x, int y) {
	int c1=1<<31;
	int c2=~c1;
	int y1=~y;
	int x_val=x&c2;
	int y_val=(y1&c2)+1;
	int x_sign=x>>31;
	int y_sign=y1>>31;
	int val=x_val+y_val;
	int sign=x_sign+y_sign+((val>>31)&1);
	int over_flow=((sign>>1)^sign)&1;
	int sub=x+y1+1;
	int greater=(!(sub>>31))&(!(!sub));
  return greater^over_flow;
  //if sub>0 and not overflow, greater
  //if sub<0 and overflow, greater
  //else smaller
}
//4
/*
 * bitParity - returns 1 if x contains an odd number of 0's
 *   Examples: bitParity(5) = 0, bitParity(7) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 4
 */
int bitParity(int x) {
	x=(x>>16)^x;
	x=(x>>8)^x;
	x=(x>>4)^x;
	x=(x>>2)^x;
	x=(x>>1)^x;
  return x&1;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
	int xx=x^(x>>31);//if x<0,then x=~x
	int c0=~0;
	int c1=c0<<16;
	int half1=!(xx&c1);//if there is 1s in the left half ,then half1=0, else half1=1
	int h1=~half1+1;//if there is 1s in the left half ,then h1=000...0, else h1=111...1
	int c2=((c1<<8)&(~h1)) | ((c1>>8)&h1);//if there is 1s in the left half ,then c2=c1<<8, else c2=c1>>8;
	int half2=!(xx&c2);
	int h2=~half2+1;
	int c3=((c2<<4)&(~h2)) | ((c2>>4)&h2);
	int half3=!(xx&c3);
	int h3=~half3+1;
	int c4=((c3<<2)&(~h3)) | ((c3>>2)&h3);
	int half4=!(xx&c4);
	int h4=~half4+1;
	int c5=((c4<<1)&(~h4)) | ((c4>>1)&h4);
	int half5=!(xx&c5);
	int result=((!half1)<<4)+((!half2)<<3)+((!half3)<<2)+((!half4)<<1)+(!half5)+1;
	result=result+(!(!xx));
  return result;
}
//float
/* 
 * float_half - Return bit-level equivalent of expression 0.5*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_half(unsigned uf) {
	int frac_ones=0x007fffff;
	int exp_ones=0x7f800000;
	int sign_ones=0x80000000;
	int frac=uf&frac_ones;
	int exp=uf&exp_ones;
	int sign=uf&sign_ones;
	int newexp=exp-0x800000;
	if( (exp==exp_ones) )//&& !frac)
		return uf;//NaN
	if(exp==0x800000){
		exp=0;
		frac=frac|0x800000;
	}
	if(!exp){
		int out=frac&1;
		int newfrac=((frac>>1)&frac_ones);
		int remain=newfrac&1;
		newfrac+=out&remain;
		return sign|exp|newfrac;
	}
  	return sign|newexp|frac;
}
/* 
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
	int p=0x80000000,sign=x&p,e=31,exp;
	unsigned frac;
	if(x==0)
	return 0;
	if(x<0) x=-x;
	frac=x;
	while((p&x)==0 && e>0){
		x<<=1;
		e--;
	}
	exp=(e+127)<<23;
	if(e>23){
		int b0=frac,b1,b2,mv=e-24;
		frac>>=mv;
		b1=frac&1;
		b0-=frac<<mv;
		frac>>=1;		
		b2=frac&1;
		if(b1){
			frac+=b0||b2;
		}
		if(frac&0x1000000){
			frac>>=1;
			exp+=0x800000;
		}
	}
	else{
		frac<<=(23-e);
	}
	frac&=0x7fffff;
  return sign|exp|frac;
}
/* 
 * float_f2i - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int float_f2i(unsigned uf) {
	int sign=uf&0x80000000, exp=uf&0x7f800000, frac=uf&0x7fffff, e=(exp>>23)-127;
	if(e>=31){
		return 0x80000000;
	}
	if(e<-1){
		return 0;
	}
	frac|=0x00800000;
	if(e>=23){
		frac<<=(e-23);
	}
	else{
		frac>>=(23-e);
		/*
		int b0=frac,b1,b2,mv=22-e;
		frac>>=mv;
		b1=frac&1;
		b0-=frac<<mv;
		frac>>=1;
		b2=frac&1;
		if(b1){
			frac+=b0||b2;
		}*/
	}
	if(sign) frac=~frac+1;
  return frac;
}
