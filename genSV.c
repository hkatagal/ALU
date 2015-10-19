#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//Function to generate the first testbench
void genTestbench1(int b, int r, int p);

//Function to generate the first testbench
void genTestbench2(int b, int r, int p);

//Function to generate the runsim file
void genRunsim(int b, int r, int p);

//Function to generate the expected results C file
void genExpectedResults(int b, int r, int p);


int main(int argc, char *argv[]) {
	if (argc != 4) { // argc == 4 means 3 arguments
		printf("ERROR: requires four arguments ./gen b r p\n");
		exit(0);
	}

	// Read b, r, and p from command line.
	// You should make sure you check that they are valid.  Make sure if you enter
	// e.g. b=-2 your generator prints an error message.
	int b = atoi(argv[1]);
	int r = atoi(argv[2]);
	int p = atoi(argv[3]);
	int temp,i;
	//check if b>=4 and is even
	if(b<4) {
		printf("ERROR: Input precision 'b' must be greater than or equal to 4\n");
		exit(0);
	}
	if(b%2 == 1) {
		printf("ERROR: Input precision 'b' must be even number\n");
		exit(0);
	}
	  //to check if r<2 and is power of 2
	if(r<2) {
		printf("ERROR: Number of Registers 'r' must be greater than or equal to 2\n");
		exit(0);
	}
	if(!(p==2 || p==1)){
		printf("ERROR: Invalid entry for pipelining. 1(Unpipelined) or 2(Pipelined)\n");
		exit(0);
	}
	temp = r;
	while(temp%2 == 0 && temp > 1)
		temp=temp/2;
	if(temp != 1){
		printf("ERROR: Number of Registers 'r' must be power of 2\n");
		exit(0);
	}
	  // Open output file for writing. 
	FILE *outfile;
	char fname[100];

	sprintf(fname, "h_katagal_calc_%d_%d_%d.sv", b, r, p);
	if ((outfile = fopen(fname, "w")) == NULL) {
	  printf("Error: cannot open file %s for writing.\n", fname);
	  exit(0);
	}
	
	fprintf(outfile, "/*\n*This source file contains the system verilog code\n");
	fprintf(outfile, "*for calculator with 16 operations with the parameters below.\n");
	fprintf(outfile, "*Number of bits:%d\n",b);
	fprintf(outfile, "*Number of registers used:%d\n",r);
	if(p==2)
		fprintf(outfile, "*Pipelining Stages:%d\n",p);
	if(p==1)
		fprintf(outfile, "*No pipelining used\n");
	fprintf(outfile, "\n*Module: calc_%d_%d_%d: Top level module for this file\n",b, r, p);
	fprintf(outfile, "*\tInput Signals: clk, reset, op, srcA, srcB, dst, immediate, regInputSrc, active\n");
	fprintf(outfile, "*\tOutput Signal: aluOutput\n");
	fprintf(outfile, "\n*Module: alu\n");
	if(p==1)
		fprintf(outfile, "*\tInput Signals: in0, in1,clk,reset, op\n");
	if(p==2)
		fprintf(outfile, "*\tInput Signals: in2, in3,clk,reset, op\n");
	fprintf(outfile, "*\tOutput Signals: out\n");
	fprintf(outfile, "*\n");
	fprintf(outfile, "*\n");
	
	
	fprintf(outfile, "*\n*File name of testbenches generated:\n");
	fprintf(outfile, "*\th_katagal_calc_tb1_%d_%d_%d.sv\n", b, r, p);
	fprintf(outfile, "*\th_katagal_calc_tb2_%d_%d_%d.sv\n", b, r, p);
	fprintf(outfile, "*\tFor more information on the testbench refer the testbench files\n");
	fprintf(outfile, "*\n*Authors: Harishkumar Katagal and Sagar Basavaraj Dhavali\n");
	fprintf(outfile, "*         Stony Brook University\n");	
	fprintf(outfile, "*/\n\n\n");
	
	fprintf(outfile, "//Top-level synthesizable module\n");
	fprintf(outfile, "\nmodule calc_%d_%d_%d (clk, reset, op, srcA, srcB, dst, immediate, regInputSrc, active, aluOutput);\n", b, r, p);
	fprintf(outfile, "  parameter W = %d;\n",b);
	fprintf(outfile, "  input clk, reset, active, regInputSrc;\n");
	fprintf(outfile, "  input [3:0] op;\n");
	fprintf(outfile, "  input signed [W-1:0] immediate;\n");
	temp = log2(r)-1;
	fprintf(outfile, "  input [%d:0] srcA, srcB, dst;\n",temp);
	fprintf(outfile, "  output logic signed [W-1:0] aluOutput;\n");
	fprintf(outfile, "  logic signed [W-1:0]");
	for(i=0;i<r;i++){
		fprintf(outfile, " reg%d",i);
		if(i!=r-1)
			fprintf(outfile, ",");
	}
	fprintf(outfile, ";\n");
	fprintf(outfile, "  logic signed [W-1:0]	aluInput0, aluInput1, regInput;\n");
	
	
	if(p==2){
		fprintf(outfile, "//Registers to implement pipelining\n");
		fprintf(outfile, "  logic act,regIoSrc;\n");
		fprintf(outfile, "  logic [%d:0] dstn;\n",temp);
		fprintf(outfile, "  logic signed [W-1:0]	imm;\n");
		fprintf(outfile, "  logic [3:0] op1;\n");
		fprintf(outfile, "\n  always_ff @(posedge clk) begin\n");
		fprintf(outfile, "    act <=active;\n");
		fprintf(outfile, "    regIoSrc <= regInputSrc;\n");
		fprintf(outfile, "    dstn <= dst;\n");
		fprintf(outfile, "    op1 <= op;\n");
		fprintf(outfile, "    imm <= immediate;\n");
		fprintf(outfile, "  end \n");
	}
	
	fprintf(outfile, "\n//This sequential block stores the data in registers based on the values of reset, regInputSrc, aluOutput and immediate.\n");
	fprintf(outfile, "\n  always_ff @(posedge clk) begin\n");
    fprintf(outfile, "    if (reset == 1) begin  //If reset=1, set all registers to zero\n");
	for(i=0;i<r;i++){
		fprintf(outfile, "     reg%d <= 0",i);
		if(i!=r-1)
			fprintf(outfile, ";\n");
	}
	fprintf(outfile, ";\n");
	fprintf(outfile, "    end\n");
    fprintf(outfile, "    else if (");
	if(p==2)
		fprintf(outfile, "act == 1) begin\n");
	else
		fprintf(outfile, "active == 1) begin\n");
	fprintf(outfile, "      case(dst");
	if(p==2)
		fprintf(outfile, "n)\n");
	else
		fprintf(outfile, ")\n");
	for(i=0;i<r-1;i++)
		fprintf(outfile, "        %d: reg%d <= regInput;\n",i,i);
	fprintf(outfile, "        default: reg%d <= regInput;\n",r-1);
	fprintf(outfile, "      endcase\n");
    fprintf(outfile, "    end\n");
    fprintf(outfile, "  end\n");
	
	
	fprintf(outfile, "\n  always_comb begin\n");
    fprintf(outfile, "    case(srcA)\n");
	for(i=0;i<r-1;i++)
		fprintf(outfile, "      %d: aluInput0 = reg%d;\n",i,i);
	fprintf(outfile, "      default: aluInput0 = reg%d;\n",r-1);
	fprintf(outfile, "    endcase\n");
    fprintf(outfile, "  end\n");
	
	fprintf(outfile, "\n  always_comb begin\n");
    fprintf(outfile, "    case(srcB)\n");
	for(i=0;i<r-1;i++)
		fprintf(outfile, "      %d: aluInput1 = reg%d;\n",i,i);
	fprintf(outfile, "      default: aluInput1 = reg%d;\n",r-1);
	fprintf(outfile, "    endcase\n");
    fprintf(outfile, "  end\n");
	
	// ALU 
	if(p==2){
		fprintf(outfile, "\n  alu aluInst(.in2(aluInput0), .in3(aluInput1),.clk(clk),.reset(reset), .op(op1), .out(aluOutput));\n");
		fprintf(outfile, "\n  assign regInput = (regIoSrc) ? imm : aluOutput;\n");
	}
	if(p==1){
		fprintf(outfile, "\n  alu aluInst(.in0(aluInput0), .in1(aluInput1),.clk(clk),.reset(reset), .op(op), .out(aluOutput));\n");
		fprintf(outfile, "\n  assign regInput = (regInputSrc) ? immediate : aluOutput;\n");	
    }	
	fprintf(outfile, "endmodule\n");
	
	//ALU MODULE
	if(p==1)
		fprintf(outfile, "\nmodule alu(in0, in1,clk,reset, op, out);\n");
	if(p==2)
		fprintf(outfile, "\nmodule alu(in2, in3,clk,reset, op, out);\n");
    fprintf(outfile, "  parameter W=%d;\n",b);
	fprintf(outfile, "  input clk,reset;\n");
	if(p==1)
		fprintf(outfile, "  input signed [W-1:0] in0, in1;\n");
	if(p==2){
		fprintf(outfile, "  input signed [W-1:0] in2, in3;\n");
		fprintf(outfile, "  logic signed [W-1:0] in0, in1;\n");
	}
	fprintf(outfile, "  input [3:0]	op;\n");
	fprintf(outfile, "  output logic signed [W-1:0] out;\n");
	fprintf(outfile, "  logic signed [W/2-1:0] in0top, in1top, in0bot, in1bot;\n");
	fprintf(outfile, "  logic signed [2*W-1:0] prod;\n");
	fprintf(outfile, "  logic signed [W-1:0] temp1,temp2;\n");
	fprintf(outfile, "  logic [W:0] sum;\n");
	fprintf(outfile, "  logic carry;\n");
	if(p==1)
		fprintf(outfile, "\n  assign prod = in0*in1;\n");
	if(p==2)
		fprintf(outfile, "\n  DW02_mult_2_stage #(%d,%d) multi(.A(in2),.B(in3),.TC(1'b1),.CLK(clk),.PRODUCT(prod));\n",b,b);
	
	
    fprintf(outfile, "\n  assign sum = in0+((op == 0) ? in1 : ((~in1)+1));\n");
	
	fprintf(outfile, "\n  always_ff@(posedge clk) begin\n");
    fprintf(outfile, "    if(reset == 1) begin\n");
	if(p==2)
		fprintf(outfile, "      in0<=0;\n      in1<=0;\n");
    fprintf(outfile, "      carry <= 0;\n    end\n");
    fprintf(outfile, "    else begin\n");
	if(p==2)
		fprintf(outfile, "      in0<=in2;\n      in1<=in3;\n");
	fprintf(outfile, "      if (op < 2)\n        carry<=sum[W];\n");
	fprintf(outfile, "    end\n  end\n");
	
	
	
	fprintf(outfile, "\n  always_comb begin\n");
	fprintf(outfile, "    in0top = in0[W-1:W/2];\n");
	fprintf(outfile, "    in1top = in1[W-1:W/2];\n");	
	fprintf(outfile, "    in0bot = in0[W/2-1:0];\n");
	fprintf(outfile, "    in1bot = in1[W/2-1:0];\n");
	fprintf(outfile, "    temp1 = in0bot*in1bot;\n");
	fprintf(outfile, "    temp2 = in0top*in1top;\n");
	fprintf(outfile, "  end\n");
	
	fprintf(outfile, "\n  always_comb begin\n");
	fprintf(outfile, "    case(op)\n");
	fprintf(outfile, "        0: out 	= sum[W-1:0];\n");
	fprintf(outfile, "        1: out 	= sum[W-1:0];\n");
	fprintf(outfile, "        2: out 	= prod[2*W-1:W];\n");
	fprintf(outfile, "        3: out 	= prod[W-1:0];\n");
	fprintf(outfile, "        4: out 	= in0 & in1;\n");
	fprintf(outfile, "        5: out 	= in0 | in1;\n");
	fprintf(outfile, "        6: out 	= in0 ^ in1;\n");
	fprintf(outfile, "        7: out 	= ~in1;\n");
	fprintf(outfile, "        8: out = carry;\n");
	fprintf(outfile, "        9: begin out[W/2-1:0] = in0bot + in1bot;\n");
	fprintf(outfile, "                 out[W-1:W/2] = in0top + in1top;\n");
	fprintf(outfile, "           end\n");
	fprintf(outfile, "        10: begin out[W/2-1:0] = in0bot - in1bot;\n");
	fprintf(outfile, "                  out[W-1:W/2] = in0top - in1top;\n");
	fprintf(outfile, "            end\n");
	fprintf(outfile, "        11: out = {temp2[W-1:W/2],temp1[W-1:W/2]};\n");
	fprintf(outfile, "        12:	out = {temp2[W/2-1:0],temp1[W/2-1:0]};\n");
	fprintf(outfile, "        13:	out = in0 >>> in1;\n");
	fprintf(outfile, "        14: out = in0 >> in1;\n");
	fprintf(outfile, "        15: out = in0 << in1;\n");
	//fprintf(outfile, "		default: out = 8'bx;\n");
	fprintf(outfile, "	endcase\n");
    fprintf(outfile, "  end\n");
	
	fprintf(outfile, "endmodule\n");
    fclose(outfile);
	genTestbench2(b,r,p);
	if(b<32 && r<1024){
		genTestbench1(b,r,p);
		genRunsim(b,r,p);
		genExpectedResults(b,r,p);
	}
	else{
		//printf("\n\n***************************************************************************************************************");
		printf("\n\n*********************************************Warning***********************************************************\n\n");
		if(b>=32){
			printf("\n******************The generated design might not work due to practical limitation on testing*******************\n\n");
			printf("\n***************Random testbench is not generated for the design since b value is greater than 30***************\n\n");
		} else 
		if(r>=1024){
			printf("\n******************The generated design might not work due to practical limitation on testing*******************\n\n");
			printf("\n***************Random testbench is not generated for the design since r value is greater than 512*************\n\n");
		}
		printf("\n***************************************************************************************************************\n\n\n");
	}
	
}


