#ifndef CPU_HPP_INCLUDED
#define CPU_HPP_INCLUDED

#define MEMORY_SIZE 65535
class CPU {
private:
    uint8_t memory[MEMORY_SIZE];     //ffff bytes
    uint8_t regA;          //accumulator
    uint8_t regX;          //x and y are index regs
    uint8_t regY;
    uint8_t regP;          //status register  -- Negative, Overflow, ignored, Break, Decimal, Interrupt, Zero, Carry
    uint8_t regSP;         //stack pointer     |    7         6         5       4       3         2        1     0
    uint16_t regPC;        //program counter
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
    }

    void ADC(uint16_t adress_index){
        uint8_t carry_bit = 1 & regP;
        uint8_t adder = memory[adress_index];
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

    void AND(uint16_t adress_index){
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

    void ASL(){
        //checking for setting the carry flag
        if((regA & 0b10000000) != 0){
            regP = regP | 0b00000001;
        }else{
            regP = regP & 0b11111110;
        }

        //doing actual operation
        regA <<= 1;

        //checking for setting the zero flag
        if(regA == 0){
            regP = regP | 0b00000010;
        }else{
            regP = regP & 0b11111101;
        }

        //checking for setting the negative flag
        if((regA & 0b10000000) != 0){
            regP = regP | 0b10000000;
        }else{
            regP = regP & 0b01111111;
        }
    }

    void BCC(uint16_t adress_index){
        if(!(regP & 0b00000001)){
            regPC = adress_index;
        }
    }

    void BCS(uint16_t adress_index){
        if(regP & 0b00000001){
            regPC = adress_index;
        }
    }

    void BEQ(uint16_t adress_index){
        if(regP & 0b00000010){
            regPC = adress_index;
        }
    }

    void BIT(uint16_t adress_index){
        uint8_t operand = memory[adress_index];

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

    void BMI(uint16_t adress_index){
        if(regP & 0b10000000){
            regPC = adress_index;
        }
    }

    void BNE(uint16_t adress_index){
        if(!(regP & 0b00000010)){
            regPC = adress_index;
        }
    }

    void BPL(){
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

    void BVC(uint16_t adress_index){
        if(!(regP & 0b01000000)){
            regPC = adress_index;
        }
    }

    void BVS(uint16_t adress_index){
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

    void CMP(uint16_t adress_index){
        uint8_t operand = memory[adress_index];

        //setting the carry flag
        if(regA >= operand){
            regP = regP | 0b00000001;
        }else{
            regP = regP & 0b11111110;
        }

        uint8_t result = regA - operand;

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

    void CPX(uint16_t adress_index){
        uint8_t operand = memory[adress_index];

        //setting the carry flag
        if(regX >= operand){
            regP = regP | 0b00000001;
        }else{
            regP = regP & 0b11111110;
        }

        uint8_t result = regX - operand;

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

    void CPY(uint16_t adress_index){
        uint8_t operand = memory[adress_index];

        //setting the carry flag
        if(regY >= operand){
            regP = regP | 0b00000001;
        }else{
            regP = regP & 0b11111110;
        }

        uint8_t result = regY - operand;

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

    void DEC(uint16_t adress_index){
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

    void EOR(uint16_t adress_index){
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

    void INC(uint16_t adress_index){
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

    void JMP(uint16_t adress_index){
        //jumping to adress
        regPC = adress_index;
    }

    void JSR(uint16_t adress_index){
        //pushing return adress to the stack
        //must always add 0x100 to stack pointer!
        memory[0x100 + (regSP--)] = regPC + 1;

        //jumping to adress
        regPC = adress_index;
    }



    void LDA(uint8_t value){
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

    void LDX(uint8_t value){
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

    void LDY(uint8_t value){
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

    void LSR(bool use_A, uint16_t adress_index){
        //TODO:
    }

    void STA(uint16_t adress_index){
        memory[adress_index] = regA;
    }

    void STX(uint16_t adress_index){
        memory[adress_index] = regX;
    }

    void STY(uint16_t adress_index){
        memory[adress_index] = regY;
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



    void printMemory(uint16_t first, uint16_t last){
        //printing status register
        uint8_t mask = 1;
        uint8_t statusReg = regP;
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
        for( ; first <= last ; first++){
            std::cout<<(unsigned int)memory[first]<<" ";
            row_counter++;
            if(row_counter % 16 == 0){
                std::cout<<std::endl;
            }
        }
    }
};

#endif // CPU_HPP_INCLUDED
