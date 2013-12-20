16-Bit-Simulator
================

Code designed to simulate a 16 bit computer

Use:
1. Run the assembler using a single .asm file as input.
2. Run the PDP16429 using the .obj file produced by the assembler. 
       Command line includes:
        Option 1: ./pdp16429 filename 
        Option 2: ./pdp16429 -V filename 
   Option 1 will produce the output from standard IOT instructions
   Option 2 will produce the verbose output, displaying each cycle and instruction from the PC.
     
  Note: filename must be of type .obj produced by the assembler

