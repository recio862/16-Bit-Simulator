/* ***************************************************************** */
/*   Program designed to interpret an object code assembled file     */
/*   and simulate a custom 16-bit computer environment               */  
/*   																 */
/*																	 */
/*     Name: Renato Recio                                            */
/*     Date: 12/12/2013                                              */
/* ***************************************************************** */


/*  Includes  */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* Definitions */
typedef char* String;
typedef short Boolean;
#define TRUE 1
#define FALSE 0
typedef short Tracetype;
#define VALUE 2
#define MEMORY 1
#define REGISTER 0


/* Variable declarations */
void execute();
Boolean verboseFlag;
Boolean jmp;
Boolean initial_code;
Boolean initial_trace;
Boolean nop;
long long time;
char code[50];
char trace[150];
FILE *input;

/* Function prototypes */
int decode_object_file_format(int memory[], Boolean defined[]);
int find_opcode(int opcode);
int non_register_non_memory(int memory[], int pc);
int register_memory(int memory[], int pc, int opcode);
int input_output(int memory[], int pc);
int non_register_memory(int memory[], int pc);
int register_register(int memory[], int pc);
int non_memory_register(int memory[], int pc);
void set_registers();
int get_register_value(int register_code);
int set_register_value(int register_value, int register_code);
int pop_stack();
int push_stack(int value);
int arithmetic_instruction(int opcode, int value1, int value2, int reg_j, int reg_k, int reg_i);
int concatenate_codes(String instruction, int register_code, Boolean use_instruction);
int concatenate_trace(int effective_address, int contents_of_address,  Tracetype type1, Tracetype type2, int register_code);
int trace_register_concatenator(int register_code);

/* Registers */
int register_A, register_B, register_C, register_D; // General purpose registers
int link_reg; //Link register
int SP_reg, SP_limit_reg, PC_reg, PSW_reg; //Stack pointer, stack pointer limit, program counter, processor status word
int stack[65536]; //Stack 

int main(int argc, char  **argv)
{   

    int namenotgiven = TRUE;

    /* main driver program.  Define the input file from either standard input or
       a name on the command line.  Process all arguments. */

           if (argc > 1)
        {
            argc--, argv++;
            
                /* Check to see if the verbose flag is set */
            if (**argv == '-' && ((argv[0][1] == 'v') || (argv[0][1] == 'V'))){
            argc--,argv++;
            namenotgiven = FALSE;
            input = fopen(*argv, "rb");
			
          if (input == NULL)
                        {
                            fprintf(stderr, "Wrong command in the command line, Can't open %s\n", *argv);
                            exit(1);
                        }
                        verboseFlag = TRUE; //Verbose flag set
                execute(); //begin execution 
                fclose(input);
            }
         
            else
                {
                    namenotgiven = FALSE;
                   
                    input = fopen(*argv, "rb");
                    if (input == NULL)
                        {
                            fprintf(stderr, "Wrong command in the command line, Can't open %s\n", *argv);
                            exit(1);
                        }
                        verboseFlag = FALSE; //Verbose flag not set
                   execute(); //begin execution
                    fclose(input);
                }
        }

       /* Name not given */
    if (namenotgiven == TRUE)
        {
            fprintf(stderr, "Wrong command in the command line");
                            exit(1);
        }

    exit(0);
}
///////////////////////////////////////////////////////////////////////////////
//Read two bytes from the object file format and merge into one 16 bit integer
///////////////////////////////////////////////////////////////////////////////
int read_two_bytes(){
    int result = 0;
    int left_half = getc(input);
     if (left_half == EOF)  {printf("EOF too early");exit(1);}
     int right_half = getc(input);
     if (right_half == EOF) {printf("EOF too early");exit(1);}
    result = (left_half << 8) + right_half;
    return result;
    
}

