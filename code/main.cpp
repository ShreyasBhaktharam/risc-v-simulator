#include<iostream>
#include<string>
#include<vector>
#include<bitset>
#include<fstream>

using namespace std;

#define MemSize 1000 // memory size, in reality, the memory size should be 2^32, but for this lab, for the space resaon, we keep it as this large number, but the memory is still 32-bit addressable.

struct IFStruct {
    bitset<32>  PC;
    bool        nop;
	IFStruct() {
		PC = bitset<32>(0);
		nop = 0;
	} 
};

struct IDStruct {
    bitset<32>  Instr;
    bool        nop;  
	IDStruct() {
		Instr = bitset<32>(0);
		nop = 0;
	}
};

struct EXStruct {
    bitset<32>  Read_data1;
    bitset<32>  Read_data2;
    bitset<16>  Imm;
    bitset<5>   Rs;
    bitset<5>   Rt;
    bitset<5>   Wrt_reg_addr;
    bool        is_I_type;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        alu_op;     //1 for addu, lw, sw, 0 for subu 
    bool        wrt_enable;
    bool        nop;
	EXStruct() {
		Read_data1 = bitset<32>(0);
		Read_data2 = bitset<32>(0);
		Imm = bitset<16>(0);
		Rs = bitset<5>(0);
		Rt = bitset<5>(0);
		Wrt_reg_addr = bitset<5>(0);
		is_I_type = 0;
		rd_mem = 0;
		wrt_mem = 0;
		alu_op = 0;
		wrt_enable = 0;
		nop = 0;
	}  
};

struct MEMStruct {
    bitset<32>  ALUresult;
    bitset<32>  Store_data;
    bitset<5>   Rs;
    bitset<5>   Rt;    
    bitset<5>   Wrt_reg_addr;
    bool        rd_mem;
    bool        wrt_mem; 
    bool        wrt_enable;    
    bool        nop;    
	MEMStruct() {
		ALUresult = bitset<32>(0);
		Store_data = bitset<32>(0);
		Rs = bitset<5>(0);
		Rt = bitset<5>(0);
		Wrt_reg_addr = bitset<5>(0);
		rd_mem = 0;
		wrt_mem = 0;
		wrt_enable = 0;
		nop = 0;
	}
};

struct WBStruct {
    bitset<32>  Wrt_data;
    bitset<5>   Rs;
    bitset<5>   Rt;     
    bitset<5>   Wrt_reg_addr;
    bool        wrt_enable;
    bool        nop;     
	WBStruct() {
		Wrt_data = bitset<32>(0);
		Rs = bitset<5>(0);
		Rt = bitset<5>(0);
		Wrt_reg_addr = bitset<5>(0);
		wrt_enable = 0;
		nop = 0;
	}
};

struct stateStruct {
    IFStruct    IF;
    IDStruct    ID;
    EXStruct    EX;
    MEMStruct   MEM;
    WBStruct    WB;
};

class InsMem
{
	public:
		string id, ioDir;
        InsMem(string name, string ioDir) {       
			id = name;
			IMem.resize(MemSize);
            ifstream imem;
			string line;
			int i=0;
			//for macos
			imem.open(ioDir + "/imem.txt");
			//for windows
			// imem.open(ioDir + "\\imem.txt");
			if (imem.is_open())
			{
				while (getline(imem,line))
				{   
					IMem[i] = bitset<8>(stoul(line, nullptr, 2));
					// IMem[i] = bitset<8>(line);
					i++;
				}                    
			}
            else cout<<"Unable to open IMEM input file.";
			imem.close();                     
		}

		bitset<32> readInstr(bitset<32> ReadAddress) {    
			// read instruction memory
			// return bitset<32> val

			int address = ReadAddress.to_ulong();

			string instructionMemory;
			instructionMemory.append(IMem[address].to_string());
			instructionMemory.append(IMem[address + 1].to_string());
			instructionMemory.append(IMem[address + 2].to_string());
			instructionMemory.append(IMem[address + 3].to_string());
			return bitset<32>(instructionMemory);
		}     
      
    private:
        vector<bitset<8> > IMem;     
};
      
