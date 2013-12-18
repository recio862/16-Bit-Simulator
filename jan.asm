		orig 0x100

//
// A program to add some numbers
//

//
// print a string.  Address of string is in A register
//
ps0:      0
prints:   sta  ps0		// store pointer, for indirect addressing
ps1:	  lda  i ps0
		  sza rss
		  ret           // return on zero (end of string)
		  iota 4,0      // print character
		  isz  ps0
		  jmp ps1
		      

// print contents of A as a decimal number
// uses a (input), b (10), c (digit)
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
		
printn:	ldb  pradd
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
		dca            // --prptr;
		sta prptr
		ldc i prptr    // load the digit
		iotc 4,0       // print the digit
		suba pradd
		sma sza
		jmp pr3
		ret


//
//   read a decimal number from input and return the value
//   if there is a number, skip; if EOF, do not skip
//   uses a (running total), b (10), c (input char), d (comparison)
//
rra:   0
readn: pop   rra      // get return address

	   cla            // a holds running sum
	   ldb m10        // b holds 10

rn1:   iotc   3,0     // read character
	   smc rss
	   jmp i rra      // EOF causes exit with A=0

	   subc czero     // c - '0'
	   smc rss
	   jmp rn1        // bad input, skip and keep reading
	   sub d,c,b      // (c - '0') < 10?
	   smd
	   jmp rn1        // still bad input,

//  we have skipped all the non-digit input before the number
//  start reading the number
	   cla 
rn2:   mul a,a,b      // a = a*10
	   add a,a,c      // a = a*10 + c - '0'
	   iotc   3,0     // read character
	   subc   czero   // c - '0'
	   smc rss
	   jmp rn3        // bad input, done with number
	   sub d,c,b      // (c - '0') < 10?
	   smd rss
	   jmp rn2        // loop back for another character

rn3:   isz rra
	   jmp i rra




///////////////////////////////////		
//   Main program.
//
//   1. write a prompt.
//   2. read numbers until EOF (0xFFFF)
//   3. add all numbers together
//   4. print numbers and sum
//

stsize:	  40
sum:    0
loopctr: 0
ninputs: 0
nptr:    0
naddr:  addr_of_numbers

main:   cla    	dca     // a = 0xFFFF
		and 	sp,a,a  // move a to sp
		suba 	stsize
		and 	spl,a,a // move a minus stack size to spl
		
		lda     prompt
		call 	prints             // print the prompt
		
		cla                        // set up count and array for numbers
		sta		ninputs
		lda		naddr
		sta     nptr
iloop:	call	readn              // read a number
		jmp		endinput           // if EOF
		sta I	nptr               // save the number
		isz		nptr
		isz		ninputs            // increase count of numbers
		jmp		iloop

// EOF on input, now add numbers
endinput: 
	    lda		ninputs
		cma ina
		sta		loopctr            // define -n as loop count

		lda 	naddr
		sta		nptr               // rest pointer to numbers
		cld cll                    // d holds running sum   
oloop:	lda I   nptr               // print number
		call	    printn

		lda I   nptr               // add number
		add     d,d,a
		smd snl rss				// check for overflow
		jmp     overflow

		isz     nptr
		isz		loopctr
		jmp     oloop1
		jmp     oloop2
oloop1:	lda     separator1         //  + 
		call	prints
		jmp		oloop

//    print sum
oloop2: lda	    separator2         // = 		
		call	prints
		and     a,d,d			   // print sum in d
		call    printn

oloop3: 
	    lda	    separator3         // newline 		
		call	prints
		hlt

overflow:  
		lda     separator2
		call    prints
		lda     overmsg
		call	prints
		jmp		oloop3

overmsg: overs
overs:	'*'
	'*'
	'*'
	' '
	'O'
	'v'
	'e'
	'r'
	'f'
	'l'
	'o'
	'w'
	' '
	'*'
	'*'
	'*'
	0


///////////////////////////////////		
prompt:  prompts
prompts: 'P'
	'l'
	'e'
	'a'
	's'
	'e'
	' '
	't'
	'y'
	'p'
	'e'
	' '
	'n'
	'u'
	'm'
	'b'
	'e'
	'r'
	's'
	' '
	't'
	'o'
	' '
	'a'
	'd'
	'd'
	':'
	' '
	0

separator1: str1
str1:	' '
	'+'
	' '
	0


separator2: str2
str2:	' '
	'='
	' '
	0

separator3: str3
str3:	10
	0

//   numbers continue as long as memory allows
addr_of_numbers:

		end main