void genTestbench1(int b, int r, int p)

{

	FILE *tbfile;
	char fname[100];
	sprintf(fname, "h_katagal_calc_tb1_%d_%d_%d.sv", b, r, p);
	if ((tbfile = fopen(fname, "w")) == NULL) {
	  printf("Error: cannot open file %s for writing.\n", fname);
	  exit(0);
	}
	
	fprintf(tbfile, "/*\n*This source file contains the testbench system verilog code\n");
	fprintf(tbfile, "*This testbench takes input from a file inputVectors.txt which is generated by C program.\n");
	fprintf(tbfile, "*It then runs the testbench for each input and writes the output to the vlogOutput.txt for comparison.\n");
	fprintf(tbfile, "*Number of bits:%d\n",b);
	fprintf(tbfile, "*Number of registers used:%d\n",r);
	if(p==2)
		fprintf(tbfile, "*Pipelining Stages:%d\n",p);
	if(p==1)
		fprintf(tbfile, "*No pipelining used\n");
	fprintf(tbfile, "\n*Module: calc_tb1_%d_%d_%d: Top level module for this file\n",b, r, p);
	fprintf(tbfile, "*\n*Authors: Harishkumar Katagal and Sagar Basavaraj Dhavali\n");
	fprintf(tbfile, "*         Stony Brook University\n");	
	fprintf(tbfile, "*/\n\n\n");
	
	
	fprintf(tbfile, "//Top level module of the testbench\n");
	
	fprintf(tbfile, "module calc_tb1_%d_%d_%d();\n", b, r, p);
	fprintf(tbfile, "  parameter W = %d;\n",b);
	fprintf(tbfile, "  logic clk,reset,active,regInputSrc;\n");
	fprintf(tbfile, "  logic [3:0] op;\n");
	int temp = log2(r)-1;
	fprintf(tbfile, "  logic [%d:0] srcA, srcB, dst;\n",temp);
	fprintf(tbfile, "  logic signed [W-1:0] immediate,aluOutput;\n");
	int temp2 = 4+(3*log2(r))+b+2-1;
	int total_top = 4+(3*log2(r));
	int total_bot = b+2;
	int temp3,temp4,temp6;

	if(total_top%4 != 0)
		temp3 = total_top + (4 - total_top % 4);
	else
		temp3 = total_top;

	if(total_bot%4 != 0)				
		temp4 = total_bot + (4 - total_bot % 4);
	else
		temp4 = total_bot;
	temp6 = temp3 + temp4 - 1;	
	int temp5=0;
	fprintf(tbfile, "\n  //inputData to store the data read from the input file\n");
	if(temp2 < 32)	//SD
		fprintf(tbfile, "  logic [%d:0] 	inputData[0:99999];\n",temp2);
	else
		fprintf(tbfile, "  logic [%d:0] 	inputData[0:99999];\n",temp6);

	fprintf(tbfile, "  integer 		i;\n");
	fprintf(tbfile, "  integer 		vlogOut;\n");
	fprintf(tbfile, "\n  //Instantiate the module under test calc_%d_%d_%d\n",b,r,p);
	fprintf(tbfile, "\n  calc_%d_%d_%d calcInst(clk, reset, op, srcA, srcB, dst, immediate, regInputSrc, active, aluOutput);\n\n",b,r,p);
	fprintf(tbfile, "  initial clk=0;\n");
    fprintf(tbfile, "  always #5 clk = ~clk;\n\n");
	fprintf(tbfile, "//File to write the output for comparison\n");
	fprintf(tbfile, "  initial vlogOut = $fopen(\"vlogOutput.txt\", \"w\");\n\n");
	fprintf(tbfile, "  initial begin\n");
	fprintf(tbfile, "    $readmemh(\"inputVectors.txt\", inputData);\n");
	temp = log2(r);
	fprintf(tbfile, "    {op, srcA, srcB, dst, immediate, regInputSrc, active} = {4'b0, %d'b0, %d'b0, %d'b0, %d'b0, 1'b0, 1'b0};\n",temp,temp,temp,b);
	fprintf(tbfile, "    @(posedge clk);\n    @(posedge clk);\n    #1;\n");
	fprintf(tbfile, "    reset = 1;\n");
	fprintf(tbfile, "    @(posedge clk);\n    #1\n    reset  = 0;\n");
	fprintf(tbfile, "    for (i=0; i<100000; i=i+1) begin\n      @(posedge clk);\n      #1;\n");
	temp2 = 4+(3*log2(r))+b+2-1;
	temp = log2(r)-1;
	if(temp2 < 32)	
	{
		fprintf(tbfile, "      {op, srcA, srcB, dst, immediate, regInputSrc, active} = ");

		//op

		fprintf(tbfile, "{inputData[i][%d:%d],",temp2,temp2-3);

		//srcA

		temp2 -=4;

		fprintf(tbfile, "inputData[i][%d:%d],",temp2,temp2-temp); 

		//srcB

		temp2 = temp2-temp-1;

		fprintf(tbfile, "inputData[i][%d:%d],",temp2,temp2-temp); 

		//dst

		temp2 = temp2-temp-1;

		fprintf(tbfile, "inputData[i][%d:%d],",temp2,temp2-temp);

		//immediate

		temp2 = temp2-temp-1;	

		fprintf(tbfile, "inputData[i][%d:%d],",temp2,temp2-b+1);

	}
	else
	{

			switch(total_top % 4)

			{						

			case 3: temp5 = temp6-1;

					fprintf(tbfile, "      {op, srcA, srcB, dst, immediate, regInputSrc, active} = ");

					//op

					fprintf(tbfile, "{inputData[i][%d:%d],",temp5,temp5-3);

					//srcA

					temp5 -=4;

					fprintf(tbfile, "inputData[i][%d:%d],",temp5,temp5-temp); 

					//srcB

					temp5 = temp5-temp-1;

					fprintf(tbfile, "inputData[i][%d:%d],",temp5,temp5-temp); 

					//dst

					temp5 = temp5-temp-1;

					fprintf(tbfile, "inputData[i][%d:%d],",temp5,temp5-temp);

					break;

			case 2: temp5 = temp6-2;

					fprintf(tbfile, "      {op, srcA, srcB, dst, immediate, regInputSrc, active} = ");

					//op

					fprintf(tbfile, "{inputData[i][%d:%d],",temp5,temp5-3);

					//srcA

					temp5 -=4;

					fprintf(tbfile, "inputData[i][%d:%d],",temp5,temp5-temp); 

					//srcB

					temp5 = temp5-temp-1;

					fprintf(tbfile, "inputData[i][%d:%d],",temp5,temp5-temp); 

					//dst

					temp5 = temp5-temp-1;

					fprintf(tbfile, "inputData[i][%d:%d],",temp5,temp5-temp);

					break;

			case 1: temp5 = temp6-3;

					fprintf(tbfile, "      {op, srcA, srcB, dst, immediate, regInputSrc, active} = ");

					//op

					fprintf(tbfile, "{inputData[i][%d:%d],",temp5,temp5-3);

					//srcA

					temp5 -=4;

					fprintf(tbfile, "inputData[i][%d:%d],",temp5,temp5-temp); 

					//srcB

					temp5 = temp5-temp-1;

					fprintf(tbfile, "inputData[i][%d:%d],",temp5,temp5-temp); 

					//dst

					temp5 = temp5-temp-1;

					fprintf(tbfile, "inputData[i][%d:%d],",temp5,temp5-temp);

					break;

			case 0: temp5 = temp6;

					fprintf(tbfile, "      {op, srcA, srcB, dst, immediate, regInputSrc, active} = ");

					//op

					fprintf(tbfile, "{inputData[i][%d:%d],",temp5,temp5-3);

					//srcA

					temp5 -=4;

					fprintf(tbfile, "inputData[i][%d:%d],",temp5,temp5-temp); 

					//srcB

					temp5 = temp5-temp-1;

					fprintf(tbfile, "inputData[i][%d:%d],",temp5,temp5-temp); 

					//dst

					temp5 = temp5-temp-1;

					fprintf(tbfile, "inputData[i][%d:%d],",temp5,temp5-temp);

					break;

			}

			//immediate

			temp5 = temp5-temp-1;

			if((total_bot%4) == 0)	

				fprintf(tbfile, "inputData[i][%d:%d],",temp5,2); 

			else

				fprintf(tbfile, "inputData[i][%d:%d],",temp5-2,2); 				

	}

	

	//regInputSrc

	fprintf(tbfile, "inputData[i][1],"); 

	//active

	fprintf(tbfile, "inputData[i][0]};\n");	
	fprintf(tbfile, "      #1; $fdisplay(vlogOut, \"%%d: %%d\", i, aluOutput);\n    end\n");

    if(p == 2)

	{

		fprintf(tbfile, "    // when pipelined, print last cycle here\n");

		fprintf(tbfile, "    @(posedge clk);\n");

		fprintf(tbfile, "    #1; $fdisplay(vlogOut, \"%%d: %%d\", i, aluOutput);\n");

	}

	fprintf(tbfile, "    #100;\n");

    fprintf(tbfile, "    $fclose(vlogOut);\n    $finish;\n  end\n");

    fprintf(tbfile, "endmodule\n");

	

	fclose(tbfile);

	return;



}