class DataMem    
{
    public: 
		string id, opFilePath, ioDir;
        DataMem(string name, string ioDir) : id(name), ioDir(ioDir) {
            DMem.resize(MemSize);
			//for macos
			opFilePath = ioDir + "/" + name + "_DMEMResult.txt";
			//for windows
			// opFilePath = ioDir + "\\" + name + "_DMEMResult.txt";
            ifstream dmem;
            string line;
            int i=0;
			//for macOS
            dmem.open(ioDir + "/dmem.txt");
			//for windows
			// dmem.open(ioDir + "\\dmem.txt");


            if (dmem.is_open())
            {
                while (getline(dmem,line))
                {   
                    DMem[i] = bitset<8>(stoul(line, nullptr, 2));
					// DMem[i] = bitset<8>(line);
                    i++;
                }
            }
            else cout<<"Unable to open DMEM input file.";
                dmem.close();          
        }
		
        bitset<32> readDataMem(bitset<32> Address) {	
			// read data memory
			// return bitset<32> val

			int address = Address.to_ulong();
			string dataMemory;
			dataMemory.append(DMem[address].to_string());
			dataMemory.append(DMem[address + 1].to_string());
			dataMemory.append(DMem[address + 2].to_string());
			dataMemory.append(DMem[address + 3].to_string());
			return bitset<32>(dataMemory);

		}
            
        void writeDataMem(bitset<32> Address, bitset<32> WriteData) {
			// write into memory
			int address = Address.to_ullong();

			DMem[address] = bitset<8>(WriteData.to_string().substr(0, 8));
			DMem[address + 1] = bitset<8>(WriteData.to_string().substr(8, 8));
			DMem[address + 2] = bitset<8>(WriteData.to_string().substr(16, 8));
			DMem[address + 3] = bitset<8>(WriteData.to_string().substr(24, 8));
        }   
                     
        void outputDataMem() {
            ofstream dmemout;
            dmemout.open(opFilePath, std::ios_base::trunc);
            if (dmemout.is_open()) {
                for (int j = 0; j< 1000; j++)
                {     
                    dmemout << DMem[j]<<endl;
                }
                     
            }
            else cout<<"Unable to open "<<id<<" DMEM result file." << endl;
            dmemout.close();
        }             

    private:
		vector<bitset<8> > DMem;      
};

class RegisterFile
{
    public:
		string outputFile;
     	RegisterFile(string ioDir): outputFile (ioDir + "RFResult.txt") {
			Registers.resize(32);  
			Registers[0] = bitset<32> (0);  
        }
	
        bitset<32> readRF(bitset<5> Reg_addr) {   
            // Fill in
			int reg = Reg_addr.to_ulong();

			return Registers[reg];
        }
    
        void writeRF(bitset<5> Reg_addr, bitset<32> Wrt_reg_data) {
            // Fill in
			int reg = Reg_addr.to_ulong();

			Registers[reg] = Wrt_reg_data;
        }
		 
		void outputRF(int cycle) {
			ofstream rfout;
			if (cycle == 0)
				rfout.open(outputFile, std::ios_base::trunc);
			else 
				rfout.open(outputFile, std::ios_base::app);
			if (rfout.is_open())
			{
				rfout<<"State of RF after executing cycle:\t"<<cycle<<endl;
				for (int j = 0; j<32; j++)
				{
					rfout << Registers[j]<<endl;
				}
			}
			else cout<<"Unable to open RF output file."<<endl;
			rfout.close();               
		} 
			
	private:
		vector<bitset<32> >Registers;
};

class Core {
	public:
		RegisterFile myRF;
		uint32_t cycle = 0;
		bool halted = false;
		string ioDir;
		struct stateStruct state, nextState;
		InsMem ext_imem;
		DataMem ext_dmem;
		int totalInstructions = 0;
		
		Core(string ioDir, InsMem &imem, DataMem &dmem): myRF(ioDir), ioDir(ioDir), ext_imem (imem), ext_dmem (dmem) {}

		virtual void step() {}

		virtual void printState() {}
};

