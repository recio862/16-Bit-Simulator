//
//  A program to print all the prime numbers
//
stsize:	  40
n:		  1		// the number we are working on
ascii_nl: 10
limit:    2000

start:	cla dca     // a = 0xFFFF
		and sp,a,a  // move a to sp
		suba stsize
		and spl,a,a // move a minus stack size to spl
		
		cld ind     // keep number we are working on in D; start D=1

label1:	and a,d,d   // move d to a
		call prime  // check if d is prime
		jmp  label2 // d is not prime
		and a,d,d   // move d to a
		call print  // print number
		lda ascii_nl
		iota 4      // print newlne

label2:	ind         // get next number
		lda limit   // compare D and limit
		sub a,a,d
		sma         // if limit < n, halt
		jmp label1
		hlt


		// print contents of A as a decimal number
parray:	0
		0
		0
		0
		0
		0
pradd:	parray
prptr:	parray
m10:	10
czero:	'0'
		
print:	ldb  pradd
		stb  prptr    // set up pointer
		ldb  m10      // base 10 printing
pr2:	mod  c,a,b    // get low-order digit (c = a mod 10)
		addc czero    // convert to character code
		stc  i prptr  // save digit
		isz  prptr    // increment pointer
		div  a,a,b    // divide a/10 -> a
		sza 
		jmp pr2       // non-zero means print char array

pr3:	lda prptr
		dca
		sta prptr
		ldc i prptr    // load the digit
		iotc 4,0       // print the digit
		suba pradd
		sma sza
		jmp pr3
		ret


// determine if the number in a is prime.
// divide by every number from 2 to a-1
pra:	0
prime:	pop pra      // get return address
		clb inb      // start at 1
p1:		inb          // try next value
		mod  c,a,b   // divide a by b, look at remainder
		szc rss
		jmp i pra    // mod is zero; divisible by b; not prime
		div  c,a,b   // check if a/b < b
		sub  c,c,b
		smc szc 
		jmp p1
		isz pra      // a is prime
		jmp i pra
		end start
		