void genTestbench2(int b, int r, int p)
{
	FILE *tbfile1;
	char fname[100];
	int val1,val2,rand1,rand2,i;
	srand(time(NULL));
	sprintf(fname, "h_katagal_calc_tb2_%d_%d_%d.sv", b, r, p);
	if ((tbfile1 = fopen(fname, "w")) == NULL) {
	  printf("Error: cannot open file %s for writing.\n", fname);
	  exit(0);
	}
	val1 = pow(2,b);
    val2 = pow(2,b-1);
	fprintf(tbfile1, "/*\n*This source file contains the testbench system verilog code\n");
	fprintf(tbfile1, "*This test bench tests all the operations of the ALU by loading fixed data into the registers and performing ALU operations.\n");
	fprintf(tbfile1, "*The value loaded into the register changes each time this file is generated\n");
	fprintf(tbfile1, "*Number of bits:%d\n",b);
	fprintf(tbfile1, "*Number of registers used:%d\n",r);
	if(p==2)
		fprintf(tbfile1, "*Pipelining Stages:%d\n",p);
	if(p==1)
		fprintf(tbfile1, "*No pipelining used\n");
	fprintf(tbfile1, "\n*Module: calc_tb2_%d_%d_%d: Top level module for this file\n",b, r, p);
	fprintf(tbfile1, "*\n*Authors: Harishkumar Katagal and Sagar Basavaraj Dhavali\n");
	fprintf(tbfile1, "*         Stony Brook University\n");	
	fprintf(tbfile1, "*/\n\n\n");
		
	
	
	fprintf(tbfile1, "//Top level module of the test bench\n");
	fprintf(tbfile1, "module calc_tb2_%d_%d_%d ();\n", b, r, p);
	fprintf(tbfile1, "  parameter W = %d;\n",b);
	fprintf(tbfile1, "  logic clk,reset,active,regInputSrc;\n");
	fprintf(tbfile1, "  logic [3:0] op;\n");
	int temp = log2(r);
	fprintf(tbfile1, "  logic [%d:0] srcA, srcB, dst;\n",temp-1);
	fprintf(tbfile1, "  logic signed [W-1:0] immediate,aluOutput;\n");
	fprintf(tbfile1, "  integer 		i,j,n;\n");
	fprintf(tbfile1, "  integer 		logOutput;\n");
	fprintf(tbfile1, "\n//Instantiate the top module calc to be tested by the testbench\n");
	fprintf(tbfile1, "  calc_%d_%d_%d calcInst(clk, reset, op, srcA, srcB, dst, immediate, regInputSrc, active, aluOutput);\n\n",b,r,p);
	fprintf(tbfile1, "  initial clk=0;\n");
    fprintf(tbfile1, "  always #5 clk = ~clk;\n\n");
	fprintf(tbfile1, "  initial logOutput = $fopen(\"logOutputtb2.txt\", \"w\");\n\n");
	fprintf(tbfile1, "  initial begin\n");
	
	fprintf(tbfile1, "    //$monitor($time,,\"op = %%d, srcA = %%d, srcB=%%d, dst=%%d, immediate=%%d, aluOutput = %%d(%%h)\", op,srcA, srcB, dst, immediate, aluOutput,aluOutput);\n");
	fprintf(tbfile1, "    @(posedge clk);\n    @(posedge clk);\n    #1;\n");
	fprintf(tbfile1, "    reset = 1;\n    @(posedge clk);\n    #1;\n");
	fprintf(tbfile1, "    reset = 0;\n");
	rand1 = (rand()%val1)-val2;
	rand2 = (rand()%val1)-val2;
	fprintf(tbfile1, "\n    //Load the values into all %d registers\n",r);
	for(i=0;i<r;i++){
		rand1 = (rand()%val1)-val2;
		rand2 = (rand()%val1)-val2;
		fprintf(tbfile1, "    @(posedge clk);\n");
		fprintf(tbfile1, "    #1; {op, srcA, srcB, dst, immediate, regInputSrc, active} = {3'bx, %d'dx, %d'dx, %d'd%d,",temp,temp,temp,i);
		if(rand1<0)
			fprintf(tbfile1, "-%d'd%d, 1'b1, 1'b1};\n",b,(rand1*-1));
		else
			fprintf(tbfile1, "%d'd%d, 1'b1, 1'b1};\n",b,rand1);
		fprintf(tbfile1, "            #1; $fdisplay(logOutput, \"op = %%d, srcA = %%d, srcB=%%d, dst=%%d, immediate=%%d, aluOutput = %%d(%%h)\", op,srcA, srcB, dst, immediate, aluOutput,aluOutput);\n");
	}
	fprintf(tbfile1, "\n    //Perform all the operations on all the %d registers\n",r);
	fprintf(tbfile1, "\n    for (i=0;i<16;i=i+1) begin\n");
	fprintf(tbfile1, "    	  for (j=1;j<%d;j=j+1) begin\n",r);
	fprintf(tbfile1, "            #1;	n = j+1;\n");
	fprintf(tbfile1, "            @(posedge clk);\n");
	//fprintf(tbfile1, "            #1; {op, srcA, srcB, regInputSrc, dst, active} = { i[3:0], 3'b0, j[2:0], 1'b0, n[2:0], 1'b0};\n");
	//fprintf(tbfile1, "            #1; {op, srcA, srcB, regInputSrc, dst, active} = { i[3:0], %d'b0, %d'b1, 1'b0, %d'd2, 1'b0};\n",temp,temp,temp);
	fprintf(tbfile1, "            #1; {op, srcA, srcB, regInputSrc, dst, active} = { i[3:0], %d'b0, j[%d:0], 1'b0, n[%d:0], 1'b0};\n",temp,temp-1,temp-1);
	fprintf(tbfile1, "            #1; $fdisplay(logOutput, \"op = %%d, srcA = %%d, srcB=%%d, dst=%%d, immediate=%%d, aluOutput = %%d(%%h)\", op,srcA, srcB, dst, immediate, aluOutput,aluOutput);\n");
	fprintf(tbfile1, "        end\n");
	fprintf(tbfile1, "      end\n");
	 if(p == 2)
	{
		fprintf(tbfile1, "    //Extra cycle for pipeline\n");
		fprintf(tbfile1, "    @(posedge clk);\n");
		fprintf(tbfile1, "    #1; $fdisplay(logOutput, \"op = %%d, srcA = %%d, srcB=%%d, dst=%%d, immediate=%%d, aluOutput = %%d(%%h)\", op,srcA, srcB, dst, immediate, aluOutput,aluOutput);\n");
	}
	fprintf(tbfile1, "\n    @(posedge clk);\n    @(posedge clk);\n    @(posedge clk);\n    @(posedge clk);\n");
	fprintf(tbfile1, "    $fclose(logOutput);\n");
	fprintf(tbfile1, "\n    $finish;\n");
    fprintf(tbfile1, "\n  end\n");
	
	fprintf(tbfile1, "endmodule\n");
	
	fclose(tbfile1);
	return;
}

