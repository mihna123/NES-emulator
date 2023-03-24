#ifndef CPU_HPP_INCLUDED
#define CPU_HPP_INCLUDED

#include<fstream>
#include<iostream>
#include<iomanip>
#include<thread>
#include<chrono>

#define KB 1024
#define MEMORY_SIZE (64 * KB)
class CPU {
private:
    std::uint8_t memory[MEMORY_SIZE];     //ffff bytes
    std::uint8_t regA;          //accumulator
    std::uint8_t regX;          //x and y are index regs
    std::uint8_t regY;
    std::uint8_t regP;          //status register  -- Negative, Overflow, ignored, Break, Decimal, Interrupt, Zero, Carry
    std::uint8_t regSP;         //stack pointer     |    7         6         5       4       3         2        1     0
    std::uint16_t regPC;        //program counter
    std::uint16_t PRG_ROM_size;
    std::uint16_t CHR_ROM_size;
    std::uint8_t flag6;
public:
    CPU(){
        for(int i = 0 ; i < MEMORY_SIZE ; i++){
            memory[i] = 0;
        }
        regA = 0;
        regX = 0;
        regY = 0;
        regP = 0;
        regSP = 0xff; //stack goes from 0x01ff to 0x0100, since its one byte you add 256(0x0100) for it to work
        regPC = 0;
        PRG_ROM_size = 0;
        CHR_ROM_size = 0;
        flag6 = 0;
    }