class SingleStageCore : public Core {
	public:
		//for windows
		// SingleStageCore(string ioDir, InsMem &imem, DataMem &dmem): Core(ioDir + "\\SS_", imem, dmem), opFilePath(ioDir + "\\StateResult_SS.txt") {}
		// for macOS
		SingleStageCore(string ioDir, InsMem &imem, DataMem &dmem): Core(ioDir + "/SS_", imem, dmem), opFilePath(ioDir + "/StateResult_SS.txt") {}
		void step() {
			/* Your implementation*/
			//Fetch inital values
			bitset<32> instruction = ext_imem.readInstr(state.IF.PC);
			int opcode = stoi(instruction.to_string().substr(25, 7), 0, 2);

			bitset<32> rs, rt, rd;
			bitset<6> funct;
			bitset<16> imm;
			bitset<3> funct3;
			bitset<7> funct7;
			bitset<26> address;
			bitset<32> temp;
			bitset<32> offset;
			string immTemp;


			//check if instruction is a halt
			if (instruction == bitset<32>(0xFFFFFFFF)) {
				nextState.IF.nop = 1;
			}

			//Decode instructions
			switch(opcode) {
				//halt instruction
				case 0x7F: break;
				case 0x00:
				state.EX.Rs = bitset<5>(instruction.to_string(), 6, 5);
				state.EX.Rt = bitset<5>(instruction.to_string(), 11, 5);
				state.EX.Wrt_reg_addr = bitset<5>(instruction.to_string(), 16, 5);
				funct = bitset<6>(instruction.to_string(), 26, 6);

				state.EX.Read_data1 = myRF.readRF(state.EX.Rs);
				state.EX.Read_data2 = myRF.readRF(state.EX.Rt);


				myRF.writeRF(0, 0);

				switch(funct.to_ulong()) {
			// 	case 0x00:		// addu
			// 	nextState.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() + state.EX.Read_data2.to_ulong());
			// 	myRF.writeRF(state.EX.Wrt_reg_addr, nextState.MEM.ALUresult);
			// 	nextState.EX.alu_op = 1;
			// 	break;
			// case 0x23:		// subu
			// 	nextState.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() - state.EX.Read_data2.to_ulong());
			// 	myRF.writeRF(state.EX.Wrt_reg_addr, nextState.MEM.ALUresult);
			// 	nextState.EX.alu_op = 0;
			// 	break;
			// case 0x24:		// and
			// 	nextState.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() & state.EX.Read_data2.to_ulong());
			// 	myRF.writeRF(state.EX.Wrt_reg_addr, nextState.MEM.ALUresult);
			// 	nextState.EX.alu_op = 0;
			// 	break;
			// case 0x25:		// or
			// 	nextState.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() | state.EX.Read_data2.to_ulong());
			// 	myRF.writeRF(state.EX.Wrt_reg_addr, nextState.MEM.ALUresult);
			// 	nextState.EX.alu_op = 0;
			// 	break;
			// case 0x26:		// nor
			// 	nextState.MEM.ALUresult = bitset<32>(~(state.EX.Read_data1.to_ulong() | state.EX.Read_data2.to_ulong()));
			// 	myRF.writeRF(state.EX.Wrt_reg_addr, nextState.MEM.ALUresult);
			// 	nextState.EX.alu_op = 0;
			// 	break;
			default:
				break;
				}
				nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4);
				break;
				//R-TYPE INSTRUCTIONS
				case 0x33:
				state.EX.Rs = bitset<5>(instruction.to_string().substr(12, 5));
				state.EX.Rt = bitset<5>(instruction.to_string().substr(7, 5));
				state.EX.Wrt_reg_addr = bitset<5>(instruction.to_string().substr(20, 5));
				funct7 = bitset<7>(instruction.to_string().substr(0, 7));
				funct3 = bitset<3>(instruction.to_string().substr(17, 3));




				switch(funct7.to_ulong()) {
					//SUB
					case 0x20:
								state.EX.Read_data1 = myRF.readRF(state.EX.Rs);
								state.EX.Read_data2 = myRF.readRF(state.EX.Rt);
								state.EX.Wrt_reg_addr = bitset<5>(instruction.to_string().substr(20, 5));
								nextState.MEM.ALUresult = state.EX.Read_data1.to_ulong() - state.EX.Read_data2.to_ulong();
								myRF.writeRF(state.EX.Wrt_reg_addr, nextState.MEM.ALUresult);
								nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4);
								break;
					case 0x00:
						switch(funct3.to_ulong()) {
							//ADD
							case 0x00:
								state.EX.Read_data1 = myRF.readRF(state.EX.Rs);
								state.EX.Read_data2 = myRF.readRF(state.EX.Rt);
								state.EX.Wrt_reg_addr = bitset<5>(instruction.to_string().substr(20, 5));
								nextState.MEM.ALUresult = state.EX.Read_data1.to_ulong() + state.EX.Read_data2.to_ulong();
								myRF.writeRF(state.EX.Wrt_reg_addr, nextState.MEM.ALUresult);
								nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4);
								break;

							//AND
							case 0x07:
								state.EX.Read_data1 = myRF.readRF(state.EX.Rs);
								state.EX.Read_data2 = myRF.readRF(state.EX.Rt);
								state.EX.Wrt_reg_addr = bitset<5>(instruction.to_string().substr(20, 5));
								nextState.MEM.ALUresult = state.EX.Read_data1.to_ulong() & state.EX.Read_data2.to_ulong();
								myRF.writeRF(state.EX.Wrt_reg_addr, nextState.MEM.ALUresult);
								nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4);
								break;