void genRunsim(int b, int r, int p)
{
	FILE *tbfile2;
	char fname[100];

	sprintf(fname, "runsim");
	if ((tbfile2 = fopen(fname, "w")) == NULL) {
	  printf("Error: cannot open file %s for writing.\n", fname);
	  exit(0);
	}
	fprintf(tbfile2, "gcc testgen_tb1_%d_%d_%d.c -o testgen_tb1_%d_%d_%d -lm\n", b, r, p,b,r,p);
	fprintf(tbfile2, "./testgen_tb1_%d_%d_%d\n",b,r,p);
	fprintf(tbfile2, "#!/bin/bash\n");
	fprintf(tbfile2, "vlib work\n");
	fprintf(tbfile2, "vlog /usr/local/synopsys/syn/dw/sim_ver/DW02_mult_2_stage.v\n");
	fprintf(tbfile2, "vlog *.sv\n");
	fprintf(tbfile2, "vsim -c calc_tb1_%d_%d_%d -do \"run -all\"\n",b, r, p);
	fprintf(tbfile2, "diff -w -y vlogOutput.txt expOutput.txt | grep -E '<|>|\\|' | grep -v 'd' > incorrect-outputs.txt\n");
	fprintf(tbfile2, "\nNUMWRONG=$(wc -l < incorrect-outputs.txt);\n");
	fprintf(tbfile2, "echo \"\"\n");
	fprintf(tbfile2, "echo \"---------------------- Results: -----------------------------\";\n");
	fprintf(tbfile2, "echo \"\";\n");
	fprintf(tbfile2, "echo \"Found $NUMWRONG incorrect outputs.\";\n");
	fprintf(tbfile2, "echo \"\";\n");
	fprintf(tbfile2, "echo \"Please check the following output files to be certain everything\";\n");
	fprintf(tbfile2, "echo \"worked as expected.\"\n");
	fprintf(tbfile2, "echo \"\";\n");
	fprintf(tbfile2, "echo \"   vlogOutput.txt (actual output)\";\n");
	fprintf(tbfile2, "echo \"   expOutput.txt (correct output)\";\n");
	fprintf(tbfile2, "echo \"   incorrect-outputs.txt (the diff of these two)\";\n");
	fprintf(tbfile2, "echo \"\";\n");
	fprintf(tbfile2, "echo \"\";\n");
	fprintf(tbfile2, "echo \"-------------------------------------------------------------\";\n");

	fprintf(tbfile2, "\nrm -r work transcript vsim.wlf\n");
	fclose(tbfile2);
	//Provided all access to runsim file for execution
	system("chmod 777 runsim");
	return;
}