    void ADC(std::uint16_t adress_index){
        std::uint8_t carry_bit = 1 & regP;
        std::uint8_t adder = memory[adress_index];
        //checks for setting carry and overflow flags
        if((adder + regA + carry_bit) > 255){
            regP = regP | 0b01000001;

        }else{
            regP = regP & 0b10111110;
        }

        //the operation
        regA += adder + carry_bit;

        //checks for setting zero flag
        if(regA == 0){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }

        //checks for setting negative flag
        if((regA & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }


    }

    void AND(std::uint16_t adress_index){
        regA = memory[adress_index] & regA;

        //checks for setting negative flag
        if((regA & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }

        //checks for setting zero flag
        if(regA == 0){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }
    }

    void ASL(std::uint16_t adress_index){
        std::uint8_t operand = memory[adress_index];
        //checking for setting the carry flag
        if((operand & 0b10000000) != 0){
            regP = regP | 0b00000001;
        }else{
            regP = regP & 0b11111110;
        }

        //doing actual operation
        operand <<= 1;

        //checking for setting the zero flag
        if(operand == 0){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }

        //checking for setting the negative flag
        if((operand & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }
    }

    void BCC(std::uint16_t adress_index){
        if(!(regP & 0b00000001)){
            regPC = adress_index;
        }
    }

    void BCS(std::uint16_t adress_index){
        if(regP & 0b00000001){
            regPC = adress_index;
        }
    }

    void BEQ(std::uint16_t adress_index){
        if(regP & 0b00000010){
            regPC = adress_index;
        }
    }

    void BIT(std::uint16_t adress_index){
        std::uint8_t operand = memory[adress_index];

        //setting negative flag
        if(operand & 0b10000000){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }

        //setting overflow flag
        if(operand & 0b01000000){
            regP = regP | 0b01000000;
        }else{
            regP = regP & 0b10111111;
        }

        //setting zero flag
        if(!(operand & regA)){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }
    }

    void BMI(std::uint16_t adress_index){
        if(regP & 0b10000000){
            regPC = adress_index;
        }
    }

    void BNE(std::uint16_t adress_index){
        if(!(regP & 0b00000010)){
            regPC = adress_index;
        }
    }

    void BPL(std::uint16_t adress_index){
        if(!(regP & 0b10000000)){
            regPC = adress_index;
        }
    }

    void BRK(){
        //pushing PC to stack
        memory[0x100 + (regSP--)] = regPC + 2;


        //setting break flag
        regP = regP | 0b00010000;

        //pushing SR to stack
        memory[0x100 + (regSP--)] = regP;
    }

    void BVC(std::uint16_t adress_index){
        if(!(regP & 0b01000000)){
            regPC = adress_index;
        }
    }

    void BVS(std::uint16_t adress_index){
        if(regP & 0b01000000){
            regPC = adress_index;
        }
    }

    void CLC(){
        regP = regP & 0b11111110;
    }

    void CLI(){
        regP = regP & 0b11111011;
    }

    void CLV(){
        regP = regP & 0b10111111;
    }

    void CMP(std::uint16_t adress_index){
        std::uint8_t operand = memory[adress_index];

        //setting the carry flag
        if(regA >= operand){
            regP = regP | 0b00000001;
        }else{
            regP = regP & 0b11111110;
        }

        std::uint8_t result = regA - operand;

        //setting the zero flag
        if(!result){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }

        //setting the negative flag
        if((result & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }
    }

    void CPX(std::uint16_t adress_index){
        std::uint8_t operand = memory[adress_index];

        //setting the carry flag
        if(regX >= operand){
            regP = regP | 0b00000001;
        }else{
            regP = regP & 0b11111110;
        }

        std::uint8_t result = regX - operand;

        //setting the zero flag
        if(!result){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }

        //setting the negative flag
        if((result & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }
    }

    void CPY(std::uint16_t adress_index){
        std::uint8_t operand = memory[adress_index];

        //setting the carry flag
        if(regY >= operand){
            regP = regP | 0b00000001;
        }else{
            regP = regP & 0b11111110;
        }

        std::uint8_t result = regY - operand;

        //setting the zero flag
        if(!result){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }

        //setting the negative flag
        if((result & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }
    }

    void CLD(){
        regP = regP & 0x11110111;
    }

    void DEC(std::uint16_t adress_index){
        memory[adress_index]--;

        //setting the zero flag
        if(!memory[adress_index]){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }

        //setting the negative flag
        if((memory[adress_index] & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }
    }

    void DEX(){
        regX--;

        //setting the zero flag
        if(!regX){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }

        //setting the negative flag
        if((regX & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }
    }

    void DEY(){
       regY--;

        //setting the zero flag
        if(!regY){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }

        //setting the negative flag
        if((regY & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }
    }

    void EOR(std::uint16_t adress_index){
        regA = regA ^ memory[adress_index];

        //setting the zero flag
        if(!regA){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }

        //setting the negative flag
        if((regA & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }
    }

    void INC(std::uint16_t adress_index){
        //setting zero and doing inc
        if(++memory[adress_index] == 0){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }

        //setting the negative flag
        if((memory[adress_index] & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }
    }

    void INX(){
        //setting zero and doing inc
        if(++regX == 0){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }

        //setting the negative flag
        if((regX & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }
    }

    void INY(){
        //setting zero and doing inc
        if(++regY == 0){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }

        //setting the negative flag
        if((regY & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }
    }

    void JMP(std::uint16_t adress_index){
        //jumping to adress
        regPC = adress_index;
    }

    void JSR(std::uint16_t adress_index){
        //pushing return adress to the stack
        //must always add 0x100 to stack pointer!
        memory[0x100 + (regSP--)] = regPC + 1;

        //jumping to adress
        regPC = adress_index;
    }



    void LDA(std::uint8_t value){
        regA = value;

        //checks for setting negative flag
        if((regA & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }

        //checks for setting zero flag
        if(regA == 0){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }
    }

    void LDX(std::uint8_t value){
        regX = value;

        //checks for setting negative flag
        if((regX & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }

        //checks for setting zero flag
        if(regX == 0){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }
    }

    void LDY(std::uint8_t value){
        regY = value;

        //checks for setting negative flag
        if((regY & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }

        //checks for setting zero flag
        if(regY == 0){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }
    }

    void LSR(std::uint16_t adress_index){
        //setting the cary flag
        if(memory[adress_index] & 0x00000001){
            regP = regP | 0x00000001;
        }else{
            regP = regP & 0x11111110;
        }

        //setting the negative flag
        regP = regP & 0x01111111;

        memory[adress_index] >>= 1;

        if(!memory[adress_index]){
            regP = regP | 0x00000010;
        }else{
            regP = regP & 0x11111101;
        }
    }

    void ORA(uint16_t adress_index){
        memory[adress_index] = memory[adress_index] | regA;

        //setting zero flag
        if(memory[adress_index] == 0){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }

        //setting negative flag
        if(memory[adress_index] & 0b10000000 != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }
    }

    void STA(std::uint16_t adress_index){
        memory[adress_index] = regA;
    }

    void STX(std::uint16_t adress_index){
        memory[adress_index] = regX;
    }

    void STY(std::uint16_t adress_index){
        memory[adress_index] = regY;
    }

    void SEC(){
        regP = regP | 0b00000001;
    }

    void SEI(){
        regP = regP | 0b00000100;
    }

    void SED(){
        regP = regP | 0b00001000;
    }

    void TAX(){
        regX = regA;

        //setting negative flag
        if((regA & 0b10000000)){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }

        //setting zero flag
        if(!regA){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }
    }

    void TAY(){
        regY = regA;

        //setting negative flag
        if((regA & 0b10000000)){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }

        //setting zero flag
        if(!regA){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }
    }

    void TSX(){
        regX = regSP;

        //setting negative flag
        if((regSP & 0b10000000)){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }

        //setting zero flag
        if(!regSP){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }
    }

    void TXA(){
        regA = regX;

        //setting negative flag
        if((regA & 0b10000000)){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }

        //setting zero flag
        if(!regA){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }
    }

    void TXS(){
        regSP = regX;

        //setting negative flag
        if((regSP & 0b10000000)){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }

        //setting zero flag
        if(!regSP){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }
    }

    void TYA(){
        regA = regY;

        //setting negative flag
        if((regA & 0b10000000)){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }

        //setting zero flag
        if(!regA){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }
    }

    void ROL(std::uint16_t adress_index){
        bool carry = false;
        //checking if carry will be 1
        if(memory[adress_index] & 0b10000000 != 0){
            carry = true;
        }
        //doing it
        memory[adress_index] <<= 1;

        if(regP & 0b00000001 != 0){
            memory[adress_index] ++;
        }

        //setting carry after
        if(carry){
            regP = regP | 0b00000001;
        }else{
            regP = regP & 0b11111110;
        }

        //setting negative flag
        if(memory[adress_index] & 0b10000000 != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }

        //setting zero flag
        if(memory[adress_index] == 0){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }
    }

    void ROR(std::uint16_t adress_index){
          bool carry = false;
        //checking if carry will be 1
        if(memory[adress_index] & 0b00000001 != 0){
            carry = true;
        }
        //doing it
        memory[adress_index] >>= 1;

        if(regP & 0b00000001 != 0){
            memory[adress_index] += 0b10000000;
        }

        //setting carry after
        if(carry){
            regP = regP | 0b00000001;
        }else{
            regP = regP & 0b11111110;
        }

        //setting negative flag
        if(memory[adress_index] & 0b10000000 != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }

        //setting zero flag
        if(memory[adress_index] == 0){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }
    }

    void RTI(){
        //pullin SR from stack
        regP = memory[0x100 + regSP++];

        //ignore fifth flag
        regP = regP & 0x11101111;

        //pulling PC from stack
        regPC = memory[0x100 + regSP++];
    }

    void RTS(){
        //just pulling PC froom stack
        regPC = memory[0x100 + regSP++];
    }

    void SBC(std::uint16_t adress_index){
        //carry flag for borrowing
        if(regA < memory[adress_index]){
            regP = regP | 0b00000001;
        }else{
            regP = regP & 0b11111110;
        }

        //setting the overflow flag
        if((int)regA - (int)memory[adress_index] < -128){
            regP = regP | 0b01000000;
        }else{
            regP = regP & 0b10111111;
        }

        regA -= memory[adress_index];

        //setting the negative flag
        if(regA & 0b10000000 != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }

        //setting the zero flag
        if(regA == 0){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }


    }

    void PHA(){
        //pushing accumulator to stack
        memory[0x100 + regSP--] = regA;
    }

    void PLA(){
        //pulling from stack
        regA = memory[0x100 + regSP++];

        //setting zero flag
        if(regA == 0){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }

        //setting negative flag
        if(regA & 0b10000000 != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }
    }

    void PHP(){
        //pushing SR to stack with break and bit 5 set to 1
        memory[0x100 + regSP--] = regP | 0b00110000;

    }

    void PLP(){
        //pulling SR from stack while ignoring break and bit 5
        regP = memory[0x100 + regSP++] & 0b11001111;
    }

    //loads a .nes file into memory
    void load(std::string file_name){
        std::ifstream file(file_name,std::ios_base::binary);
        char byte;
        //begining of PRG_ROM memory
        std::uint16_t program_adress = 0x4020;

        //get past unused bytes
        for(int i = 0; i < 4; ++i){
            file.get(byte);
        }

        //set flags and program sizes
        file.get(byte);
        PRG_ROM_size = 16 * KB * byte;
        file.get(byte);
        CHR_ROM_size = 8 * KB * byte;
        file.get(byte);
        flag6 = byte;

        //get past the rest of header bytes
        for(int i = 7; i < 16; i++){
            file.get(byte);
        }

        while(file.get(byte) && program_adress <= 0xffff){
            memory[program_adress] = byte;
            program_adress++;
        }

        file.close();
    }

    void do_operation(std::uint8_t op_code){
        std::uint8_t adress_8bit;
        std::uint16_t adress_16bit;
        switch(op_code){
            //ADC (ADD with Carry)
            //imidiate
            case 0x69:
                adress_16bit = regPC + 1;
                ADC(adress_16bit);
                regPC += 2;
                break;
            //zero page
            case 0x65:
                adress_8bit = memory[regPC + 1];
                ADC(adress_8bit);
                regPC += 2;
                break;
            //zero page, x
            case 0x75:
                adress_8bit = memory[regPC + 1] + regX;
                ADC(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0x6d:
                //6502 is little endian
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                ADC(adress_16bit);
                regPC += 3;
                break;
            //absolute, x
            case 0x7d:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regX;
                ADC(adress_16bit);
                regPC += 3;
                break;
            //absolute, y
            case 0x79:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regY;
                ADC(adress_16bit);
                regPC += 3;
                break;
            //indirect, x
            case 0x61:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1 + regX];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1] + regX];
                ADC(adress_16bit);
                regPC += 2;
                break;
            //indirect, y
            case 0x71:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1 + regY];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1] + regY];
                ADC(adress_16bit);
                regPC += 2;
                break;
            
            //AND (Bitwise and with Accumulator)
            //imidiate
            case 0x29:
                adress_16bit = regPC + 1;
                AND(adress_16bit);
                regPC += 2;
                break;
            //zero page
            case 0x25:
                adress_8bit = memory[regPC + 1];
                AND(adress_8bit);
                regPC += 2;
                break;
            //zero page, x
            case 0x35:
                adress_8bit = memory[regPC + 1] + regX;
                AND(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0x2d:
                //6502 is little endian
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                AND(adress_16bit);
                regPC += 3;
                break;
            //absolute, x
            case 0x3d:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regX;
                AND(adress_16bit);
                regPC += 3;
                break;
            //absolute, y
            case 0x39:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regY;
                AND(adress_16bit);
                regPC += 3;
                break;
            //indirect, X
            case 0x21:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1 + regX];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1] + regX];
                AND(adress_16bit);
                regPC += 2;
                break;
            //indirect, Y
            case 0x31:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1 + regY];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1] + regY];
                AND(adress_16bit);
                regPC += 2;
                break;

            //ASL (Arithmetic Shift Left)
            //Accumulator
            case 0x0a:
                memory[0] = regA;
                ASL(0);
                regA = memory[0];
                regPC ++;
                break;
            //zero page
            case 0x06:
                adress_8bit = memory[regPC + 1];
                ASL(adress_8bit);
                regPC += 2;
                break;
            //zero page, x
            case 0x16:
                adress_8bit = memory[regPC + 1] + regX;
                ASL(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0x0e:
                //6502 is little endian
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                ASL(adress_16bit);
                regPC += 3;
                break;
            //absolute, x
            case 0x1e:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regX;
                ASL(adress_16bit);
                regPC += 3;
                break;

            //BIT (test BITs)
            //zero page
            case 0x24:
                adress_8bit = memory[regPC + 1];
                BIT(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0x2c:
                //6502 is little endian
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                BIT(adress_16bit);
                regPC += 3;
                break;

            //BRANCH instructions
            case 0x10:
                //fix jumps TODO 
                adress_16bit = ++regPC + (std::int8_t)memory[regPC + 1];
                BPL(adress_16bit);
                break;
            case 0x30:
                adress_16bit = ++regPC + (std::int8_t)memory[regPC + 1];
                BMI(adress_16bit);
                break;
            case 0x50:
                adress_16bit = ++regPC + (std::int8_t)memory[regPC + 1];
                BVC(adress_16bit);
                break;
            case 0x70:
                adress_16bit = ++regPC + (std::int8_t)memory[regPC + 1];
                BVS(adress_16bit);
                break;
            case 0x90:
                adress_16bit = ++regPC + (std::int8_t)memory[regPC + 1];
                BCC(adress_16bit);
                break;
            case 0xb0:
                adress_16bit = ++regPC + (std::int8_t)memory[regPC + 1];
                BCS(adress_16bit);
                break;
            case 0xd0:
                adress_16bit = ++regPC + (std::int8_t)memory[regPC + 1];
                BNE(adress_16bit);
                break;
            case 0xf0:
                adress_16bit = ++regPC + (std::int8_t)memory[regPC + 1];
                BEQ(adress_16bit);
                break;

            //BRK (Break)
            case 0x00:
                BRK();
                regPC ++;
                break;
            
            //CMP (Compare accumulator)
            //immediate
            case 0xc9:
                adress_16bit = regPC + 1;
                CMP(adress_16bit);
                regPC += 2;
                break;
            //zero page
            case 0xc5:
                adress_8bit = memory[regPC + 1];
                CMP(adress_8bit);
                regPC += 2;
                break;
            //zero page, x
            case 0xd5:
                adress_8bit = memory[regPC + 1] + regX;
                CMP(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0xcd:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                CMP(adress_16bit);
                regPC += 3;
                break;
            //absolute, x
            case 0xdd:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regX;
                CMP(adress_16bit);
                regPC += 3;
                break;
            //absolute, y
            case 0xd9:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regY;
                CMP(adress_16bit);
                regPC += 3;
                break;
            //indirect, x
            case 0xc1:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1 + regX];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1] + regX];
                CMP(adress_16bit);
                regPC += 2;
                break;
            //indirect, y
            case 0xd1:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1 + regY];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1] + regY];
                CMP(adress_16bit);
                regPC += 2;
                break;
            
            //CPX (Compare X Register)
            //immediate
            case 0xe0:
                adress_16bit = regPC + 1;
                CPX(adress_16bit);
                regPC += 2;
                break;
            //zero page
            case 0xe4:
                adress_8bit = memory[regPC + 1];
                CPX(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0xec:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                CPX(adress_16bit);
                regPC += 3;
                break;
            
            //CPY (Compare Y Register)
            //immediate
            case 0xc0:
                adress_16bit = regPC + 1;
                CPY(adress_16bit);
                regPC += 2;
                break;
            //zero page
            case 0xc4:
                adress_8bit = memory[regPC + 1];
                CPY(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0xcc:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                CPY(adress_16bit);
                regPC += 3;
                break;
            
            //DEC (Decrement memory)
            //zero page
            case 0xc6:
                adress_8bit = memory[regPC + 1];
                DEC(adress_8bit);
                regPC += 2;
                break;
            //zero page,x
            case 0xd6:
                adress_8bit = memory[regPC + 1] + regX;
                DEC(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0xce:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                DEC(adress_16bit);
                regPC += 3;
                break;
            //absolute,x
            case 0xde:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regX;
                DEC(adress_16bit);
                regPC += 3;
                break;
            
            //EOR (bitwise Exclusive OR)
            //imidiate
            case 0x49:
                adress_16bit = regPC + 1;
                EOR(adress_16bit);
                regPC += 2;
                break;
            //zero page
            case 0x45:
                adress_8bit = memory[regPC + 1];
                EOR(adress_8bit);
                regPC += 2;
                break;
            //zero page, x
            case 0x55:
                adress_8bit = memory[regPC + 1] + regX;
                EOR(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0x4d:
                //6502 is little endian
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                EOR(adress_16bit);
                regPC += 3;
                break;
            //absolute, x
            case 0x5d:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regX;
                EOR(adress_16bit);
                regPC += 3;
                break;
            //absolute, y
            case 0x59:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regY;
                EOR(adress_16bit);
                regPC += 3;
                break;
            //indirect, X
            case 0x41:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1 + regX];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1] + regX];
                EOR(adress_16bit);
                regPC += 2;
                break;
            //indirect, Y
            case 0x51:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1 + regY];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1] + regY];
                EOR(adress_16bit);
                regPC += 2;
                break;

            //FLAG Instructions
            case 0x18:
                CLC();
                regPC++;
                break;
            case 0x38:
                SEC();
                regPC++;
                break;
            case 0x58:
                CLI();
                regPC++;
                break;
            case 0x78:
                SEI();
                regPC++;
                break;
            case 0xb8:
                CLV();
                regPC++;
                break;
            case 0xd8:
                CLD();
                regPC++;
                break;
            case 0xf8:
                SED();
            
            //INC (Increment Memory)
            //zero page
            case 0xe6:
                adress_8bit = memory[regPC + 1];
                INC(adress_8bit);
                regPC += 2;
                break;
            //zero page,x
            case 0xf6:
                adress_8bit = memory[regPC + 1] + regX;
                INC(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0xee:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                INC(adress_16bit);
                regPC += 3;
                break;
            //absolute,x
            case 0xfe:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regX;
                INC(adress_16bit);
                regPC += 3;
                break;
            
            //JMP (Jump)
            //absolute
            case 0x4c:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                JMP(adress_16bit);
                regPC += 3;
                break;
            //indirect
            case 0x6c:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1]];
                JMP(adress_16bit);
                regPC += 2;
                break;
            
            //JSR (Jump To Subroutine)
            //absolute
            case 0x20:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                JSR(adress_16bit);
                regPC += 3;
                break;

            //LDA (Load Accumulator)
            //imidiate
            case 0xa9:
                adress_16bit = regPC + 1;
                LDA(adress_16bit);
                regPC += 2;
                break;
            //zero page
            case 0xa5:
                adress_8bit = memory[regPC + 1];
                LDA(adress_8bit);
                regPC += 2;
                break;
            //zero page, x
            case 0xb5:
                adress_8bit = memory[regPC + 1] + regX;
                LDA(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0xad:
                //6502 is little endian
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                LDA(adress_16bit);
                regPC += 3;
                break;
            //absolute, x
            case 0xbd:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regX;
                LDA(adress_16bit);
                regPC += 3;
                break;
            //absolute, y
            case 0xb9:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regY;
                LDA(adress_16bit);
                regPC += 3;
                break;
            //indirect, X
            case 0xa1:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1 + regX];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1] + regX];
                LDA(adress_16bit);
                regPC += 2;
                break;
            //indirect, Y
            case 0xb1:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1 + regY];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1] + regY];
                LDA(adress_16bit);
                regPC += 2;
                break;

            //LDX (Load X Register)
            //imidiate
            case 0xa2:
                adress_16bit = regPC + 1;
                LDX(adress_16bit);
                regPC += 2;
                break;
            //zero page
            case 0xa6:
                adress_8bit = memory[regPC + 1];
                LDX(adress_8bit);
                regPC += 2;
                break;
            //zero page, y
            case 0xb6:
                adress_8bit = memory[regPC + 1] + regY;
                LDX(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0xae:
                //6502 is little endian
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                LDX(adress_16bit);
                regPC += 3;
                break;
            //absolute, y
            case 0xbe:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regY;
                LDX(adress_16bit);
                regPC += 3;
                break;

            //LDY (Load Y Register)
            //imidiate
            case 0xa0:
                adress_16bit = regPC + 1;
                LDY(adress_16bit);
                regPC += 2;
                break;
            //zero page
            case 0xa4:
                adress_8bit = memory[regPC + 1];
                LDY(adress_8bit);
                regPC += 2;
                break;
            //zero page, x
            case 0xb4:
                adress_8bit = memory[regPC + 1] + regX;
                LDY(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0xac:
                //6502 is little endian
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                LDY(adress_16bit);
                regPC += 3;
                break;
            //absolute, x
            case 0xbc:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regX;
                LDY(adress_16bit);
                regPC += 3;
                break;
            
            //LSR (Logical Shift Right)
            //Accumulator
            case 0x4a:
                memory[0] = regA;
                LSR(0);
                regA = memory[0];
                regPC ++;
                break;
            //zero page
            case 0x46:
                adress_8bit = memory[regPC + 1];
                LSR(adress_8bit);
                regPC += 2;
                break;
            //zero page, x
            case 0x56:
                adress_8bit = memory[regPC + 1] + regX;
                LSR(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0x4e:
                //6502 is little endian
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                LSR(adress_16bit);
                regPC += 3;
                break;
            //absolute, x
            case 0x5e:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regX;
                LSR(adress_16bit);
                regPC += 3;
                break;
            
            //NOP (No Operation)
            //implied
            case 0xea:
                regPC ++;
                break;
            
            //ORA (Bitwise Or With Accumulator)
            //imidiate
            case 0x09:
                adress_16bit = regPC + 1;
                ORA(adress_16bit);
                regPC += 2;
                break;
            //zero page
            case 0x05:
                adress_8bit = memory[regPC + 1];
                ORA(adress_8bit);
                regPC += 2;
                break;
            //zero page, x
            case 0x15:
                adress_8bit = memory[regPC + 1] + regX;
                ORA(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0x0d:
                //6502 is little endian
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                ORA(adress_16bit);
                regPC += 3;
                break;
            //absolute, x
            case 0x1d:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regX;
                ORA(adress_16bit);
                regPC += 3;
                break;
            //absolute, y
            case 0x19:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regY;
                ORA(adress_16bit);
                regPC += 3;
                break;
            //indirect, X
            case 0x01:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1 + regX];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1] + regX];
                ORA(adress_16bit);
                regPC += 2;
                break;
            //indirect, Y
            case 0x11:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1 + regY];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1] + regY];
                ORA(adress_16bit);
                regPC += 2;
                break;
            
            //Register instructions
            case 0xaa:
                TAX();
                regPC++;
                break;
            case 0x8a:
                TXA();
                regPC++;
                break;
            case 0xca:
                DEX();
                regPC++;
                break;
            case 0xe8:
                INX();
                regPC++;
                break;
            case 0xa8:
                TAY();
                regPC++;
                break;
            case 0x98:
                TYA();
                regPC++;
                break;
            case 0x88:
                DEY();
                regPC++;
                break;
            case 0xc8:
                INY();
                regPC++;
                break;
            
            //ROL (Rotate Left)
            //Accumulator
            case 0x2a:
                memory[0] = regA;
                ROL(0);
                regA = memory[0];
                regPC ++;
                break;
            //zero page
            case 0x26:
                adress_8bit = memory[regPC + 1];
                ROL(adress_8bit);
                regPC += 2;
                break;
            //zero page, x
            case 0x36:
                adress_8bit = memory[regPC + 1] + regX;
                ROL(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0x2e:
                //6502 is little endian
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                ROL(adress_16bit);
                regPC += 3;
                break;
            //absolute, x
            case 0x3e:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regX;
                ROL(adress_16bit);
                regPC += 3;
                break;

            //ROR (Rotate Right)
            //Accumulator
            case 0x6a:
                memory[0] = regA;
                ROR(0);
                regA = memory[0];
                regPC ++;
                break;
            //zero page
            case 0x66:
                adress_8bit = memory[regPC + 1];
                ROR(adress_8bit);
                regPC += 2;
                break;
            //zero page, x
            case 0x76:
                adress_8bit = memory[regPC + 1] + regX;
                ROR(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0x6e:
                //6502 is little endian
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                ROR(adress_16bit);
                regPC += 3;
                break;
            //absolute, x
            case 0x7e:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regX;
                ROR(adress_16bit);
                regPC += 3;
                break;

            //RTI (Return from Intertupt)
            //implied
            case 0x40:
                RTI();
                break;
            //RST (Return from Subroutine)
            //implied
            case 0x60:
                RTS();
                break;
            
            //SBC (Subtract with Carry)
             //imidiate
            case 0xe9:
                adress_16bit = regPC + 1;
                SBC(adress_16bit);
                regPC += 2;
                break;
            //zero page
            case 0xe5:
                adress_8bit = memory[regPC + 1];
                SBC(adress_8bit);
                regPC += 2;
                break;
            //zero page, x
            case 0xf5:
                adress_8bit = memory[regPC + 1] + regX;
                SBC(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0xed:
                //6502 is little endian
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                SBC(adress_16bit);
                regPC += 3;
                break;
            //absolute, x
            case 0xfd:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regX;
                SBC(adress_16bit);
                regPC += 3;
                break;
            //absolute, y
            case 0xf9:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regY;
                SBC(adress_16bit);
                regPC += 3;
                break;
            //indirect, X
            case 0xe1:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1 + regX];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1] + regX];
                SBC(adress_16bit);
                regPC += 2;
                break;
            //indirect, Y
            case 0xf1:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1 + regY];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1] + regY];
                SBC(adress_16bit);
                regPC += 2;
                break;

            //STA (Store Accumulator)
            //zero page
            case 0x85:
                adress_8bit = memory[regPC + 1];
                STA(adress_8bit);
                regPC += 2;
                break;
            //zero page, x
            case 0x95:
                adress_8bit = memory[regPC + 1] + regX;
                STA(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0x8d:
                //6502 is little endian
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                STA(adress_16bit);
                regPC += 3;
                break;
            //absolute, x
            case 0x9d:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regX;
                STA(adress_16bit);
                regPC += 3;
                break;
            //absolute, y
            case 0x99:
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                adress_16bit += regY;
                STA(adress_16bit);
                regPC += 3;
                break;
            //indirect, X
            case 0x81:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1 + regX];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1] + regX];
                STA(adress_16bit);
                regPC += 2;
                break;
            //indirect, Y
            case 0x91:
                //high byte
                adress_16bit = memory[memory[regPC + 1] + 1 + regY];
                adress_16bit <<= 4;
                //low byte
                adress_16bit += memory[memory[regPC + 1] + regY];
                STA(adress_16bit);
                regPC += 2;
                break;
            
            //Stack INstructions
            case 0x9a:
                TXS();
                regPC++;
                break;
            case 0xba:
                TSX();
                regPC++;
                break;
            case 0x48:
                PHA(); 
                regPC++;
                break;
            case 0x68:
                PLA();
                regPC++;
                break;
            case 0x08:
                PHP();
                regPC++;
                break;
            case 0x28:
                PLP();
                regPC++;
                break;
            
            //STX (Store X Register)
            //zero page
            case 0x86:
                adress_8bit = memory[regPC + 1];
                STX(adress_8bit);
                regPC += 2;
                break;
            //zero page, y
            case 0x96:
                adress_8bit = memory[regPC + 1] + regY;
                STX(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0x8e:
                //6502 is little endian
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                STX(adress_16bit);
                regPC += 3;
                break;

            //STY (Store Y Register)
            //zero page
            case 0x84:
                adress_8bit = memory[regPC + 1];
                STY(adress_8bit);
                regPC += 2;
                break;
            //zero page, x
            case 0x94:
                adress_8bit = memory[regPC + 1] + regX;
                STY(adress_8bit);
                regPC += 2;
                break;
            //absolute
            case 0x8c:
                //6502 is little endian
                adress_16bit = memory[regPC + 2];
                adress_16bit <<= 4;
                adress_16bit += memory[regPC + 1];
                STY(adress_16bit);
                regPC += 3;
                break;
            
            default:
                std::cout<<"Error: Op Code not supported!"<<std::endl;
            
        }
    }

    void run(){
        //beggining of the program
        regPC = 0x4020;
        std::uint8_t op_code = 0;
        while(true){
            op_code = memory[regPC];
            std::cout<<"OP_CODE: "<<std::hex<<std::setw(2)<<std::setfill('0')<<(int)op_code<<std::endl;
            do_operation(op_code);
            printMemory(0,0);
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    void printMemory(std::uint16_t first, std::uint16_t last){
        //printing status register
        std::uint8_t mask = 1;
        std::uint8_t statusReg = regP;
        std::cout<<"Status register:"<<std::endl;
        std::cout<<"C Z I D B - V N"<<std::endl;
        for(int i = 0 ; i < 8 ; i++){
            int status = mask & statusReg;
            std::cout<<status<<" ";
            statusReg >>= 1;
        }
        std::cout<<std::endl<<std::endl;

        //printing memory
        std::cout<<"Memory:"<<std::endl;
        int row_counter = 0;
        std::cout<<std::hex;
        for( ; first <= last ; first++){
            std::cout<<std::setfill('0')<<std::setw(2)<<(unsigned int)memory[first]<<" ";
            row_counter++;
            if(row_counter % 16 == 0){
                std::cout<<std::endl;
            }
        }
        std::cout<<std::dec;
    }
};

#endif // CPU_HPP_INCLUDED