////////////////////////////////////////////////////////////////////////////////
//Opcode Table - Find which type of instruction to execute based on 4-bit opcode
////////////////////////////////////////////////////////////////////////////////
int find_opcode(int opcode){
    int i = 0;
    
    //Define possible opcode values
    int non_register_non_memory = 0;
   int register_memory[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9}; 
   int input_output = 10;
   int non_register_memory[2] = {11, 12};
   int register_register = 14;
   int non_memory_register = 15;
   
   //Compare possible values with real opcode and return result
   if (opcode == non_register_non_memory) return 1; //1 = Non register non memory instruction
   for (i = 0; i < 9; i++) if (opcode == register_memory[i]) return 2; //2 = register memory instruction
   if (opcode == input_output) return 3; //3 = input output instruction
   for (i = 0; i < 2; i++) if (opcode == non_register_memory[i]) return 4; // 4 = non register memory instruction
   if (opcode == register_register) return 5; // 5 = register register instruction
   if (opcode == non_memory_register) return 6; // 6 = non memory register instruction
   
   return -1; //Invalid opcode
}


///////////////////////////////////////////////////////////////////////////////
//Decode object file and insert instructions into appropriate memory locations
///////////////////////////////////////////////////////////////////////////////
int decode_object_file_format(int memory[], Boolean defined[]){
    char objg[5] = {'O','B','J','G'};
    char ch;
    int c, i, entry_point, address, instruction, block_size, left_half, right_half;
    c = getc(input);
    i = 0;
    /* Check header for OBJG format and grab entry point address*/
    while (c != EOF && i <= 4){
          if (i <= 3){
          ch = (char)c;
          if (ch != objg[i]){
          fprintf(stdout, "Wrong header format");
          exit(1);
          }
          if (i < 3)
           c = getc(input);}
          else{
              entry_point = read_two_bytes();
               }
          i++;
          }

      c = getc(input);
     /* Process rest of file */
     while (c != EOF){
           
           block_size = c;
          
           if (block_size < 5 || (block_size % 2 == 0)) {fprintf(stdout, "Invalid block size");exit(1);}
           
           address = read_two_bytes();
           
           for (i = block_size - 3; i >= 2; i -=2){
               instruction = read_two_bytes();
               memory[address] = instruction;
               defined[address] = TRUE;
               //printf("Address: 0x%04X Instruction: 0x%04X\n", address, instruction);
               address++;
               }
          
           c = getc(input);
           }
     return entry_point;
}

///////////////////////////////////////////////////////////////////////////////
// Set the register values to default
///////////////////////////////////////////////////////////////////////////////
void set_registers(){
     int i;
     register_A = 0;
     register_B = 0;
     register_C = 0;
     register_D = 0; 
     link_reg = 0; 
     SP_reg = 0;
     SP_limit_reg = 0;
     PC_reg = 0;
     PSW_reg = 0;
     for (i = 0; i < 65536; i++) stack[i] = -1;
     time = 0;
     }



