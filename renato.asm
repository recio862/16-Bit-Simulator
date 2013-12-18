 ORIG 0x40
ZERO:    0
TOZERO:    ZERO
        
        ORIG 0x1000
MSP:    0xFFFF
MSPL:    0XFF00
        
begin:  NOP
        NOP
        // set up stack pointer and stack limit
        LDA   MSP    
        AND   SP,A,A
        LDD   MSPL
        OR    SPL,D,D



        // different registers, execute instructions
        CLA   INA
        CLB   DCB
        CLC   CMC
        CLD   CMD DCD
        CLA
        // basic memory reference instructions
        // all the instructions
        // all the registers
        // addressing modes
       
        // basic memory reference instructions
        // all the instructions
        // all the registers
        // addressing modes
        ADDA  D12
        SUBA  D2
        CLA
        ADDA MFFFF
        SUBA MFFFF
        CLA
        ADDA MFFFF
        SUBA D12
        CLA
        ADDA D12
        SUBA MFFFF
        CLA
        ADDA M8000
        SUBA D12
        CLA
        ADDA MFFFF
        SUBA M8000
D12:    1
D2:    16
D2A:    0
MAA:    0x0FFF
MA52:   0xa52
M7FFF:  0x7FFF
M8000:  0x8000
MFFFF:  0xFFFF
        END begin
		
