#ifndef _INSTRUCTIONS_H_
#define _INSTRUCTIONS_H_

#include "cpu.h"
#include <stdio.h>

int decode_CB_instruction(byte instr, CPU* cpu);

int NOP_00(CPU* cpu)
{
	//no op
	cpu->pc++;
    return 1;
}
int LD_01(CPU* cpu)
{
	//load imm 16 bit into BC register
	cpu->pc++;
	cpu->c = cpu->ram[cpu->pc++];
	cpu->b = cpu->ram[cpu->pc++];
    return 3;
}
int LD_02(CPU* cpu)
{
	//store a in address BC
	cpu->pc++;
	uint16_t address = ((uint16_t)cpu->b << 8) | cpu->c;
    cpu->writeMemory(address, cpu->a);
    return 2;
}
int INC_03(CPU* cpu)
{
	//inc BC

	cpu->pc++;
	if (cpu->c< 0xFF) cpu->c++;
    else
    {
        cpu->c = 0;
        if (cpu->b < 0xFF)
        {
            cpu->b++;
        }
        else
        {
            cpu->b = 0;
        }
    }
    return 2;
}
int INC_04(CPU* cpu)
{
    // INC B - Increment register B
    cpu->pc++;
    uint8_t original = cpu->b;
    cpu->b++;
    cpu->z_flag = (cpu->b == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((original & 0x0F) == 0x0F);
    return 1;
}
int DEC_05(CPU* cpu)
{
    // DEC B - Decrement register B
    cpu->pc++;
    uint8_t original = cpu->b;
    cpu->b--;
    cpu->z_flag = (cpu->b == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((original & 0x0F) == 0x00);
    return 1;
}

int LD_06(CPU* cpu)
{
    // LD B, n - Load immediate value into register B
    cpu->pc++;
    cpu->b = cpu->ram   [cpu->pc++];
    return 2;
}

int RLCA_07(CPU* cpu)
{
    // RLCA - Rotate A left circular
    cpu->pc++;
    uint8_t carry = (cpu->a & 0x80) >> 7;
    cpu->a = (cpu->a << 1) | carry;
    cpu->z_flag = false;
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = (carry == 1);
    return 1;
}
int LD_08(CPU* cpu)
{
    // LD (nn), SP - Store SP into memory at address nn
    cpu->pc++;
    uint16_t address = cpu->ram   [cpu->pc++] | (cpu->ram   [cpu->pc++] << 8);
    cpu->writeMemory(address, cpu->sp & 0xFF);
    cpu->writeMemory(address + 1, (cpu->sp >> 8) & 0xFF);
    return 5;
}

int ADD_09(CPU* cpu)
{
    // ADD HL, BC - Add BC to HL
    cpu->pc++;
    uint16_t hl = (cpu->h << 8) | cpu->l;
    uint16_t de = (cpu->b << 8) | cpu->c;
    uint32_t result = hl + de;
    cpu->h = (result >> 8) & 0xFF;
    cpu->l = result & 0xFF;
    cpu->n_flag = false;
    cpu->h_flag = ((hl & 0x0FFF) + (de & 0x0FFF) > 0x0FFF);
    cpu->c_flag = (result > 0xFFFF);
    return 2;
}

int LD_0A(CPU* cpu)
{
    // LD A, (BC) - Load value from memory address BC into A
    cpu->pc++;
    uint16_t address = (cpu->b << 8) | cpu->c;
    //if (address == 0xFF44) ly = 0x90;
    cpu->a = cpu->readMemory(address);
    //cpu->a = cpu->ram[address];
    return 2;
}

int DEC_0B(CPU* cpu)
{
    // DEC BC - Decrement BC register pair
    cpu->pc++;
    uint16_t bc = (cpu->b << 8) | cpu->c;
    bc--;
    cpu->b = (bc >> 8) & 0xFF;
    cpu->c = bc & 0xFF;
    return 2;
}

int INC_0C(CPU* cpu)
{
    // INC C - Increment register C
    cpu->pc++;
    uint8_t original = cpu->c;
    cpu->c++;
    cpu->z_flag = (cpu->c == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((original & 0x0F) == 0x0F);
    return 1;
}

int DEC_0D(CPU* cpu)
{
    // DEC C - Decrement register C
    cpu->pc++;
    uint8_t original = cpu->c;
    cpu->c--;
    cpu->z_flag = (cpu->c == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((original & 0x0F) == 0x00);
    return 1;
}

int LD_0E(CPU* cpu)
{
    // LD C, n - Load immediate value into register C
    cpu->pc++;
    cpu->c = cpu->ram[cpu->pc++];
    return 2;
}

int RRCA_0F(CPU* cpu)
{
    // RRCA - Rotate A right circular
    cpu->pc++;
    uint8_t carry = cpu->a & 0x01;
    cpu->a = (cpu->a >> 1) | (carry << 7);
    cpu->z_flag = false;
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = carry;
    return 1;
}
int STOP_10(CPU* cpu)
{
    //TODO: implement
    cpu->pc++;
    return 99;
    return 1;
}
int LD_11(CPU* cpu)
{
    //load imm 16 bit into DE register
    cpu->pc++;
    cpu->e = cpu->ram[cpu->pc];
    cpu->pc++;
    cpu->d = cpu->ram[cpu->pc];
    cpu->pc++;
    return 3;
}
int LD_12(CPU* cpu)
{
    //store a in address DE
    cpu->pc++;
    uint16_t address = ((uint16_t)cpu->d << 8) | cpu->e;
    cpu->writeMemory(address, cpu->a);
    return 2;
}
int INC_13(CPU* cpu)
{
    //inc r8 de
    cpu->pc++;
    if (cpu->e < 0xFF) cpu->e++;
    else
    {
        cpu->e = 0;
        if (cpu->d < 0xFF)
        {
            cpu->d++;
        }
        else
        {
            cpu->d = 0;
        }
    }
    return 2;
}
int INC_14(CPU* cpu)
{
    // INC d - Increment register d
    cpu->pc++;
    uint8_t original = cpu->d;
    cpu->d++;
    cpu->z_flag = (cpu->d == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((original & 0x0F) == 0x0F);
    return 1;
}
int DEC_15(CPU* cpu)
{
    // DEC B - Decrement register d
    cpu->pc++;
    uint8_t original = cpu->d;
    cpu->d--;
    cpu->z_flag = (cpu->d == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((original & 0x0F) == 0x00);
    return 1;
}
int LD_16(CPU* cpu)
{
    // LD D, n - Load immediate value into register D
    cpu->pc++;
    cpu->d = cpu->ram   [cpu->pc++];
    return 2;
}
int RLA_17(CPU* cpu)
{
    // RLA - Rotate A left through carry
    cpu->pc++;
    uint8_t carry = cpu->c_flag ? 1 : 0;
    uint8_t new_carry = (cpu->a & 0x80) >> 7;
    cpu->a = (cpu->a << 1) | carry;
    cpu->z_flag = false;
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;
    return 1;
}
int JR_18(CPU* cpu)
{
    // Jump to signed 8 imm
    cpu->pc++;
    int8_t offset = static_cast<int8_t>(cpu->ram[cpu->pc++]);
    cpu->pc += offset;
    return 3;
}
int ADD_19(CPU* cpu)
{
    // ADD HL, DE - Add DE to HL
    cpu->pc++;
    uint16_t hl = (cpu->h << 8) | cpu->l;
    uint16_t de = (cpu->d << 8) | cpu->e;
    uint32_t result = hl + de;
    cpu->h = (result >> 8) & 0xFF;
    cpu->l = result & 0xFF;
    cpu->n_flag = false;
    cpu->h_flag = ((hl & 0x0FFF) + (de & 0x0FFF) > 0x0FFF);
    cpu->c_flag = (result > 0xFFFF);
    return 2;
}
int LD_1A(CPU* cpu)
{
    // LD A, (DE) - Load value from memory address DE into A
    cpu->pc++;
    uint16_t address = (cpu->d << 8) | cpu->e;
    //if (address == 0xFF44) ly = 0x90;
    //cpu->a = cpu->ram[address];
    cpu->a = cpu->readMemory(address);
    return 2;
}
int DEC_1B(CPU* cpu)
{
    // DEC DE - Decrement DE register pair
    cpu->pc++;
    uint16_t de = (cpu->d << 8) | cpu->e;
    de--;
    cpu->d = (de >> 8) & 0xFF;
    cpu->e = de & 0xFF;
    return 2;
}
int INC_1C(CPU* cpu)
{
    // INC E - Increment register E
    cpu->pc++;
    uint8_t original = cpu->e;
    cpu->e++;
    cpu->z_flag = (cpu->e == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((original & 0x0F) == 0x0F);
    return 1;
}
int DEC_1D(CPU* cpu)
{
    // DEC E - Decrement register E
    cpu->pc++;
    uint8_t original = cpu->e;
    cpu->e--;
    cpu->z_flag = (cpu->e == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((original & 0x0F) == 0x00);
    return 1;
}
int LD_1E(CPU* cpu)
{
    // LD C, n - Load immediate value into register E
    cpu->pc++;
    cpu->e = cpu->ram   [cpu->pc++];
    return 2;
}
int RRA_1F(CPU* cpu)
{
    // RRA - Rotate A right through carry
    cpu->pc++;
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t carry_out = cpu->a & 0x01;
    cpu->a = (cpu->a >> 1) | (carry_in << 7);
    cpu->z_flag = false;
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = carry_out;
    return 1;
}
int JR_20(CPU* cpu)
{
    //CHECK if need to add a pc++
    // WARN
    // jump conditional
    cpu->pc++;
    if (!cpu->z_flag)
    {
        int8_t offset = static_cast<int8_t>(cpu->ram[cpu->pc++]);
        cpu->pc += offset;
        //printf("0x%04X\n", cpu->pc);
        return 3;
    }
    else
    {
        cpu->pc++;
        return 2;
    }
}
int LD_21(CPU* cpu)
{
    //CHECK
    //load imm 16 bit into HL register
    cpu->pc++;
    cpu->l = cpu->ram[cpu->pc];
    cpu->pc++;
    cpu->h = cpu->ram[cpu->pc];
    cpu->pc++;
    cpu->pc &= 0xFFFF;
    return 3;
}
int LD_22(CPU* cpu)
{
    //store A in address HL then increment HL
    cpu->pc++;
    uint16_t address = ((uint16_t)cpu->h << 8) | cpu->l;
    cpu->writeMemory(address, cpu->a);
    if (cpu->l < 0xFF) cpu->l++;
    else if (cpu->h < 0xFF)
    {
        cpu->h++;
        cpu->l = 0;
    }
    return 2;
}
int INC_23(CPU* cpu)
{
    //inc r8 HL
    cpu->pc++;
    if (cpu->l < 0xFF) cpu->l++;
    else
    {
        cpu->l = 0;
        if (cpu->h < 0xFF)
        {
            cpu->h++;
        }
        else
        {
            cpu->h = 0;
        }
    }
    return 2;
}
int INC_24(CPU* cpu)
{
    // INC h - Increment register H
    cpu->pc++;
    uint8_t original = cpu->h;
    cpu->h++;
    cpu->z_flag = (cpu->h == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((original & 0x0F) == 0x0F);
    return 1;
}
int DEC_25(CPU* cpu)
{
    // DEC h - Decrement register h
    cpu->pc++;
    uint8_t original = cpu->h;
    cpu->h--;
    cpu->z_flag = (cpu->h == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((original & 0x0F) == 0x00);
    return 1;
}
int LD_26(CPU* cpu)
{
    // LD H, n - Load immediate value into register H
    cpu->pc++;
    cpu->h = cpu->ram   [cpu->pc++];
    return 2;
}
int DAA_27(CPU* cpu)
{
    // DAA - Decimal Adjust Accumulator
    uint8_t t = cpu->a;
    uint8_t corr = 0;

    // Apply corrections based on the flags
    if (cpu->h_flag) corr |= 0x06;
    if (cpu->c_flag) corr |= 0x60;

    if (cpu->n_flag)
    {
        t -= corr;
    }
    else
    {
        if ((t & 0x0F) > 0x09) corr |= 0x06;
        if (t > 0x99) corr |= 0x60;
        t += corr;
    }

    // Setting the flags
    cpu->z_flag = ((t & 0xFF) == 0); // Set Zero flag if result is zero
    cpu->c_flag = (corr & 0x60) != 0; // Set Carry flag based on correction
    cpu->h_flag = false; // Reset Half-Carry flag

    t &= 0xFF; // Ensure the result is within 8 bits
    cpu->a = t;
    cpu->pc++;

    return 1;
}
int JR_28(CPU* cpu)
{
    // JR Z, e8 - Jump relative if Zero flag is set

    //WARN remove maybe
    cpu->pc++;
    if (cpu->z_flag)
    {
        int8_t offset = static_cast<int8_t>(cpu->ram[cpu->pc]);
        cpu->pc++;
        cpu->pc += offset;
        return 3;
    }
    else
    {
        cpu->pc++;
        return 2;
    }

    /*cpu->pc++;
    if (cpu->z_flag)
    {
        uint8_t offset = cpu->ram[cpu->pc];
        cpu->pc++;
        cpu->pc += ((offset ^ 0x80) - 0x80);
        return 3;
    }
    else
    {
        cpu->pc++;
        return 2;
    }*/
}
int ADD_29(CPU* cpu)
{
    // ADD HL, HL - Add HL to HL
    cpu->pc++;
    uint16_t hl = (cpu->h << 8) | cpu->l;
    uint32_t result = hl + hl;
    cpu->h = (result >> 8) & 0xFF;
    cpu->l = result & 0xFF;
    cpu->n_flag = false;
    cpu->h_flag = ((hl & 0x0FFF) + (hl & 0x0FFF) > 0x0FFF);
    cpu->c_flag = (result > 0xFFFF);
    return 2;
}
int LD_2A(CPU* cpu)
{
    // LD A, (HL) - Load value from memory address HL into A and increment HL
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;

    cpu->a = cpu->readMemory(address);

    if (cpu->l < 0xFF) cpu->l++;
    else if (cpu->h < 0xFF)
    {
        cpu->h++; 
        cpu->l = 0;
    }
    else
    {
        cpu->h = 0;
        cpu->l = 0;
    }
    return 2;
}
int DEC_2B(CPU* cpu)
{
    // DEC HL - Decrement HL register pair
    cpu->pc++;
    uint16_t hl = (cpu->h << 8) | cpu->l;
    hl--;
    cpu->h = (hl >> 8) & 0xFF;
    cpu->l = hl & 0xFF;
    return 2;
}
int INC_2C(CPU* cpu)
{
    // INC L - Increment register L
    cpu->pc++;
    uint8_t original = cpu->l;
    cpu->l++;
    cpu->z_flag = (cpu->l == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((original & 0x0F) == 0x0F);
    return 1;
}
int DEC_2D(CPU* cpu)
{
    // DEC L - Decrement register L
    cpu->pc++;
    uint8_t original = cpu->l;
    cpu->l--;
    cpu->z_flag = (cpu->l == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((original & 0x0F) == 0x00);
    return 1;
}
int LD_2E(CPU* cpu)
{
    // LD C, n - Load immediate value into register L
    cpu->pc++;
    cpu->l = cpu->ram   [cpu->pc++];
    return 2;
}
int CPL_2F(CPU* cpu)
{
    // CPL - Complement Accumulator
    cpu->pc++;
    cpu->a = ~cpu->a;
    cpu->n_flag = true;   // Set the Negative flag
    cpu->h_flag = true;   // Set the Half-Carry flag
    return 1;
}
//////////////////////
//////////////////////
int JR_30(CPU* cpu)
{
    // JR NC, e8
    cpu->pc++;
    if (!cpu->c_flag)
    {
        int8_t offset = static_cast<int8_t>(cpu->ram[cpu->pc]);
        cpu->pc++;
        cpu->pc += offset;
        return 3;
    }
    else
    {
        cpu->pc++;
        return 2;
    }
}
int LD_31(CPU* cpu)
{
    //load imm 16 bit into SP register
    cpu->pc++;
    uint16_t value = cpu->ram   [cpu->pc++] | (cpu->ram   [cpu->pc++] << 8);
    cpu->sp = value;
    return 3;
}
int LD_32(CPU* cpu)
{
    //CHECK
    //store A in address HL then decrement HL
    cpu->pc++;
    uint16_t address = ((uint16_t)cpu->h << 8) | cpu->l;
    cpu->writeMemory(address, cpu->a);

    if (cpu->l > 0x00) { cpu->l--; }
    else
    {
        cpu->l = 0xFF;
        if (cpu->h > 0x00)
        {
            cpu->h--;
        }
        else
        {
            cpu->h = 0xFF;
        }
    }
    return 2;
}
int INC_33(CPU* cpu)
{
    //inc r8 SP
    cpu->pc++;
    cpu->sp++;
    return 2;
}
int INC_34(CPU* cpu)
{
    // INC [HL] - Increment the byte at address HL
    cpu->pc++;
    uint16_t address = ((uint16_t)cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];
    value++;
    cpu->writeMemory(address, value);
    cpu->z_flag = (value == 0); // Set Zero flag if result is zero
    cpu->n_flag = false;        // Reset Negative flag
    cpu->h_flag = ((value & 0x0F) == 0); // Set Half-Carry flag if there was a carry from bit 3 to bit 4
    return 3;
}
int DEC_35(CPU* cpu)
{
    // DEC [HL]
    cpu->pc++;
    uint16_t address = ((uint16_t)cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];
    value--;
    cpu->writeMemory(address, value);
    cpu->z_flag = (value == 0); // Set Zero flag if result is zero
    cpu->n_flag = true;         // Set Negative flag
    cpu->h_flag = ((value & 0x0F) == 0x0F);
    return 3;
}
int LD_36(CPU* cpu)
{
    // LD [HL]
    cpu->pc++;
    uint8_t value = cpu->ram[cpu->pc++];
    uint16_t address = ((uint16_t)cpu->h << 8) | cpu->l;
    cpu->writeMemory(address, value);
    return 3;
}
int SCF_37(CPU* cpu)
{
    // Set carry flag
    cpu->pc++;
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = true;
    return 1;
}
int JR_38(CPU* cpu)
{
    // JR Z, e8 - Jump relative if Carry flag is set
    cpu->pc++;
    if (cpu->c_flag)
    {
        int8_t offset = static_cast<int8_t>(cpu->ram[cpu->pc++]);
        cpu->pc += offset;
        return 3;
    }
    else
    {
        cpu->pc++;
        return 2;
    }
}
int ADD_39(CPU* cpu)
{
    // ADD HL, SP - Add SP to HL
    cpu->pc++;
    uint16_t hl = (cpu->h << 8) | cpu->l;
    uint32_t result = hl + cpu->sp;
    cpu->h = (result >> 8) & 0xFF;
    cpu->l = result & 0xFF;
    cpu->n_flag = false;
    cpu->h_flag = ((hl & 0x0FFF) + (cpu->sp & 0x0FFF) > 0x0FFF);
    cpu->c_flag = (result > 0xFFFF);
    return 2;
}
int LD_3A(CPU* cpu)
{
    // LD A, (HL) - Load value from memory address HL into A and decrement HL
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    //if (address == 0xFF44) ly = 0x90;
    //cpu->a = cpu->ram[address];
    cpu->a = cpu->readMemory(address);
    if (cpu->l > 0x00) cpu->l--;
    else if (cpu->h > 0x00) cpu->h--;
    return 2;
}
int DEC_3B(CPU* cpu)
{
    // DEC SP - Decrement SP register
    cpu->pc++;
    cpu->sp--;
    return 2;
}
int INC_3C(CPU* cpu)
{
    // INC A - Increment register A
    cpu->pc++;
    uint8_t original = cpu->a;
    cpu->a++;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((original & 0x0F) == 0x0F);
    return 1;
}
int DEC_3D(CPU* cpu)
{
    // DEC A - Decrement register A
    cpu->pc++;
    uint8_t original = cpu->a;
    cpu->a--;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((original & 0x0F) == 0x00);
    return 1;
}
int LD_3E(CPU* cpu)
{
    // LD A, n - Load immediate value into register A
    cpu->pc++;
    cpu->a = cpu->ram   [cpu->pc++];
    return 2;
}
int CCF_3F(CPU* cpu)
{
    // CCF - Complement Carry Flag
    cpu->pc++;
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = !cpu->c_flag;
    return 1;
}
int LD_40(CPU* cpu)
{
    //LD B, B
    //std::cout << "LD B, B called\n";
    cpu->pc++;
    return 1;
}
int LD_41(CPU* cpu)
{
    //LD B, C
    cpu->pc++;
    cpu->b = cpu->c;
    return 1;
}
int LD_42(CPU* cpu)
{
    //LD B, D
    cpu->pc++;
    cpu->b = cpu->d;
    return 1;
}
int LD_43(CPU* cpu)
{
    //LD B, E
    cpu->pc++;
    cpu->b = cpu->e;
    return 1;
}
int LD_44(CPU* cpu)
{
    //LD B, H
    cpu->pc++;
    cpu->b = cpu->h;
    return 1;
}
int LD_45(CPU* cpu)
{
    //LD B, L
    cpu->pc++;
    cpu->b = cpu->l;
    return 1;
}
int LD_46(CPU* cpu)
{
    //LD B, [HL]
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    //if (address == 0xFF44) ly = 0x90;
    //cpu->b = cpu->ram[address];
    cpu->b = cpu->readMemory(address);
    return 2;
}
int LD_47(CPU* cpu)
{
    //LD B, A
    cpu->pc++;
    cpu->b = cpu->a;
    return 1;
}
int LD_48(CPU* cpu)
{
    //LD C, B
    cpu->pc++;
    cpu->c = cpu->b;
    return 1;
}
int LD_49(CPU* cpu)
{
    //LD C, C
    cpu->pc++;
    return 1;
}
int LD_4A(CPU* cpu)
{
    //LD C, D
    cpu->pc++;
    cpu->c = cpu->d;
    return 1;
}
int LD_4B(CPU* cpu)
{
    //LD C, E
    cpu->pc++;
    cpu->c = cpu->e;
    return 1;
}
int LD_4C(CPU* cpu)
{
    //LD C, H
    cpu->pc++;
    cpu->c = cpu->h;
    return 1;
}
int LD_4D(CPU* cpu)
{
    //LD C, L
    cpu->pc++;
    cpu->c = cpu->l;
    return 1;
}
int LD_4E(CPU* cpu)
{
    //LD C, [HL]
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    //if (address == 0xFF44) ly = 0x90;
    //cpu->c = cpu->ram[address];
    cpu->c = cpu->readMemory(address);
    return 2;
}
int LD_4F(CPU* cpu)
{
    //LD C, A
    cpu->pc++;
    cpu->c = cpu->a;
    return 1;
}
int LD_50(CPU* cpu)
{
    //LD D, B
    cpu->pc++;
    cpu->d = cpu->b;
    return 1;
}
int LD_51(CPU* cpu)
{
    //LD D, C
    cpu->pc++;
    cpu->d = cpu->c;
    return 1;
}
int LD_52(CPU* cpu)
{
    //LD D, D
    cpu->pc++;
    return 1;
}
int LD_53(CPU* cpu)
{
    //LD D, E
    cpu->pc++;
    cpu->d = cpu->e;
    return 1;
}
int LD_54(CPU* cpu)
{
    //LD D, H
    cpu->pc++;
    cpu->d = cpu->h;
    return 1;
}
int LD_55(CPU* cpu)
{
    //LD D, L
    cpu->pc++;
    cpu->d = cpu->l;
    return 1;
}
int LD_56(CPU* cpu)
{
    //LD D, [HL]
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    //if (address == 0xFF44) ly = 0x90;
    //cpu->d = cpu->ram[address];
    cpu->d = cpu->readMemory(address);
    return 2;
}
int LD_57(CPU* cpu)
{
    //LD D, A
    cpu->pc++;
    cpu->d = cpu->a;
    return 1;
}
int LD_58(CPU* cpu)
{
    //LD E, B
    cpu->pc++;
    cpu->e = cpu->b;
    return 1;
}
int LD_59(CPU* cpu)
{
    //LD E, C
    cpu->pc++;
    cpu->e = cpu->c;
    return 1;
}
int LD_5A(CPU* cpu)
{
    //LD E, D
    cpu->pc++;
    cpu->e = cpu->d;
    return 1;
}
int LD_5B(CPU* cpu)
{
    //LD E, E
    cpu->pc++;
    return 1;
}
int LD_5C(CPU* cpu)
{
    //LD E, H
    cpu->pc++;
    cpu->e = cpu->h;
    return 1;
}
int LD_5D(CPU* cpu)
{
    //LD E, L
    cpu->pc++;
    cpu->e = cpu->l;
    return 1;
}
int LD_5E(CPU* cpu)
{
    //LD E, [HL]
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    //if (address == 0xFF44) ly = 0x90;
    //cpu->e = cpu->ram[address];
    cpu->e = cpu->readMemory(address);
    return 2;
}
int LD_5F(CPU* cpu)
{
    //LD E, A
    cpu->pc++;
    cpu->e = cpu->a;
    return 1;
}
int LD_60(CPU* cpu)
{
    //LD H, B
    cpu->pc++;
    cpu->h = cpu->b;
    return 1;
}
int LD_61(CPU* cpu)
{
    //LD H, C
    cpu->pc++;
    cpu->h = cpu->c;
    return 1;
}
int LD_62(CPU* cpu)
{
    //LD H, D
    cpu->pc++;
    cpu->h = cpu->d;
    return 1;
}
int LD_63(CPU* cpu)
{
    //LD H, E
    cpu->pc++;
    cpu->h = cpu->e;
    return 1;
}
int LD_64(CPU* cpu)
{
    //LD H, H
    cpu->pc++;
    return 1;
}
int LD_65(CPU* cpu)
{
    //LD H, L
    cpu->pc++;
    cpu->h = cpu->l;
    return 1;
}
int LD_66(CPU* cpu)
{
    //LD H, [HL]
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    //if (address == 0xFF44) ly = 0x90;
    //cpu->h = cpu->ram[address];
    cpu->h = cpu->readMemory(address);
    return 2;
}
int LD_67(CPU* cpu)
{
    //LD H, A
    cpu->pc++;
    cpu->h = cpu->a;
    return 1;
}
int LD_68(CPU* cpu)
{
    //LD L, B
    cpu->pc++;
    cpu->l = cpu->b;
    return 1;
}
int LD_69(CPU* cpu)
{
    //LD L, C
    cpu->pc++;
    cpu->l = cpu->c;
    return 1;
}
int LD_6A(CPU* cpu)
{
    //LD L, D
    cpu->pc++;
    cpu->l = cpu->d;
    return 1;
}
int LD_6B(CPU* cpu)
{
    //LD L, E
    cpu->pc++;
    cpu->l = cpu->e;
    return 1;
}
int LD_6C(CPU* cpu)
{
    //LD L, H
    cpu->pc++;
    cpu->l = cpu->h;
    return 1;
}
int LD_6D(CPU* cpu)
{
    //LD L, L
    cpu->pc++;
    return 1;
}
int LD_6E(CPU* cpu)
{
    //LD L, [HL]
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    //if (address == 0xFF44) ly = 0x90;
    //cpu->l = cpu->ram[address];
    cpu->l = cpu->readMemory(address);
    return 2;
}
int LD_6F(CPU* cpu)
{
    //LD L, A
    cpu->pc++;
    cpu->l = cpu->a;
    return 1;
}
//////////////////////
int LD_70(CPU* cpu)
{
    //LD [HL], B
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    cpu->writeMemory(address, cpu->b);
    return 2;
}
int LD_71(CPU* cpu)
{
    //LD [HL], C
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    cpu->writeMemory(address, cpu->c);
    return 2;
}
int LD_72(CPU* cpu)
{
    //LD [HL], D
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    cpu->writeMemory(address, cpu->d);
    return 2;
}
int LD_73(CPU* cpu)
{
    //LD [HL], E
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    cpu->writeMemory(address, cpu->e);
    return 2;
}
int LD_74(CPU* cpu)
{
    //LD [HL], H
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    cpu->writeMemory(address, cpu->h);
    return 2;
}
int LD_75(CPU* cpu)
{
    //LD [HL], L
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    cpu->writeMemory(address, cpu->l);
    return 2;
}
int LD_76(CPU* cpu)
{
    //Halt
    cpu->pc++;
    //return 1;
    //FIX BAD ERROR WARN HACK
    if (cpu->readMemory(0xFFFF) != 0x0 && cpu->readMemory(0xFF0F) != 0x00)
    {
        cpu->halt = false;
    }
    else
    {
        cpu->halt = true;
    }
    return 1;
}
int LD_77(CPU* cpu)
{
    //LD [HL], A
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    cpu->writeMemory(address, cpu->a);
    return 2;
}
int LD_78(CPU* cpu)
{
    //LD A, B
    cpu->pc++;
    cpu->a = cpu->b;
    return 1;
}
int LD_79(CPU* cpu)
{
    //LD A, C
    cpu->pc++;
    cpu->a = cpu->c;
    return 1;
}
int LD_7A(CPU* cpu)
{
    //LD A, D
    cpu->pc++;
    cpu->a = cpu->d;
    return 1;
}
int LD_7B(CPU* cpu)
{
    //LD A, E
    cpu->pc++;
    cpu->a = cpu->e;
    return 1;
}
int LD_7C(CPU* cpu)
{
    //LD A, H
    cpu->pc++;
    cpu->a = cpu->h;
    return 1;
}
int LD_7D(CPU* cpu)
{
    //LD A, L
    cpu->pc++;
    cpu->a = cpu->l;
    return 1;
}
int LD_7E(CPU* cpu)
{
    //LD A, [HL]
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    //if (address == 0xFF44) ly = 0x90;
    cpu->a = cpu->ram[address];
    return 2;
}
int LD_7F(CPU* cpu)
{
    //LD A, A
    cpu->pc++;
    return 1;
}
//////////////////////
int ADD_80(CPU* cpu)
{
    //ADD A, B
    cpu->pc++;
    uint8_t lower_nibble_a = cpu->a & 0x0F;
    uint8_t lower_nibble_b = cpu->b & 0x0F;
    uint16_t result = cpu->a + cpu->b;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((lower_nibble_a + lower_nibble_b) & 0x10) != 0;
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int ADD_81(CPU* cpu)
{
    //ADD A, C
    cpu->pc++;
    uint8_t lower_nibble_a = cpu->a & 0x0F;
    uint8_t lower_nibble_c = cpu->c & 0x0F;
    uint16_t result = cpu->a + cpu->c;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((lower_nibble_a + lower_nibble_c) & 0x10) != 0;
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int ADD_82(CPU* cpu)
{
    //ADD A, D
    cpu->pc++;
    uint8_t lower_nibble_a = cpu->a & 0x0F;
    uint8_t lower_nibble_d = cpu->d & 0x0F;
    uint16_t result = cpu->a + cpu->d;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((lower_nibble_a + lower_nibble_d) & 0x10) != 0;
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int ADD_83(CPU* cpu)
{
    //ADD A, E
    cpu->pc++;
    uint8_t lower_nibble_a = cpu->a & 0x0F;
    uint8_t lower_nibble_e = cpu->e & 0x0F;
    uint16_t result = cpu->a + cpu->e;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((lower_nibble_a + lower_nibble_e) & 0x10) != 0;
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int ADD_84(CPU* cpu)
{
    //ADD A, H
    cpu->pc++;
    uint8_t lower_nibble_a = cpu->a & 0x0F;
    uint8_t lower_nibble_h = cpu->h & 0x0F;
    uint16_t result = cpu->a + cpu->h;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((lower_nibble_a + lower_nibble_h) & 0x10) != 0;
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int ADD_85(CPU* cpu)
{
    //ADD A, L
    cpu->pc++;
    uint8_t lower_nibble_a = cpu->a & 0x0F;
    uint8_t lower_nibble_l = cpu->l & 0x0F;
    uint16_t result = cpu->a + cpu->l;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((lower_nibble_a + lower_nibble_l) & 0x10) != 0;
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int ADD_86(CPU* cpu)
{
    //ADD A, [HL]
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    //uint8_t value = cpu->ram[cpu->h << 8 | cpu->l];
    uint8_t value = cpu->readMemory(address);
    uint8_t lower_nibble_a = cpu->a & 0x0F;
    uint8_t lower_nibble_l = value & 0x0F;
    uint16_t result = cpu->a + value;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((lower_nibble_a + lower_nibble_l) & 0x10) != 0;
    cpu->c_flag = (result > 0xFF);
    return 2;
}
int ADD_87(CPU* cpu)
{
    //ADD A, A
    cpu->pc++;
    uint16_t result = cpu->a + cpu->a;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = (((cpu->a & 0x0F) + (cpu->a & 0x0F)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int ADC_88(CPU* cpu)
{
    // ADC A, B
    cpu->pc++;
    uint8_t value = cpu->b;
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t lower_nibble_a = cpu->a & 0x0F;
    uint8_t lower_nibble_value = value & 0x0F;
    uint16_t result = cpu->a + value + carry_in;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((lower_nibble_a + lower_nibble_value + carry_in) & 0x10) != 0;
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int ADC_89(CPU* cpu)
{
    //ADC A, C
    cpu->pc++;
    uint8_t value = cpu->c;
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t lower_nibble_a = cpu->a & 0x0F;
    uint8_t lower_nibble_value = value & 0x0F;
    uint16_t result = cpu->a + value + carry_in;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((lower_nibble_a + lower_nibble_value + carry_in) & 0x10) != 0;
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int ADC_8A(CPU* cpu)
{
    //ADC A, D
    cpu->pc++;
    uint8_t value = cpu->d;
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t lower_nibble_a = cpu->a & 0x0F;
    uint8_t lower_nibble_value = value & 0x0F;
    uint16_t result = cpu->a + value + carry_in;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((lower_nibble_a + lower_nibble_value + carry_in) & 0x10) != 0;
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int ADC_8B(CPU* cpu)
{
    //ADC A, E
    cpu->pc++;
    uint8_t value = cpu->e;
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t lower_nibble_a = cpu->a & 0x0F;
    uint8_t lower_nibble_value = value & 0x0F;
    uint16_t result = cpu->a + value + carry_in;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((lower_nibble_a + lower_nibble_value + carry_in) & 0x10) != 0;
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int ADC_8C(CPU* cpu)
{
    //ADC A, H
    cpu->pc++;
    uint8_t value = cpu->h;
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t lower_nibble_a = cpu->a & 0x0F;
    uint8_t lower_nibble_value = value & 0x0F;
    uint16_t result = cpu->a + value + carry_in;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((lower_nibble_a + lower_nibble_value + carry_in) & 0x10) != 0;
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int ADC_8D(CPU* cpu)
{
    //ADC A, L
    cpu->pc++;
    uint8_t value = cpu->l;
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t lower_nibble_a = cpu->a & 0x0F;
    uint8_t lower_nibble_value = value & 0x0F;
    uint16_t result = cpu->a + value + carry_in;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((lower_nibble_a + lower_nibble_value + carry_in) & 0x10) != 0;
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int ADC_8E(CPU* cpu)
{
    //ADC A, [HL]
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    //uint8_t value = cpu->ram[address];
    uint8_t value = cpu->readMemory(address);
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t lower_nibble_a = cpu->a & 0x0F;
    uint8_t lower_nibble_value = value & 0x0F;
    uint16_t result = cpu->a + value + (cpu->c_flag ? 1 : 0);
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = ((lower_nibble_a + lower_nibble_value + carry_in) & 0x10) != 0;
    cpu->c_flag = (result > 0xFF);
    return 2;
}
int ADC_8F(CPU* cpu)
{
    //ADC A, A
    cpu->pc++;
    uint8_t value = cpu->a;
    uint16_t result = cpu->a + value + (cpu->c_flag ? 1 : 0);
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = (((cpu->a & 0x0F) + (value & 0x0F) + (cpu->c_flag ? 1 : 0)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 1;
}
////////////////////
////////////////////
int SUB_90(CPU* cpu)
{
    //SUB A, B
    cpu->pc++;
    uint16_t result = cpu->a - cpu->b;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (cpu->b & 0x0F)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int SUB_91(CPU* cpu)
{
    //SUB A, C
    cpu->pc++;
    uint16_t result = cpu->a - cpu->c;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (cpu->c & 0x0F)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int SUB_92(CPU* cpu)
{
    //sub A, D
    cpu->pc++;
    uint16_t result = cpu->a - cpu->d;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (cpu->d & 0x0F)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int SUB_93(CPU* cpu)
{
    //SUB A, E
    cpu->pc++;
    uint16_t result = cpu->a - cpu->e;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (cpu->e & 0x0F)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int SUB_94(CPU* cpu)
{
    //SUB A, H
    cpu->pc++;
    uint16_t result = cpu->a - cpu->h;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (cpu->h & 0x0F)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int SUB_95(CPU* cpu)
{
    //SUB A, L
    cpu->pc++;
    uint16_t result = cpu->a - cpu->l;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (cpu->l & 0x0F)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int SUB_96(CPU* cpu)
{
    //SUB A, [HL]
    cpu->pc++;
    //uint8_t value = cpu->ram[cpu->h << 8 | cpu->l];
    uint8_t value = cpu->readMemory(cpu->h << 8 | cpu->l);
    uint16_t result = cpu->a - value;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (value & 0x0F)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 2;
}
int SUB_97(CPU* cpu)
{
    //SUB A, A
    cpu->pc++;
    uint16_t result = cpu->a - cpu->a;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (cpu->a & 0x0F)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int SBC_98(CPU* cpu)
{
    //SBC A, B
    cpu->pc++;
    uint8_t value = cpu->b;
    uint16_t result = cpu->a - value - (cpu->c_flag ? 1 : 0);
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (value & 0x0F) + (cpu->c_flag ? 1 : 0)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int SBC_99(CPU* cpu)
{
    //SBC A, C
    cpu->pc++;
    uint8_t value = cpu->c;
    uint16_t result = cpu->a - value - (cpu->c_flag ? 1 : 0);
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (value & 0x0F) + (cpu->c_flag ? 1 : 0)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int SBC_9A(CPU* cpu)
{
    //SBC A, D
    cpu->pc++;
    uint8_t value = cpu->d;
    uint16_t result = cpu->a - value - (cpu->c_flag ? 1 : 0);
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (value & 0x0F) + (cpu->c_flag ? 1 : 0)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int SBC_9B(CPU* cpu)
{
    //SBC A, E
    cpu->pc++;
    uint8_t value = cpu->e;
    uint16_t result = cpu->a - value - (cpu->c_flag ? 1 : 0);
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (value & 0x0F) + (cpu->c_flag ? 1 : 0)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int SBC_9C(CPU* cpu)
{
    //SBC A, H
    cpu->pc++;
    uint8_t value = cpu->h;
    uint16_t result = cpu->a - value - (cpu->c_flag ? 1 : 0);
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (value & 0x0F) + (cpu->c_flag ? 1 : 0)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int SBC_9D(CPU* cpu)
{
    //SBC A, L
    cpu->pc++;
    uint8_t value = cpu->l;
    uint16_t result = cpu->a - value - (cpu->c_flag ? 1 : 0);
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (value & 0x0F) + (cpu->c_flag ? 1 : 0)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 1;
}
int SBC_9E(CPU* cpu)
{
    //SBC A, [HL]
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    //uint8_t value = cpu->ram[address];
    uint8_t value = cpu->readMemory(address);
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t lower_nibble_a = cpu->a & 0x0F;
    uint8_t lower_nibble_value = value & 0x0F;
    uint16_t result = cpu->a - value - (cpu->c_flag ? 1 : 0);
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((lower_nibble_a - lower_nibble_value - carry_in) & 0x10) != 0;
    cpu->c_flag = (result > 0xFF);
    return 2;
}
int SBC_9F(CPU* cpu)
{
    //SBC A, A
    cpu->pc++;
    uint8_t value = cpu->a;
    uint16_t result = cpu->a - value - (cpu->c_flag ? 1 : 0);
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (value & 0x0F) + (cpu->c_flag ? 1 : 0)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 1;
}
////////////////////
////////////////////
int AND_A0(CPU* cpu)
{
    //AND A, B
    cpu->pc++;
    cpu->a &= cpu->b;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = true;
    cpu->c_flag = false;
    return 1;
}
int AND_A1(CPU* cpu)
{
    //AND A, C
    cpu->pc++;
    cpu->a &= cpu->c;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = true;
    cpu->c_flag = false;
    return 1;
}
int AND_A2(CPU* cpu)
{
    //AND A, D
    cpu->pc++;
    cpu->a &= cpu->d;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = true;
    cpu->c_flag = false;
    return 1;
}
int AND_A3(CPU* cpu)
{
    //AND A, E
    cpu->pc++;
    cpu->a &= cpu->e;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = true;
    cpu->c_flag = false;
    return 1;
}
int AND_A4(CPU* cpu)
{
    //AND A, H
    cpu->pc++;
    cpu->a &= cpu->h;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = true;
    cpu->c_flag = false;
    return 1;
}
int AND_A5(CPU* cpu)
{
    //AND A, L
    cpu->pc++;
    cpu->a &= cpu->l;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = true;
    cpu->c_flag = false;
    return 1;
}
int AND_A6(CPU* cpu)
{
    //AND A, [HL]
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    //uint8_t value = cpu->ram[address];
    uint8_t value = cpu->readMemory(address);
    cpu->a &= value;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = true;
    cpu->c_flag = false;
    return 2;
}
int AND_A7(CPU* cpu)
{
    //AND A, A
    cpu->pc++;
    cpu->a &= cpu->a;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = true;
    cpu->c_flag = false;
    return 1;
}
int XOR_A8(CPU* cpu)
{
    //XOR A, B
    cpu->pc++;
    cpu->a ^= cpu->b;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 1;
}
int XOR_A9(CPU* cpu)
{
    //XOR A, C
    cpu->pc++;
    cpu->a ^= cpu->c;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 1;
}
int XOR_AA(CPU* cpu)
{
    //XOR A, D
    cpu->pc++;
    cpu->a ^= cpu->d;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 1;
}
int XOR_AB(CPU* cpu)
{
    //XOR A, E
    cpu->pc++;
    cpu->a ^= cpu->e;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 1;
}
int XOR_AC(CPU* cpu)
{
    //XOR A, H
    cpu->pc++;
    cpu->a ^= cpu->h;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 1;
}
int XOR_AD(CPU* cpu)
{
    //XOR A, L
    cpu->pc++;
    cpu->a ^= cpu->l;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 1;
}
int XOR_AE(CPU* cpu)
{
    //XOR A, [HL]
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    //uint8_t value = cpu->ram[address];
    uint8_t value = cpu->readMemory(address);
    cpu->a ^= value;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 2;
}
int XOR_AF(CPU* cpu)
{
    //XOR A, A
    cpu->pc++;
    cpu->a ^= cpu->a;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 1;
}
////////////////////////////////
/////////////////////////////////
int OR_B0(CPU* cpu)
{
    //OR A, B
    cpu->pc++;
    cpu->a |= cpu->b;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 1;
}
int OR_B1(CPU* cpu)
{
    //OR A, C
    cpu->pc++;
    cpu->a |= cpu->c;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 1;
}
int OR_B2(CPU* cpu)
{
    //OR A, D
    cpu->pc++;
    cpu->a |= cpu->d;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 1;
}
int OR_B3(CPU* cpu)
{
    //OR A, E
    cpu->pc++;
    cpu->a |= cpu->e;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 1;
}
int OR_B4(CPU* cpu)
{
    //OR A, H
    cpu->pc++;
    cpu->a |= cpu->h;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 1;
}
int OR_B5(CPU* cpu)
{
    //OR A, L
    cpu->pc++;
    cpu->a |= cpu->l;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 1;
}
int OR_B6(CPU* cpu)
{
    //OR A, [HL]
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    //uint8_t value = cpu->ram[address];
    uint8_t value = cpu->readMemory(address);
    cpu->a |= value;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 2;
}
int OR_B7(CPU* cpu)
{
    //OR A, A
    cpu->pc++;
    cpu->a |= cpu->a;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 1;
}
int CP_B8(CPU* cpu)
{
    //CP A, B
    cpu->pc++;
    uint16_t result = cpu->a - cpu->b;
    cpu->z_flag = (result == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((cpu->a & 0x0F) < (cpu->b & 0x0F));
    cpu->c_flag = (cpu->a < cpu->b);
    return 1;
}
int CP_B9(CPU* cpu)
{
    //CP A, C
    cpu->pc++;
    uint16_t result = cpu->a - cpu->c;
    cpu->z_flag = (result == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((cpu->a & 0x0F) < (cpu->c & 0x0F));
    cpu->c_flag = (cpu->a < cpu->c);
    return 1;
}
int CP_BA(CPU* cpu)
{
    //CP A, D
    cpu->pc++;
    uint16_t result = cpu->a - cpu->d;
    cpu->z_flag = (result == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((cpu->a & 0x0F) < (cpu->d & 0x0F));
    cpu->c_flag = (cpu->a < cpu->d);
    return 1;
}
int CP_BB(CPU* cpu)
{
    //CP A, E
    cpu->pc++;
    uint16_t result = cpu->a - cpu->e;
    cpu->z_flag = (result == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((cpu->a & 0x0F) < (cpu->e & 0x0F));
    cpu->c_flag = (cpu->a < cpu->e);
    return 1;
}
int CP_BC(CPU* cpu)
{
    //CP A, H
    cpu->pc++;
    uint16_t result = cpu->a - cpu->h;
    cpu->z_flag = (result == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((cpu->a & 0x0F) < (cpu->h & 0x0F));
    cpu->c_flag = (cpu->a < cpu->h);
    return 1;
}
int CP_BD(CPU* cpu)
{
    //CP A, L
    cpu->pc++;
    uint16_t result = cpu->a - cpu->l;
    cpu->z_flag = (result == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((cpu->a & 0x0F) < (cpu->l & 0x0F));
    cpu->c_flag = (cpu->a < cpu->l);
    return 1;
}
int CP_BE(CPU* cpu)
{
    //CP A, [HL]
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    //uint8_t value = cpu->ram[address];
    uint8_t value = cpu->readMemory(address);
    uint16_t result = cpu->a - value;
    cpu->z_flag = (result == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((cpu->a & 0x0F) < (value & 0x0F));
    cpu->c_flag = (cpu->a < value);
    return 1;
}
int CP_BF(CPU* cpu)
{
    //CP A, A
    cpu->pc++;
    uint16_t result = cpu->a - cpu->a;
    cpu->z_flag = (result == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((cpu->a & 0x0F) < (cpu->a & 0x0F));
    cpu->c_flag = (cpu->a < cpu->a);
    return 1;
}
//////////////////////////////
/////////////////////////////
int RET_C0(CPU* cpu)
{
    //RET NZ
    cpu->pc++;
    if (!cpu->z_flag)
    {
        uint16_t return_address = (cpu->ram[cpu->sp + 1] << 8) | cpu->ram[cpu->sp];
        cpu->sp += 2;
        cpu->pc = return_address;
        return 5;
    }
    else
    {
        return 2;
    }
}
int POP_C1(CPU* cpu)
{
    //POP BC
    cpu->pc++;
    uint16_t value = (cpu->ram[cpu->sp + 1] << 8) | cpu->ram[cpu->sp];
    cpu->b = (value >> 8) & 0xFF;
    cpu->c = value & 0xFF;
    cpu->sp += 2;
    return 3;
}
int JP_C2(CPU* cpu)
{
    //JP NZ a16
    uint16_t address = (cpu->ram[cpu->pc + 1]) | cpu->ram[cpu->pc + 2] << 8;
    if (!cpu->z_flag)
    {
        cpu->pc = address;
        return 4;
    }
    else
    {
        cpu->pc += 3;
        return 3;
    }
}
int JP_C3(CPU* cpu)
{
    //JP a16
    uint16_t address = (cpu->ram[cpu->pc + 1]) | (cpu->ram[cpu->pc + 2] << 8);
    //uint16_t address = (cpu->ram[cpu->pc + 1] << 8) | (cpu->ram[cpu->pc + 2]);
    //uint16_t address = (cpu->ram[cpu->pc + 2]) | (cpu->ram[cpu->pc + 1] << 8);
    //uint16_t address = (cpu->ram[cpu->pc + 1]) | (cpu->ram[cpu->pc + 2] << 8);
    cpu->pc = address;
    return 4;
}
int CALL_C4(CPU* cpu)
{
    // CALL NZ a16
    uint16_t address = (cpu->ram[cpu->pc + 2] << 8) | cpu->ram[cpu->pc + 1];
    cpu->pc += 3;
    cpu->pc &= 0xFFFF;
    if (!cpu->z_flag) {
        cpu->sp -= 2;
        cpu->writeMemory(cpu->sp, (cpu->pc & 0xFF));
        cpu->writeMemory(cpu->sp + 1, ((cpu->pc >> 8) & 0xFF));
        cpu->pc = address;
        return 6;
    }
    else
    {
        return 3;
    }
}
int PUSH_C5(CPU* cpu)
{
    //PUSH BC
    cpu->pc++;
    cpu->writeMemory(cpu->sp - 1, cpu->b);
    cpu->writeMemory(cpu->sp - 2, cpu->c);
    cpu->sp -= 2;
    return 4;
}
int ADD_C6(CPU* cpu)
{
    cpu->pc++;
    uint8_t value = cpu->ram[cpu->pc++];
    uint16_t result = cpu->a + value;
    cpu->h_flag = (((cpu->a & 0xF) + (value & 0xF)) > 0xF);
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->c_flag = (result > 0xFF);
    return 2;
}
int RST_C7(CPU* cpu)
{
    //RST $00
    cpu->pc++;
    uint16_t return_address = cpu->pc;
    cpu->pc++;
    cpu->writeMemory(cpu->sp - 1, static_cast<uint8_t>(return_address >> 8));
    cpu->writeMemory(cpu->sp - 2, static_cast<uint8_t>(return_address & 0xFF));
    cpu->sp -= 2;
    cpu->pc = 0x0000;
    return 4;
}
int RET_C8(CPU* cpu)
{
    //RET Z
    cpu->pc++;
    if (cpu->z_flag)
    {
        uint16_t return_address = (cpu->ram[cpu->sp + 1] << 8) | cpu->ram[cpu->sp];
        cpu->sp += 2;
        cpu->pc = return_address;
        return 5;
    }
    else
    {
        return 2;
    }
}
int RET_C9(CPU* cpu)
{
    //RET
    uint16_t low_byte = cpu->ram[cpu->sp];
    uint16_t high_byte = cpu->ram[cpu->sp + 1] << 8;
    cpu->pc = low_byte | high_byte;
    cpu->sp += 2;
    cpu->sp &= 0xFFFF;
    return 4;
}
int JP_CA(CPU* cpu)
{
    //JP Z a16
    uint16_t address = (cpu->ram[cpu->pc + 2] << 8) | cpu->ram[cpu->pc + 1];
    if (cpu->z_flag)
    {
        cpu->pc = address;
        return 4;
    }
    else
    {
        cpu->pc += 3;
        return 3;
    }
}
int PREFIX_CB(CPU* cpu)
{
    cpu->pc++;
    cpu->pc &= 0xFFFF;
    return 1 + decode_CB_instruction(cpu->ram[cpu->pc], cpu);
}
int CALL_CC(CPU* cpu)
{
    // CALL Z a16
    uint16_t address = (cpu->ram[cpu->pc + 2] << 8) | cpu->ram[cpu->pc + 1];
    cpu->pc += 3;

    if (cpu->z_flag) {
        cpu->sp -= 2;
        cpu->writeMemory(cpu->sp, (cpu->pc & 0xFF));
        cpu->writeMemory(cpu->sp + 1, ((cpu->pc >> 8) & 0xFF));
        cpu->pc = address;
        return 6;
    }
    else
    {
        return 3;
    }
}
int CALL_CD(CPU* cpu)
{
    uint16_t address = (cpu->ram[cpu->pc + 2] << 8) | cpu->ram[cpu->pc + 1];
    cpu->pc += 3;
    cpu->sp -= 2;
    cpu->writeMemory(cpu->sp, (cpu->pc & 0xFF));;
    cpu->writeMemory(cpu->sp + 1, ((cpu->pc >> 8) & 0xFF));
    cpu->pc = address;
    return 6;
}
int ADC_CE(CPU* cpu)
{
    uint8_t immediate_value = cpu->ram[cpu->pc + 1];
    cpu->pc += 2;

    // Compute the half-carry before updating cpu->a
    bool half_carry = (((cpu->a & 0x0F) + (immediate_value & 0x0F) + (cpu->c_flag ? 1 : 0)) > 0x0F);

    // Perform the full addition
    uint16_t result = cpu->a + immediate_value + (cpu->c_flag ? 1 : 0);
    cpu->a = static_cast<uint8_t>(result);

    // Set the flags
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = half_carry;
    cpu->c_flag = (result > 0xFF);

    return 2;
}
int RST_CF(CPU* cpu)
{
    //RST $08
    cpu->pc++;
    uint16_t return_address = cpu->pc;
    cpu->pc++;
    cpu->writeMemory(cpu->sp - 1, static_cast<uint8_t>(return_address >> 8));
    cpu->writeMemory(cpu->sp - 2, static_cast<uint8_t>(return_address & 0xFF));
    cpu->sp -= 2;
    cpu->pc = 0x0008;
    return 4;
}
/////////////////////
////////////////////
int RET_D0(CPU* cpu)
{
    // RET NC
    cpu->pc++;
    if (!cpu->c_flag)
    {
        uint16_t return_address = (cpu->ram[cpu->sp + 1] << 8) | cpu->ram[cpu->sp];
        cpu->sp += 2;
        cpu->pc = return_address;
        return 5;
    }
    else
    {
        return 2;
    }
}
int POP_D1(CPU* cpu)
{
    //POP DE
    cpu->pc++;
    uint16_t value = (cpu->ram[cpu->sp + 1] << 8) | cpu->ram[cpu->sp];
    cpu->d = (value >> 8) & 0xFF;
    cpu->e = value & 0xFF;
    cpu->sp += 2;
    return 3;
}
int JP_D2(CPU* cpu)
{
    //JP NC a16
    uint16_t address = (cpu->ram[cpu->pc + 2] << 8) | cpu->ram[cpu->pc + 1];
    if (!cpu->c_flag)
    {
        cpu->pc = address;
        return 4;
    }
    else
    {
        cpu->pc += 3;
        return 3;
    }
}
int ILLEGAL_D3(CPU* cpu)
{
    cpu->pc++;
    return 0;
}
int CALL_D4(CPU* cpu)
{
    // CALL NC a16
    uint16_t address = (cpu->ram[cpu->pc + 2] << 8) | cpu->ram[cpu->pc + 1];
    cpu->pc += 3;

    if (!cpu->c_flag) {
        cpu->sp -= 2;
        cpu->writeMemory(cpu->sp, (cpu->pc & 0xFF));;
        cpu->writeMemory(cpu->sp + 1, ((cpu->pc >> 8) & 0xFF));
        cpu->pc = address;
        return 6;
    }
    else
    {
        return 3;
    }
}
int PUSH_D5(CPU* cpu)
{
    //PUSH DE
    cpu->pc++;
    cpu->writeMemory(cpu->sp - 1, cpu->d);
    cpu->writeMemory(cpu->sp - 2, cpu->e);
    cpu->sp -= 2;
    return 4;
}
int SUB_D6(CPU* cpu)
{
    //SUB A n8
    cpu->pc++;
    uint8_t value = cpu->ram[cpu->pc++];
    uint16_t result = cpu->a - value;
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (value & 0x0F)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 2;
}
int RST_D7(CPU* cpu)
{
    //RST $10
    cpu->pc++;
    uint16_t return_address = cpu->pc;
    cpu->pc++;
    cpu->writeMemory(cpu->sp - 1, static_cast<uint8_t>(return_address >> 8));
    cpu->writeMemory(cpu->sp - 2, static_cast<uint8_t>(return_address & 0xFF));
    cpu->sp -= 2;
    cpu->pc = 0x0010;
    return 4;
}
int RET_D8(CPU* cpu)
{
    // RET C
    cpu->pc++;
    if (cpu->c_flag)
    {
        uint16_t return_address = (cpu->ram[cpu->sp + 1] << 8) | cpu->ram[cpu->sp];
        cpu->sp += 2;
        cpu->pc = return_address;
        return 5;
    }
    else
    {
        return 2;
    }
}
int RETI_D9(CPU* cpu)
{
    // RETI
    cpu->pc++;
    uint8_t low = cpu->ram[cpu->sp++];
    uint8_t high = cpu->ram[cpu->sp++];
    //printf("RETI: 0x%04X\n", (high << 8) | low);
    cpu->pc = (high << 8) | low;
    //cpu->pc++;
    //ENABLE FIX
    cpu->IME_flag = true;
    return 4;
}
int JP_DA(CPU* cpu)
{
    //JP C a16
    uint16_t address = (cpu->ram[cpu->pc + 2] << 8) | cpu->ram[cpu->pc + 1];
    if (cpu->c_flag)
    {
        cpu->pc = address;
        return 4;
    }
    else
    {
        cpu->pc += 3;
        return 3;
    }
}
int ILLEGAL_DB(CPU* cpu)
{
    cpu->pc++;
    return 0;
}
int CALL_DC(CPU* cpu)
{
    // CALL Z a16
    uint16_t address = (cpu->ram[cpu->pc + 2] << 8) | cpu->ram[cpu->pc + 1];
    cpu->pc += 3;

    if (cpu->c_flag) {
        cpu->sp -= 2;
        cpu->writeMemory(cpu->sp, (cpu->pc & 0xFF));;
        cpu->writeMemory(cpu->sp + 1, ((cpu->pc >> 8) & 0xFF));
        cpu->pc = address;
        return 6;
    }
    else
    {
        return 3;
    }
}
int ILLEGAL_DD(CPU* cpu)
{
    cpu->pc++;
    return 0;
}
int SBC_DE(CPU* cpu)
{
    //SBC A n8
    uint8_t immediate_value = cpu->ram[cpu->pc + 1];
    cpu->pc += 2;
    uint16_t result = cpu->a - immediate_value - (cpu->c_flag ? 1 : 0);
    cpu->a = static_cast<uint8_t>(result);
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = true;
    cpu->h_flag = (((cpu->a & 0x0F) + (immediate_value & 0x0F) + (cpu->c_flag ? 1 : 0)) > 0x0F);
    cpu->c_flag = (result > 0xFF);
    return 2;
}
int RST_DF(CPU* cpu)
{
    //RST $18
    cpu->pc++;
    uint16_t return_address = cpu->pc;
    cpu->pc++;
    cpu->writeMemory(cpu->sp - 1, static_cast<uint8_t>(return_address >> 8));
    cpu->writeMemory(cpu->sp - 2, static_cast<uint8_t>(return_address & 0xFF));
    cpu->sp -= 2;
    cpu->pc = 0x0018;
    return 4;
}
/////////////////////
////////////////////
int LDH_E0(CPU* cpu)
{
    // LDH [a8] A
    cpu->pc++;
    uint8_t offset = cpu->readMemory(cpu->pc);
    cpu->pc++;
    uint16_t address = 0xFF00 + offset;
    cpu->writeMemory(address, cpu->a);
    return 3;
}
int POP_E1(CPU* cpu)
{
    //POP HL
    cpu->pc++;

    uint8_t low_byte = cpu->ram[cpu->sp++];
    uint8_t high_byte = cpu->ram[cpu->sp++];

    if (low_byte == 0xFA && high_byte == 0x02)
    {
        //printf("stack address'    low-0x%04X{0x%02x}    |    high-0x%04X{0x%02x}\n", cpu->sp - 2, low_byte, cpu->sp - 1, high_byte);
    }

    cpu->l = low_byte;
    //Why is l --?
    cpu->h = high_byte;
    return 3;
}
int LD_E2(CPU* cpu)
{
    //LD [C] A
    cpu->pc++;
    uint16_t address = 0xFF00 | cpu->c;
    cpu->writeMemory(address, cpu->a);
    cpu->writeMemory(address, cpu->a);
    return 2;
}
int ILLEGAL_E3(CPU* cpu)
{
    cpu->pc++;
    return 0;
}
int ILLEGAL_E4(CPU* cpu)
{
    cpu->pc++;
    return 0;
}
int PUSH_E5(CPU* cpu)
{
    //PUSH HL
    cpu->pc++;
    cpu->writeMemory(cpu->sp - 1, cpu->h);
    cpu->writeMemory(cpu->sp - 2, cpu->l);
    cpu->sp -= 2;
    return 4;
}
int AND_E6(CPU* cpu)
{
    //AND A n8
    cpu->pc++;
    uint8_t value = cpu->ram[cpu->pc++];
    cpu->a &= value;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = true;
    cpu->c_flag = false;
    return 2;
}
int RST_E7(CPU* cpu)
{
    //RST $20
    cpu->pc++;
    uint16_t return_address = cpu->pc;
    cpu->pc++;
    cpu->writeMemory(cpu->sp - 1, static_cast<uint8_t>(return_address >> 8));
    cpu->writeMemory(cpu->sp - 2, static_cast<uint8_t>(return_address & 0xFF));
    cpu->sp -= 2;
    cpu->pc = 0x0020;
    return 4;
}
int ADD_E8(CPU* cpu)
{
    // ADD SP e8
    cpu->pc++;
    int8_t offset = static_cast<int8_t>(cpu->ram[cpu->pc]);
    cpu->pc++;
    uint16_t old_sp = cpu->sp;
    cpu->sp += offset;
    cpu->z_flag = false;
    cpu->n_flag = false;
    cpu->h_flag = ((old_sp & 0x0F) + (offset & 0x0F)) > 0x0F;
    cpu->c_flag = ((old_sp & 0xFF) + (offset & 0xFF)) > 0xFF;
    return 4;
}
int JP_E9(CPU* cpu)
{
    // JP HL
    cpu->pc++;
    cpu->pc = (cpu->h << 8) | cpu->l;
    return 1;
}
int LD_EA(CPU* cpu)
{
    //LD [a16] A
    cpu->pc++;
    uint16_t address = (cpu->readMemory(cpu->pc + 1) << 8) | cpu->readMemory(cpu->pc);
    cpu->writeMemory(address, cpu->a);
    cpu->pc += 2;
    return 4;
}
int ILLEGAL_EB(CPU* cpu)
{
    cpu->pc++;
    return 0;
}
int ILLEGAL_EC(CPU* cpu)
{
    cpu->pc++;
    return 0;
}
int ILLEGAL_ED(CPU* cpu)
{
    cpu->pc++;
    return 0;
}
int XOR_EE(CPU* cpu)
{
    //XOR A n8
    cpu->pc++;
    uint8_t immediate_value = cpu->ram[cpu->pc];
    cpu->pc++;
    cpu->a ^= immediate_value;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 2;
}
int RST_EF(CPU* cpu)
{
    //RST $28
    cpu->pc++;
    uint16_t return_address = cpu->pc;
    cpu->pc++;
    cpu->writeMemory(cpu->sp - 1, static_cast<uint8_t>(return_address >> 8));
    cpu->writeMemory(cpu->sp - 2, static_cast<uint8_t>(return_address & 0xFF));
    cpu->sp -= 2;
    cpu->pc = 0x0028;
    return 4;
}
////////////////////////
///////////////////////
int LDH_F0(CPU* cpu)
{
    // LDH A [a8]
    cpu->pc++;
    uint8_t offset = cpu->readMemory(cpu->pc);
    cpu->pc++;
    uint16_t address = 0xFF00 + offset;
    cpu->a = cpu->readMemory(address);
    return 3;
}
int POP_F1(CPU* cpu)
{
    //POP AF
    cpu->pc++;
    uint8_t low_byte = cpu->ram[cpu->sp];
    uint8_t high_byte = cpu->ram[cpu->sp + 1];
    cpu->a = high_byte;
    cpu->sp += 2;
    cpu->z_flag = (low_byte & 0x80) != 0;
    cpu->n_flag = (low_byte & 0x40) != 0;
    cpu->h_flag = (low_byte & 0x20) != 0;
    cpu->c_flag = (low_byte & 0x10) != 0;
    return 3;
}
int LD_F2(CPU* cpu)
{
    //LD A [C]
    cpu->pc++;
    uint16_t address = 0xFF00 | cpu->c;
    //if (address == 0xFF44) ly = 0x90;
    //cpu->a = cpu->ram[address];
    cpu->a = cpu->readMemory(address);
    return 2;
}
int DI_F3(CPU* cpu)
{
    //ERROR
    cpu->pc++;
    cpu->IME_flag = false;
    return 1;
}
int ILLEGAL_F4(CPU* cpu)
{
    cpu->pc++;
    return 0;
}
int PUSH_F5(CPU* cpu)
{
    //PUSH AF
    cpu->pc++;
    uint8_t flags = 0;
    if (cpu->z_flag) flags |= (1 << 7);
    if (cpu->n_flag) flags |= (1 << 6);
    if (cpu->h_flag) flags |= (1 << 5);
    if (cpu->c_flag) flags |= (1 << 4);
    cpu->writeMemory(cpu->sp - 1, cpu->a);
    cpu->writeMemory(cpu->sp - 2, flags);
    cpu->sp -= 2;
    return 4;
}
int OR_F6(CPU* cpu)
{
    //OR A n8
    cpu->pc++;
    uint8_t value = cpu->ram[cpu->pc++];
    cpu->a |= value;
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;
    return 2;
}
int RST_F7(CPU* cpu)
{
    //RST $30
    cpu->pc++;
    uint16_t return_address = cpu->pc;
    cpu->pc++;
    cpu->writeMemory(cpu->sp - 1, static_cast<uint8_t>(return_address >> 8));
    cpu->writeMemory(cpu->sp - 2, static_cast<uint8_t>(return_address & 0xFF));
    cpu->sp -= 2;
    cpu->pc = 0x0030;
    return 4;
}
int LD_F8(CPU* cpu)
{
    // LD HL, SP + e8
    cpu->pc++;
    int8_t offset = static_cast<int8_t>(cpu->ram[cpu->pc++]);
    uint16_t sp_plus_offset = cpu->sp + offset;
    cpu->h = static_cast<uint8_t>(sp_plus_offset >> 8);
    cpu->l = static_cast<uint8_t>(sp_plus_offset & 0xFF);
    cpu->z_flag = false;
    cpu->n_flag = false;
    cpu->h_flag = ((cpu->sp & 0x0F) + (offset & 0x0F)) > 0x0F;
    cpu->c_flag = ((cpu->sp & 0xFF) + (offset & 0xFF)) > 0xFF;
    return 4;
}
int LD_F9(CPU* cpu)
{
    // LD SP HL
    cpu->pc++;
    uint16_t hl = (cpu->h << 8) | cpu->l;
    cpu->sp = hl;
    return 2;
}
int LD_FA(CPU* cpu)
{
    //LD A [a16]
    cpu->pc++;
    //uint16_t address = (cpu->ram[cpu->pc + 0x01] << 8) | cpu->ram[cpu->pc];
    //cpu->a = cpu->ram[address];
    uint16_t address = (cpu->readMemory(cpu->pc + 1) << 8) | cpu->readMemory(cpu->pc);
    cpu->a = cpu->readMemory(address);
    cpu->pc += 2;
    return 4;
}
int EI_FB(CPU* cpu)
{
    cpu->pc++;
    cpu->deferredIME = true;
    return 4;
}
int ILLEGAL_FC(CPU* cpu)
{
    cpu->pc++;
    return 0;
}
int ILLEGAL_FD(CPU* cpu)
{
    cpu->pc++;
    return 0;
}
int CP_FE(CPU* cpu)
{
    //if imm is 0x94 stub to 0x94?
    //CP A n8
    cpu->pc++;
    uint8_t immediate_value = cpu->ram[cpu->pc];
    cpu->pc++;
    uint16_t result = cpu->a - immediate_value;
    cpu->z_flag = (result == 0);
    cpu->n_flag = true;
    cpu->h_flag = ((cpu->a & 0x0F) < (immediate_value & 0x0F));
    cpu->c_flag = (result > 0xFF);
    return 2;
}
int RST_FF(CPU* cpu)
{
    //RST $38
    cpu->pc++;
    uint16_t return_address = cpu->pc;
    cpu->pc++;
    cpu->writeMemory(cpu->sp - 1, static_cast<uint8_t>(return_address >> 8));
    cpu->writeMemory(cpu->sp - 2, static_cast<uint8_t>(return_address & 0xFF));
    cpu->sp -= 2;
    cpu->pc = 0x0038;
    return 4;
}

//CB Instruction table
int RLC_100(CPU* cpu)
{
    // RLC B - Rotate B left with carry
    cpu->pc++;

    // Perform the rotation
    uint8_t new_carry = (cpu->b & 0x80) >> 7; // Get the most significant bit (MSB) as the new carry
    uint8_t result = (cpu->b << 1) | new_carry; // Rotate left and insert MSB into LSB

    // Set the flags
    cpu->z_flag = (result == 0);  // Set Zero flag if result is zero
    cpu->n_flag = false;          // Clear Subtract flag
    cpu->h_flag = false;          // Clear Half-Carry flag
    cpu->c_flag = new_carry;      // Set Carry flag to old MSB

    // Store the result in B
    cpu->b = result;

    return 2;
}
int RLC_101(CPU* cpu)
{
    // RLC C - Rotate C left with carry
    cpu->pc++;

    // Perform the rotation
    uint8_t new_carry = (cpu->c & 0x80) >> 7; // Get the most significant bit (MSB) as the new carry
    uint8_t result = (cpu->c << 1) | new_carry; // Rotate left and insert MSB into LSB

    // Set the flags
    cpu->z_flag = (result == 0);  // Set Zero flag if result is zero
    cpu->n_flag = false;          // Clear Subtract flag
    cpu->h_flag = false;          // Clear Half-Carry flag
    cpu->c_flag = new_carry;      // Set Carry flag to old MSB

    // Store the result in C
    cpu->c = result;

    return 2;
}
int RLC_102(CPU* cpu)
{
    // RLC D - Rotate D left with carry
    cpu->pc++;

    // Perform the rotation
    uint8_t new_carry = (cpu->d & 0x80) >> 7; // Get the most significant bit (MSB) as the new carry
    uint8_t result = (cpu->d << 1) | new_carry; // Rotate left and insert MSB into LSB

    // Set the flags
    cpu->z_flag = (result == 0);  // Set Zero flag if result is zero
    cpu->n_flag = false;          // Clear Subtract flag
    cpu->h_flag = false;          // Clear Half-Carry flag
    cpu->c_flag = new_carry;      // Set Carry flag to old MSB

    // Store the result in D
    cpu->d = result;

    return 2;
}
int RLC_103(CPU* cpu)
{
    // RLC E - Rotate E left with carry
    cpu->pc++;

    // Perform the rotation
    uint8_t new_carry = (cpu->e & 0x80) >> 7; // Get the most significant bit (MSB) as the new carry
    uint8_t result = (cpu->e << 1) | new_carry; // Rotate left and insert MSB into LSB

    // Set the flags
    cpu->z_flag = (result == 0);  // Set Zero flag if result is zero
    cpu->n_flag = false;          // Clear Subtract flag
    cpu->h_flag = false;          // Clear Half-Carry flag
    cpu->c_flag = new_carry;      // Set Carry flag to old MSB

    // Store the result in E
    cpu->e = result;

    return 2;
}
int RLC_104(CPU* cpu)
{
    // RLC H - Rotate H left with carry
    cpu->pc++;

    // Perform the rotation
    uint8_t new_carry = (cpu->h & 0x80) >> 7; // Get the most significant bit (MSB) as the new carry
    uint8_t result = (cpu->h << 1) | new_carry; // Rotate left and insert MSB into LSB

    // Set the flags
    cpu->z_flag = (result == 0);  // Set Zero flag if result is zero
    cpu->n_flag = false;          // Clear Subtract flag
    cpu->h_flag = false;          // Clear Half-Carry flag
    cpu->c_flag = new_carry;      // Set Carry flag to old MSB

    // Store the result in H
    cpu->h = result;

    return 2;
}
int RLC_105(CPU* cpu)
{
    // RLC L - Rotate L left with carry
    cpu->pc++;

    // Perform the rotation
    uint8_t new_carry = (cpu->l & 0x80) >> 7; // Get the most significant bit (MSB) as the new carry
    uint8_t result = (cpu->l << 1) | new_carry; // Rotate left and insert MSB into LSB

    // Set the flags
    cpu->z_flag = (result == 0);  // Set Zero flag if result is zero
    cpu->n_flag = false;          // Clear Subtract flag
    cpu->h_flag = false;          // Clear Half-Carry flag
    cpu->c_flag = new_carry;      // Set Carry flag to old MSB

    // Store the result in L
    cpu->l = result;

    return 2;
}
int RL_106(CPU* cpu)
{
    // RL (HL) - Rotate value at address HL left through the carry flag
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t new_carry = (value & 0x80) >> 7;
    uint8_t result = (value << 1) | carry_in;

    // Update the value at the address HL
    cpu->writeMemory(address, result);

    // Update flags
    cpu->z_flag = ((value & 0xFF) == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = (value > 0xFF);

    return 4;
}
int RLC_107(CPU* cpu)
{
    // RLC A - Rotate A left with carry
    cpu->pc++;

    // Perform the rotation
    uint8_t new_carry = (cpu->a & 0x80) >> 7; // Get the most significant bit (MSB) as the new carry
    uint8_t result = (cpu->a << 1) | new_carry; // Rotate left and insert MSB into LSB

    // Set the flags
    cpu->z_flag = (result == 0);  // Set Zero flag if result is zero
    cpu->n_flag = false;          // Clear Subtract flag
    cpu->h_flag = false;          // Clear Half-Carry flag
    cpu->c_flag = new_carry;      // Set Carry flag to old MSB

    // Store the result in A
    cpu->a = result;

    return 2;
}
int RRC_108(CPU* cpu)
{
    // RRC B - Rotate B right with carry
    cpu->pc++;

    // Perform the rotation
    uint8_t new_carry = cpu->b & 0x01; // Get the least significant bit (LSB) as the new carry
    uint8_t result = (cpu->b >> 1) | (new_carry << 7); // Rotate right and insert LSB into MSB

    // Set the flags
    cpu->z_flag = (result == 0);  // Set Zero flag if result is zero
    cpu->n_flag = false;          // Clear Subtract flag
    cpu->h_flag = false;          // Clear Half-Carry flag
    cpu->c_flag = new_carry;      // Set Carry flag to old LSB

    // Store the result in B
    cpu->b = result;

    return 2;
}
int RRC_109(CPU* cpu)
{
    // RRC C - Rotate C right with carry
    cpu->pc++;

    // Perform the rotation
    uint8_t new_carry = cpu->c & 0x01; // Get the least significant bit (LSB) as the new carry
    uint8_t result = (cpu->c >> 1) | (new_carry << 7); // Rotate right and insert LSB into MSB

    // Set the flags
    cpu->z_flag = (result == 0);  // Set Zero flag if result is zero
    cpu->n_flag = false;          // Clear Subtract flag
    cpu->h_flag = false;          // Clear Half-Carry flag
    cpu->c_flag = new_carry;      // Set Carry flag to old LSB

    // Store the result in C
    cpu->c = result;

    return 2;
}
int RRC_10A(CPU* cpu)
{
    // RRC D - Rotate D right with carry
    cpu->pc++;

    // Perform the rotation
    uint8_t new_carry = cpu->d & 0x01; // Get the least significant bit (LSB) as the new carry
    uint8_t result = (cpu->d >> 1) | (new_carry << 7); // Rotate right and insert LSB into MSB

    // Set the flags
    cpu->z_flag = (result == 0);  // Set Zero flag if result is zero
    cpu->n_flag = false;          // Clear Subtract flag
    cpu->h_flag = false;          // Clear Half-Carry flag
    cpu->c_flag = new_carry;      // Set Carry flag to old LSB

    // Store the result in D
    cpu->d = result;

    return 2;
}
int RRC_10B(CPU* cpu)
{
    // RRC E - Rotate E right with carry
    cpu->pc++;

    // Perform the rotation
    uint8_t new_carry = cpu->e & 0x01; // Get the least significant bit (LSB) as the new carry
    uint8_t result = (cpu->e >> 1) | (new_carry << 7); // Rotate right and insert LSB into MSB

    // Set the flags
    cpu->z_flag = (result == 0);  // Set Zero flag if result is zero
    cpu->n_flag = false;          // Clear Subtract flag
    cpu->h_flag = false;          // Clear Half-Carry flag
    cpu->c_flag = new_carry;      // Set Carry flag to old LSB

    // Store the result in E
    cpu->e = result;

    return 2;
}
int RRC_10C(CPU* cpu)
{
    // RRC H - Rotate H right with carry
    cpu->pc++;

    // Perform the rotation
    uint8_t new_carry = cpu->h & 0x01; // Get the least significant bit (LSB) as the new carry
    uint8_t result = (cpu->h >> 1) | (new_carry << 7); // Rotate right and insert LSB into MSB

    // Set the flags
    cpu->z_flag = (result == 0);  // Set Zero flag if result is zero
    cpu->n_flag = false;          // Clear Subtract flag
    cpu->h_flag = false;          // Clear Half-Carry flag
    cpu->c_flag = new_carry;      // Set Carry flag to old LSB

    // Store the result in H
    cpu->h = result;

    return 2;
}
int RRC_10D(CPU* cpu)
{
    // RRC L - Rotate L right with carry
    cpu->pc++;

    // Perform the rotation
    uint8_t new_carry = cpu->l & 0x01; // Get the least significant bit (LSB) as the new carry
    uint8_t result = (cpu->l >> 1) | (new_carry << 7); // Rotate right and insert LSB into MSB

    // Set the flags
    cpu->z_flag = (result == 0);  // Set Zero flag if result is zero
    cpu->n_flag = false;          // Clear Subtract flag
    cpu->h_flag = false;          // Clear Half-Carry flag
    cpu->c_flag = new_carry;      // Set Carry flag to old LSB

    // Store the result in L
    cpu->l = result;

    return 2;
}
int RRC_10E(CPU* cpu)
{
    // RRC (HL) - Rotate value at address HL right with carry
    cpu->pc++;

    // Get the value from memory at address HL
    uint16_t address = (cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];

    // Perform the rotation
    uint8_t new_carry = value & 0x01; // Get the least significant bit (LSB) as the new carry
    uint8_t result = (value >> 1) | (new_carry << 7); // Rotate right and insert LSB into MSB

    // Set the flags
    cpu->z_flag = (result == 0);  // Set Zero flag if result is zero
    cpu->n_flag = false;          // Clear Subtract flag
    cpu->h_flag = false;          // Clear Half-Carry flag
    cpu->c_flag = new_carry;      // Set Carry flag to old LSB

    // Store the result back into memory
    cpu->writeMemory(address, result);

    return 4;
}
int RRC_10F(CPU* cpu)
{
    // RRC A - Rotate A right with carry
    cpu->pc++;

    // Perform the rotation
    uint8_t new_carry = cpu->a & 0x01; // Get the least significant bit (LSB) as the new carry
    uint8_t result = (cpu->a >> 1) | (new_carry << 7); // Rotate right and insert LSB into MSB

    // Set the flags
    cpu->z_flag = (result == 0);           // RRC A does not affect the zero flag
    cpu->n_flag = false;           // Clear Subtract flag
    cpu->h_flag = false;           // Clear Half-Carry flag
    cpu->c_flag = new_carry;       // Set Carry flag to old LSB

    // Store the result in A
    cpu->a = result;

    return 2;
}
int RL_110(CPU* cpu)
{
    // RL B - Rotate B left through the carry flag
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint16_t result = (cpu->b << 1) | carry_in;

    // Update B register with the result
    cpu->b = static_cast<uint8_t>(result & 0xFF);

    // Update flags
    cpu->z_flag = (cpu->b == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = (result > 0xFF);

    // Increment PC by 2 (CB prefix + opcode)
    cpu->pc += 1;
    cpu->pc &= 0xFFFF;

    return 2;
}
int RL_111(CPU* cpu)
{
    // RL C - Rotate C left through the carry flag
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint16_t result = (cpu->c << 1) | carry_in;

    // Update C register with the result
    cpu->c = static_cast<uint8_t>(result & 0xFF);

    // Update flags
    cpu->z_flag = (cpu->c == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = (result > 0xFF);

    // Increment PC by 2 (CB prefix + opcode)
    cpu->pc += 1;
    cpu->pc &= 0xFFFF;

    return 2;
}
int RL_112(CPU* cpu)
{
    // RL D - Rotate D left through the carry flag
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint16_t result = (cpu->d << 1) | carry_in;

    // Update D register with the result
    cpu->d = static_cast<uint8_t>(result & 0xFF);

    // Update flags
    cpu->z_flag = (cpu->d == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = (result > 0xFF);

    // Increment PC by 2 (CB prefix + opcode)
    cpu->pc += 1;
    cpu->pc &= 0xFFFF;

    return 2;
}

int RL_113(CPU* cpu)
{
    // RL E - Rotate E left through the carry flag
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint16_t result = (cpu->e << 1) | carry_in;

    // Update E register with the result
    cpu->e = static_cast<uint8_t>(result & 0xFF);

    // Update flags
    cpu->z_flag = (cpu->e == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = (result > 0xFF);

    // Increment PC by 2 (CB prefix + opcode)
    cpu->pc += 1;
    cpu->pc &= 0xFFFF;

    return 2;
}

int RL_114(CPU* cpu)
{
    // RL H - Rotate H left through the carry flag
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint16_t result = (cpu->h << 1) | carry_in;

    // Update H register with the result
    cpu->h = static_cast<uint8_t>(result & 0xFF);

    // Update flags
    cpu->z_flag = (cpu->h == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = (result > 0xFF);

    // Increment PC by 2 (CB prefix + opcode)
    cpu->pc += 1;
    cpu->pc &= 0xFFFF;

    return 2;
}

int RL_115(CPU* cpu)
{
    // RL L - Rotate L left through the carry flag
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint16_t result = (cpu->l << 1) | carry_in;

    // Update L register with the result
    cpu->l = static_cast<uint8_t>(result & 0xFF);

    // Update flags
    cpu->z_flag = (cpu->l == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = (result > 0xFF);

    // Increment PC by 2 (CB prefix + opcode)
    cpu->pc += 1;
    cpu->pc &= 0xFFFF;

    return 2;
}

int RL_116(CPU* cpu)
{
    // RL (HL) - Rotate value at address HL left through the carry flag
    uint16_t address = (cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint16_t result = (value << 1) | carry_in;

    // Update memory with the result
    cpu->writeMemory(address, static_cast<uint8_t>(result & 0xFF));

    // Update flags
    cpu->z_flag = (cpu->ram[address] == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = (result > 0xFF);

    // Increment PC by 2 (CB prefix + opcode)
    cpu->pc += 1;
    cpu->pc &= 0xFFFF;

    return 4;
}

int RL_117(CPU* cpu)
{
    // RL A - Rotate A left through the carry flag
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint16_t result = (cpu->a << 1) | carry_in;

    // Update A register with the result
    cpu->a = static_cast<uint8_t>(result & 0xFF);

    // Update flags
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = (result > 0xFF);

    // Increment PC by 2 (CB prefix + opcode)
    cpu->pc += 1;
    cpu->pc &= 0xFFFF;

    return 2;
}
int RR_118(CPU* cpu)
{
    // RR B - Rotate B right through the carry flag
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t new_carry = cpu->b & 0x01;
    uint16_t result = (cpu->b >> 1) | (carry_in << 7);

    // Update B register with the result
    cpu->b = static_cast<uint8_t>(result & 0xFF);

    // Update flags
    cpu->z_flag = (cpu->b == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    // Increment PC by 2 (CB prefix + opcode)
    cpu->pc += 1;
    cpu->pc &= 0xFFFF;

    return 2;
}
int RR_119(CPU* cpu)
{
    // RR C - Rotate C right through the carry flag
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t new_carry = cpu->c & 0x01;
    uint16_t result = (cpu->c >> 1) | (carry_in << 7);

    // Update C register with the result
    cpu->c = static_cast<uint8_t>(result & 0xFF);

    // Update flags
    cpu->z_flag = (cpu->c == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    // Increment PC by 2 (CB prefix + opcode)
    cpu->pc += 1;
    cpu->pc &= 0xFFFF;

    return 2;
}
int RR_11A(CPU* cpu)
{
    // RR D - Rotate D right through the carry flag
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t new_carry = cpu->d & 0x01;
    uint16_t result = (cpu->d >> 1) | (carry_in << 7);

    // Update D register with the result
    cpu->d = static_cast<uint8_t>(result & 0xFF);

    // Update flags
    cpu->z_flag = (cpu->d == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    // Increment PC by 2 (CB prefix + opcode)
    cpu->pc += 1;
    cpu->pc &= 0xFFFF;

    return 2;
}
int RR_11B(CPU* cpu)
{
    // RR E - Rotate E right through the carry flag
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t new_carry = cpu->e & 0x01;
    uint16_t result = (cpu->e >> 1) | (carry_in << 7);

    // Update E register with the result
    cpu->e = static_cast<uint8_t>(result & 0xFF);

    // Update flags
    cpu->z_flag = (cpu->e == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    // Increment PC by 2 (CB prefix + opcode)
    cpu->pc += 1;
    cpu->pc &= 0xFFFF;

    return 2;
}
int RR_11C(CPU* cpu)
{
    // RR H - Rotate H right through the carry flag
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t new_carry = cpu->h & 0x01;
    uint16_t result = (cpu->h >> 1) | (carry_in << 7);

    // Update H register with the result
    cpu->h = static_cast<uint8_t>(result & 0xFF);

    // Update flags
    cpu->z_flag = (cpu->h == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    // Increment PC by 2 (CB prefix + opcode)
    cpu->pc += 1;
    cpu->pc &= 0xFFFF;

    return 2;
}
int RR_11D(CPU* cpu)
{
    // RR L - Rotate L right through the carry flag
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t new_carry = cpu->l & 0x01;
    uint16_t result = (cpu->l >> 1) | (carry_in << 7);

    // Update L register with the result
    cpu->l = static_cast<uint8_t>(result & 0xFF);

    // Update flags
    cpu->z_flag = (cpu->l == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    // Increment PC by 2 (CB prefix + opcode)
    cpu->pc += 1;
    cpu->pc &= 0xFFFF;

    return 2;
}
int RR_11E(CPU* cpu)
{
    // RR (HL) - Rotate value at address HL right through the carry flag
    uint16_t address = (cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t new_carry = value & 0x01;
    uint16_t result = (value >> 1) | (carry_in << 7);

    // Update memory with the result
    cpu->writeMemory(address, static_cast<uint8_t>(result & 0xFF));

    // Update flags
    cpu->z_flag = (cpu->ram[address] == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    // Increment PC by 2 (CB prefix + opcode)
    cpu->pc += 1;
    cpu->pc &= 0xFFFF;

    return 4;
}
int RR_11F(CPU* cpu)
{
    // RR A - Rotate A right through the carry flag
    uint8_t carry_in = cpu->c_flag ? 1 : 0;
    uint8_t new_carry = cpu->a & 0x01;
    uint16_t result = (cpu->a >> 1) | (carry_in << 7);

    // Update A register with the result
    cpu->a = static_cast<uint8_t>(result & 0xFF);

    // Update flags
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    // Increment PC by 2 (CB prefix + opcode)
    cpu->pc += 1;
    cpu->pc &= 0xFFFF;

    return 2;
}
int SLA_120(CPU* cpu)
{
    // SLA B - Shift B left arithmetic
    cpu->pc++;
    uint8_t new_carry = (cpu->b & 0x80) >> 7;
    uint8_t result = cpu->b << 1;

    // Update B register with the result
    cpu->b = result;

    // Update flags
    cpu->z_flag = (cpu->b == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}
int SLA_121(CPU* cpu)
{
    // SLA C - Shift C left arithmetic
    cpu->pc++;
    uint8_t new_carry = (cpu->c & 0x80) >> 7;
    uint8_t result = cpu->c << 1;

    // Update C register with the result
    cpu->c = result;

    // Update flags
    cpu->z_flag = (cpu->c == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}
int SLA_122(CPU* cpu)
{
    // SLA D - Shift D left arithmetic
    cpu->pc++;
    uint8_t new_carry = (cpu->d & 0x80) >> 7;
    uint8_t result = cpu->d << 1;

    // Update D register with the result
    cpu->d = result;

    // Update flags
    cpu->z_flag = (cpu->d == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}
int SLA_123(CPU* cpu)
{
    // SLA E - Shift E left arithmetic
    cpu->pc++;
    uint8_t new_carry = (cpu->e & 0x80) >> 7;
    uint8_t result = cpu->e << 1;

    // Update E register with the result
    cpu->e = result;

    // Update flags
    cpu->z_flag = (cpu->e == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}
int SLA_124(CPU* cpu)
{
    // SLA H - Shift H left arithmetic
    cpu->pc++;
    uint8_t new_carry = (cpu->h & 0x80) >> 7;
    uint8_t result = cpu->h << 1;

    // Update H register with the result
    cpu->h = result;

    // Update flags
    cpu->z_flag = (cpu->h == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}
int SLA_125(CPU* cpu)
{
    // SLA L - Shift L left arithmetic
    cpu->pc++;
    uint8_t new_carry = (cpu->l & 0x80) >> 7;
    uint8_t result = cpu->l << 1;

    // Update L register with the result
    cpu->l = result;

    // Update flags
    cpu->z_flag = (cpu->l == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}
int SLA_126(CPU* cpu)
{
    // SLA (HL) - Shift value at address HL left arithmetic
    uint16_t address = (cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];
    uint8_t new_carry = (value & 0x80) >> 7;
    uint8_t result = value << 1;

    // Update memory with the result
    cpu->writeMemory(address, result);

    // Update flags
    cpu->z_flag = (cpu->ram[address] == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 4;
}
int SLA_127(CPU* cpu)
{
    // SLA A - Shift A left arithmetic
    cpu->pc++;
    uint8_t new_carry = (cpu->a & 0x80) >> 7;
    uint8_t result = cpu->a << 1;

    // Update A register with the result
    cpu->a = result;

    // Update flags
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}
int SRA_128(CPU* cpu)
{
    // SRA B - Shift B right arithmetic
    cpu->pc++;
    uint8_t new_carry = cpu->b & 0x01;
    uint8_t result = (cpu->b >> 1) | (cpu->b & 0x80); // Keep the MSB (bit 7) unchanged

    // Update B register with the result
    cpu->b = result;

    // Update flags
    cpu->z_flag = (cpu->b == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}

int SRA_129(CPU* cpu)
{
    // SRA C - Shift C right arithmetic
    cpu->pc++;
    uint8_t new_carry = cpu->c & 0x01;
    uint8_t result = (cpu->c >> 1) | (cpu->c & 0x80); // Keep the MSB (bit 7) unchanged

    // Update C register with the result
    cpu->c = result;

    // Update flags
    cpu->z_flag = (cpu->c == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}

int SRA_12A(CPU* cpu)
{
    // SRA D - Shift D right arithmetic
    cpu->pc++;
    uint8_t new_carry = cpu->d & 0x01;
    uint8_t result = (cpu->d >> 1) | (cpu->d & 0x80); // Keep the MSB (bit 7) unchanged

    // Update D register with the result
    cpu->d = result;

    // Update flags
    cpu->z_flag = (cpu->d == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}

int SRA_12B(CPU* cpu)
{
    // SRA E - Shift E right arithmetic
    cpu->pc++;
    uint8_t new_carry = cpu->e & 0x01;
    uint8_t result = (cpu->e >> 1) | (cpu->e & 0x80); // Keep the MSB (bit 7) unchanged

    // Update E register with the result
    cpu->e = result;

    // Update flags
    cpu->z_flag = (cpu->e == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}

int SRA_12C(CPU* cpu)
{
    // SRA H - Shift H right arithmetic
    cpu->pc++;
    uint8_t new_carry = cpu->h & 0x01;
    uint8_t result = (cpu->h >> 1) | (cpu->h & 0x80); // Keep the MSB (bit 7) unchanged

    // Update H register with the result
    cpu->h = result;

    // Update flags
    cpu->z_flag = (cpu->h == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}

int SRA_12D(CPU* cpu)
{
    // SRA L - Shift L right arithmetic
    cpu->pc++;
    uint8_t new_carry = cpu->l & 0x01;
    uint8_t result = (cpu->l >> 1) | (cpu->l & 0x80); // Keep the MSB (bit 7) unchanged

    // Update L register with the result
    cpu->l = result;

    // Update flags
    cpu->z_flag = (cpu->l == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}

int SRA_12E(CPU* cpu)
{
    // SRA (HL) - Shift value at address HL right arithmetic
    uint16_t address = (cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];
    uint8_t new_carry = value & 0x01;
    uint8_t result = (value >> 1) | (value & 0x80); // Keep the MSB (bit 7) unchanged

    // Update memory with the result
    cpu->writeMemory(address, result);

    // Update flags
    cpu->z_flag = (cpu->ram[address] == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 4;
}

int SRA_12F(CPU* cpu)
{
    // SRA A - Shift A right arithmetic
    cpu->pc++;
    uint8_t new_carry = cpu->a & 0x01;
    uint8_t result = (cpu->a >> 1) | (cpu->a & 0x80); // Keep the MSB (bit 7) unchanged

    // Update A register with the result
    cpu->a = result;

    // Update flags
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}
int SWAP_130(CPU* cpu)
{
    // SWAP B - Swap upper and lower nibbles of B
    cpu->pc++;
    cpu->b = (cpu->b << 4) | (cpu->b >> 4);

    // Update flags
    cpu->z_flag = (cpu->b == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;

    return 2;
}

int SWAP_131(CPU* cpu)
{
    // SWAP C - Swap upper and lower nibbles of C
    cpu->pc++;
    cpu->c = (cpu->c << 4) | (cpu->c >> 4);

    // Update flags
    cpu->z_flag = (cpu->c == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;

    return 2;
}

int SWAP_132(CPU* cpu)
{
    // SWAP D - Swap upper and lower nibbles of D
    cpu->pc++;
    cpu->d = (cpu->d << 4) | (cpu->d >> 4);

    // Update flags
    cpu->z_flag = (cpu->d == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;

    return 2;
}

int SWAP_133(CPU* cpu)
{
    // SWAP E - Swap upper and lower nibbles of E
    cpu->pc++;
    cpu->e = (cpu->e << 4) | (cpu->e >> 4);

    // Update flags
    cpu->z_flag = (cpu->e == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;

    return 2;
}

int SWAP_134(CPU* cpu)
{
    // SWAP H - Swap upper and lower nibbles of H
    cpu->pc++;
    cpu->h = (cpu->h << 4) | (cpu->h >> 4);

    // Update flags
    cpu->z_flag = (cpu->h == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;

    return 2;
}

int SWAP_135(CPU* cpu)
{
    // SWAP L - Swap upper and lower nibbles of L
    cpu->pc++;
    cpu->l = (cpu->l << 4) | (cpu->l >> 4);

    // Update flags
    cpu->z_flag = (cpu->l == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;

    return 2;
}

int SWAP_136(CPU* cpu)
{
    // SWAP (HL) - Swap upper and lower nibbles of value at address HL
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];
    cpu->writeMemory(address, (value << 4) | (value >> 4));

    // Update flags
    cpu->z_flag = (cpu->ram[address] == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;

    return 4;
}

int SWAP_137(CPU* cpu)
{
    // SWAP A - Swap upper and lower nibbles of A
    cpu->pc++;
    cpu->a = (cpu->a << 4) | (cpu->a >> 4);

    // Update flags
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = false;

    return 2;
}
int SRL_138(CPU* cpu)
{
    // SRL B - Shift B right logical
    cpu->pc++;
    uint8_t new_carry = cpu->b & 0x01;
    uint8_t result = cpu->b >> 1;

    // Update B register with the result
    cpu->b = result;

    // Update flags
    cpu->z_flag = (cpu->b == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}

int SRL_139(CPU* cpu)
{
    // SRL C - Shift C right logical
    cpu->pc++;
    uint8_t new_carry = cpu->c & 0x01;
    uint8_t result = cpu->c >> 1;

    // Update C register with the result
    cpu->c = result;

    // Update flags
    cpu->z_flag = (cpu->c == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}

int SRL_13A(CPU* cpu)
{
    // SRL D - Shift D right logical
    cpu->pc++;
    uint8_t new_carry = cpu->d & 0x01;
    uint8_t result = cpu->d >> 1;

    // Update D register with the result
    cpu->d = result;

    // Update flags
    cpu->z_flag = (cpu->d == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}

int SRL_13B(CPU* cpu)
{
    // SRL E - Shift E right logical
    cpu->pc++;
    uint8_t new_carry = cpu->e & 0x01;
    uint8_t result = cpu->e >> 1;

    // Update E register with the result
    cpu->e = result;

    // Update flags
    cpu->z_flag = (cpu->e == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}

int SRL_13C(CPU* cpu)
{
    // SRL H - Shift H right logical
    cpu->pc++;
    uint8_t new_carry = cpu->h & 0x01;
    uint8_t result = cpu->h >> 1;

    // Update H register with the result
    cpu->h = result;

    // Update flags
    cpu->z_flag = (cpu->h == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}

int SRL_13D(CPU* cpu)
{
    // SRL L - Shift L right logical
    cpu->pc++;
    uint8_t new_carry = cpu->l & 0x01;
    uint8_t result = cpu->l >> 1;

    // Update L register with the result
    cpu->l = result;

    // Update flags
    cpu->z_flag = (cpu->l == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}

int SRL_13E(CPU* cpu)
{
    // SRL (HL) - Shift value at address HL right logical
    uint16_t address = (cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];
    uint8_t new_carry = value & 0x01;
    uint8_t result = value >> 1;

    // Update memory with the result
    cpu->writeMemory(address, result);

    // Update flags
    cpu->z_flag = (cpu->ram[address] == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 4;
}

int SRL_13F(CPU* cpu)
{
    // SRL A - Shift A right logical
    cpu->pc++;
    uint8_t new_carry = cpu->a & 0x01;
    uint8_t result = cpu->a >> 1;

    // Update A register with the result
    cpu->a = result;

    // Update flags
    cpu->z_flag = (cpu->a == 0);
    cpu->n_flag = false;
    cpu->h_flag = false;
    cpu->c_flag = new_carry;

    return 2;
}
int BIT_140(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->b & 0x01); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_141(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->c & 0x01); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_142(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->d & 0x01); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_143(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->e & 0x01); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_144(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->h & 0x01); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_145(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->l & 0x01); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_146(CPU* cpu) { cpu->pc++; uint16_t address = (cpu->h << 8) | cpu->l; cpu->z_flag = !(cpu->ram[address] & 0x01); cpu->n_flag = false; cpu->h_flag = true; return 4; }
int BIT_147(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->a & 0x01); cpu->n_flag = false; cpu->h_flag = true; return 2; }

int BIT_148(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->b & 0x02); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_149(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->c & 0x02); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_14A(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->d & 0x02); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_14B(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->e & 0x02); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_14C(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->h & 0x02); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_14D(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->l & 0x02); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_14E(CPU* cpu) { cpu->pc++; uint16_t address = (cpu->h << 8) | cpu->l; cpu->z_flag = !(cpu->ram[address] & 0x02); cpu->n_flag = false; cpu->h_flag = true; return 4; }
int BIT_14F(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->a & 0x02); cpu->n_flag = false; cpu->h_flag = true; return 2; }

int BIT_150(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->b & 0x04); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_151(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->c & 0x04); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_152(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->d & 0x04); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_153(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->e & 0x04); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_154(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->h & 0x04); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_155(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->l & 0x04); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_156(CPU* cpu) { cpu->pc++; uint16_t address = (cpu->h << 8) | cpu->l; cpu->z_flag = !(cpu->ram[address] & 0x04); cpu->n_flag = false; cpu->h_flag = true; return 4; }
int BIT_157(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->a & 0x04); cpu->n_flag = false; cpu->h_flag = true; return 2; }

int BIT_158(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->b & 0x08); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_159(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->c & 0x08); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_15A(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->d & 0x08); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_15B(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->e & 0x08); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_15C(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->h & 0x08); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_15D(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->l & 0x08); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_15E(CPU* cpu) { cpu->pc++; uint16_t address = (cpu->h << 8) | cpu->l; cpu->z_flag = !(cpu->ram[address] & 0x08); cpu->n_flag = false; cpu->h_flag = true; return 4; }
int BIT_15F(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->a & 0x08); cpu->n_flag = false; cpu->h_flag = true; return 2; }

int BIT_160(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->b & 0x10); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_161(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->c & 0x10); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_162(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->d & 0x10); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_163(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->e & 0x10); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_164(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->h & 0x10); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_165(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->l & 0x10); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_166(CPU* cpu) { cpu->pc++; uint16_t address = (cpu->h << 8) | cpu->l; cpu->z_flag = !(cpu->ram[address] & 0x10); cpu->n_flag = false; cpu->h_flag = true; return 4; }
int BIT_167(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->a & 0x10); cpu->n_flag = false; cpu->h_flag = true; return 2; }

int BIT_168(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->b & 0x20); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_169(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->c & 0x20); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_16A(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->d & 0x20); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_16B(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->e & 0x20); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_16C(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->h & 0x20); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_16D(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->l & 0x20); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_16E(CPU* cpu) { cpu->pc++; uint16_t address = (cpu->h << 8) | cpu->l; cpu->z_flag = !(cpu->ram[address] & 0x20); cpu->n_flag = false; cpu->h_flag = true; return 4; }
int BIT_16F(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->a & 0x20); cpu->n_flag = false; cpu->h_flag = true; return 2; }

int BIT_170(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->b & 0x40); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_171(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->c & 0x40); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_172(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->d & 0x40); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_173(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->e & 0x40); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_174(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->h & 0x40); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_175(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->l & 0x40); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_176(CPU* cpu) { cpu->pc++; uint16_t address = (cpu->h << 8) | cpu->l; cpu->z_flag = !(cpu->ram[address] & 0x40); cpu->n_flag = false; cpu->h_flag = true; return 4; }
int BIT_177(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->a & 0x40); cpu->n_flag = false; cpu->h_flag = true; return 2; }

int BIT_178(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->b & 0x80); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_179(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->c & 0x80); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_17A(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->d & 0x80); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_17B(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->e & 0x80); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_17C(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->h & 0x80); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_17D(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->l & 0x80); cpu->n_flag = false; cpu->h_flag = true; return 2; }
int BIT_17E(CPU* cpu) { cpu->pc++; uint16_t address = (cpu->h << 8) | cpu->l; cpu->z_flag = !(cpu->ram[address] & 0x80); cpu->n_flag = false; cpu->h_flag = true; return 4; }
int BIT_17F(CPU* cpu) { cpu->pc++; cpu->z_flag = !(cpu->a & 0x80); cpu->n_flag = false; cpu->h_flag = true; return 2; }

int RES_180(CPU* cpu) { cpu->pc++; cpu->b &= ~0x01; return 2; }
int RES_181(CPU* cpu) { cpu->pc++; cpu->c &= ~0x01; return 2; }
int RES_182(CPU* cpu) { cpu->pc++; cpu->d &= ~0x01; return 2; }
int RES_183(CPU* cpu) { cpu->pc++; cpu->e &= ~0x01; return 2; }
int RES_184(CPU* cpu) { cpu->pc++; cpu->h &= ~0x01; return 2; }
int RES_185(CPU* cpu) { cpu->pc++; cpu->l &= ~0x01; return 2; }
int RES_186(CPU* cpu) { cpu->pc++; uint16_t address = (cpu->h << 8) | cpu->l; cpu->ram[address] &= ~0x01; return 4; }
int RES_187(CPU* cpu) { cpu->pc++; cpu->a &= ~0x01; return 2; }

int RES_188(CPU* cpu) { cpu->pc++; cpu->b &= ~0x02; return 2; }
int RES_189(CPU* cpu) { cpu->pc++; cpu->c &= ~0x02; return 2; }
int RES_18A(CPU* cpu) { cpu->pc++; cpu->d &= ~0x02; return 2; }
int RES_18B(CPU* cpu) { cpu->pc++; cpu->e &= ~0x02; return 2; }
int RES_18C(CPU* cpu) { cpu->pc++; cpu->h &= ~0x02; return 2; }
int RES_18D(CPU* cpu) { cpu->pc++; cpu->l &= ~0x02; return 2; }
int RES_18E(CPU* cpu) { cpu->pc++; uint16_t address = (cpu->h << 8) | cpu->l; cpu->ram[address] &= ~0x02; return 4; }
int RES_18F(CPU* cpu) { cpu->pc++; cpu->a &= ~0x02; return 2; }

int RES_190(CPU* cpu) { cpu->pc++; cpu->b &= ~0x04; return 2; }
int RES_191(CPU* cpu) { cpu->pc++; cpu->c &= ~0x04; return 2; }
int RES_192(CPU* cpu) { cpu->pc++; cpu->d &= ~0x04; return 2; }
int RES_193(CPU* cpu) { cpu->pc++; cpu->e &= ~0x04; return 2; }
int RES_194(CPU* cpu) { cpu->pc++; cpu->h &= ~0x04; return 2; }
int RES_195(CPU* cpu) { cpu->pc++; cpu->l &= ~0x04; return 2; }
int RES_196(CPU* cpu) { cpu->pc++; uint16_t address = (cpu->h << 8) | cpu->l; cpu->ram[address] &= ~0x04; return 4; }
int RES_197(CPU* cpu) { cpu->pc++; cpu->a &= ~0x04; return 2; }

int RES_198(CPU* cpu) { cpu->pc++; cpu->b &= ~0x08; return 2; }
int RES_199(CPU* cpu) { cpu->pc++; cpu->c &= ~0x08; return 2; }
int RES_19A(CPU* cpu) { cpu->pc++; cpu->d &= ~0x08; return 2; }
int RES_19B(CPU* cpu) { cpu->pc++; cpu->e &= ~0x08; return 2; }
int RES_19C(CPU* cpu) { cpu->pc++; cpu->h &= ~0x08; return 2; }
int RES_19D(CPU* cpu) { cpu->pc++; cpu->l &= ~0x08; return 2; }
int RES_19E(CPU* cpu) { cpu->pc++; uint16_t address = (cpu->h << 8) | cpu->l; cpu->ram[address] &= ~0x08; return 4; }
int RES_19F(CPU* cpu) { cpu->pc++; cpu->a &= ~0x08; return 2; }

int RES_1A0(CPU* cpu) { cpu->pc++; cpu->b &= ~0x10; return 2; }
int RES_1A1(CPU* cpu) { cpu->pc++; cpu->c &= ~0x10; return 2; }
int RES_1A2(CPU* cpu) { cpu->pc++; cpu->d &= ~0x10; return 2; }
int RES_1A3(CPU* cpu) { cpu->pc++; cpu->e &= ~0x10; return 2; }
int RES_1A4(CPU* cpu) { cpu->pc++; cpu->h &= ~0x10; return 2; }
int RES_1A5(CPU* cpu) { cpu->pc++; cpu->l &= ~0x10; return 2; }
int RES_1A6(CPU* cpu) { cpu->pc++; uint16_t address = (cpu->h << 8) | cpu->l; cpu->ram[address] &= ~0x10; return 4; }
int RES_1A7(CPU* cpu) { cpu->pc++; cpu->a &= ~0x10; return 2; }

int RES_1A8(CPU* cpu) { cpu->pc++; cpu->b &= ~0x20; return 2; }
int RES_1A9(CPU* cpu) { cpu->pc++; cpu->c &= ~0x20; return 2; }
int RES_1AA(CPU* cpu) { cpu->pc++; cpu->d &= ~0x20; return 2; }
int RES_1AB(CPU* cpu) { cpu->pc++; cpu->e &= ~0x20; return 2; }
int RES_1AC(CPU* cpu) { cpu->pc++; cpu->h &= ~0x20; return 2; }
int RES_1AD(CPU* cpu) { cpu->pc++; cpu->l &= ~0x20; return 2; }
int RES_1AE(CPU* cpu) { cpu->pc++; uint16_t address = (cpu->h << 8) | cpu->l; cpu->ram[address] &= ~0x20; return 4; }
int RES_1AF(CPU* cpu) { cpu->pc++; cpu->a &= ~0x20; return 2; }

int RES_1B0(CPU* cpu) { cpu->pc++; cpu->b &= ~0x40; return 2; }
int RES_1B1(CPU* cpu) { cpu->pc++; cpu->c &= ~0x40; return 2; }
int RES_1B2(CPU* cpu) { cpu->pc++; cpu->d &= ~0x40; return 2; }
int RES_1B3(CPU* cpu) { cpu->pc++; cpu->e &= ~0x40; return 2; }
int RES_1B4(CPU* cpu) { cpu->pc++; cpu->h &= ~0x40; return 2; }
int RES_1B5(CPU* cpu) { cpu->pc++; cpu->l &= ~0x40; return 2; }
int RES_1B6(CPU* cpu) { cpu->pc++; uint16_t address = (cpu->h << 8) | cpu->l; cpu->ram[address] &= ~0x40; return 4; }
int RES_1B7(CPU* cpu) { cpu->pc++; cpu->a &= ~0x40; return 2; }

int RES_1B8(CPU* cpu) { cpu->pc++; cpu->b &= ~0x80; return 2; }
int RES_1B9(CPU* cpu) { cpu->pc++; cpu->c &= ~0x80; return 2; }
int RES_1BA(CPU* cpu) { cpu->pc++; cpu->d &= ~0x80; return 2; }
int RES_1BB(CPU* cpu) { cpu->pc++; cpu->e &= ~0x80; return 2; }
int RES_1BC(CPU* cpu) { cpu->pc++; cpu->h &= ~0x80; return 2; }
int RES_1BD(CPU* cpu) { cpu->pc++; cpu->l &= ~0x80; return 2; }
int RES_1BE(CPU* cpu) { cpu->pc++; uint16_t address = (cpu->h << 8) | cpu->l; cpu->ram[address] &= ~0x80; return 4; }
int RES_1BF(CPU* cpu) { cpu->pc++; cpu->a &= ~0x80; return 2; }

int SET_1C0(CPU* cpu) { cpu->pc++; cpu->b |= (1 << 0); return 2; }
int SET_1C1(CPU* cpu) { cpu->pc++; cpu->c |= (1 << 0); return 2; }
int SET_1C2(CPU* cpu) { cpu->pc++; cpu->d |= (1 << 0); return 2; }
int SET_1C3(CPU* cpu) { cpu->pc++; cpu->e |= (1 << 0); return 2; }
int SET_1C4(CPU* cpu) { cpu->pc++; cpu->h |= (1 << 0); return 2; }
int SET_1C5(CPU* cpu) { cpu->pc++; cpu->l |= (1 << 0); return 2; }
int SET_1C6(CPU* cpu)
{ 
    //printf("Set_1C6 Called\n");
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];
    value |= (1 << 0);
    //cpu->ram[address] = value;
    cpu->writeMemory(address, value);
    return 4;
}
int SET_1C7(CPU* cpu) { cpu->pc++; cpu->a |= (1 << 0); return 2; }

int SET_1C8(CPU* cpu) { cpu->pc++; cpu->b |= (1 << 1); return 2; }
int SET_1C9(CPU* cpu) { cpu->pc++; cpu->c |= (1 << 1); return 2; }
int SET_1CA(CPU* cpu) { cpu->pc++; cpu->d |= (1 << 1); return 2; }
int SET_1CB(CPU* cpu) { cpu->pc++; cpu->e |= (1 << 1); return 2; }
int SET_1CC(CPU* cpu) { cpu->pc++; cpu->h |= (1 << 1); return 2; }
int SET_1CD(CPU* cpu) { cpu->pc++; cpu->l |= (1 << 1); return 2; }
int SET_1CE(CPU* cpu) 
{ 
    //printf("Set_1CE Called\n");
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];
    value |= (1 << 1);
    cpu->writeMemory(address, value);
    return 4;
}
int SET_1CF(CPU* cpu) { cpu->pc++; cpu->a |= (1 << 1); return 2; }

int SET_1D0(CPU* cpu) { cpu->pc++; cpu->b |= (1 << 2); return 2; }
int SET_1D1(CPU* cpu) { cpu->pc++; cpu->c |= (1 << 2); return 2; }
int SET_1D2(CPU* cpu) { cpu->pc++; cpu->d |= (1 << 2); return 2; }
int SET_1D3(CPU* cpu) { cpu->pc++; cpu->e |= (1 << 2); return 2; }
int SET_1D4(CPU* cpu) { cpu->pc++; cpu->h |= (1 << 2); return 2; }
int SET_1D5(CPU* cpu) { cpu->pc++; cpu->l |= (1 << 2); return 2; }
int SET_1D6(CPU* cpu)
{  
    //printf("Set_1D6 Called\n");
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];
    value |= (1 << 2);
    cpu->writeMemory(address, value);
    return 4;
}
int SET_1D7(CPU* cpu) { cpu->pc++; cpu->a |= (1 << 2); return 2; }

int SET_1D8(CPU* cpu) { cpu->pc++; cpu->b |= (1 << 3); return 2; }
int SET_1D9(CPU* cpu) { cpu->pc++; cpu->c |= (1 << 3); return 2; }
int SET_1DA(CPU* cpu) { cpu->pc++; cpu->d |= (1 << 3); return 2; }
int SET_1DB(CPU* cpu) { cpu->pc++; cpu->e |= (1 << 3); return 2; }
int SET_1DC(CPU* cpu) { cpu->pc++; cpu->h |= (1 << 3); return 2; }
int SET_1DD(CPU* cpu) { cpu->pc++; cpu->l |= (1 << 3); return 2; }
int SET_1DE(CPU* cpu)
{  
    //printf("Set_1DE Called\n");
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];
    value |= (1 << 3);
    cpu->writeMemory(address, value);
    return 4;
}
int SET_1DF(CPU* cpu) { cpu->pc++; cpu->a |= (1 << 3); return 2; }

int SET_1E0(CPU* cpu) { cpu->pc++; cpu->b |= (1 << 4); return 2; }
int SET_1E1(CPU* cpu) { cpu->pc++; cpu->c |= (1 << 4); return 2; }
int SET_1E2(CPU* cpu) { cpu->pc++; cpu->d |= (1 << 4); return 2; }
int SET_1E3(CPU* cpu) { cpu->pc++; cpu->e |= (1 << 4); return 2; }
int SET_1E4(CPU* cpu) { cpu->pc++; cpu->h |= (1 << 4); return 2; }
int SET_1E5(CPU* cpu) { cpu->pc++; cpu->l |= (1 << 4); return 2; }
int SET_1E6(CPU* cpu)
{  
    //printf("Set_1E6 Called\n");
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    //printf("SET 1E6: HL = %04X, LCDC before = %02X\n", address, cpu->ram[0xFF40]);
    uint8_t value = cpu->ram[address];
    value |= (1 << 4);
    cpu->writeMemory(address, value);
    //printf("SET 1E6: LCDC after = %02X\n", cpu->ram[0xFF40]);
    return 4;
}
int SET_1E7(CPU* cpu) { cpu->pc++; cpu->a |= (1 << 4); return 2; }

int SET_1E8(CPU* cpu) { cpu->pc++; cpu->b |= (1 << 5); return 2; }
int SET_1E9(CPU* cpu) { cpu->pc++; cpu->c |= (1 << 5); return 2; }
int SET_1EA(CPU* cpu) { cpu->pc++; cpu->d |= (1 << 5); return 2; }
int SET_1EB(CPU* cpu) { cpu->pc++; cpu->e |= (1 << 5); return 2; }
int SET_1EC(CPU* cpu) { cpu->pc++; cpu->h |= (1 << 5); return 2; }
int SET_1ED(CPU* cpu) { cpu->pc++; cpu->l |= (1 << 5); return 2; }
int SET_1EE(CPU* cpu)
{ 
    //printf("Set_1EE Called\n");
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];
    value |= (1 << 5);
    cpu->writeMemory(address, value);
    return 4; 
}
int SET_1EF(CPU* cpu) { cpu->pc++; cpu->a |= (1 << 5); return 2; }

int SET_1F0(CPU* cpu) { cpu->pc++; cpu->b |= (1 << 6); return 2; }
int SET_1F1(CPU* cpu) { cpu->pc++; cpu->c |= (1 << 6); return 2; }
int SET_1F2(CPU* cpu) { cpu->pc++; cpu->d |= (1 << 6); return 2; }
int SET_1F3(CPU* cpu) { cpu->pc++; cpu->e |= (1 << 6); return 2; }
int SET_1F4(CPU* cpu) { cpu->pc++; cpu->h |= (1 << 6); return 2; }
int SET_1F5(CPU* cpu) { cpu->pc++; cpu->l |= (1 << 6); return 2; }
int SET_1F6(CPU* cpu) 
{ 
    //printf("Set_1F6 Called\n");
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];
    value |= (1 << 6);
    cpu->writeMemory(address, value);
    return 4;
}
int SET_1F7(CPU* cpu) { cpu->pc++; cpu->a |= (1 << 6); return 2; }

int SET_1F8(CPU* cpu) { cpu->pc++; cpu->b |= (1 << 7); return 2; }
int SET_1F9(CPU* cpu) { cpu->pc++; cpu->c |= (1 << 7); return 2; }
int SET_1FA(CPU* cpu) { cpu->pc++; cpu->d |= (1 << 7); return 2; }
int SET_1FB(CPU* cpu) { cpu->pc++; cpu->e |= (1 << 7); return 2; }
int SET_1FC(CPU* cpu) { cpu->pc++; cpu->h |= (1 << 7); return 2; }
int SET_1FD(CPU* cpu) { cpu->pc++; cpu->l |= (1 << 7); return 2; }
int SET_1FE(CPU* cpu) 
{ 
    //printf("Set_1FE Called\n");
    cpu->pc++;
    uint16_t address = (cpu->h << 8) | cpu->l;
    uint8_t value = cpu->ram[address];
    value |= (1 << 7);
    cpu->writeMemory(address, value);
    return 4;
}
int SET_1FF(CPU* cpu) { cpu->pc++; cpu->a |= (1 << 7); return 2; }

int decode_CB_instruction(byte instr, CPU* cpu)
{
    switch (instr)
    {
    case 0x00:
        return RLC_100(cpu);
        break;
    case 0x01:
        return RLC_101(cpu);
        break;
    case 0x02:
        return RLC_102(cpu);
        break;
    case 0x03:
        return RLC_103(cpu);
        break;
    case 0x04:
        return RLC_104(cpu);
        break;
    case 0x05:
        return RLC_105(cpu);
        break;
    case 0x06:
        return RL_106(cpu);
    case 0x07:
        return RLC_107(cpu);
        break;
    case 0x08:
        return RRC_108(cpu);
        break;
    case 0x09:
        return RRC_109(cpu);
        break;
    case 0x0A:
        return RRC_10A(cpu);
        break;
    case 0x0B:
        return RRC_10B(cpu);
        break;
    case 0x0C:
        return RRC_10C(cpu);
        break;
    case 0x0D:
        return RRC_10D(cpu);
        break;
    case 0x0E:
        return RRC_10E(cpu);
        break;
    case 0x0F:
        return RRC_10F(cpu);
        break;
    case 0x10:
        return RL_110(cpu);
        break;
    case 0x11:
        return RL_111(cpu);
        break;
    case 0x12:
        return RL_112(cpu);
        break;
    case 0x13:
        return RL_113(cpu);
        break;
    case 0x14:
        return RL_114(cpu);
        break;
    case 0x15:
        return RL_115(cpu);
        break;
    case 0x16:
        return RL_116(cpu);
        break;
    case 0x17:
        return RL_117(cpu);
        break;
    case 0x18:
        return RR_118(cpu);
        break;
    case 0x19:
        return RR_119(cpu);
        break;
    case 0x1A:
        return RR_11A(cpu);
        break;
    case 0x1B:
        return RR_11B(cpu);
        break;
    case 0x1C:
        return RR_11C(cpu);
        break;
    case 0x1D:
        return RR_11D(cpu);
        break;
    case 0x1E:
        return RR_11E(cpu);
        break;
    case 0x1F:
        return RR_11F(cpu);
        break;
    case 0x20:
        return SLA_120(cpu);
        break;
    case 0x21:
        return SLA_121(cpu);
        break;
    case 0x22:
        return SLA_122(cpu);
        break;
    case 0x23:
        return SLA_123(cpu);
        break;
    case 0x24:
        return SLA_124(cpu);
        break;
    case 0x25:
        return SLA_125(cpu);
        break;
    case 0x26:
        return SLA_126(cpu);
        break;
    case 0x27:
        return SLA_127(cpu);
        break;
    case 0x28:
        return SRA_128(cpu);
        break;
    case 0x29:
        return SRA_129(cpu);
        break;
    case 0x2A:
        return SRA_12A(cpu);
        break;
    case 0x2B:
        return SRA_12B(cpu);
        break;
    case 0x2C:
        return SRA_12C(cpu);
        break;
    case 0x2D:
        return SRA_12D(cpu);
        break;
    case 0x2E:
        return SRA_12E(cpu);
        break;
    case 0x2F:
        return SRA_12F(cpu);
        break;
    case 0x30:
        return SWAP_130(cpu);
        break;
    case 0x31:
        return SWAP_131(cpu);
        break;
    case 0x32:
        return SWAP_132(cpu);
        break;
    case 0x33:
        return SWAP_133(cpu);
        break;
    case 0x34:
        return SWAP_134(cpu);
        break;
    case 0x35:
        return SWAP_135(cpu);
        break;
    case 0x36:
        return SWAP_136(cpu);
        break;
    case 0x37:
        return SWAP_137(cpu);
        break;
    case 0x38:
        return SRL_138(cpu);
        break;
    case 0x39:
        return SRL_139(cpu);
        break;
    case 0x3A:
        return SRL_13A(cpu);
        break;
    case 0x3B:
        return SRL_13B(cpu);
        break;
    case 0x3C:
        return SRL_13C(cpu);
        break;
    case 0x3D:
        return SRL_13D(cpu);
        break;
    case 0x3E:
        return SRL_13E(cpu);
        break;
    case 0x3F:
        return SRL_13F(cpu);
        break;
    case 0x40: return BIT_140(cpu); break;
    case 0x41: return BIT_141(cpu); break;
    case 0x42: return BIT_142(cpu); break;
    case 0x43: return BIT_143(cpu); break;
    case 0x44: return BIT_144(cpu); break;
    case 0x45: return BIT_145(cpu); break;
    case 0x46: return BIT_146(cpu); break;
    case 0x47: return BIT_147(cpu); break;
    case 0x48: return BIT_148(cpu); break;
    case 0x49: return BIT_149(cpu); break;
    case 0x4A: return BIT_14A(cpu); break;
    case 0x4B: return BIT_14B(cpu); break;
    case 0x4C: return BIT_14C(cpu); break;
    case 0x4D: return BIT_14D(cpu); break;
    case 0x4E: return BIT_14E(cpu); break;
    case 0x4F: return BIT_14F(cpu); break;
    case 0x50: return BIT_150(cpu); break;
    case 0x51: return BIT_151(cpu); break;
    case 0x52: return BIT_152(cpu); break;
    case 0x53: return BIT_153(cpu); break;
    case 0x54: return BIT_154(cpu); break;
    case 0x55: return BIT_155(cpu); break;
    case 0x56: return BIT_156(cpu); break;
    case 0x57: return BIT_157(cpu); break;
    case 0x58: return BIT_158(cpu); break;
    case 0x59: return BIT_159(cpu); break;
    case 0x5A: return BIT_15A(cpu); break;
    case 0x5B: return BIT_15B(cpu); break;
    case 0x5C: return BIT_15C(cpu); break;
    case 0x5D: return BIT_15D(cpu); break;
    case 0x5E: return BIT_15E(cpu); break;
    case 0x5F: return BIT_15F(cpu); break;
    case 0x60: return BIT_160(cpu); break;
    case 0x61: return BIT_161(cpu); break;
    case 0x62: return BIT_162(cpu); break;
    case 0x63: return BIT_163(cpu); break;
    case 0x64: return BIT_164(cpu); break;
    case 0x65: return BIT_165(cpu); break;
    case 0x66: return BIT_166(cpu); break;
    case 0x67: return BIT_167(cpu); break;
    case 0x68: return BIT_168(cpu); break;
    case 0x69: return BIT_169(cpu); break;
    case 0x6A: return BIT_16A(cpu); break;
    case 0x6B: return BIT_16B(cpu); break;
    case 0x6C: return BIT_16C(cpu); break;
    case 0x6D: return BIT_16D(cpu); break;
    case 0x6E: return BIT_16E(cpu); break;
    case 0x6F: return BIT_16F(cpu); break;
    case 0x70: return BIT_170(cpu); break;
    case 0x71: return BIT_171(cpu); break;
    case 0x72: return BIT_172(cpu); break;
    case 0x73: return BIT_173(cpu); break;
    case 0x74: return BIT_174(cpu); break;
    case 0x75: return BIT_175(cpu); break;
    case 0x76: return BIT_176(cpu); break;
    case 0x77: return BIT_177(cpu); break;
    case 0x78: return BIT_178(cpu); break;
    case 0x79: return BIT_179(cpu); break;
    case 0x7A: return BIT_17A(cpu); break;
    case 0x7B: return BIT_17B(cpu); break;
    case 0x7C: return BIT_17C(cpu); break;
    case 0x7D: return BIT_17D(cpu); break;
    case 0x7E: return BIT_17E(cpu); break;
    case 0x7F: return BIT_17F(cpu); break;
    case 0x80: return RES_180(cpu); break;
    case 0x81: return RES_181(cpu); break;
    case 0x82: return RES_182(cpu); break;
    case 0x83: return RES_183(cpu); break;
    case 0x84: return RES_184(cpu); break;
    case 0x85: return RES_185(cpu); break;
    case 0x86: return RES_186(cpu); break;
    case 0x87: return RES_187(cpu); break;
    case 0x88: return RES_188(cpu); break;
    case 0x89: return RES_189(cpu); break;
    case 0x8A: return RES_18A(cpu); break;
    case 0x8B: return RES_18B(cpu); break;
    case 0x8C: return RES_18C(cpu); break;
    case 0x8D: return RES_18D(cpu); break;
    case 0x8E: return RES_18E(cpu); break;
    case 0x8F: return RES_18F(cpu); break;
    case 0x90: return RES_190(cpu); break;
    case 0x91: return RES_191(cpu); break;
    case 0x92: return RES_192(cpu); break;
    case 0x93: return RES_193(cpu); break;
    case 0x94: return RES_194(cpu); break;
    case 0x95: return RES_195(cpu); break;
    case 0x96: return RES_196(cpu); break;
    case 0x97: return RES_197(cpu); break;
    case 0x98: return RES_198(cpu); break;
    case 0x99: return RES_199(cpu); break;
    case 0x9A: return RES_19A(cpu); break;
    case 0x9B: return RES_19B(cpu); break;
    case 0x9C: return RES_19C(cpu); break;
    case 0x9D: return RES_19D(cpu); break;
    case 0x9E: return RES_19E(cpu); break;
    case 0x9F: return RES_19F(cpu); break;
    case 0xA0: return RES_1A0(cpu); break;
    case 0xA1: return RES_1A1(cpu); break;
    case 0xA2: return RES_1A2(cpu); break;
    case 0xA3: return RES_1A3(cpu); break;
    case 0xA4: return RES_1A4(cpu); break;
    case 0xA5: return RES_1A5(cpu); break;
    case 0xA6: return RES_1A6(cpu); break;
    case 0xA7: return RES_1A7(cpu); break;
    case 0xA8: return RES_1A8(cpu); break;
    case 0xA9: return RES_1A9(cpu); break;
    case 0xAA: return RES_1AA(cpu); break;
    case 0xAB: return RES_1AB(cpu); break;
    case 0xAC: return RES_1AC(cpu); break;
    case 0xAD: return RES_1AD(cpu); break;
    case 0xAE: return RES_1AE(cpu); break;
    case 0xAF: return RES_1AF(cpu); break;
    case 0xB0: return RES_1B0(cpu); break;
    case 0xB1: return RES_1B1(cpu); break;
    case 0xB2: return RES_1B2(cpu); break;
    case 0xB3: return RES_1B3(cpu); break;
    case 0xB4: return RES_1B4(cpu); break;
    case 0xB5: return RES_1B5(cpu); break;
    case 0xB6: return RES_1B6(cpu); break;
    case 0xB7: return RES_1B7(cpu); break;
    case 0xB8: return RES_1B8(cpu); break;
    case 0xB9: return RES_1B9(cpu); break;
    case 0xBA: return RES_1BA(cpu); break;
    case 0xBB: return RES_1BB(cpu); break;
    case 0xBC: return RES_1BC(cpu); break;
    case 0xBD: return RES_1BD(cpu); break;
    case 0xBE: return RES_1BE(cpu); break;
    case 0xBF: return RES_1BF(cpu); break;
    case 0xC0: return SET_1C0(cpu); break;
    case 0xC1: return SET_1C1(cpu); break;
    case 0xC2: return SET_1C2(cpu); break;
    case 0xC3: return SET_1C3(cpu); break;
    case 0xC4: return SET_1C4(cpu); break;
    case 0xC5: return SET_1C5(cpu); break;
    case 0xC6: return SET_1C6(cpu); break;
    case 0xC7: return SET_1C7(cpu); break;
    case 0xC8: return SET_1C8(cpu); break;
    case 0xC9: return SET_1C9(cpu); break;
    case 0xCA: return SET_1CA(cpu); break;
    case 0xCB: return SET_1CB(cpu); break;
    case 0xCC: return SET_1CC(cpu); break;
    case 0xCD: return SET_1CD(cpu); break;
    case 0xCE: return SET_1CE(cpu); break;
    case 0xCF: return SET_1CF(cpu); break;
    case 0xD0: return SET_1D0(cpu); break;
    case 0xD1: return SET_1D1(cpu); break;
    case 0xD2: return SET_1D2(cpu); break;
    case 0xD3: return SET_1D3(cpu); break;
    case 0xD4: return SET_1D4(cpu); break;
    case 0xD5: return SET_1D5(cpu); break;
    case 0xD6: return SET_1D6(cpu); break;
    case 0xD7: return SET_1D7(cpu); break;
    case 0xD8: return SET_1D8(cpu); break;
    case 0xD9: return SET_1D9(cpu); break;
    case 0xDA: return SET_1DA(cpu); break;
    case 0xDB: return SET_1DB(cpu); break;
    case 0xDC: return SET_1DC(cpu); break;
    case 0xDD: return SET_1DD(cpu); break;
    case 0xDE: return SET_1DE(cpu); break;
    case 0xDF: return SET_1DF(cpu); break;
    case 0xE0: return SET_1E0(cpu); break;
    case 0xE1: return SET_1E1(cpu); break;
    case 0xE2: return SET_1E2(cpu); break;
    case 0xE3: return SET_1E3(cpu); break;
    case 0xE4: return SET_1E4(cpu); break;
    case 0xE5: return SET_1E5(cpu); break;
    case 0xE6: return SET_1E6(cpu); break;
    case 0xE7: return SET_1E7(cpu); break;
    case 0xE8: return SET_1E8(cpu); break;
    case 0xE9: return SET_1E9(cpu); break;
    case 0xEA: return SET_1EA(cpu); break;
    case 0xEB: return SET_1EB(cpu); break;
    case 0xEC: return SET_1EC(cpu); break;
    case 0xED: return SET_1ED(cpu); break;
    case 0xEE: return SET_1EE(cpu); break;
    case 0xEF: return SET_1EF(cpu); break;
    case 0xF0: return SET_1F0(cpu); break;
    case 0xF1: return SET_1F1(cpu); break;
    case 0xF2: return SET_1F2(cpu); break;
    case 0xF3: return SET_1F3(cpu); break;
    case 0xF4: return SET_1F4(cpu); break;
    case 0xF5: return SET_1F5(cpu); break;
    case 0xF6: return SET_1F6(cpu); break;
    case 0xF7: return SET_1F7(cpu); break;
    case 0xF8: return SET_1F8(cpu); break;
    case 0xF9: return SET_1F9(cpu); break;
    case 0xFA: return SET_1FA(cpu); break;
    case 0xFB: return SET_1FB(cpu); break;
    case 0xFC: return SET_1FC(cpu); break;
    case 0xFD: return SET_1FD(cpu); break;
    case 0xFE: return SET_1FE(cpu); break;
    case 0xFF: return SET_1FF(cpu); break;
    default:
        cpu->pc++;
        return 0;
        break;
    }
}

int ret = 99;

int decode_instruction(byte instr, CPU* cpu)
{
    
    switch (instr)
    {
    case 0x00:
        ret = NOP_00(cpu);
        break;
    case 0x01:
        ret = LD_01(cpu);
        break;
    case 0x02:
        ret = LD_02(cpu);
        break;
    case 0x03:
        ret = INC_03(cpu);
        break;
    case 0x04:
        ret = INC_04(cpu);
        break;
    case 0x05:
        ret = DEC_05(cpu);
        break;
    case 0x06:
        ret = LD_06(cpu);
        break;
    case 0x07:
        ret = RLCA_07(cpu);
        break;
    case 0x08:
        ret = LD_08(cpu);
        break;
    case 0x09:
        ret = ADD_09(cpu);
        break;
    case 0x0A:
        ret = LD_0A(cpu);
        break;
    case 0x0B:
        ret = DEC_0B(cpu);
        break;
    case 0x0C:
        ret = INC_0C(cpu);
        break;
    case 0x0D:
        ret = DEC_0D(cpu);
        break;
    case 0x0E:
        ret = LD_0E(cpu);
        break;
    case 0x0F:
        ret = RRCA_0F(cpu);
        break;
        //////////
    case 0x10:
        ret = STOP_10(cpu);
        break;
    case 0x11:
        ret = LD_11(cpu);
        break;
    case 0x12:
        ret = LD_12(cpu);
        break;
    case 0x13:
        ret = INC_13(cpu);
        break;
    case 0x14:
        ret = INC_14(cpu);
        break;
    case 0x15:
        ret = DEC_15(cpu);
        break;
    case 0x16:
        ret = LD_16(cpu);
        break;
    case 0x17:
        ret = RLA_17(cpu);
        break;
    case 0x18:
        ret = JR_18(cpu);
        break;
    case 0x19:
        ret = ADD_19(cpu);
        break;
    case 0x1A:
        ret = LD_1A(cpu);
        break;
    case 0x1B:
        ret = DEC_1B(cpu);
        break;
    case 0x1C:
        ret = INC_1C(cpu);
        break;
    case 0x1D:
        ret = DEC_1D(cpu);
        break;
    case 0x1E:
        ret = LD_1E(cpu);
        break;
    case 0x1F:
        ret = RRA_1F(cpu);
        break;
        //////////
    case 0x20:
        ret = JR_20(cpu);
        break;
    case 0x21:
        ret = LD_21(cpu);
        break;
    case 0x22:
        ret = LD_22(cpu);
        break;
    case 0x23:
        ret = INC_23(cpu);
        break;
    case 0x24:
        ret = INC_24(cpu);
        break;
    case 0x25:
        ret = DEC_25(cpu);
        break;
    case 0x26:
        ret = LD_26(cpu);
        break;
    case 0x27:
        ret = DAA_27(cpu);
        break;
    case 0x28:
        ret = JR_28(cpu);
        break;
    case 0x29:
        ret = ADD_29(cpu);
        break;
    case 0x2A:
        ret = LD_2A(cpu);
        break;
    case 0x2B:
        ret = DEC_2B(cpu);
        break;
    case 0x2C:
        ret = INC_2C(cpu);
        break;
    case 0x2D:
        ret = DEC_2D(cpu);
        break;
    case 0x2E:
        ret = LD_2E(cpu);
        break;
    case 0x2F:
        ret = CPL_2F(cpu);
        break;
        //////////
    case 0x30:
        ret = JR_30(cpu);
        break;
    case 0x31:
        ret = LD_31(cpu);
        break;
    case 0x32:
        ret = LD_32(cpu);
        break;
    case 0x33:
        ret = INC_33(cpu);
        break;
    case 0x34:
        ret = INC_34(cpu);
        break;
    case 0x35:
        ret = DEC_35(cpu);
        break;
    case 0x36:
        ret = LD_36(cpu);
        break;
    case 0x37:
        ret = SCF_37(cpu);
        break;
    case 0x38:
        ret = JR_38(cpu);
        break;
    case 0x39:
        ret = ADD_39(cpu);
        break;
    case 0x3A:
        ret = LD_3A(cpu);
        break;
    case 0x3B:
        ret = DEC_3B(cpu);
        break;
    case 0x3C:
        ret = INC_3C(cpu);
        break;
    case 0x3D:
        ret = DEC_3D(cpu);
        break;
    case 0x3E:
        ret = LD_3E(cpu);
        break;
    case 0x3F:
        ret = CCF_3F(cpu);
        break;
        //////////
    case 0x40:
        ret = LD_40(cpu);
        break;
    case 0x41:
        ret = LD_41(cpu);
        break;
    case 0x42:
        ret = LD_42(cpu);
        break;
    case 0x43:
        ret = LD_43(cpu);
        break;
    case 0x44:
        ret = LD_44(cpu);
        break;
    case 0x45:
        ret = LD_45(cpu);
        break;
    case 0x46:
        ret = LD_46(cpu);
        break;
    case 0x47:
        ret = LD_47(cpu);
        break;
    case 0x48:
        ret = LD_48(cpu);
        break;
    case 0x49:
        ret = LD_49(cpu);
        break;
    case 0x4A:
        ret = LD_4A(cpu);
        break;
    case 0x4B:
        ret = LD_4B(cpu);
        break;
    case 0x4C:
        ret = LD_4C(cpu);
        break;
    case 0x4D:
        ret = LD_4D(cpu);
        break;
    case 0x4E:
        ret = LD_4E(cpu);
        break;
    case 0x4F:
        ret = LD_4F(cpu);
        break;
        //////////
    case 0x50:
        ret = LD_50(cpu);
        break;
    case 0x51:
        ret = LD_51(cpu);
        break;
    case 0x52:
        ret = LD_52(cpu);
        break;
    case 0x53:
        ret = LD_53(cpu);
        break;
    case 0x54:
        ret = LD_54(cpu);
        break;
    case 0x55:
        ret = LD_55(cpu);
        break;
    case 0x56:
        ret = LD_56(cpu);
        break;
    case 0x57:
        ret = LD_57(cpu);
        break;
    case 0x58:
        ret = LD_58(cpu);
        break;
    case 0x59:
        ret = LD_59(cpu);
        break;
    case 0x5A:
        ret = LD_5A(cpu);
        break;
    case 0x5B:
        ret = LD_5B(cpu);
        break;
    case 0x5C:
        ret = LD_5C(cpu);
        break;
    case 0x5D:
        ret = LD_5D(cpu);
        break;
    case 0x5E:
        ret = LD_5E(cpu);
        break;
    case 0x5F:
        ret = LD_5F(cpu);
        break;
        //////////
    case 0x60:
        ret = LD_60(cpu);
        break;
    case 0x61:
        ret = LD_61(cpu);
        break;
    case 0x62:
        ret = LD_62(cpu);
        break;
    case 0x63:
        ret = LD_63(cpu);
        break;
    case 0x64:
        ret = LD_64(cpu);
        break;
    case 0x65:
        ret = LD_65(cpu);
        break;
    case 0x66:
        ret = LD_66(cpu);
        break;
    case 0x67:
        ret = LD_67(cpu);
        break;
    case 0x68:
        ret = LD_68(cpu);
        break;
    case 0x69:
        ret = LD_69(cpu);
        break;
    case 0x6A:
        ret = LD_6A(cpu);
        break;
    case 0x6B:
        ret = LD_6B(cpu);
        break;
    case 0x6C:
        ret = LD_6C(cpu);
        break;
    case 0x6D:
        ret = LD_6D(cpu);
        break;
    case 0x6E:
        ret = LD_6E(cpu);
        break;
    case 0x6F:
        ret = LD_6F(cpu);
        break;
        //////////
    case 0x70:
        ret = LD_70(cpu);
        break;
    case 0x71:
        ret = LD_71(cpu);
        break;
    case 0X72:
        ret = LD_72(cpu);
        break;
    case 0x73:
        ret = LD_73(cpu);
        break;
    case 0x74:
        ret = LD_74(cpu);
        break;
    case 0x75:
        ret = LD_75(cpu);
        break;
    case 0x76:
        ret = LD_76(cpu);
        break;
    case 0x77:
        ret = LD_77(cpu);
        break;
    case 0x78:
        ret = LD_78(cpu);
        break;
    case 0x79:
        ret = LD_79(cpu);
        break;
    case 0x7A:
        ret = LD_7A(cpu);
        break;
    case 0x7B:
        ret = LD_7B(cpu);
        break;
    case 0x7C:
        ret = LD_7C(cpu);
        break;
    case 0x7D:
        ret = LD_7D(cpu);
        break;
    case 0x7E:
        ret = LD_7E(cpu);
        break;
    case 0x7F:
        ret = LD_7F(cpu);
        break;
        //////////
    case 0x80:
        ret = ADD_80(cpu);
        break;
    case 0x81:
        ret = ADD_81(cpu);
        break;
    case 0X82:
        ret = ADD_82(cpu);
        break;
    case 0x83:
        ret = ADD_83(cpu);
        break;
    case 0x84:
        ret = ADD_84(cpu);
        break;
    case 0x85:
        ret = ADD_85(cpu);
        break;
    case 0x86:
        ret = ADD_86(cpu);
        break;
    case 0x87:
        ret = ADD_87(cpu);
        break;
    case 0x88:
        ret = ADC_88(cpu);
        break;
    case 0x89:
        ret = ADC_89(cpu);
        break;
    case 0x8A:
        ret = ADC_8A(cpu);
        break;
    case 0x8B:
        ret = ADC_8B(cpu);
        break;
    case 0x8C:
        ret = ADC_8C(cpu);
        break;
    case 0x8D:
        ret = ADC_8D(cpu);
        break;
    case 0x8E:
        ret = ADC_8E(cpu);
        break;
    case 0x8F:
        ret = ADC_8F(cpu);
        break;
        //////////
    case 0x90:
        ret = SUB_90(cpu);
        break;
    case 0x91:
        ret = SUB_91(cpu);
        break;
    case 0X92:
        ret = SUB_92(cpu);
        break;
    case 0x93:
        ret = SUB_93(cpu);
        break;
    case 0x94:
        ret = SUB_94(cpu);
        break;
    case 0x95:
        ret = SUB_95(cpu);
        break;
    case 0x96:
        ret = SUB_96(cpu);
        break;
    case 0x97:
        ret = SUB_97(cpu);
        break;
    case 0x98:
        ret = SBC_98(cpu);
        break;
    case 0x99:
        ret = SBC_99(cpu);
        break;
    case 0x9A:
        ret = SBC_9A(cpu);
        break;
    case 0x9B:
        ret = SBC_9B(cpu);
        break;
    case 0x9C:
        ret = SBC_9C(cpu);
        break;
    case 0x9D:
        ret = SBC_9D(cpu);
        break;
    case 0x9E:
        ret = SBC_9E(cpu);
        break;
    case 0x9F:
        ret = SBC_9F(cpu);
        break;
        //////////
    case 0xA0:
        ret = AND_A0(cpu);
        break;
    case 0xA1:
        ret = AND_A1(cpu);
        break;
    case 0XA2:
        ret = AND_A2(cpu);
        break;
    case 0xA3:
        ret = AND_A3(cpu);
        break;
    case 0xA4:
        ret = AND_A4(cpu);
        break;
    case 0xA5:
        ret = AND_A5(cpu);
        break;
    case 0xA6:
        ret = AND_A6(cpu);
        break;
    case 0xA7:
        ret = AND_A7(cpu);
        break;
    case 0xA8:
        ret = XOR_A8(cpu);
        break;
    case 0xA9:
        ret = XOR_A9(cpu);
        break;
    case 0xAA:
        ret = XOR_AA(cpu);
        break;
    case 0xAB:
        ret = XOR_AB(cpu);
        break;
    case 0xAC:
        ret = XOR_AC(cpu);
        break;
    case 0xAD:
        ret = XOR_AD(cpu);
        break;
    case 0xAE:
        ret = XOR_AE(cpu);
        break;
    case 0xAF:
        ret = XOR_AF(cpu);
        break;
        //////////
    case 0xB0:
        ret = OR_B0(cpu);
        break;
    case 0xB1:
        ret = OR_B1(cpu);
        break;
    case 0XB2:
        ret = OR_B2(cpu);
        break;
    case 0xB3:
        ret = OR_B3(cpu);
        break;
    case 0xB4:
        ret = OR_B4(cpu);
        break;
    case 0xB5:
        ret = OR_B5(cpu);
        break;
    case 0xB6:
        ret = OR_B6(cpu);
        break;
    case 0xB7:
        ret = OR_B7(cpu);
        break;
    case 0xB8:
        ret = CP_B8(cpu);
        break;
    case 0xB9:
        ret = CP_B9(cpu);
        break;
    case 0xBA:
        ret = CP_BA(cpu);
        break;
    case 0xBB:
        ret = CP_BB(cpu);
        break;
    case 0xBC:
        ret = CP_BC(cpu);
        break;
    case 0xBD:
        ret = CP_BD(cpu);
        break;
    case 0xBE:
        ret = CP_BE(cpu);
        break;
    case 0xBF:
        ret = CP_BF(cpu);
        break;
        //////////
    case 0xC0:
        ret = RET_C0(cpu);
        break;
    case 0xC1:
        ret = POP_C1(cpu);
        break;
    case 0XC2:
        ret = JP_C2(cpu);
        break;
    case 0xC3:
        ret = JP_C3(cpu);
        break;
    case 0xC4:
        ret = CALL_C4(cpu);
        break;
    case 0xC5:
        ret = PUSH_C5(cpu);
        break;
    case 0xC6:
        ret = ADD_C6(cpu);
        break;
    case 0xC7:
        ret = RST_C7(cpu);
        break;
    case 0xC8:
        ret = RET_C8(cpu);
        break;
    case 0xC9:
        ret = RET_C9(cpu);
        break;
    case 0xCA:
        ret = JP_CA(cpu);
        break;
    case 0xCB:
        //ret = decode_instruction(++cpu->pc, cpu);
        //ret = rl.l;
        cpu->pc++;
        ret = decode_CB_instruction(cpu->ram[cpu->pc], cpu) + 1;
        //ret = 1;
        break;
    case 0xCC:
        ret = CALL_CC(cpu);
        break;
    case 0xCD:
        ret = CALL_CD(cpu);
        break;
    case 0xCE:
        ret = ADC_CE(cpu);
        break;
    case 0xCF:
        ret = RST_CF(cpu);
        break;
        //////////
    case 0xD0:
        ret = RET_D0(cpu);
        break;
    case 0xD1:
        ret = POP_D1(cpu);
        break;
    case 0XD2:
        ret = JP_D2(cpu);
        break;
    case 0xD3:
        ret = ILLEGAL_D3(cpu);
        break;
    case 0xD4:
        ret = CALL_D4(cpu);
        break;
    case 0xD5:
        ret = PUSH_D5(cpu);
        break;
    case 0xD6:
        ret = SUB_D6(cpu);
        break;
    case 0xD7:
        ret = RST_D7(cpu);
        break;
    case 0xD8:
        ret = RET_D8(cpu);
        break;
    case 0xD9:
        ret = RETI_D9(cpu);
        break;
    case 0xDA:
        ret = JP_DA(cpu);
        break;
    case 0xDB:
        ret = ILLEGAL_DB(cpu);
        break;
    case 0xDC:
        ret = CALL_DC(cpu);
        break;
    case 0xDD:
        ret = ILLEGAL_DD(cpu);
        break;
    case 0xDE:
        ret = SBC_DE(cpu);
        break;
    case 0xDF:
        ret = RST_DF(cpu);
        break;
        //////////
    case 0xE0:
        ret = LDH_E0(cpu);
        break;
    case 0xE1:
        ret = POP_E1(cpu);
        break;
    case 0XE2:
        ret = LD_E2(cpu);
        break;
    case 0xE3:
        ret = ILLEGAL_E3(cpu);
        break;
    case 0xE4:
        ret = ILLEGAL_E4(cpu);
        break;
    case 0xE5:
        ret = PUSH_E5(cpu);
        break;
    case 0xE6:
        ret = AND_E6(cpu);
        break;
    case 0xE7:
        ret = RST_E7(cpu);
        break;
    case 0xE8:
        ret = ADD_E8(cpu);
        break;
    case 0xE9:
        ret = JP_E9(cpu);
        break;
    case 0xEA:
        ret = LD_EA(cpu);
        break;
    case 0xEB:
        ret = ILLEGAL_EB(cpu);
        break;
    case 0xEC:
        ret = ILLEGAL_EC(cpu);
        break;
    case 0xED:
        ret = ILLEGAL_ED(cpu);
        break;
    case 0xEE:
        ret = XOR_EE(cpu);
        break;
    case 0xEF:
        ret = RST_EF(cpu);
        break;
        //////////
    case 0xF0:
        ret = LDH_F0(cpu);
        break;
    case 0xF1:
        ret = POP_F1(cpu);
        break;
    case 0XF2:
        ret = LD_F2(cpu);
        break;
    case 0xF3:
        ret = DI_F3(cpu);
        break;
    case 0xF4:
        ret = ILLEGAL_F4(cpu);
        break;
    case 0xF5:
        ret = PUSH_F5(cpu);
        break;
    case 0xF6:
        ret = OR_F6(cpu);
        break;
    case 0xF7:
        ret = RST_F7(cpu);
        break;
    case 0xF8:
        ret = LD_F8(cpu);
        break;
    case 0xF9:
        ret = LD_F9(cpu);
        break;
    case 0xFA:
        ret = LD_FA(cpu);
        break;
    case 0xFB:
        ret = EI_FB(cpu);
        break;
    case 0xFC:
        ret = ILLEGAL_FC(cpu);
        break;
    case 0xFD:
        ret = ILLEGAL_FD(cpu);
        break;
    case 0xFE:
        ret = CP_FE(cpu);
        break;
    case 0xFF:
        ret = RST_FF(cpu);
        break;
    default:
        cpu->pc++;
        ret = 99;
        break;
    }
    //ret_pair rp;
    //rp.r = ret;
    //rp.l = ly;
    return ret;
}

#endif