///////////////////////////////////////////////////////////////////////////////
//Concatenate Single Register Code into Trace
///////////////////////////////////////////////////////////////////////////////
int trace_register_concatenator(int register_code){

switch(register_code){
          case(0):
                   strcat(trace, "A");  break;
          case(1):
                   strcat(trace, "B");break;
          case(2): 
                   strcat(trace, "C"); break;
          case(3):
                   strcat(trace, "D"); break;
          case(4):
                    strcat(trace, "PC"); break;
          case(5): 
                   strcat(trace, "PSW"); break;
          case(6): 
                    strcat(trace, "SP"); break;
          case(7):
                    strcat(trace, "SPL"); break;
          case(8): 
		    strcat(trace, "L"); break;
	  default: 
		    return -1;
                                           
     }

return 1;

}
///////////////////////////////////////////////////////////////////////////////
//Concatenate Memory / Register Trace into string
///////////////////////////////////////////////////////////////////////////////
int concatenate_trace(int value1, int value2, Tracetype type1, Tracetype type2, int register_code){
    char temp_trace[50] = {'\0'};
    int register_value = 0;
    
    if (initial_trace == TRUE)
    initial_trace = FALSE;
    else 
    strcat(trace, ", ");
    
    if (type1 == MEMORY){
    sprintf(temp_trace, "M[0x%04hX] -> 0x%04hX", value1, value2);
    strcat(trace, temp_trace);
    }
    if (type1 == VALUE && type2 == MEMORY){
    sprintf(temp_trace, "0x%04hX -> M[0x%04hX]", value1, value2); 
    strcat(trace, temp_trace);  
    }
    if (type1 == VALUE && type2 == REGISTER){
    sprintf(temp_trace, "0x%04hX -> ", value1);
    strcat(trace, temp_trace);
    trace_register_concatenator(register_code);
    }
    if (type1 == REGISTER){
    trace_register_concatenator(register_code);
    sprintf(temp_trace, " -> 0x%04hX", value2);
    strcat(trace, temp_trace);
    }
    
    
     
     return register_value;
    
}
///////////////////////////////////////////////////////////////////////////////
//Concatenate name of Register or Instruction type into string
///////////////////////////////////////////////////////////////////////////////
int concatenate_codes(String instruction, int register_code, Boolean use_instruction){
    int register_value = 0;
    
    if (initial_code == TRUE)
    initial_code = FALSE;
    else if ((use_instruction && register_code != -1) || register_code == -1)
    strcat(code, " ");
    
    
    if (use_instruction){
    strcat(code, instruction);
    }
    
    switch(register_code){
          case(0):
                   strcat(code, "A");  break;
          case(1):
                   strcat(code, "B");break;
          case(2): 
                   strcat(code, "C"); break;
          case(3):
                   strcat(code, "D"); break;
          case(4):
                    strcat(code, "PC"); break;
          case(5): 
                   strcat(code, "PSW"); break;
          case(6): 
                    strcat(code, "SP"); break;
          case(7):
                    strcat(code, "SPL"); break;
                                           
     }
     
     return register_value;
    
}

///////////////////////////////////////////////////////////////////////////////
//Get register value from a register code
///////////////////////////////////////////////////////////////////////////////
int get_register_value(int register_code){
    int register_value = 0;
    switch(register_code){
          case(0):
                  register_value = register_A & 0xFFFF; break;
          case(1):
                  register_value = register_B & 0xFFFF; break;
          case(2): 
                  register_value = register_C & 0xFFFF; break;
          case(3):
                  register_value = register_D & 0xFFFF; break;
          case(4):
                  register_value = PC_reg & 0xFFFF; break;
          case(5): 
                  register_value = PSW_reg & 0xFFFF; break;
          case(6): 
                  register_value = SP_reg & 0xFFFF; break;
          case(7):
                  register_value = SP_limit_reg & 0xFFFF; break;
                                           
     }
     
     return register_value;
    
}
///////////////////////////////////////////////////////////////////////////////
//Set the register code to the register value
///////////////////////////////////////////////////////////////////////////////
int set_register_value(int register_value, int register_code){

    switch(register_code){
          case(0):
                  register_A = register_value & 0xFFFF; break;
          case(1):
                  register_B = register_value & 0xFFFF; break;
          case(2): 
                  register_C = register_value & 0xFFFF; break;
          case(3):
                  register_D = register_value & 0xFFFF; break;
          case(4):
                  PC_reg = register_value & 0xFFFF; break;
          case(5): 
                  PSW_reg = register_value & 0xFFFF; break;
          case(6): 
                  SP_reg = register_value & 0xFFFF; 
                  stack_start = register_value & 0xFFFF; break;
          case(7):
                  SP_limit_reg = register_value & 0xFFFF; break;
                                           
     }
     
     return register_value;

}