							//XOR
							case 0x04: 
								state.EX.Read_data1 = myRF.readRF(state.EX.Rs);
								state.EX.Read_data2 = myRF.readRF(state.EX.Rt);
								state.EX.Wrt_reg_addr = bitset<5>(instruction.to_string().substr(20, 5));
								nextState.MEM.ALUresult = state.EX.Read_data1.to_ulong() ^ state.EX.Read_data2.to_ulong();
								myRF.writeRF(state.EX.Wrt_reg_addr, nextState.MEM.ALUresult);
								nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4);
								break;

							//OR
							case 0x06: 
								state.EX.Read_data1 = myRF.readRF(state.EX.Rs);
								state.EX.Read_data2 = myRF.readRF(state.EX.Rt);
								state.EX.Wrt_reg_addr = bitset<5>(instruction.to_string().substr(20, 5));
								nextState.MEM.ALUresult = state.EX.Read_data1.to_ulong() | state.EX.Read_data2.to_ulong();
								myRF.writeRF(state.EX.Wrt_reg_addr, nextState.MEM.ALUresult);
								nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4);
								break;
						}

				}
				break;

				//Branch instructions
				case 0x63:
				funct3 = bitset<3>(instruction.to_string().substr(17, 3));
				state.EX.Rs = bitset<5>(instruction.to_string().substr(12, 5));
				state.EX.Rt = bitset<5>(instruction.to_string().substr(7, 5));
				//imm[12|10:5] + imm[4:1|11]
				immTemp = instruction.to_string().substr(0, 1) + instruction.to_string().substr(1, 6) + instruction.to_string().substr(20, 4) + instruction.to_string().substr(19, 1);
				imm = stoi(immTemp, 0, 2);
				if (immTemp[0] == '1') {
    				imm |= (~0U << 12);
				}
				imm <<= 2;

				state.EX.Read_data1 = myRF.readRF(state.EX.Rs);
				state.EX.Read_data2 = myRF.readRF(state.EX.Rt);

				switch(funct3.to_ulong()) {
					        case 0x00: // BEQ
            				if (state.EX.Read_data1 == state.EX.Read_data2) {
								bitset<32> branchTarget = bitset<32>(imm.to_ulong()) << 1;
                			nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + imm.to_ulong());
							
							nextState.ID.nop = 1;
            				} 
							else {
                			nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4);
            				}
            				break;
            
        			case 0x01: // BNE
            		if(state.EX.Read_data1 != state.EX.Read_data2) {
						bitset<32> branchTarget = bitset<32>(imm.to_ulong()) << 1;
                		nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + imm.to_ulong());
						nextState.ID.nop = 1;
           			 } 
					 else {
                		nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4);
            		}		
            		break;
				}
				break;


				//JAL
				case 0x6F:
					//imm[20|10:1|11|19:12]
    				state.EX.Wrt_reg_addr = bitset<5>(instruction.to_string().substr(20, 5));
    				immTemp = instruction.to_string().substr(0, 1) + 
					instruction.to_string().substr(1, 10) + 
					instruction.to_string().substr(11, 1) + 
					instruction.to_string().substr(12, 8);
					imm = stoi(immTemp, 0, 2);

					if (immTemp[0] == '1') {
    					imm |= (~0U << 20); // Sign extend from 21 bits
					}
					imm <<= 1;
    
					nextState.MEM.ALUresult = bitset<32>(state.IF.PC.to_ulong() + 4);
					nextState.MEM.wrt_enable = 1;
					myRF.writeRF(state.EX.Wrt_reg_addr, bitset<32>(state.IF.PC.to_ulong() + 4));
					nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + imm.to_ulong());

					nextState.ID.nop = 1;
					break;

				//I-TYPE INSTRUCTIONS
				case 0x13:
				funct3 = bitset<3>(instruction.to_string().substr(17, 3));
				imm = bitset<16>(instruction.to_string().substr(0, 12));
				state.EX.Rs = bitset<5>(instruction.to_string().substr(12, 5));
				state.EX.Wrt_reg_addr = bitset<5>(instruction.to_string().substr(20, 5));
				state.EX.Read_data1 = myRF.readRF(state.EX.Rs);

				switch(funct3.to_ulong()) {
					//ADDI
					case 0x00: 
						nextState.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() + (imm.to_ulong()));
						myRF.writeRF(state.EX.Wrt_reg_addr, nextState.MEM.ALUresult);
						nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4);
						break;
					//XORI
					case 0x04:
						nextState.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() ^ (imm.to_ulong()));
						myRF.writeRF(state.EX.Wrt_reg_addr, nextState.MEM.ALUresult);
						nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4);
						break;
					
					//ORI
					case 0x06: 
						nextState.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() | (imm.to_ulong()));
						myRF.writeRF(state.EX.Wrt_reg_addr, nextState.MEM.ALUresult);
						nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4);
						break;
					break;

					//ANDI
					case 0x07: 
						nextState.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() & (imm.to_ulong()));
						myRF.writeRF(state.EX.Wrt_reg_addr, nextState.MEM.ALUresult);
						nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4);
						break;

					default: break;
				}
				break;

				//sw instruction
				case 0x23:
					state.EX.Rs = bitset<5>(instruction.to_string(), 12, 5);
					state.EX.Rt = bitset<5>(instruction.to_string(), 7, 5);
					immTemp = instruction.to_string().substr(0, 7) + instruction.to_string().substr(20, 5);
					imm = stoi(immTemp, 0, 2);
					rs = bitset<32>(state.EX.Rs.to_ulong() + imm.to_ulong());
					myRF.readRF(state.EX.Rs);
					ext_dmem.writeDataMem(rs, myRF.readRF(state.EX.Rt));
					nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4);
					break;

				//lw instruction
				case 0x03:
					state.EX.Rs = bitset<5>(instruction.to_string(), 12, 5);
					state.EX.Wrt_reg_addr = bitset<5>(instruction.to_string(), 20, 5);
					offset = bitset<32>(instruction.to_string(), 0, 12);
					rs = bitset<32>(state.EX.Rs.to_ulong() + bitset<32>(offset.to_ulong()).to_ulong());
					myRF.writeRF(state.EX.Wrt_reg_addr, ext_dmem.readDataMem(rs));
					nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4);
					break;

				default:
					cout << "no instruction found" << endl;
					break;
			}

			totalInstructions++;
			// halted = true;
			if (state.IF.nop) {
				halted = true;
				outputMetrics();
			}
			
			myRF.outputRF(cycle); // dump RF
			printState(nextState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
			
			state = nextState; // The end of the cycle and updates the current state with the values calculated in this cycle
			cycle++;
		}

		//Values of CPI and IPC are interchanged. CPI is calculated as IPC and vice versa
		void outputMetrics() {
			float cpi = (float)cycle / totalInstructions;
			float instructionsPerCycle = (float)totalInstructions / cycle;
			cout << "SINGLE STAGE CORE PERFORMANCE METRICS" << endl;
			cout << "-------------------------------------" << endl;
			cout << "Total instructions: " << totalInstructions << endl;
			cout << "Total cycles: " << cycle << endl;
			cout << "CPI: " << instructionsPerCycle << endl;
			cout << "Instructions per cycle: " << cpi << endl;
			cout << "-------------------------------------" << endl;
		}

		void printState(stateStruct state, int cycle) {
    		ofstream printstate;
			if (cycle == 0)
				printstate.open(opFilePath, std::ios_base::trunc);
			else 
    			printstate.open(opFilePath, std::ios_base::app);
    		if (printstate.is_open()) {
    		    printstate<<"State after executing cycle:\t"<<cycle<<endl; 

    		    printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;
    		    printstate<<"IF.nop:\t"<<state.IF.nop<<endl;
    		}
    		else cout<<"Unable to open SS StateResult output file." << endl;
    		printstate.close();
		}
	private:
		string opFilePath;
};

