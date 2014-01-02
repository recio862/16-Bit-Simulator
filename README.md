16-Bit-Simulator
================

This is a project designed by our 429 Computer Architecture group. 

Use:

1. Run the assembler using a single .asm file as input.


2. Run the PDP16429 using the .obj file produced by the assembler. 

       Command line includes:
       
        Option 1: ./pdp16429 filename 
        
        Option 2: ./pdp16429 -V filename 
        
   Option 1 will produce the output from standard IOT instructions
   
   Option 2 will produce the verbose output, displaying each cycle and instruction from the PC.
    Verbose mode prints one line for each instruction executed, and it includes:
    The time (in cycles) after each instruction is executed.
    The PC of the instruction executed.
    The hex of the entire instruction executed.
    The symbolic opcode(s) of the instruction executed.
    The names and values of all memory and registers that are referenced by the exection of the instruction. 
    Each instruction takes 1 cycle for every memory reference (including the instruction fetch), but no additional       time for any computation. So, time is dominated by just the memory fetch time. 
  
  
  Note: filename must be of type .obj produced by the assembler
  

===================

 Memory
Memory is 16-bit words. Memory addresses are 16-bit, word-addressable. This allows up to 65,536 16-bit words of memory.

Integers are represented in two's complement binary notation. There is no hardware support for floating point numbers. Characters are 8-bit ASCII.
Registers
All registers are 16 bits (except the Link bit). There are 4 general purpose registers: A, B, C, D.

The Link bit is one bit. It is never cleared by hardware. It is set whenever an arithmetic operation causes an overflow (ADD/SUB/Increment/Decrement/MUL) or a divide by zero (DIV).

In addition, there are 4 special purpose registers: stack pointer (SP), stack pointer limit (SPL), program counter (PC), and processor status word (PSW), The low order bit of the processor status word indicates if we are running or halted.

The stack grows down in memory; the stack pointer (SP) points to an empty location. Pushing to the stack stores in the word that the stack pointer points to, and then the stack pointer is decremented by one. Popping from the stack first increments the stack pointer by one, and then uses the word that the stack pointer points at. Errors include: (a) stack overflow, if SP < SPL when a stack push starts and (b) stack underflow, if SP wraps around to zero (SP = 0xFFFF when a stack pop starts).

The program counter (PC) is the address of the next instruction. After an instruction is fetched, the PC is incremented by one to point to the next instruction. A skip instruction may increment by one more. A jump, call or return instruction may reset the PC to another value. Arithmetic on the PC is modulo 16 bits, and does not cause overflow.
Instructions
There are 6 classes of instructions:

    Non-register, Non-memory Instructions
    Register Memory Reference Instructions
    I/O Transfer Instructions
    Non-register Memory Reference Instructions
    Register-to-Register Instructions
    Non-memory Register Instructions 

Non-register, Non-memory Instructions
There are a small number of non-register, non-memory reference instructions. These instructions use the entire 16 bits as an opcode. The high-order 6 bits of these instructions are 000000.

    000000.0000000000 -- NOP.
    000000.0000000001 -- HLT. The low-order bit of the PSW is set to 0; the machine halts.
    000000.0000000010 -- RET. Pop the stack into the PC. 

Register Memory Reference Instructions
This class of instructions specifies a 4-bit opcode, a 2-bit register selector, and a 10-bit memory address selector.

The 2-bit register selector determines the general purpose register used in the instruction:

    00 -- A register
    01 -- B register
    10 -- C register
    11 -- D register 

The memory address selector includes a zero/current page indicator (Z/C), a direct/indirect (D/I) indicator, and an 8-bit page offset. These are used to determine a memory address, just as with the PDP-8, except using pages of 256 words. The memory-operand is the contents of that memory address.

The opcodes are:

    0001 -- ADD*: the register + memory-operand -> Reg
    0010 -- SUB*: the register - memory-operand -> Reg
    0011 -- MUL*: the register * memory-operand -> Reg
    0100 -- DIV*: the register / memory-operand -> Reg
    0101 -- AND*: the register & memory-operand -> Reg
    0110 -- OR*: the register | memory-operand -> Reg
    0111 -- XOR*: the register ^ memory-operand -> Reg
    1000 -- LD*: memory-operand -> Reg
    1001 -- ST*: the register -> memory-operand 

The "*" in the opcode indicates the register selected: ADDA, ADDB, ADDC, ADDD, and so on.
I/O Transfer Instructions
The IOT instruction has a 4-bit opcode of 1010, a 2-bit register selector, a 7-bit device number and a 3-bit function field.

The 2-bit register selector is the same as for the Register Memory Reference Instructions.

    1010 -- IOT*: the register, function -> Device 

Non-register Memory Reference Instructions
There are a small number of non-register memory reference instructions. These instructions combine the 4-bit opcode and 2-bit register fields of the Register Memory Reference Instructions to define a 6-bit opcode.

The address of the memory-operand is computed the same as for the Register Memory Reference Instructions.

The opcodes are:

    1011.00 -- ISZ: memory-operand + 1 -> memory-operand; if memory-operand == 0, PC + 1 -> PC
    1011.01 -- JMP: address of memory-operand -> PC
    1011.10 -- CALL: push return address (PC + 1) on stack; address of memory-operand -> PC
    1100.00 -- PUSH: push memory-operand to the stack
    1100.01 -- POP: pop top of stack and store in memory-operand. 

Register-to-Register Instructions
The register-to-register instructions operate only on registers. A 3-bit register specifier is used to define a register. The register-to-register instructions are 3-operand instructions. Each instruction includes three 3-bit register specifiers (i,j,k) and define an instruction Rj op Rk -> Ri. The instruction specifies a 4-bit opcode of 1110 followed by a 3-bit sub-opcode, and three 3-bit register specifiers.

The register specifiers are:

    000 -- A register
    001 -- B register
    010 -- C register
    011 -- D register
    100 -- PC register (program counter)
    101 -- PSW register (processor status word)
    110 -- SP register (stack pointer)
    111 -- SPL register (stack pointer limit) 

The instructions are:

    1110.000 -- MOD: Reg[j] % Reg[k] -> Reg[i]
    1110.001 -- ADD: Reg[j] + Reg[k] -> Reg[i]
    1110.010 -- SUB: Reg[j] - Reg[k] -> Reg[i]
    1110.011 -- MUL: Reg[j] * Reg[k] -> Reg[i]
    1110.100 -- DIV: Reg[j] / Reg[k] -> Reg[i]
    1110.101 -- AND: Reg[j] & Reg[k] -> Reg[i]
    1110.110 -- OR: Reg[j] | Reg[k] -> Reg[i]
    1110.111 -- XOR: Reg[j] ^ Reg[k] -> Reg[i] 

Non-memory Register Instructions
This is like the operate instruction for the PDP-8.

The 2-bit register selector determines which general purpose register is used for this instruction. The encoding is the same as for the Register Memory Reference Instructions.

The individual bits for the Non-memory Register Instruction are:

    SM* -- Skip if the register is negative (sign bit is 1)
    SZ* -- Skip if the register is zero
    SNL -- Skip if the Link bit is non-zero
    RSS -- Reverse the Skip Sense
    CL* -- Clear the register
    CLL -- Clear the Link bit
    CM* -- Complement the register
    CML -- Complement the Link bit
    DC* -- Decrement the register by one
    IN* -- Increment the register by one 

The "*" in the opcode indicates the register selected: SMA, SMB, SMC, SMD, and so on.

The bits are evaluated in the order listed above. 