///////////////////////////////////////////////////////////////////////////////
//Function for calculating arithmetic operations
///////////////////////////////////////////////////////////////////////////////
int arithmetic_instruction(int opcode, int val1, int val2, int reg_code_1, int reg_code_2, int reg_code_3){
    int result;
    signed short final_value, value1, value2,val1_sign, val2_sign;
    final_value = 0;

	
    value1 = (signed short) val1;
    value2 = (signed short) val2;
    val1_sign = 0x8000 & value1; 
    val2_sign = 0x8000 & value2;

    Boolean overflow = FALSE;
 	switch(opcode){
                             case(0): //MOD
				     				  concatenate_codes("MOD", -1, TRUE);
                                      if (value2 == 0){
                                                 overflow = TRUE; break;
                                                 }
                                     final_value = value1 % value2;  break;
                                     
                             case(1): //ADD
                                     final_value =  value1 + value2;
                                     if ((val1_sign & val2_sign) && ((final_value & 0x8000) == 0)) overflow = TRUE; 
                                     if ((!val1_sign & !val2_sign) && ((final_value & 0x8000))) overflow = TRUE;
                                     concatenate_codes("ADD", -1, TRUE);break;
			     
                              case(2): //SUB
                                       value2 = -value2;
                                     val2_sign = 0x8000 & value2;
                                     final_value = value1 + value2;
                                     if ((val1_sign & val2_sign) && ((final_value & 0x8000) == 0) && (value2 != 0x8000 && val2 != 0x8000)) overflow = TRUE;
                                     if ((!val1_sign & !val2_sign) && ((final_value & 0x8000)) && (value2 != 0x8000 && val2 != 0x8000)) overflow = TRUE;
                                     concatenate_codes("SUB", -1, TRUE);
                                     break;
			     
                             case(3): //MUL
                                     result =  val1 * val2;
				    				 final_value = (short) result;
				    				 if ((val1_sign & val2_sign) && ((final_value & 0x8000) == 0)) overflow = TRUE;
                                     if ((!val1_sign & !val2_sign) && ((final_value & 0x8000))) overflow = TRUE;
                                     concatenate_codes("MUL", -1, TRUE); break;
                             case(4): //DIV
                                      concatenate_codes("DIV", -1, TRUE); 
                                      if (value2 == 0){
                                                 overflow = TRUE; break;
                                                 }
                                     final_value =  value1 / value2; break;
                             case(5): //AND
                                      concatenate_codes("AND", -1, TRUE);
                                     final_value =  value1 & value2; break;
                             case(6): //OR
                                       concatenate_codes("OR", -1, TRUE);
                                     final_value =  value1 | value2; break;
                             case(7): //XOR
                                      concatenate_codes("XOR", -1, TRUE); 
                                     final_value =  value1 ^ value2; break;
                             case(8): //LD
                                      concatenate_codes("LD", -1, TRUE);
                                     final_value = value2; break;
                                     }
      
     
      //Check for overflow / underflow                                                               
     if ((((final_value > 0xFFFF) ) || (result > 0xFFFF) || overflow == TRUE) && (reg_code_3 != 4)){
                link_reg = 1;
	concatenate_trace(1, -1, VALUE, REGISTER, 8);
	}
                     
     final_value = final_value & 0xFFFF;   
   
    
	result = (int)final_value;
    return final_value;
}

///////////////////////////////////////////////////////////////////////////////
//Pop the stack and return the value
///////////////////////////////////////////////////////////////////////////////
int pop_stack(){
    int pop;
    concatenate_trace(-1, SP_reg,  REGISTER, VALUE, 6);
    if (SP_reg == 0xFFFF || SP_reg < 0){
	strcat(code, " STACK OVERFLOW");
	time--;
	PSW_reg = 0;
    return -1;
	} //Error
    SP_reg++;
    pop = stack[SP_reg];
    concatenate_trace(SP_reg, -1,  VALUE, REGISTER, 6);
    concatenate_trace(SP_reg, pop,  MEMORY, VALUE, -1);
    stack[SP_reg] = 100; //stack at SP location is No longer defined
    return pop & 0xFFFF;
}