class FiveStageCore : public Core{
	public:
		//for windows
		// FiveStageCore(string ioDir, InsMem &imem, DataMem &dmem): Core(ioDir + "\\FS_", imem, dmem), opFilePath(ioDir + "\\StateResult_FS.txt") {}
		//for macOS
		FiveStageCore(string ioDir, InsMem &imem, DataMem &dmem): Core(ioDir + "/FS_", imem, dmem), opFilePath(ioDir + "/StateResult_FS.txt") {}


		void step() {
			/* Your implementation */
			/* --------------------- WB stage --------------------- */
			if(!state.WB.nop) {
				if(state.WB.wrt_enable) {
					myRF.writeRF(state.WB.Wrt_reg_addr, state.WB.Wrt_data);
				}
				if(state.MEM.nop) {
					nextState.WB.nop = state.MEM.nop;
				}
			}	
			
			/* --------------------- MEM stage -------------------- */
		if (!state.MEM.nop) {
			nextState.WB.Rs = state.MEM.Rs;
			nextState.WB.Rt = state.MEM.Rt;
			nextState.WB.Wrt_reg_addr = state.MEM.Wrt_reg_addr;
			nextState.WB.wrt_enable = state.MEM.wrt_enable;

			//lw instructions
			if (state.MEM.rd_mem) {
				nextState.WB.Wrt_data = ext_dmem.readDataMem(state.MEM.ALUresult);
				nextState.WB.nop = 0;
			}

			//sw instructions
			else if (state.MEM.wrt_mem) {

				// RAW Hazard: MEM-MEM
				if (state.WB.wrt_enable) {
					if (state.WB.Wrt_reg_addr == state.MEM.Rt)
						state.MEM.Store_data = state.WB.Wrt_data;
				}

				ext_dmem.writeDataMem(state.MEM.ALUresult, state.MEM.Store_data);
				nextState.WB.Wrt_data = ext_dmem.readDataMem(state.MEM.ALUresult);
				nextState.WB.nop = 0;
			}
			// addu & subu & beq
			else {
				nextState.WB.Wrt_data = state.MEM.ALUresult;
				nextState.WB.nop = 0;
			}

			if (state.EX.nop) {
				nextState.MEM.nop = state.EX.nop;
			}
		}
			
			
			
			/* --------------------- EX stage --------------------- */
			if(!state.EX.nop) {
				nextState.MEM.Rs = state.EX.Rs;
				nextState.MEM.Rt = state.EX.Rt;
				nextState.MEM.Wrt_reg_addr = state.EX.Wrt_reg_addr;
				nextState.MEM.rd_mem = state.EX.rd_mem;
				nextState.MEM.wrt_mem = state.EX.wrt_mem;
				nextState.MEM.Store_data = state.EX.Read_data2;
				nextState.MEM.wrt_enable = state.EX.wrt_enable;

				// RAW Hazards: MEM-EX
				if (state.MEM.wrt_enable)
				{
					if (state.MEM.Wrt_reg_addr == state.EX.Rs)
					{
						state.EX.Read_data1 = nextState.WB.Wrt_data;
					}
					if (state.MEM.Wrt_reg_addr == state.EX.Rt)
					{
						state.EX.Read_data2 = nextState.WB.Wrt_data;
						nextState.MEM.Store_data = state.EX.Read_data2;
					}
				}

				// RAW Hazards: MEM-EX
				if (state.WB.wrt_enable)
				{
					if (state.WB.Wrt_reg_addr == state.EX.Rs)
					{
						state.EX.Read_data1 = state.WB.Wrt_data;
					}
					if (state.WB.Wrt_reg_addr == state.EX.Rt)
					{
						state.EX.Read_data2 = state.WB.Wrt_data;
						nextState.MEM.Store_data = state.EX.Read_data2;
					}
				}

				if (!state.EX.is_I_type) {
					//addu instruction
					if (state.EX.alu_op) {
						nextState.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() + state.EX.Read_data2.to_ulong());
					}

					//subs
					else {
						nextState.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() - state.EX.Read_data2.to_ulong());
					}
				} else {
				bitset<32> Offset = bitset<32>(state.EX.Imm.to_ulong());
				if (state.EX.alu_op)
				{
					if (state.EX.Imm[15] == 1)
						for (int i = 31; i > 15; i--)
							Offset[i] = 1;

					//lw instruction
					if (state.EX.rd_mem)    
					{
						nextState.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() + bitset<32>(Offset.to_ulong()).to_ulong());
					}
					//sw instruction
					else
					{
						nextState.MEM.ALUresult = bitset<32>(state.EX.Read_data1.to_ulong() + bitset<32>(Offset.to_ulong()).to_ulong());
					}
				}
				}
				nextState.MEM.nop = 0;
				if (state.ID.nop) {
					nextState.EX.nop = state.ID.nop;
				}
			}
			
			
			
			/* --------------------- ID stage --------------------- */
			if(!state.ID.nop) {
				nextState.EX.Rs = bitset<5>(state.ID.Instr.to_string(), 6, 5);
				nextState.EX.Rt = bitset<5>(state.ID.Instr.to_string(), 11, 5);
				nextState.EX.Imm = bitset<16>(state.ID.Instr.to_string(), 16, 16);
				nextState.EX.Read_data1 = myRF.readRF(nextState.EX.Rs);
				nextState.EX.Read_data2 = myRF.readRF(nextState.EX.Rt);

				int opcode = stoi(state.ID.Instr.to_string().substr(25, 7), 0, 2);
				switch(opcode) {
					//halt instruction
					case 0x7F:
						nextState.EX.nop = 1;
						nextState.ID.nop = 1;
						break;
					//R-type instructions
					case 0x00:
						nextState.EX.Wrt_reg_addr = bitset<5>(state.ID.Instr.to_string().substr(16, 5));
						nextState.EX.is_I_type = 0;
						nextState.EX.rd_mem = 0;
						nextState.EX.wrt_mem = 0;
						nextState.EX.wrt_enable = 1;
						switch(bitset<6>(state.ID.Instr.to_string().substr(26, 6)).to_ulong()) {
							case 0x21:
								nextState.EX.alu_op = 1;
								nextState.EX.nop = 0;
								break;
							case 0x23:
								nextState.EX.alu_op = 0;
								nextState.EX.nop = 0;
								break;

							default: break;
						}
						break;
					//lw
					case 0x23:
						nextState.EX.Wrt_reg_addr = nextState.EX.Rt;
						nextState.EX.alu_op = 1;
						nextState.EX.is_I_type = 1;
						nextState.EX.wrt_enable = 1;
						nextState.EX.rd_mem = 1;
						nextState.EX.wrt_mem = 0;
						nextState.EX.nop = 0;
						break;

					case 0x2B:
						nextState.EX.Wrt_reg_addr = nextState.EX.Rt;
						nextState.EX.alu_op = 1;
						nextState.EX.is_I_type = 1;
						nextState.EX.wrt_enable = 0;
						nextState.EX.rd_mem = 0;
						nextState.EX.wrt_mem = 1;
						nextState.EX.nop = 0;
						break;

					//beq
					case 0x04:
					nextState.EX.Wrt_reg_addr = nextState.EX.Rt;
					nextState.EX.alu_op = 0;
					nextState.EX.is_I_type = 1;
					nextState.EX.wrt_enable = 0;
					nextState.EX.rd_mem = 0;
					nextState.EX.wrt_mem = 0;

					if (nextState.EX.Read_data1 != nextState.EX.Read_data2)
					{
						bitset<32> BranchAddr;

						// SignExtend
						if (nextState.EX.Imm[15] == 1)
						{
							BranchAddr = bitset<32>(nextState.EX.Imm.to_ulong()) << 2;
							for (int i = 31; i > 17; i--)
							{
								BranchAddr[i] = 1;
							}
						}
						else
						{
							BranchAddr = bitset<32>(nextState.EX.Imm.to_ulong()) << 2;
						}

						state.IF.PC = bitset<32>(state.IF.PC.to_ulong() + BranchAddr.to_ulong());
					}
					nextState.EX.nop = 0;
					break;

					default: break;
				}

				if(nextState.MEM.rd_mem && !nextState.EX.is_I_type) {
					if (halted)
					{
						if (nextState.MEM.Wrt_reg_addr == nextState.EX.Rs || nextState.MEM.Wrt_reg_addr == nextState.EX.Rt)
						{
							nextState.EX.nop = true;
							state.IF.PC = bitset<32>(state.IF.PC.to_ulong() - 4);
							halted = false;
						}
					}
					else
					{
						halted = true;
					}
				}
			}
			
			
			
			/* --------------------- IF stage --------------------- */
			if(!state.IF.nop) {
				nextState.ID.Instr = ext_imem.readInstr(state.IF.PC);
				if(nextState.ID.Instr == bitset<32>(0xFFFFFFFF)) {
					state.IF.nop = true;
					nextState.IF.nop = true;
				} else {
					nextState.IF.PC = bitset<32>(state.IF.PC.to_ulong() + 4);
					state.IF.nop = false;
					nextState.IF.nop = false;
				}
			}
			
			totalInstructions++;
			// halted = true;
			if (state.IF.nop && state.ID.nop && state.EX.nop && state.MEM.nop && state.WB.nop) {
				halted = true;
				outputMetrics();
			}
        
            myRF.outputRF(cycle); // dump RF
			printState(nextState, cycle); //print states after executing cycle 0, cycle 1, cycle 2 ... 
       
			state = nextState; //The end of the cycle and updates the current state with the values calculated in this cycle
			cycle++;
		}

		void outputMetrics() {
			float cpi = (float)cycle / totalInstructions;
			float instructionsPerCycle = (float)totalInstructions / cycle;
			cout << "\nFIVE STAGE PIPELINE PERFORMANCE METRICS" << endl;
			cout << "-------------------------------------" << endl;
			cout << "Total instructions: " << totalInstructions << endl;
			cout << "Total cycles: " << cycle << endl;
			cout << "CPI: " << cpi << endl;
			cout << "Instructions per cycle: " << instructionsPerCycle << endl;
			cout << "-------------------------------------" << endl;
		}

		void printState(stateStruct state, int cycle) {
		    ofstream printstate;
			if (cycle == 0)
				printstate.open(opFilePath, std::ios_base::trunc);
			else 
		    	printstate.open(opFilePath, std::ios_base::app);
		    if (printstate.is_open()) {
		        printstate<<"State after executing cycle:\t"<<cycle<<endl; 

		        printstate<<"IF.PC:\t"<<state.IF.PC.to_ulong()<<endl;        
		        printstate<<"IF.nop:\t"<<state.IF.nop<<endl; 

		        printstate<<"ID.Instr:\t"<<state.ID.Instr<<endl; 
		        printstate<<"ID.nop:\t"<<state.ID.nop<<endl;

		        printstate<<"EX.Read_data1:\t"<<state.EX.Read_data1<<endl;
		        printstate<<"EX.Read_data2:\t"<<state.EX.Read_data2<<endl;
		        printstate<<"EX.Imm:\t"<<state.EX.Imm<<endl; 
		        printstate<<"EX.Rs:\t"<<state.EX.Rs<<endl;
		        printstate<<"EX.Rt:\t"<<state.EX.Rt<<endl;
		        printstate<<"EX.Wrt_reg_addr:\t"<<state.EX.Wrt_reg_addr<<endl;
		        printstate<<"EX.is_I_type:\t"<<state.EX.is_I_type<<endl; 
		        printstate<<"EX.rd_mem:\t"<<state.EX.rd_mem<<endl;
		        printstate<<"EX.wrt_mem:\t"<<state.EX.wrt_mem<<endl;        
		        printstate<<"EX.alu_op:\t"<<state.EX.alu_op<<endl;
		        printstate<<"EX.wrt_enable:\t"<<state.EX.wrt_enable<<endl;
		        printstate<<"EX.nop:\t"<<state.EX.nop<<endl;        

		        printstate<<"MEM.ALUresult:\t"<<state.MEM.ALUresult<<endl;
		        printstate<<"MEM.Store_data:\t"<<state.MEM.Store_data<<endl; 
		        printstate<<"MEM.Rs:\t"<<state.MEM.Rs<<endl;
		        printstate<<"MEM.Rt:\t"<<state.MEM.Rt<<endl;   
		        printstate<<"MEM.Wrt_reg_addr:\t"<<state.MEM.Wrt_reg_addr<<endl;              
		        printstate<<"MEM.rd_mem:\t"<<state.MEM.rd_mem<<endl;
		        printstate<<"MEM.wrt_mem:\t"<<state.MEM.wrt_mem<<endl; 
		        printstate<<"MEM.wrt_enable:\t"<<state.MEM.wrt_enable<<endl;         
		        printstate<<"MEM.nop:\t"<<state.MEM.nop<<endl;        

		        printstate<<"WB.Wrt_data:\t"<<state.WB.Wrt_data<<endl;
		        printstate<<"WB.Rs:\t"<<state.WB.Rs<<endl;
		        printstate<<"WB.Rt:\t"<<state.WB.Rt<<endl;
		        printstate<<"WB.Wrt_reg_addr:\t"<<state.WB.Wrt_reg_addr<<endl;
		        printstate<<"WB.wrt_enable:\t"<<state.WB.wrt_enable<<endl;
		        printstate<<"WB.nop:\t"<<state.WB.nop<<endl; 
		    }
		    else cout<<"Unable to open FS StateResult output file." << endl;
		    printstate.close();
		}
	private:
		string opFilePath;
};

int main(int argc, char* argv[]) {
	
	string ioDir = "";
    if (argc == 1) {
        cout << "Enter path containing the memory files: ";
        cin >> ioDir;
    }
    else if (argc > 2) {
        cout << "Invalid number of arguments. Machine stopped." << endl;
        return -1;
    }
    else {
        ioDir = argv[1];
        cout << "IO Directory: " << ioDir << endl;
    }

    InsMem imem = InsMem("Imem", ioDir);
    DataMem dmem_ss = DataMem("SS", ioDir);
	DataMem dmem_fs = DataMem("FS", ioDir);

	SingleStageCore SSCore(ioDir, imem, dmem_ss);
	FiveStageCore FSCore(ioDir, imem, dmem_fs);

    while (1) {
		if (!SSCore.halted)
			SSCore.step();
		
		if (!FSCore.halted)
			FSCore.step();

		if (SSCore.halted && FSCore.halted)
			break;
    }
    
	// dump SS and FS data mem.
	dmem_ss.outputDataMem();
	dmem_fs.outputDataMem();

	return 0;
}