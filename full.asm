        ORIG 0x40
ZERO:	0
TOZERO:	ZERO
tofunct:		funct
		
        ORIG 0x1000
MSP:	0xFFFF
MSPL:	0XFF00
        
begin:  NOP
		// set up stack pointer and stack limit
		LDA   MSP    
		AND   SP,A,A
		LDD   MSPL
		OR    SPL,D,D

		// now that stack is set up, call a function
        CALL  I tofunct

		// different registers, execute instructions
        CLA   INA
        CLB   DCB
        CLC   CMC
        CLD   CMD DCD

		// basic memory reference instructions
		// all the instructions
		// all the registers
		// addressing modes
        ADDA  M7FFF
        ADDA  M7FFF          // cause overflow
        SUBB  MA52
        LDB   MAA
        MULB  D16
        DIVB  D16
		DIVB I TOZERO
        ANDD  MA52
        ORA   MA52
        XORC  MA52

		CLA CLL INA
		INA
		MULA  M7FFF         // cause overflow
		CLB CLL INB
		LDA  M8000
		MUL  B,B,A          // no overflow
		CMB INB


		// register to register
        XOR   A,A,A          // should clear A
        OR    A,B,B          // copies B to A
		AND   C,SPL,SPL      // gets the stack limit register
		OR    D,SP,SP        // gets the stack pointer
		SUB   B,D,C          // compute the size of the stack
		OR    B,PC,PC        // get PC
		CALL  getPC          // should put PC on stack
RA:		SUB   C,A,B          // diff of two PC's
		MULC  D16
		CLD, IND
		IND
		MOD   B,C,D          // mod
		DIV   A,C,D
		LDA   MAA
		CLD DCD
		DIV   B,A,D
		MOD   B,A,D          // what is mod for -1?
		cld
		mod   B,A,D
		ADD   A,A,A
		MUL   A,A,A
		DIV   d,d,d

		// push and pop
        PUSH  MAA
        POP   MAA

		// isz and skip
		cla cma dca
		sta who
		isz who
		isz who
who:	

		// operate instructions
        CLA,DCA
        SZA,SMA
        HLT        // should be skipped
        SZA SMA RSS
        NOP        // should be executed

		CLL CML SNL RSS
		NOP
		SNL RSS
		NOP

		// I/O -- output instructions
		LDA  ToAstring
		CALL prints

        HLT

				// simple function, just return
funct:  RET

savePC:	0
GetPC:	POP  savePC  // get Return Address off stack
		LDA  savePC
		OR   PC,A,A  // move Return Address to PC
		HLT          // should never get here

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


// 
//  constants
D16:    16
MAA:    0xAA
MA52:   0xa52
M7FFF:  0x7FFF
M8000:  0x8000

ToAstring:		Astring
Astring:		'D'
		'o'
		'n'
		'e'
		10
		0
		
        END begin
        