///////////////////////////////////////////////////////////////////////////////
//Push value onto the stack
///////////////////////////////////////////////////////////////////////////////
int push_stack(int value){
    concatenate_trace(value, SP_reg,  VALUE, MEMORY, -1);
    
    if (SP_reg < SP_limit_reg){
    PSW_reg = 0; 
    time--;
    strcat(code, " STACK OVERFLOW");  
    return -1; //Error 
	}
    stack[SP_reg] = value;
    SP_reg--;
    
    if (SP_reg == 0)
    SP_reg = 0xFFFF;
  
    concatenate_trace(SP_reg, -1,  VALUE, REGISTER, 6);  
 
    return value & 0xFFFF;
}



///////////////////////////////////////////////////////////////////////////////
//Begin execution to Simulate the PDP 429
///////////////////////////////////////////////////////////////////////////////
void execute(){
     
     int pc, entry_point, opcode, instruction_type,i;
     Boolean invalid_instruction = FALSE;
     int memory[65536] = {0};
     Boolean defined[65536] = {FALSE};
     entry_point = decode_object_file_format(memory, defined);
     set_registers();
     PC_reg = entry_point;
     PSW_reg = 1;
     for (pc = PC_reg; pc < 65536; pc++){
         
           if (defined[pc]){
		       jmp = FALSE;
                       nop = FALSE;
                       pc = PC_reg;
		       
                        initial_code = TRUE; initial_trace = TRUE; 
                         opcode = memory[pc]>>12; //mask to obtain 4-bit opcode        
                          instruction_type = find_opcode(opcode);
                                for (i = 0; i < 50; i++)  code[i] = '\0'; //Reset code for next string
				for (i = 0; i < 150; i++) trace[i] = '\0'; //Reset register code for next string
                                  switch(instruction_type){
                                   case(1): non_register_non_memory(memory, pc); break;
                                   case(2): register_memory(memory, pc, opcode); break;
                                   case(3): input_output(memory, pc); break;
                                   case(4): non_register_memory(memory, pc); break;
                                   case(5): register_register(memory, pc); break;
                                   case(6): non_memory_register(memory, pc); break;
                                   default: invalid_instruction = TRUE; break;     
                                       }   
                                       if (invalid_instruction){ invalid_instruction = FALSE; continue; }
     
              				if (verboseFlag){
                                       fprintf(stderr, "Time %3lld: PC=0x%04X instruction = 0x%04X (%s)",
                                           time, pc, memory[pc], code);
                                       if (!nop) fprintf(stderr, ": %s\n", trace);   
				       else fprintf(stderr, "\n");
   					}


                                           while (pc >= 65535) pc = pc - 65536;
                   
     
                    }
                    
             
             if (PC_reg == pc && !jmp)
             PC_reg++;
             else
             pc = PC_reg;
              if (PSW_reg != 1) break; //HLT program
              }
     
     }

///////////////////////////////////////////////////////////////////////////////
//Non Register Non Memory Instructions
///////////////////////////////////////////////////////////////////////////////
int non_register_non_memory(int memory[], int pc){
    int stack_value;
    if (memory[pc] == 0)
    nop = TRUE;
    if (memory[pc] == 1){
    PSW_reg = 0;
    concatenate_codes("HLT", -1, TRUE);
    concatenate_trace(-1, 1,  REGISTER, VALUE, 5);
    concatenate_trace(0, -1, VALUE , REGISTER, 5);  
    }
    if (memory[pc] == 2){
    stack_value = pop_stack();
              if (stack_value != -1){
                 PC_reg = stack_value;
		 concatenate_trace(PC_reg, -1,  VALUE, REGISTER, 4);
                 concatenate_codes("RET", -1, TRUE);
	             	time++;
                 }
              else
                  nop = TRUE;
    }
    
    if (nop)
    concatenate_codes("NOP", -1, TRUE);
    
    time++;
    return pc;
}