void genExpectedResults(int b, int r, int p){
	FILE *tbfile3;
	char fname[100];
	srand(time(NULL));
	sprintf(fname, "testgen_tb1_%d_%d_%d.c", b, r, p);
	if ((tbfile3 = fopen(fname, "w")) == NULL) {
	  printf("Error: cannot open file %s for writing.\n", fname);
	  exit(0);
	}
	fprintf(tbfile3, "\n");
	
	fprintf(tbfile3, "#include <stdio.h>\n");
	fprintf(tbfile3, "#include <time.h>\n");
	fprintf(tbfile3, "#include <math.h>\n");

	fprintf(tbfile3, "\n# define b  %d\n",b);
	fprintf(tbfile3, "# define r  %d\n",r);
	fprintf(tbfile3, "# define p  %d\n",p);

	fprintf(tbfile3, "\nstruct nib{\n");
	fprintf(tbfile3, "  int top : b/2;\n");
	fprintf(tbfile3, "  int bot : b/2;\n");	
	fprintf(tbfile3, "};\n");

	fprintf(tbfile3, "\nstruct nib1{\n");
    fprintf(tbfile3, "  long long int Cbits : b+1;\n");	
	fprintf(tbfile3, "};\n");

	fprintf(tbfile3, "\nstruct nib2{\n");
	fprintf(tbfile3, "  unsigned long long int bits : b;\n");
	fprintf(tbfile3, "};\n");

	fprintf(tbfile3, "\nstruct nib3{\n");
	fprintf(tbfile3, "  long long int bits1 : b;\n");
	fprintf(tbfile3, "};\n");
	
	
	fprintf(tbfile3, "\nint op, srcA, srcB, dst, immediate, regInputSrc, active;\n");
	fprintf(tbfile3, "int op1=0, srcA1=0, srcB1=0, dst1=0, immediate1=0, regInputSrc1=1, active1=0;\n");
	fprintf(tbfile3, "int destTemp =0,flag=0,flag1=1,flag2=0;\n");
	fprintf(tbfile3, "long long int valTemp=0,data=0;\n");
	fprintf(tbfile3, "long long int reg[r] = {0};\n");
	fprintf(tbfile3, "int count=0;\n");
	fprintf(tbfile3, "int z3=0;\n");
	fprintf(tbfile3, "int reg_bits,total_bits,op_Sbits,srcA_Sbits,srcB_Sbits,dst_Sbits,immediate_Sbits,regInputSrc_Sbits,active_Sbits; \n");
	fprintf(tbfile3, "int reg_Ebits = 0,immediate_Ebits = 0;\n");
	fprintf(tbfile3, "int srcB_Sbits_64 = 0,srcA_Sbits_64 = 0,op_Sbits_64 = 0;\n");
	fprintf(tbfile3, "int top_mask, bot_mask;\n");
	fprintf(tbfile3, "struct nib1 c1,c2,c3;\n");
	fprintf(tbfile3, "FILE *in, *out, *regState, *debug;\n");

	fprintf(tbfile3, "\nvoid printLine();\n");
	fprintf(tbfile3, "void updateRegs();\n");

	fprintf(tbfile3, "\nint main() {\n");
	fprintf(tbfile3, "  int inputs = 100000;\n");
	fprintf(tbfile3, "  srand(time(NULL));\n");
	fprintf(tbfile3, "  int i;\n");
	fprintf(tbfile3, "  int temp1,temp2,tmp1,tmp2;\n");
	fprintf(tbfile3, "  char fname[100];\n");
		  
	// Shift Bits - Calculations
	fprintf(tbfile3, "\n  reg_bits = log2(r);\n");
	fprintf(tbfile3, "  total_bits = 4 + reg_bits + reg_bits + reg_bits + b + 1 + 1 ; \n");
	fprintf(tbfile3, "  op_Sbits = total_bits - 4;\n");
	fprintf(tbfile3, "  srcA_Sbits = op_Sbits - reg_bits;\n");
	fprintf(tbfile3, "  srcB_Sbits = srcA_Sbits - reg_bits;\n");
	fprintf(tbfile3, "  dst_Sbits = srcB_Sbits - reg_bits;\n");
	fprintf(tbfile3, "  immediate_Sbits = dst_Sbits - b;\n");
	fprintf(tbfile3, "  regInputSrc_Sbits = immediate_Sbits - 1;\n");
	fprintf(tbfile3, "  //active_Sbits = regInputSrc_Sbits - 1;\n"); 
	
	fprintf(tbfile3, "\n  // for 64 bit - Packed input vectors\n");  
	fprintf(tbfile3, "  srcB_Sbits_64 = reg_bits;\n");
	fprintf(tbfile3, "  srcA_Sbits_64 = 2*reg_bits;\n");
	fprintf(tbfile3, "  op_Sbits_64 = 3*reg_bits;\n");  
	  
	fprintf(tbfile3, "\n  //Extract Bits - Calculations \n");
	fprintf(tbfile3, "  reg_Ebits |= (1 << reg_bits);\n");
	fprintf(tbfile3, "  reg_Ebits = reg_Ebits - 1;\n");
	fprintf(tbfile3, "  immediate_Ebits |= (1 << b);\n");
	fprintf(tbfile3, "  immediate_Ebits = immediate_Ebits - 1;\n");
	  
	fprintf(tbfile3, "\n  //Masking Bits\n");
	fprintf(tbfile3, "  tmp1 |= (1 << (b/2));\n");
	fprintf(tbfile3, "  tmp1 = tmp1 - 1;\n");
	fprintf(tbfile3, "  top_mask = immediate_Ebits - tmp1;\n");
	fprintf(tbfile3, "  tmp2 = top_mask >> (b/2);\n");
	fprintf(tbfile3, "  bot_mask = tmp2;\n");
		
	fprintf(tbfile3, "\n  in = fopen(\"inputVectors.txt\",\"w\");\n");
	fprintf(tbfile3, "  out = fopen(\"expOutput.txt\", \"w\");\n");
	fprintf(tbfile3, "  regState = fopen(\"regVals.txt\", \"w\");\n");

	fprintf(tbfile3, "\n  temp1 = pow(2,b);\n");
	fprintf(tbfile3, "  temp2 = pow(2,b-1);\n");
	fprintf(tbfile3, "\n  // Begin by randomly setting values for the 'r' registers\n");
	fprintf(tbfile3, "  for (i=0; i<r; i++) {\n");
	fprintf(tbfile3, "    op = 0; srcA = 0; srcB = 0; dst = i; immediate = (rand()%%temp1)-temp2; regInputSrc = 1; active = 1;\n");
	fprintf(tbfile3, "    printLine();\n");
	fprintf(tbfile3, "    updateRegs();\n");
	fprintf(tbfile3, "  }\n");
	
	fprintf(tbfile3, "\n  for (i=r; i<inputs; i++) {\n");

    fprintf(tbfile3, "    // op, srcA, srcB, and dst are random values between 0 and 7\n");
    fprintf(tbfile3, "    op = rand()%%16;\n");
    fprintf(tbfile3, "    srcA = rand()%%r;\n");
    fprintf(tbfile3, "    srcB = rand()%%r;\n");
    fprintf(tbfile3, "    dst = rand()%%r;\n");
    fprintf(tbfile3, "    immediate = (rand()%%temp1)-temp2;\n");
    fprintf(tbfile3, "    regInputSrc = rand()%%r;\n");
    fprintf(tbfile3, "    regInputSrc = (regInputSrc == 0);\n");
    fprintf(tbfile3, "    active = rand()%%r;\n");    
    fprintf(tbfile3, "    active = (active>0);\n");

    fprintf(tbfile3, "\n    printLine();\n");

    fprintf(tbfile3, "\n    updateRegs();\n");
    fprintf(tbfile3, "  }\n");
    fprintf(tbfile3, "\n  if(p==2)\n");
	fprintf(tbfile3, "    updateRegs();\n");
	fprintf(tbfile3, "  fclose(in);\n");
	fprintf(tbfile3, "  fclose(out);\n");
	fprintf(tbfile3, "  fclose(regState);\n");
	fprintf(tbfile3, "}\n");
		
	
	fprintf(tbfile3, "\n\nvoid printLine() {");
	fprintf(tbfile3, "\n  if(total_bits < 33)");
	fprintf(tbfile3, "\n  {");
	int temp8;
	int temp9 = b+2;
	temp8 = (temp9+temp9%4)/4;
  	
	fprintf(tbfile3, "\n    int printVal = ((op & 0x0F) << op_Sbits) + "); 
	fprintf(tbfile3, "((srcA & reg_Ebits) << srcA_Sbits) + "); 
	fprintf(tbfile3, "((srcB & reg_Ebits) << srcB_Sbits) + "); 
	fprintf(tbfile3, "((dst & reg_Ebits) << dst_Sbits) + "); 
	fprintf(tbfile3, "((immediate & immediate_Ebits) << immediate_Sbits) + ");
	fprintf(tbfile3, "((regInputSrc & 0x1) << regInputSrc_Sbits) + "); 
	fprintf(tbfile3, "(active & 0x1);  ");
	
  	fprintf(tbfile3, "\n    fprintf(in, \"%%x\\n\", printVal);");
	fprintf(tbfile3, "\n  }");
	fprintf(tbfile3, "\n  else");
	fprintf(tbfile3, "\n  {");
	
  	fprintf(tbfile3, "\n   long long int printVal = ((immediate & immediate_Ebits) << immediate_Sbits) + ");
	fprintf(tbfile3, "((regInputSrc & 0x1) << regInputSrc_Sbits) + (active & 0x1);");
	
  	fprintf(tbfile3, "\n  long long int printVal1 = (((op & 0x0F) << op_Sbits_64) + "); 
	fprintf(tbfile3, "((srcA & reg_Ebits) << srcA_Sbits_64) + ");
	fprintf(tbfile3, "((srcB & reg_Ebits) << srcB_Sbits_64) + ");
	fprintf(tbfile3, "(dst & reg_Ebits)); 	");
	
	fprintf(tbfile3, "\n    fprintf(in, \"%%x%%0%dx\\n\", printVal1,printVal);",temp8);
	fprintf(tbfile3, "\n  }\n}");
	
	
	fprintf(tbfile3, "\nvoid updateRegs() {");
	fprintf(tbfile3, "\n long long int z;");
	fprintf(tbfile3, "\n  int temp6,temp5;");
	fprintf(tbfile3, "\n  struct nib3 p1,p2,p3;");
	fprintf(tbfile3, "\n  temp5 = pow(2,b);");
	fprintf(tbfile3, "\n  temp6 = pow(2,b-1) - 1;\n");
	fprintf(tbfile3, "\n  if(p==1){");
	fprintf(tbfile3, "\n     op1 = op;");
	fprintf(tbfile3, "\n     srcA1 = srcA;");
	fprintf(tbfile3, "\n     srcB1 = srcB;");
	fprintf(tbfile3, "\n     dst1 = dst;");
	fprintf(tbfile3, "\n     immediate1 = immediate;");
	fprintf(tbfile3, "\n     regInputSrc1 = regInputSrc;");
	fprintf(tbfile3, "\n     active1 = active;");
	fprintf(tbfile3, "\n  }\n");   
	fprintf(tbfile3, "\n  if (regInputSrc1 == 1) {");
    fprintf(tbfile3, "\n     fprintf(out, \"%%d: d\\n\", count++);");
    fprintf(tbfile3, "\n     if(op1 == 0)");
    fprintf(tbfile3, "\n     {");
    fprintf(tbfile3, "\n         z = 0;");
	fprintf(tbfile3, "\n         p1.bits1 = reg[srcA1];");
	fprintf(tbfile3, "\n         p2.bits1 = reg[srcB1];");
	fprintf(tbfile3, "\n         c1.Cbits = p1.bits1 + p2.bits1;");
	fprintf(tbfile3, "\n         z3 = c1.Cbits >> b;");
    fprintf(tbfile3, "\n     }\n");
    fprintf(tbfile3, "\n     else if(op1 == 1)");
    fprintf(tbfile3, "\n     {");
    fprintf(tbfile3, "\n         z = 0;");
	fprintf(tbfile3, "\n         p1.bits1 = reg[srcA1];");
	fprintf(tbfile3, "\n         p2.bits1 = reg[srcB1];");
	fprintf(tbfile3, "\n         c1.Cbits = p1.bits1 - p2.bits1;");
	fprintf(tbfile3, "\n         z3 = c1.Cbits >> b;");   	
    fprintf(tbfile3, "\n     }\n");
	fprintf(tbfile3, "\n  }\n\n");
	
	
	fprintf(tbfile3, "  else {\n");
    fprintf(tbfile3, "     long long int z2;\n");
    fprintf(tbfile3, "     int temp1 = 0, temp2 = 0, temp3 = 0, temp4 = 0;\n");
	fprintf(tbfile3, "     struct nib first,second;\n");
	fprintf(tbfile3, "     struct nib2 t1,t2,t3;\n");
		
	
    fprintf(tbfile3, "\n     switch (op1) {\n");
    fprintf(tbfile3, "\n        case 0:  z = reg[srcA1]+reg[srcB1];\n");
	fprintf(tbfile3, "                 p1.bits1 = reg[srcA1];\n");
	fprintf(tbfile3, "                 p2.bits1 = reg[srcB1];\n");
	fprintf(tbfile3, "                 c1.Cbits = p1.bits1 + p2.bits1;\n");
	fprintf(tbfile3, "                 z3 = c1.Cbits >> b;\n");
	fprintf(tbfile3, "                 break;\n");
    fprintf(tbfile3, "\n        case 1:  z = reg[srcA1]-reg[srcB1];\n");
	fprintf(tbfile3, "                 p1.bits1 = reg[srcA1];\n");
	fprintf(tbfile3, "                 p2.bits1 = reg[srcB1];\n");
	fprintf(tbfile3, "                 c1.Cbits = p1.bits1 - p2.bits1;\n");
	fprintf(tbfile3, "                 z3 = c1.Cbits >> b;\n");			 
	fprintf(tbfile3, "                 break;\n");
    fprintf(tbfile3, "\n        case 2:  z2 = (reg[srcA1]*reg[srcB1]) >> b; z = z2&immediate_Ebits; break;\n");
    fprintf(tbfile3, "\n        case 3:  z = reg[srcA1]*reg[srcB1]; break;\n");
    fprintf(tbfile3, "\n        case 4:  z = reg[srcA1]&reg[srcB1]; break;\n");
    fprintf(tbfile3, "\n        case 5:  z = reg[srcA1]|reg[srcB1]; break;\n");
    fprintf(tbfile3, "\n        case 6:  z = reg[srcA1]^reg[srcB1]; break;\n");
    fprintf(tbfile3, "\n        case 7:  z = ~reg[srcB1]; break;\n");
	fprintf(tbfile3, "\n        case 8:  z = z3 & 0x01; break;\n");
	fprintf(tbfile3, "\n        case 9:  first.top = (reg[srcA1] & top_mask) >> (b/2);\n");
	fprintf(tbfile3, "                 first.bot = (reg[srcA1] & bot_mask);\n");
	fprintf(tbfile3, "                 second.top = (reg[srcB1] & top_mask) >> (b/2);\n");
	fprintf(tbfile3, "                 second.bot = (reg[srcB1] & bot_mask);\n");
	fprintf(tbfile3, "                 temp1 = first.top + second.top;\n");
	fprintf(tbfile3, "                 temp2 = first.bot + second.bot;\n");
	fprintf(tbfile3, "                 temp3 = temp1 << (b/2);\n");
	fprintf(tbfile3, "                 temp4 = temp2 & bot_mask;\n");
	fprintf(tbfile3, "                 z = temp3 + temp4;\n");
	fprintf(tbfile3, "                 break;\n");
	fprintf(tbfile3, "\n        case 10: first.top = (reg[srcA1] & top_mask) >> (b/2);\n");
	fprintf(tbfile3, "                 first.bot = (reg[srcA1] & bot_mask);\n");
	fprintf(tbfile3, "                 second.top = (reg[srcB1] & top_mask) >> (b/2);\n");
	fprintf(tbfile3, "                 second.bot = (reg[srcB1] & bot_mask);\n");
	fprintf(tbfile3, "                 temp1 = first.top - second.top;\n");
	fprintf(tbfile3, "                 temp2 = first.bot - second.bot;\n");
	fprintf(tbfile3, "                 temp3 = temp1 << (b/2);\n");
	fprintf(tbfile3, "                 temp4 = temp2 & bot_mask;\n");
	fprintf(tbfile3, "                 z = temp3 + temp4;\n");
	fprintf(tbfile3, "                 break;\n");
	fprintf(tbfile3, "\n        case 11: first.top = (reg[srcA1] & top_mask) >> (b/2);\n");
	fprintf(tbfile3, "                 first.bot = (reg[srcA1] & bot_mask);\n");
	fprintf(tbfile3, "                 second.top = (reg[srcB1] & top_mask) >> (b/2);\n");
	fprintf(tbfile3, "                 second.bot = (reg[srcB1] & bot_mask);\n");
	fprintf(tbfile3, "                 temp1 = first.top * second.top;\n");
	fprintf(tbfile3, "                 temp2 = first.bot * second.bot;\n");
	fprintf(tbfile3, "                 z = (temp1 & top_mask) + ((temp2 & top_mask) >> (b/2));\n");
	fprintf(tbfile3, "                 break;	\n");
	fprintf(tbfile3, "\n        case 12: first.top = (reg[srcA1] & top_mask) >> (b/2);\n");
	fprintf(tbfile3, "                 first.bot = (reg[srcA1] & bot_mask);\n");
	fprintf(tbfile3, "                 second.top = (reg[srcB1] & top_mask) >> (b/2);\n");
	fprintf(tbfile3, "                 second.bot = (reg[srcB1] & bot_mask);\n");
	fprintf(tbfile3, "                 temp1 = first.top * second.top;\n");
	fprintf(tbfile3, "                 temp2 = first.bot * second.bot;\n");
	fprintf(tbfile3, "                 z = ((temp1 & bot_mask) << (b/2)) + (temp2 & bot_mask);\n");
	fprintf(tbfile3, "                 break;	\n");
	fprintf(tbfile3, "\n        case 13: p1.bits1 = reg[srcA1] & immediate_Ebits;\n");
	fprintf(tbfile3, "                 p2.bits1 = reg[srcB1] & immediate_Ebits;\n");
	fprintf(tbfile3, "                 t2.bits = reg[srcB1] & immediate_Ebits;\n");
	fprintf(tbfile3, "                 p3.bits1 = p1.bits1 >> p2.bits1;\n");
	fprintf(tbfile3, "                 if(p1.bits1 < 0 && t2.bits >= b)\n");
	fprintf(tbfile3, "                   z = -1;\n");
	fprintf(tbfile3, "                 else if(p1.bits1 > 0 && t2.bits >= b)\n");
	fprintf(tbfile3, "                   z = 0;\n");
	fprintf(tbfile3, "                 else if (p1.bits1 == 0)	\n");
	fprintf(tbfile3, "                   z = 0;\n");
	fprintf(tbfile3, "                 else\n");
	fprintf(tbfile3, "                   z = (p3.bits1 & immediate_Ebits);\n");
	fprintf(tbfile3, "                 break;\n");
	fprintf(tbfile3, "\n        case 14: t1.bits = reg[srcA1] & immediate_Ebits;\n");
	fprintf(tbfile3, "                 t2.bits = reg[srcB1] & immediate_Ebits;\n");
	fprintf(tbfile3, "                 t3.bits = t1.bits >> t2.bits;\n");
	fprintf(tbfile3, "                 if(t2.bits >= b)\n");
	fprintf(tbfile3, "                   z = 0;\n");
	fprintf(tbfile3, "                 else\n");
	fprintf(tbfile3, "                   z = (t3.bits & immediate_Ebits);\n");
	fprintf(tbfile3, "                 break;\n");
	fprintf(tbfile3, "\n        case 15: t1.bits = reg[srcA1] & immediate_Ebits;\n");
	fprintf(tbfile3, "                 t2.bits = reg[srcB1] & immediate_Ebits;\n");
	fprintf(tbfile3, "                 t3.bits = t1.bits << t2.bits;\n");
	fprintf(tbfile3, "                 if(t2.bits >= b)\n");
	fprintf(tbfile3, "                   z = 0;\n");
	fprintf(tbfile3, "                 else\n");
	fprintf(tbfile3, "                   z = (t3.bits & immediate_Ebits);\n");
	fprintf(tbfile3, "                 break;\n");
	fprintf(tbfile3, "     }\n");
	
	fprintf(tbfile3, "\n  z = (z & immediate_Ebits);\n");
	fprintf(tbfile3, "  if(z > temp6)\n");
	fprintf(tbfile3, "     z = z - temp5;\n");
	fprintf(tbfile3, "  fprintf(out, \"%%d: %%d\\n\", count++, z);\n");
    fprintf(tbfile3, "  }\n");

	fprintf(tbfile3, "\n  if(p==2){\n");
	fprintf(tbfile3, "    if(flag == 1){\n");
	fprintf(tbfile3, "       reg[destTemp] = valTemp;\n");
	fprintf(tbfile3, "       flag=0;\n");
	fprintf(tbfile3, "    }\n");
	  
	fprintf(tbfile3, "\n    if(flag2==1){\n");
	fprintf(tbfile3, "       reg[r-1] = data;\n");
	fprintf(tbfile3, "       flag2=0;\n");
	fprintf(tbfile3, "    }\n");

	fprintf(tbfile3, "\n    if (active1 == 1) {\n");
	fprintf(tbfile3, "      if (regInputSrc1 == 1)\n");
	fprintf(tbfile3, "         if(dst1==r-1 && flag1==1){\n");
	fprintf(tbfile3, "            flag1=0;\n");
	fprintf(tbfile3, "            flag2=1;\n");
	fprintf(tbfile3, "            data = immediate1;\n");
	fprintf(tbfile3, "         }\n");
	fprintf(tbfile3, "      else{ \n");
	fprintf(tbfile3, "         valTemp =immediate1;\n");
	fprintf(tbfile3, "         destTemp = dst1;\n");
	fprintf(tbfile3, "         flag=1;\n");
	fprintf(tbfile3, "      }\n");
	fprintf(tbfile3, "\n    else{\n");
	fprintf(tbfile3, "       valTemp =z;\n");
	fprintf(tbfile3, "       destTemp = dst1;\n");
	fprintf(tbfile3, "       flag=1; \n");
	fprintf(tbfile3, "    } \n");
	fprintf(tbfile3, "    }\n");
	fprintf(tbfile3, "    op1 = op;\n");
	fprintf(tbfile3, "    srcA1 = srcA;\n");
	fprintf(tbfile3, "    srcB1 = srcB;\n");
	fprintf(tbfile3, "    dst1 = dst;\n");
	fprintf(tbfile3, "    immediate1 = immediate;\n");
	fprintf(tbfile3, "    regInputSrc1 = regInputSrc;\n");
	fprintf(tbfile3, "    active1 = active;\n");
	fprintf(tbfile3, "  }\n");
	fprintf(tbfile3, "\n  if(p==1){\n");
	fprintf(tbfile3, "     if (active1 == 1) {\n");
	fprintf(tbfile3, "        if (regInputSrc1 == 1)\n");
	fprintf(tbfile3, "           reg[dst1] = immediate;\n");
	fprintf(tbfile3, "        else\n");
	fprintf(tbfile3, "           reg[dst1] = z;\n");
	fprintf(tbfile3, "     }\n");
	fprintf(tbfile3, "   }\n");
	
	
	fprintf(tbfile3, "\n  fprintf(regState,\"cnt = %%d\",count);\n");
	fprintf(tbfile3, "  int l = 0;\n");
	fprintf(tbfile3, "  for(l=0;l<r;l++)\n");
	fprintf(tbfile3, "     fprintf(regState, \"%%d\",reg[l]);\n");
	fprintf(tbfile3, "  fprintf(regState,\"\\n\");\n");
    
	fprintf(tbfile3, " }\n");
	
	fclose(tbfile3);
	
}