///////////////////////////////////////////////////////////////////////////////
//Register-Memory Instructions
///////////////////////////////////////////////////////////////////////////////
int register_memory(int memory[], int pc, int opcode){
    int final_value, register_value, register_code, effective_address, contents_of_address;
    Boolean store = FALSE;
    Boolean overflow = FALSE;
    Boolean load = FALSE;
    register_code = (memory[pc] >> 10) & 0x3;
    register_value = get_register_value(register_code);
    effective_address = memory[pc] & 0x00FF;
    if (opcode == 9)
                    store = TRUE;
    if (opcode == 8)
		    load = TRUE;
    
    if (memory[pc] & 0x0100){ //check Z/C bit for page address
                   effective_address = effective_address | (pc & 0xFF00);
                   }
                   
    contents_of_address = memory[effective_address];               
    
    if ((!(memory[pc] & 0x0200)) && !load && !store) {
	concatenate_trace(-1, register_value,  REGISTER, VALUE , register_code);
	concatenate_trace(effective_address, contents_of_address, MEMORY, VALUE , register_code); 
	}
    else if (!store) concatenate_trace(effective_address, contents_of_address,  MEMORY, MEMORY, register_code);
    
  
    
    if (memory[pc] & 0x0200){ //check D/I bit for indirection
		if ((opcode != 8) && !store) concatenate_trace(-1, register_value,  REGISTER, VALUE , register_code);
		 else if (store)concatenate_trace(effective_address, contents_of_address,  MEMORY, MEMORY, register_code);
                   effective_address = contents_of_address;
                   contents_of_address = memory[effective_address];
                   time++;
                   concatenate_codes("I", -1, TRUE);
		
		 if (!store) concatenate_trace(effective_address, contents_of_address,  MEMORY, MEMORY, register_code);
                   }
                   
    
   
      
    if (!store){
       final_value = arithmetic_instruction(opcode, register_value, contents_of_address, -1, -1, register_code);
       set_register_value(final_value, register_code);
	}
    else{ 
          concatenate_codes("ST", -1, TRUE); 
       memory[effective_address] = register_value;
        concatenate_trace(-1, register_value,  REGISTER, VALUE, register_code);
           }
      
       concatenate_codes(" ", register_code, FALSE);   
    if (store) concatenate_trace(register_value, effective_address,  VALUE, MEMORY, register_code);
    else  concatenate_trace(final_value, -1,  VALUE, REGISTER, register_code);
     time+=2;
    return pc;
}

///////////////////////////////////////////////////////////////////////////////
//I/O Transfer Instructions
///////////////////////////////////////////////////////////////////////////////
int input_output(int memory[], int pc){
char ch;
int register_code, device, c, eof;
eof = 0xFFFF;
register_code = (memory[pc] >> 10) & 0x3;
device = (memory[pc] >> 3) & 0x7F;
 strcat(code, "IOT ");

if (device == 3){
c = getchar();
if (c == eof){
set_register_value(eof, register_code);
concatenate_trace(eof, -1,  VALUE, REGISTER, register_code);
}
else{
set_register_value(c, register_code);
concatenate_trace(c, -1,  VALUE, REGISTER, register_code);
}
 strcat(code, "3");
}

if (device == 4){
c = get_register_value(register_code) & 0xFF;
concatenate_trace(-1, c,  REGISTER, VALUE, register_code);
ch = (char)c; 
printf("%c", ch);
 strcat(code, "4");
}

time++;
return pc;
}

///////////////////////////////////////////////////////////////////////////////
//Non-Register Memory Instructions
///////////////////////////////////////////////////////////////////////////////
int non_register_memory(int memory[], int pc){
    int stack_value, memory_operand, effective_address, contents_of_address;
    Boolean nop = FALSE;
    int opcode = memory[pc] >> 10;
    effective_address = memory[pc] & 0x00FF;
    
    if (memory[pc] & 0x0100){ //check Z/C bit for page address
                   effective_address = effective_address | (pc & 0xFF00);
                   }
                   
    contents_of_address = memory[effective_address];               
                   
    if (memory[pc] & 0x0200){ //check D/I bit for indirection
		   concatenate_trace(effective_address, contents_of_address,  MEMORY, VALUE, -1);
                   
                   effective_address = contents_of_address;
                   contents_of_address = memory[effective_address];
                   time++;
                   concatenate_codes("I", -1, TRUE);
                   }
    
    
           switch(opcode){
                          case(0x2C): //ISZ
					  concatenate_trace(effective_address, contents_of_address,  MEMORY, VALUE, 4);
                                          contents_of_address = contents_of_address + 1;
                                          contents_of_address = contents_of_address & 0xFFFF;
					  memory[effective_address] = contents_of_address;
					  concatenate_trace(contents_of_address, effective_address,  VALUE, MEMORY, 4);
                                          if (contents_of_address == 0){
                                          PC_reg = (PC_reg + 2) & 0xFFFF;
                                          concatenate_trace(PC_reg, -1,  VALUE, REGISTER, 4);
                                          }
                                           concatenate_codes("ISZ", -1, TRUE);
                                           time+=3;
                                          break;
                          case(0x2D): //JMP
                                          PC_reg = effective_address;
					  jmp = TRUE;
                                          concatenate_trace(PC_reg, -1,  VALUE, REGISTER, 4);
                                           concatenate_codes("JMP", -1, TRUE); 
                                           time++;
                                          break;
                          case(0x2E): //CALL
					  concatenate_codes("CALL", -1, TRUE);
                                          if (PC_reg == 65535)
                                          push_stack(0);
                                          else
                                          push_stack((PC_reg) + 1); 			  
                                          PC_reg = effective_address;
                                           concatenate_trace(PC_reg, -1,  VALUE, REGISTER, 4);
                                           time+=2;
                                          break;
                          
                          case(0x30): //PUSH
					
					 concatenate_codes("PUSH", -1, TRUE);
					  concatenate_trace(effective_address, contents_of_address,  MEMORY, VALUE, -1);
                                          push_stack(contents_of_address);
						                   time+=3;
                                          break;
                          case(0x31): //POP
					   concatenate_codes("POP", -1, TRUE);
                                          stack_value = pop_stack();
                                          if (stack_value != -1){
                                           memory[effective_address] = stack_value  ;
                                                 concatenate_trace(stack_value, effective_address,  VALUE, MEMORY, 4);
					                       	time+=3;
                                           }
                                           else 
                                           nop = TRUE;
                                           break;
                          default: 
                                   nop = TRUE; break;
                          }
                          
	if (nop) concatenate_codes("NOP", -1, TRUE);
    
   
    return pc;
}

///////////////////////////////////////////////////////////////////////////////
//Register-Register Instructions
///////////////////////////////////////////////////////////////////////////////
int register_register(int memory[], int pc){
    int sub_opcode, reg_i, reg_j, reg_k, final_value, reg_j_value, reg_k_value;
    sub_opcode = (memory[pc] >> 9) & 0x7;
    reg_i = (memory[pc] >> 6) & 0x7;
    reg_j = (memory[pc] >> 3) & 0x7;
    reg_k = (memory[pc] & 0x7);

    reg_j_value = get_register_value(reg_j);
    reg_k_value = get_register_value(reg_k);

 


   if (reg_j == 4)
     reg_j_value = (reg_j_value+1) & 0xFFFF;
    if (reg_k == 4)
      reg_k_value = (reg_k_value+1) & 0xFFFF;

       concatenate_trace(-1, reg_j_value, REGISTER, VALUE, reg_j);
    concatenate_trace(-1, reg_k_value,  REGISTER, VALUE, reg_k);

    final_value = arithmetic_instruction(sub_opcode, reg_j_value, reg_k_value, reg_j, reg_k, reg_i);

     concatenate_trace(final_value, -1,  VALUE, REGISTER, reg_i);
     set_register_value(final_value, reg_i);
       
     time++;                       
    return pc;
}


///////////////////////////////////////////////////////////////////////////////
//Non-Memory Register Instructions
///////////////////////////////////////////////////////////////////////////////
int non_memory_register(int memory[], int pc){
    int register_value, register_code;
        

	Boolean skip_flag = FALSE;
        Boolean printed_value = FALSE;
    register_code = (memory[pc] >> 10) & 0x3;
    register_value = get_register_value(register_code);


             if(memory[pc] & 0x0200){
                   if ((register_value & 0x8000)) skip_flag = TRUE;
                         concatenate_codes("SM", register_code, TRUE);
				 concatenate_trace(-1, register_value,  REGISTER, VALUE, register_code);
				if (skip_flag)printed_value = TRUE;				
				}
                       
             if(memory[pc] & 0x0100){
                   if (register_value == 0)  skip_flag = TRUE;
                           concatenate_codes("SZ", register_code, TRUE);
			 if (!printed_value) concatenate_trace(-1, register_value,  REGISTER, VALUE, register_code);
				}

		
                       
             if(memory[pc] & 0x0080){
		   
                   if (link_reg != 0) skip_flag = TRUE;
                           concatenate_codes("SNL", -1, TRUE);
			   concatenate_trace(-1, link_reg,  REGISTER, VALUE, 8);
			   
                       }

             if(memory[pc] & 0x0040){
			if (skip_flag == FALSE) skip_flag = TRUE;
			
			
			else if (skip_flag == TRUE) skip_flag = FALSE;
                   	 concatenate_codes("RSS", -1, TRUE);
			
                    }
             if(memory[pc] & 0x0020){
                     register_value = set_register_value(0, register_code);
                     concatenate_codes("CL", register_code, TRUE);
		     concatenate_trace(0, -1,  VALUE, REGISTER, register_code);
                     }
             if(memory[pc] & 0x0010){
                     link_reg = 0;
		     concatenate_trace(0, -1,  VALUE, REGISTER, 8);
                     concatenate_codes("CLL", -1, TRUE);
                     }
             if(memory[pc] & 0x0008){
                     concatenate_codes("CM", register_code, TRUE);
		     concatenate_trace(-1, register_value,  REGISTER, VALUE, register_code);
 		     register_value = set_register_value((~register_value), register_code);
                     concatenate_trace(register_value, -1,  VALUE, REGISTER, register_code);
                     }
             if(memory[pc] & 0x0004){
		     concatenate_trace(-1, link_reg,  REGISTER, VALUE, 8);
                     link_reg = !link_reg & 1;
		     concatenate_trace(link_reg, -1,  VALUE, REGISTER, 8);
                     concatenate_codes("CML", -1, TRUE);
                     }
             if(memory[pc] & 0x0002){
		      concatenate_trace(-1, register_value,  REGISTER, VALUE, register_code);
		      if(register_value == 0) register_value = 0xFFFF;
		      else register_value = register_value - 1;
                      register_value = set_register_value((register_value), register_code);
                      concatenate_codes("DC", register_code, TRUE);
		      concatenate_trace(register_value, -1,  VALUE, REGISTER, register_code);
                      }
             if(memory[pc] & 0x0001){
		      concatenate_trace(-1, register_value,  REGISTER, VALUE, register_code);
                      
			if ((register_value != 0xFFFF)&& !(register_value & 0x8000) && ((register_value + 1) & 0x8000)){
			concatenate_trace(1, -1, VALUE, REGISTER, 8);
			link_reg = 1;	
			}
			else if(register_value == 0xFFFF) {
			register_value = -1;	
			} 			
			register_value = (register_value + 1) & 0xFFFF;   
                      register_value = set_register_value(register_value, register_code);
                      concatenate_codes("IN", register_code, TRUE);
		       concatenate_trace(register_value, -1,  VALUE, REGISTER, register_code);
                      }
    if (skip_flag) {
  
     PC_reg = PC_reg+2;
     concatenate_trace(PC_reg, -1,  VALUE, REGISTER, 4);
    } 
                    
    time++;       
    return pc;
}
