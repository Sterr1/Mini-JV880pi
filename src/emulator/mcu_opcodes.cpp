/*
 * Copyright (C) 2021, 2024 nukeykt
 *
 *  Redistribution and use of this code or any derivative works are permitted
 *  provided that the following conditions are met:
 *
 *   - Redistributions may not be sold, nor may they be used in a commercial
 *     product or activity.
 *
 *   - Redistributions that are modified from the original source must include the
 *     complete source code, including the source code for all components used by a
 *     binary built from the modified sources. However, as a special exception, the
 *     source code distributed need not include anything that is normally distributed
 *     (in either source or binary form) with the major components (compiler, kernel,
 *     and so on) of the operating system on which the executable runs, unless that
 *     component itself accompanies the executable.
 *
 *   - Redistributions must reproduce the above copyright notice, this list of
 *     conditions and the following disclaimer in the documentation and/or other
 *     materials provided with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include "mcu.h"
#include "mcu_opcodes.h"

static const char* s_bcc_names[16] = {
    "BRA","BRN","BHI","BLS","BCC","BCS","BNE","BEQ",
    "BVC","BVS","BPL","BMI","BGE","BLT","BGT","BLE"
};

int32_t MCU_SUB_Common(MCU *mcu, int32_t t1, int32_t t2, int32_t c_bit, uint32_t siz)
{
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
                "SUB t1=%x t2=%x c_bit=%x siz=%x", t1, t2, c_bit, siz);
    int32_t st1, st2;
    int32_t N, Z, C, V = 0;
    if (siz)
    {
        st1 = (int16_t)t1;
        st2 = (int16_t)t2;
        t1 = (uint16_t)t1;
        t2 = (uint16_t)t2;
        t1 -= t2;
        t1 -= c_bit;
        C = (t1 >> 16) & 1;
        t1 &= 0xffff;
        N = (t1 & 0x8000) != 0;
        Z = t1 == 0;
        st1 -= st2;
        st1 -= c_bit;
        if (st1 < INT16_MIN || st1 > INT16_MAX)
            V = 1;
    }
    else
    {
        st1 = (int8_t)t1;
        st2 = (int8_t)t2;
        t1 = (uint8_t)t1;
        t2 = (uint8_t)t2;
        t1 -= t2;
        t1 -= c_bit;
        C = (t1 >> 8) & 1;
        t1 &= 0xff;
        N = (t1 & 0x80) != 0;
        Z = t1 == 0;
        st1 -= st2;
        st1 -= c_bit;
        if (st1 < INT8_MIN || st1 > INT8_MAX)
            V = 1;
    }
    mcu->MCU_SetStatus(N, STATUS_N);
    mcu->MCU_SetStatus(Z, STATUS_Z);
    mcu->MCU_SetStatus(C, STATUS_C);
    mcu->MCU_SetStatus(V, STATUS_V);
    return t1;
}

int32_t MCU_ADD_Common(MCU *mcu, int32_t t1, int32_t t2, int32_t c_bit, uint32_t siz)
{
    int32_t st1, st2;
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
                "ADD t1=%x t2=%x c_bit=%x siz=%x", t1, t2, c_bit, siz);
    int32_t N, Z, C, V = 0;
    if (siz)
    {
        st1 = (int16_t)t1;
        st2 = (int16_t)t2;
        t1 = (uint16_t)t1;
        t2 = (uint16_t)t2;
        t1 += t2;
        t1 += c_bit;
        C = (t1 >> 16) & 1;
        t1 &= 0xffff;
        N = (t1 & 0x8000) != 0;
        Z = t1 == 0;
        st1 += st2;
        st1 += c_bit;
        if (st1 < INT16_MIN || st1 > INT16_MAX)
            V = 1;
    }
    else
    {
        st1 = (int8_t)t1;
        st2 = (int8_t)t2;
        t1 = (uint8_t)t1;
        t2 = (uint8_t)t2;
        t1 += t2;
        t1 += c_bit;
        C = (t1 >> 8) & 1;
        t1 &= 0xff;
        N = (t1 & 0x80) != 0;
        Z = t1 == 0;
        st1 += st2;
        st1 += c_bit;
        if (st1 < INT8_MIN || st1 > INT8_MAX)
            V = 1;
    }
    mcu->MCU_SetStatus(N, STATUS_N);
    mcu->MCU_SetStatus(Z, STATUS_Z);
    mcu->MCU_SetStatus(C, STATUS_C);
    mcu->MCU_SetStatus(V, STATUS_V);
    return t1;
}

void MCU_Operand_Nop(MCU *mcu, uint8_t operand)
{
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "NOP");
}

void MCU_Operand_Sleep(MCU *mcu, uint8_t operand)
{
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "SLEEP");
    mcu->mcu.sleep = 1;
}

void MCU_Operand_NotImplemented(MCU *mcu, uint8_t operand)
{
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "NOT_IMPLEMENTED op=%02x", operand);
    mcu->MCU_ErrorTrap();
}

enum {
    GENERAL_DIRECT = 0,
    GENERAL_INDIRECT,
    GENERAL_ABSOLUTE,
    GENERAL_IMMEDIATE
};

enum {
    OPERAND_BYTE = 0,
    OPERAND_WORD
};

enum {
    INCREASE_NONE = 0,
    INCREASE_DECREASE,
    INCREASE_INCREASE
};

void MCU_LDM(MCU *mcu, uint8_t operand)
{
    uint8_t rlist = mcu->MCU_ReadCodeAdvance();
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
        "LDM rlist=%02x  sp=%04x", rlist, mcu->mcu.r[7]);
    int32_t i;
    for (i = 0; i < 8; i++)
    {
        if (rlist & (1 << i))
        {
            uint16_t data = mcu->MCU_PopStack();
            if (i != 7)
                mcu->mcu.r[i] = data;
        }
    }
}

void MCU_STM(MCU *mcu, uint8_t operand)
{
    uint8_t rlist = mcu->MCU_ReadCodeAdvance();
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
        "STM rlist=%02x  sp=%04x", rlist, mcu->mcu.r[7]);
    int32_t i;
    for (i = 7; i >= 0; i--)
    {
        if (rlist & (1 << i))
        {
            uint16_t data = mcu->mcu.r[i];
            if (i == 7)
                data -= 2;
            mcu->MCU_PushStack(data);
        }
    }
}

void MCU_TRAPA(MCU *mcu, uint8_t operand)
{
    uint32_t opcode = mcu->MCU_ReadCodeAdvance();
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
        "TRAPA #%02x", opcode & 0x0f);
    if ((opcode & 0xf0) == 0x10)
        mcu->MCU_Interrupt_TRAPA(opcode & 0x0f);
    else
        mcu->MCU_ErrorTrap();
}

void MCU_LINK(MCU *mcu, uint8_t operand)
{
    if (operand == 0x17)
    {
        int16_t data = (int8_t)mcu->MCU_ReadCodeAdvance();
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
            "LINK #%d  r6=%04x r7=%04x", data, mcu->mcu.r[6], mcu->mcu.r[7]);
        mcu->MCU_PushStack(mcu->mcu.r[6]);
        mcu->mcu.r[6] = mcu->mcu.r[7];
        mcu->mcu.r[7] += data;
    }
    else if (operand == 0x1f)
    {
        uint32_t dataH = mcu->MCU_ReadCodeAdvance();
        uint32_t dataL = mcu->MCU_ReadCodeAdvance();
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 3),
            "LINK.W #%04x  NOT_IMPL", (dataH << 8) | dataL);
        mcu->MCU_ErrorTrap();
    }
    else
        mcu->MCU_ErrorTrap();
}

void MCU_UNLK(MCU *mcu, uint8_t operand)
{
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "UNLK  r6=%04x r7=%04x", mcu->mcu.r[6], mcu->mcu.r[7]);
    mcu->mcu.r[7] = mcu->mcu.r[6];
    mcu->mcu.r[6] = mcu->MCU_PopStack();
}

void MCU_Jump_PJSR(MCU *mcu, uint8_t operand)
{
    uint8_t  page    = mcu->MCU_ReadCodeAdvance();
    uint16_t address = mcu->MCU_ReadCodeAdvance() << 8;
    address |= mcu->MCU_ReadCodeAdvance();
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 4),
        "PJSR %02x:%04x  (from %02x:%04x)",
        page, address, mcu->mcu.cp, mcu->mcu.pc);
    mcu->MCU_PushStack(mcu->mcu.pc);
    mcu->MCU_PushStack(mcu->mcu.cp);
    mcu->mcu.cp = page;
    if (mcu->mcu.cp == 0x27)
        mcu->mcu.cp += 0;
    mcu->mcu.pc = address;
}

void MCU_Jump_JSR(MCU *mcu, uint8_t operand)
{
    uint16_t address = mcu->MCU_ReadCodeAdvance() << 8;
    address |= mcu->MCU_ReadCodeAdvance();
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 3),
        "JSR %04x  (from %02x:%04x)",
        address, mcu->mcu.cp, mcu->mcu.pc);
    mcu->MCU_PushStack(mcu->mcu.pc);
    mcu->mcu.pc = address;
}

void MCU_Jump_RTE(MCU *mcu, uint8_t operand)
{
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "RTE  sp=%04x", mcu->mcu.r[7]);
    mcu->mcu.sr = mcu->MCU_PopStack();
    mcu->mcu.cp = (uint8_t)mcu->MCU_PopStack();
    mcu->mcu.pc = mcu->MCU_PopStack();
    mcu->mcu.ex_ignore = 1;
}

void MCU_Jump_Bcc(MCU *mcu, uint8_t operand)
{
    uint16_t disp;
    uint32_t cond  = operand & 0x0f;
    uint32_t branch = 0;
    uint32_t N, C, Z, V;

    if (operand & 0x10)
    {
        disp  = mcu->MCU_ReadCodeAdvance() << 8;
        disp |= mcu->MCU_ReadCodeAdvance();
    }
    else
        disp = (int8_t)mcu->MCU_ReadCodeAdvance();

    N = (mcu->mcu.sr & STATUS_N) != 0;
    C = (mcu->mcu.sr & STATUS_C) != 0;
    Z = (mcu->mcu.sr & STATUS_Z) != 0;
    V = (mcu->mcu.sr & STATUS_V) != 0;

    switch (cond)
    {
    case 0x0: branch = 1;                   break;
    case 0x1: branch = 0;                   break;
    case 0x2: branch = (C | Z) == 0;        break;
    case 0x3: branch = (C | Z) == 1;        break;
    case 0x4: branch = C == 0;              break;
    case 0x5: branch = C == 1;              break;
    case 0x6: branch = Z == 0;              break;
    case 0x7: branch = Z == 1;              break;
    case 0x8: branch = V == 0;              break;
    case 0x9: branch = V == 1;              break;
    case 0xa: branch = N == 0;              break;
    case 0xb: branch = N == 1;              break;
    case 0xc: branch = (N ^ V) == 0;        break;
    case 0xd: branch = (N ^ V) == 1;        break;
    case 0xe: branch = (Z | (N ^ V)) == 0;  break;
    case 0xf: branch = (Z | (N ^ V)) == 1;  break;
    }

    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - (operand & 0x10 ? 3 : 2)),
        "%s disp=%04x  branch=%d  target=%04x  sr=%04x",
        s_bcc_names[cond], (uint16_t)disp, branch,
        branch ? (uint16_t)(mcu->mcu.pc + disp) : mcu->mcu.pc,
        mcu->mcu.sr);

    if (branch)
        mcu->mcu.pc += disp;
}

void MCU_Jump_RTS(MCU *mcu, uint8_t operand)
{
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "RTS  sp=%04x  ret=%04x",
        mcu->mcu.r[7], mcu->MCU_Read16(mcu->mcu.r[7]));
    mcu->mcu.pc = mcu->MCU_PopStack();
}

void MCU_Jump_RTD(MCU *mcu, uint8_t operand)
{
    int16_t imm = (int8_t)mcu->MCU_ReadCodeAdvance();
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
        "RTD #%d  sp=%04x", imm, mcu->mcu.r[7]);
    mcu->mcu.pc = mcu->MCU_PopStack();
    if (operand == 0x14)
    {
        mcu->mcu.r[7] += imm;
        if (mcu->mcu.r[7] & 1)
            mcu->MCU_ErrorTrap();
    }
    else if (operand == 0x1c)
        mcu->MCU_ErrorTrap();
    else
        mcu->MCU_ErrorTrap();
}

void MCU_Jump_JMP(MCU *mcu, uint8_t operand)
{
    if (operand == 0x11)
    {
        uint8_t opcode   = mcu->MCU_ReadCodeAdvance();
        uint8_t opcode_h = opcode >> 3;
        uint8_t opcode_l = opcode & 0x07;
        if (opcode == 0x19)
        {
            Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
                "PJMP @SP  sp=%04x", mcu->mcu.r[7]);
            mcu->mcu.cp = (uint8_t)mcu->MCU_PopStack();
            mcu->mcu.pc = mcu->MCU_PopStack();
        }
        else if (opcode_h == 0x18)
        {
            Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
                "PJMP R%d:R%d  %02x:%04x",
                opcode_l, opcode_l+1,
                mcu->mcu.r[opcode_l] & 0xff, mcu->mcu.r[opcode_l+1]);
            mcu->mcu.cp = mcu->mcu.r[opcode_l] & 0xff;
            mcu->mcu.pc = mcu->mcu.r[opcode_l + 1];
        }
        else if (opcode_h == 0x19)
        {
            opcode_l &= ~1;
            Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
                "PJSR R%d:R%d  %02x:%04x",
                opcode_l, opcode_l+1,
                mcu->mcu.r[opcode_l] & 0xff, mcu->mcu.r[opcode_l+1]);
            mcu->MCU_PushStack(mcu->mcu.pc);
            mcu->MCU_PushStack(mcu->mcu.cp);
            mcu->mcu.cp = mcu->mcu.r[opcode_l] & 0xff;
            mcu->mcu.pc = mcu->mcu.r[opcode_l + 1];
        }
        else if (opcode_h == 0x1a)
        {
            Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
                "JMP R%d  %04x", opcode_l, mcu->mcu.r[opcode_l]);
            mcu->mcu.pc = mcu->mcu.r[opcode_l];
        }
        else if (opcode_h == 0x1b)
        {
            Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
                "JSR R%d  %04x", opcode_l, mcu->mcu.r[opcode_l]);
            mcu->MCU_PushStack(mcu->mcu.pc);
            mcu->mcu.pc = mcu->mcu.r[opcode_l];
        }
        else if (opcode_h == 0x1c)
        {
            uint8_t disp8 = mcu->MCU_ReadCodeAdvance();
            Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 3),
                "JMP R%d+%02x  %04x",
                opcode_l, disp8, (uint16_t)(mcu->mcu.r[opcode_l] + disp8));
            mcu->mcu.pc = mcu->mcu.r[opcode_l] + disp8;
        }
        else if (opcode_h == 0x1e)
        {
            uint32_t addr  = mcu->MCU_ReadCodeAdvance() << 8;
            addr |= mcu->MCU_ReadCodeAdvance();
            Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 4),
                "JMP R%d+%04x  %04x",
                opcode_l, addr, (uint16_t)(mcu->mcu.r[opcode_l] + addr));
            mcu->mcu.pc = mcu->mcu.r[opcode_l] + addr;
        }
        else
            mcu->MCU_ErrorTrap();
    }
    else if (operand == 0x01)
    {
        uint8_t opcode  = mcu->MCU_ReadCodeAdvance();
        uint8_t reg     = opcode & 0x07;
        opcode >>= 3;
        if (opcode == 0x17)
        {
            uint16_t disp = (int8_t)mcu->MCU_ReadCodeAdvance();
            mcu->mcu.r[reg]--;
            Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 3),
                "DBRA R%d  r%d=%04x  branch=%d  target=%04x",
                reg, reg, mcu->mcu.r[reg],
                mcu->mcu.r[reg] != 0xffff,
                (uint16_t)(mcu->mcu.pc + disp));
            if (mcu->mcu.r[reg] != 0xffff)
                mcu->mcu.pc += disp;
        }
        else
            mcu->MCU_ErrorTrap();
    }
    else if (operand == 0x10)
    {
        uint32_t addr  = mcu->MCU_ReadCodeAdvance() << 8;
        addr |= mcu->MCU_ReadCodeAdvance();
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 3),
            "JMP %04x", addr);
        mcu->mcu.pc = addr;
    }
    else if (operand == 0x06)
    {
        uint8_t opcode  = mcu->MCU_ReadCodeAdvance();
        uint8_t reg     = opcode & 0x07;
        opcode >>= 3;
        if (opcode == 0x17)
        {
            uint16_t disp = (int8_t)mcu->MCU_ReadCodeAdvance();
            uint32_t Z    = (mcu->mcu.sr & STATUS_Z) != 0;
            if (Z)
            {
                mcu->mcu.r[reg]--;
                Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 3),
                    "DBEQ R%d  r%d=%04x  branch=%d  target=%04x",
                    reg, reg, mcu->mcu.r[reg],
                    mcu->mcu.r[reg] != 0xffff,
                    (uint16_t)(mcu->mcu.pc + disp));
                if (mcu->mcu.r[reg] != 0xffff)
                    mcu->mcu.pc += disp;
            }
        }
        else
            mcu->MCU_ErrorTrap();
    }
    else if (operand == 0x07)
    {
        uint8_t opcode  = mcu->MCU_ReadCodeAdvance();
        uint8_t reg     = opcode & 0x07;
        opcode >>= 3;
        if (opcode == 0x17)
        {
            uint16_t disp = (int8_t)mcu->MCU_ReadCodeAdvance();
            uint32_t Z    = (mcu->mcu.sr & STATUS_Z) != 0;
            if (!Z)
            {
                mcu->mcu.r[reg]--;
                Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 3),
                    "DBNE R%d  r%d=%04x  branch=%d  target=%04x",
                    reg, reg, mcu->mcu.r[reg],
                    mcu->mcu.r[reg] != 0xffff,
                    (uint16_t)(mcu->mcu.pc + disp));
                if (mcu->mcu.r[reg] != 0xffff)
                    mcu->mcu.pc += disp;
            }
        }
        else
            mcu->MCU_ErrorTrap();
    }
    else
        mcu->MCU_ErrorTrap();
}

void MCU_Jump_BSR(MCU *mcu, uint8_t operand)
{
    uint16_t disp;
    if (operand == 0x0e)
        disp = (int8_t)mcu->MCU_ReadCodeAdvance();
    else
    {
        disp  = mcu->MCU_ReadCodeAdvance() << 8;
        disp |= mcu->MCU_ReadCodeAdvance();
    }
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - (operand == 0x0e ? 2 : 3)),
        "BSR disp=%04x  target=%04x",
        (uint16_t)disp, (uint16_t)(mcu->mcu.pc + disp));
    mcu->MCU_PushStack(mcu->mcu.pc);
    mcu->mcu.pc += disp;
}

void MCU_Jump_PJMP(MCU *mcu, uint8_t operand)
{
    uint8_t  page    = mcu->MCU_ReadCodeAdvance();
    uint16_t address = mcu->MCU_ReadCodeAdvance() << 8;
    address |= mcu->MCU_ReadCodeAdvance();
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 4),
        "PJMP %02x:%04x", page, address);
    mcu->mcu.cp = page;
    mcu->mcu.pc = address;
}

// ── helpers ──────────────────────────────────────────────────────────────────
static const char* operand_type_str(uint32_t t)
{
    switch(t) {
    case GENERAL_DIRECT:    return "Rn";
    case GENERAL_INDIRECT:  return "@ea";
    case GENERAL_ABSOLUTE:  return "@abs";
    case GENERAL_IMMEDIATE: return "#imm";
    }
    return "?";
}

uint32_t MCU_Operand_Read(MCU *mcu)
{
    switch (mcu->operand_type)
    {
    case GENERAL_DIRECT:
        if (mcu->operand_size)
            return mcu->mcu.r[mcu->operand_reg];
        return mcu->mcu.r[mcu->operand_reg] & 0xff;
    case GENERAL_INDIRECT:
    case GENERAL_ABSOLUTE:
        if (mcu->operand_size)
        {
            if (mcu->operand_ea & 1)
                mcu->MCU_Interrupt_Exception(EXCEPTION_SOURCE_ADDRESS_ERROR);
            return mcu->MCU_Read16(mcu->MCU_GetAddress(mcu->operand_ep, mcu->operand_ea));
        }
        return mcu->MCU_Read(mcu->MCU_GetAddress(mcu->operand_ep, mcu->operand_ea));
    case GENERAL_IMMEDIATE:
        return mcu->operand_data;
    }
    return 0;
}

void MCU_Operand_Write(MCU *mcu, uint32_t data)
{
    switch (mcu->operand_type)
    {
    case GENERAL_DIRECT:
        if (mcu->operand_size)
            mcu->mcu.r[mcu->operand_reg] = data;
        else
        {
            mcu->mcu.r[mcu->operand_reg] &= ~0xff;
            mcu->mcu.r[mcu->operand_reg] |= data & 0xff;
        }
        break;
    case GENERAL_INDIRECT:
    case GENERAL_ABSOLUTE:
        if (mcu->operand_size)
        {
            if (mcu->operand_ea & 1)
                mcu->MCU_Interrupt_Exception(EXCEPTION_SOURCE_ADDRESS_ERROR);
            mcu->MCU_Write16(mcu->MCU_GetAddress(mcu->operand_ep, mcu->operand_ea), data);
        }
        else
            mcu->MCU_Write(mcu->MCU_GetAddress(mcu->operand_ep, mcu->operand_ea), data);
        break;
    case GENERAL_IMMEDIATE:
        mcu->MCU_Interrupt_Exception(EXCEPTION_SOURCE_INVALID_INSTRUCTION);
        break;
    }
}

void MCU_Operand_General(MCU *mcu, uint8_t operand)
{
    uint32_t type     = GENERAL_DIRECT;
    uint32_t disp     = 0;
    uint32_t increase = INCREASE_NONE;
    uint32_t reg      = 0;
    uint32_t siz      = OPERAND_BYTE;
    uint32_t data     = 0;
    uint32_t addr     = 0;
    uint32_t addrpage = 0;
    uint32_t ea       = 0;
    uint32_t ep       = 0;
    uint8_t  opcode;
    uint8_t  opcode_reg;

    if (operand & 0x08) siz = OPERAND_WORD;
    else                siz = OPERAND_BYTE;
    reg = operand & 0x07;

    switch (operand & 0xf0)
    {
    case 0xa0: type = GENERAL_DIRECT;   break;
    case 0xd0: type = GENERAL_INDIRECT; break;
    case 0xe0:
        type = GENERAL_INDIRECT;
        disp = (int8_t)mcu->MCU_ReadCodeAdvance();
        break;
    case 0xf0:
        type = GENERAL_INDIRECT;
        disp  = mcu->MCU_ReadCodeAdvance() << 8;
        disp |= mcu->MCU_ReadCodeAdvance();
        break;
    case 0xb0:
        type     = GENERAL_INDIRECT;
        increase = INCREASE_DECREASE;
        break;
    case 0xc0:
        type     = GENERAL_INDIRECT;
        increase = INCREASE_INCREASE;
        break;
    case 0x00:
        if (reg == 5)
        {
            type     = GENERAL_ABSOLUTE;
            addr     = mcu->mcu.br << 8;
            addr    |= mcu->MCU_ReadCodeAdvance();
            addrpage = 0;
        }
        else if (reg == 4)
        {
            type = GENERAL_IMMEDIATE;
            data = mcu->MCU_ReadCodeAdvance();
            if (siz)
            {
                data <<= 8;
                data |= mcu->MCU_ReadCodeAdvance();
            }
        }
        break;
    case 0x10:
        if (reg == 5)
        {
            type     = GENERAL_ABSOLUTE;
            addr     = mcu->MCU_ReadCodeAdvance() << 8;
            addr    |= mcu->MCU_ReadCodeAdvance();
            addrpage = mcu->mcu.dp;
        }
        break;
    }

    if (type == GENERAL_INDIRECT)
    {
        if (increase == INCREASE_DECREASE)
        {
            if (siz || reg == 7) mcu->mcu.r[reg] -= 2;
            else                 mcu->mcu.r[reg] -= 1;
        }
        ea = mcu->mcu.r[reg] + disp;
        if (increase == INCREASE_INCREASE)
        {
            if (siz || reg == 7) mcu->mcu.r[reg] += 2;
            else                 mcu->mcu.r[reg] += 1;
        }
        ea &= 0xffff;
        ep  = mcu->MCU_GetPageForRegister(reg) & 0xff;
    }
    else if (type == GENERAL_ABSOLUTE)
    {
        ea = addr & 0xffff;
        ep = addrpage & 0xff;
    }

    opcode = mcu->MCU_ReadCodeAdvance();
    mcu->opcode_extended = opcode == 0x00;
    if (mcu->opcode_extended)
        opcode = mcu->MCU_ReadCodeAdvance();
    opcode_reg = opcode & 0x07;
    opcode >>= 3;

    mcu->operand_type   = type;
    mcu->operand_ea     = ea;
    mcu->operand_ep     = ep;
    mcu->operand_size   = siz;
    mcu->operand_reg    = reg;
    mcu->operand_data   = data;
    mcu->operand_status = 0;

    MCU_Opcode_Table[opcode](mcu, opcode, opcode_reg);
}

void MCU_SetStatusCommon(MCU *mcu, uint32_t val, uint32_t siz)
{
    if (siz) val &= 0xffff;
    else     val &= 0xff;
    if (siz) mcu->MCU_SetStatus(val & 0x8000, STATUS_N);
    else     mcu->MCU_SetStatus(val & 0x80,   STATUS_N);
    mcu->MCU_SetStatus(val == 0, STATUS_Z);
    mcu->MCU_SetStatus(0, STATUS_V);
}

void MCU_Opcode_Short_NotImplemented(MCU *mcu, uint8_t opcode)
{
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "SHORT_NOT_IMPL op=%02x", opcode);
    mcu->MCU_ErrorTrap();
}

void MCU_Opcode_Short_MOVE(MCU *mcu, uint8_t opcode)
{
    uint32_t reg  = opcode & 0x07;
    uint8_t  data = mcu->MCU_ReadCodeAdvance();
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
        "MOV.B R%d, #%02x  r%d=%02x->%02x",
        reg, data, reg, mcu->mcu.r[reg] & 0xff, data);
    mcu->mcu.r[reg] &= ~0xff;
    mcu->mcu.r[reg] |= data;
    MCU_SetStatusCommon(mcu, data, 0);
}

void MCU_Opcode_Short_MOVI(MCU *mcu, uint8_t opcode)
{
    uint32_t reg  = opcode & 0x07;
    uint16_t data = mcu->MCU_ReadCodeAdvance() << 8;
    data |= mcu->MCU_ReadCodeAdvance();
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 3),
        "MOV.W R%d, #%04x  r%d=%04x->%04x",
        reg, data, reg, mcu->mcu.r[reg], data);
    mcu->mcu.r[reg] = data;
    MCU_SetStatusCommon(mcu, data, 1);
}

void MCU_Opcode_Short_MOVF(MCU *mcu, uint8_t opcode)
{
    uint32_t reg  = opcode & 0x07;
    uint32_t siz  = (opcode & 0x08) != 0;
    int8_t   disp = mcu->MCU_ReadCodeAdvance();
    uint32_t addr = (mcu->mcu.r[6] + disp) & 0xffff;
    addr |= mcu->mcu.tp << 16;
    if ((opcode & 0x10) == 0)
    {
        uint16_t data;
        if (siz)
        {
            data = mcu->MCU_Read16(addr);
            Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
                "MOVF.B R%d, (R6+%d)  @%05x=%02x->r%d",
                reg, disp, addr, data & 0xff, reg);
            mcu->mcu.r[reg] &= ~0xff;
            mcu->mcu.r[reg] |= data;
            MCU_SetStatusCommon(mcu, data, 0);
        }
        else
        {
            data = mcu->MCU_Read(addr);
            Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
                "MOVF.W R%d, (R6+%d)  @%05x=%04x->r%d",
                reg, disp, addr, data, reg);
            mcu->mcu.r[reg] = data;
            MCU_SetStatusCommon(mcu, data, 1);
        }
    }
    else
    {
        uint16_t data;
        if (siz)
        {
            data = mcu->mcu.r[reg] & 0xff;
            Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
                "MOVF.B (R6+%d), R%d  r%d=%02x->@%05x",
                disp, reg, reg, data, addr);
            mcu->MCU_Write(addr, data);
            MCU_SetStatusCommon(mcu, data, 0);
        }
        else
        {
            data = mcu->mcu.r[reg];
            Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
                "MOVF.W (R6+%d), R%d  r%d=%04x->@%05x",
                disp, reg, reg, data, addr);
            mcu->MCU_Write16(addr, data);
            MCU_SetStatusCommon(mcu, data, 1);
        }
    }
}

void MCU_Opcode_Short_MOVL(MCU *mcu, uint8_t opcode)
{
    uint32_t reg  = opcode & 0x07;
    uint32_t siz  = (opcode & 0x08) != 0;
    uint16_t addr = mcu->mcu.br << 8;
    uint32_t data;
    addr |= mcu->MCU_ReadCodeAdvance();
    if (siz)
    {
        if (addr & 1) mcu->MCU_Interrupt_Exception(EXCEPTION_SOURCE_ADDRESS_ERROR);
        data = mcu->MCU_Read16(addr);
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
            "MOVL.W R%d, @%04x  val=%04x", reg, addr, data);
        mcu->mcu.r[reg] = data;
        MCU_SetStatusCommon(mcu, data, 1);
    }
    else
    {
        data = mcu->MCU_Read(addr);
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
            "MOVL.B R%d, @%04x  val=%02x", reg, addr, data);
        mcu->mcu.r[reg] &= ~0xff;
        mcu->mcu.r[reg] |= data;
        MCU_SetStatusCommon(mcu, data, 0);
    }
}

void MCU_Opcode_Short_MOVS(MCU *mcu, uint8_t opcode)
{
    uint32_t reg  = opcode & 0x07;
    uint32_t siz  = (opcode & 0x08) != 0;
    uint16_t addr = mcu->mcu.br << 8;
    uint32_t data;
    addr |= mcu->MCU_ReadCodeAdvance();
    if (siz)
    {
        if (addr & 1) mcu->MCU_Interrupt_Exception(EXCEPTION_SOURCE_ADDRESS_ERROR);
        data = mcu->mcu.r[reg];
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
            "MOVS.W @%04x, R%d  val=%04x", addr, reg, data);
        mcu->MCU_Write16(addr, data);
        MCU_SetStatusCommon(mcu, data, 1);
    }
    else
    {
        data = mcu->mcu.r[reg] & 0xff;
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
            "MOVS.B @%04x, R%d  val=%02x", addr, reg, data);
        mcu->MCU_Write(addr, data);
        MCU_SetStatusCommon(mcu, data, 0);
    }
}

void MCU_Opcode_Short_CMP(MCU *mcu, uint8_t opcode)
{
    uint32_t reg = opcode & 0x07;
    uint32_t siz = (opcode & 0x08) != 0;
    int32_t  t1, t2;
    if (siz)
    {
        t2  = mcu->MCU_ReadCodeAdvance() << 8;
        t2 |= mcu->MCU_ReadCodeAdvance();
    }
    else
        t2 = mcu->MCU_ReadCodeAdvance();
    t1 = mcu->mcu.r[reg];
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - (siz ? 3 : 2)),
        "CMP.%c R%d, #%0*x  r%d=%0*x  sr=%04x",
        siz ? 'W' : 'B',
        reg, siz ? 4 : 2, t2,
        reg, siz ? 4 : 4, t1,
        mcu->mcu.sr);
    MCU_SUB_Common(mcu, t1, t2, 0, siz);
}

void MCU_Opcode_NotImplemented(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "OPCODE_NOT_IMPL op=%02x reg=%d", opcode, opcode_reg);
    mcu->MCU_ErrorTrap();
}

void MCU_Opcode_MOVG_Immediate(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    uint32_t data;
    if (opcode_reg == 6 && (mcu->operand_type == GENERAL_INDIRECT || mcu->operand_type == GENERAL_ABSOLUTE))
    {
        data = (int8_t)mcu->MCU_ReadCodeAdvance();
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "MOV.%c @%02x:%04x, #%02x",
            mcu->operand_size ? 'W' : 'B', mcu->operand_ep, mcu->operand_ea, data & 0xff);
        MCU_Operand_Write(mcu, data);
        MCU_SetStatusCommon(mcu, data, mcu->operand_size);
    }
    else if (opcode_reg == 7 && (mcu->operand_type == GENERAL_INDIRECT || mcu->operand_type == GENERAL_ABSOLUTE))
    {
        data  = mcu->MCU_ReadCodeAdvance() << 8;
        data |= mcu->MCU_ReadCodeAdvance();
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
            "MOV.W @%02x:%04x, #%04x",
            mcu->operand_ep, mcu->operand_ea, data);
        MCU_Operand_Write(mcu, data);
        MCU_SetStatusCommon(mcu, data, mcu->operand_size);
    }
    else if (opcode_reg == 4 && (mcu->operand_type == GENERAL_INDIRECT || mcu->operand_type == GENERAL_ABSOLUTE) && mcu->operand_size == OPERAND_BYTE)
    {
        uint32_t t1 = MCU_Operand_Read(mcu);
        uint32_t t2 = mcu->MCU_ReadCodeAdvance();
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "CMP.B @%02x:%04x, #%02x  val=%02x",
            mcu->operand_ep, mcu->operand_ea, t2, t1);
        MCU_SUB_Common(mcu, t1, t2, 0, OPERAND_BYTE);
    }
    else if (opcode_reg == 4 && (mcu->operand_type == GENERAL_INDIRECT || mcu->operand_type == GENERAL_ABSOLUTE) && mcu->operand_size == OPERAND_WORD)
    {
        uint32_t t1 = MCU_Operand_Read(mcu);
        uint32_t t2 = (uint16_t)(int8_t)mcu->MCU_ReadCodeAdvance();
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "CMP.W @%02x:%04x, #%04x  val=%04x",
            mcu->operand_ep, mcu->operand_ea, t2, t1);
        MCU_SUB_Common(mcu, t1, t2, 0, OPERAND_WORD);
    }
    else if (opcode_reg == 5 && (mcu->operand_type == GENERAL_INDIRECT || mcu->operand_type == GENERAL_ABSOLUTE) && mcu->operand_size == OPERAND_WORD)
    {
        uint32_t t1, t2;
        t1  = MCU_Operand_Read(mcu);
        t2  = mcu->MCU_ReadCodeAdvance() << 8;
        t2 |= mcu->MCU_ReadCodeAdvance();
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
            "CMP.W @%02x:%04x, #%04x  val=%04x",
            mcu->operand_ep, mcu->operand_ea, t2, t1);
        MCU_SUB_Common(mcu, t1, t2, 0, OPERAND_WORD);
    }
    else if (opcode_reg == 5 && (mcu->operand_type == GENERAL_INDIRECT || mcu->operand_type == GENERAL_ABSOLUTE) && mcu->operand_size == OPERAND_BYTE)
    {
        uint32_t t1, t2;
        t1  = MCU_Operand_Read(mcu);
        t2  = mcu->MCU_ReadCodeAdvance() << 8;
        t2 |= mcu->MCU_ReadCodeAdvance();
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 2),
            "CMP.B @%02x:%04x, #%04x  val=%02x",
            mcu->operand_ep, mcu->operand_ea, t2, t1 & 0xff);
        MCU_SUB_Common(mcu, t1, t2, 0, OPERAND_BYTE);
    }
    else
        mcu->MCU_ErrorTrap();
}

void MCU_Opcode_BSET_ORC(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    if (mcu->operand_type == GENERAL_IMMEDIATE)
    {
        uint32_t data = MCU_Operand_Read(mcu);
        uint32_t val  = mcu->MCU_ControlRegisterRead(opcode_reg, mcu->operand_size);
        val |= data;
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "ORC CR%d, #%04x  val=%04x->%04x",
            opcode_reg, data, mcu->MCU_ControlRegisterRead(opcode_reg, mcu->operand_size), val);
        mcu->MCU_ControlRegisterWrite(opcode_reg, mcu->operand_size, val);
        if (opcode_reg >= 2) MCU_SetStatusCommon(mcu, val, mcu->operand_size);
        mcu->mcu.ex_ignore = 1;
    }
    else
    {
        uint32_t data = MCU_Operand_Read(mcu);
        uint32_t bit  = mcu->mcu.r[opcode_reg] & 0x0f;
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "BSET %s bit%d  val=%02x->%02x",
            operand_type_str(mcu->operand_type), bit,
            data, data | (1 << bit));
        mcu->MCU_SetStatus((data & (1 << bit)) == 0, STATUS_Z);
        data |= 1 << bit;
        MCU_Operand_Write(mcu, data);
    }
}

void MCU_Opcode_BCLR_ANDC(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    if (mcu->operand_type == GENERAL_IMMEDIATE)
    {
        uint32_t data = MCU_Operand_Read(mcu);
        uint32_t val  = mcu->MCU_ControlRegisterRead(opcode_reg, mcu->operand_size);
        val &= data;
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "ANDC CR%d, #%04x  val=%04x->%04x",
            opcode_reg, data, mcu->MCU_ControlRegisterRead(opcode_reg, mcu->operand_size), val);
        mcu->MCU_ControlRegisterWrite(opcode_reg, mcu->operand_size, val);
        if (opcode_reg >= 2) MCU_SetStatusCommon(mcu, val, mcu->operand_size);
        mcu->mcu.ex_ignore = 1;
    }
    else
    {
        uint32_t data = MCU_Operand_Read(mcu);
        uint32_t bit  = mcu->mcu.r[opcode_reg] & 0x0f;
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "BCLR %s bit%d  val=%02x->%02x",
            operand_type_str(mcu->operand_type), bit,
            data, data & ~(1 << bit));
        mcu->MCU_SetStatus((data & (1 << bit)) == 0, STATUS_Z);
        data &= ~(1 << bit);
        MCU_Operand_Write(mcu, data);
    }
}

void MCU_Opcode_BTST(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    if (mcu->operand_type != GENERAL_IMMEDIATE)
    {
        uint32_t data = MCU_Operand_Read(mcu);
        uint32_t bit  = mcu->mcu.r[opcode_reg] & 0x0f;
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "BTST %s R%d bit%d  val=%02x  Z=%d",
            operand_type_str(mcu->operand_type), opcode_reg, bit,
            data, (data & (1 << bit)) == 0);
        mcu->MCU_SetStatus((data & (1 << bit)) == 0, STATUS_Z);
    }
    else
        mcu->MCU_ErrorTrap();
}

void MCU_Opcode_CLR(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    if (opcode_reg == 3 && mcu->operand_type != GENERAL_IMMEDIATE)
    {
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "CLR.%c %s",
            mcu->operand_size ? 'W' : 'B', operand_type_str(mcu->operand_type));
        MCU_Operand_Write(mcu, 0);
        mcu->MCU_SetStatus(0, STATUS_N); mcu->MCU_SetStatus(1, STATUS_Z);
        mcu->MCU_SetStatus(0, STATUS_V); mcu->MCU_SetStatus(0, STATUS_C);
    }
    else if (opcode_reg == 6 && mcu->operand_type != GENERAL_IMMEDIATE)
    {
        uint32_t data = MCU_Operand_Read(mcu);
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "TST.%c %s  val=%04x",
            mcu->operand_size ? 'W' : 'B', operand_type_str(mcu->operand_type), data);
        MCU_SetStatusCommon(mcu, data, mcu->operand_size);
        mcu->MCU_SetStatus(0, STATUS_C);
    }
    else if (opcode_reg == 2 && mcu->operand_type == GENERAL_DIRECT && mcu->operand_size == 0)
    {
        uint32_t data = (uint8_t)mcu->mcu.r[mcu->operand_reg];
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "EXTU R%d  r%d=%04x->%04x",
            mcu->operand_reg, mcu->operand_reg, mcu->mcu.r[mcu->operand_reg], data);
        mcu->mcu.r[mcu->operand_reg] = data;
        mcu->MCU_SetStatus(0, STATUS_N); mcu->MCU_SetStatus(data == 0, STATUS_Z);
        mcu->MCU_SetStatus(0, STATUS_V); mcu->MCU_SetStatus(0, STATUS_C);
    }
    else if (opcode_reg == 0 && mcu->operand_type == GENERAL_DIRECT && mcu->operand_size == 0)
    {
        uint32_t data   = mcu->mcu.r[mcu->operand_reg];
        uint32_t swapped = ((data & 0xff) << 8) | (data >> 8);
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "SWAP R%d  r%d=%04x->%04x",
            mcu->operand_reg, mcu->operand_reg, data, swapped);
        mcu->mcu.r[mcu->operand_reg] = swapped;
        MCU_SetStatusCommon(mcu, swapped, OPERAND_WORD);
    }
    else if (opcode_reg == 5 && mcu->operand_type != GENERAL_IMMEDIATE)
    {
        uint32_t data = MCU_Operand_Read(mcu);
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "NOT.%c %s  val=%04x->%04x",
            mcu->operand_size ? 'W' : 'B',
            operand_type_str(mcu->operand_type), data, (~data) & (mcu->operand_size ? 0xffff : 0xff));
        data = ~data;
        MCU_Operand_Write(mcu, data);
        MCU_SetStatusCommon(mcu, data, mcu->operand_size);
    }
    else if (opcode_reg == 4 && mcu->operand_type != GENERAL_IMMEDIATE)
    {
        uint32_t data = MCU_Operand_Read(mcu);
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "NEG.%c %s  val=%04x",
            mcu->operand_size ? 'W' : 'B',
            operand_type_str(mcu->operand_type), data);
        data = MCU_SUB_Common(mcu, 0, data, 0, mcu->operand_size);
        MCU_Operand_Write(mcu, data);
    }
    else if (opcode_reg == 1 && mcu->operand_type == GENERAL_DIRECT && mcu->operand_size == 0)
    {
        uint32_t data = mcu->mcu.r[mcu->operand_reg];
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "EXTS R%d  r%d=%04x->%04x",
            mcu->operand_reg, mcu->operand_reg, data, (uint16_t)(int8_t)data);
        mcu->mcu.r[mcu->operand_reg] = (int8_t)data;
        MCU_SetStatusCommon(mcu, data, OPERAND_WORD);
    }
    else
        mcu->MCU_ErrorTrap();
}

void MCU_Opcode_LDC(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    if (mcu->operand_reg == 7 && opcode_reg == 4)
    {
        mcu->operand_size = 1;
        uint32_t data = MCU_Operand_Read(mcu);
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "LDC EP:DP, %s  val=%04x", operand_type_str(mcu->operand_type), data);
        mcu->MCU_ControlRegisterWrite(4, 0, data & 0xff);
        mcu->MCU_ControlRegisterWrite(5, 0, data >> 8);
    }
    else
    {
        uint32_t data = MCU_Operand_Read(mcu);
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "LDC CR%d, %s  val=%04x", opcode_reg, operand_type_str(mcu->operand_type), data);
        mcu->MCU_ControlRegisterWrite(opcode_reg, mcu->operand_size, data);
    }
    mcu->mcu.ex_ignore = 1;
}

void MCU_Opcode_STC(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    if (mcu->operand_reg == 7 && opcode_reg == 4)
    {
        mcu->operand_size = 1;
        uint32_t dataL = mcu->MCU_ControlRegisterRead(4, 0);
        uint32_t dataH = mcu->MCU_ControlRegisterRead(5, 0);
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "STC %s, EP:DP  val=%04x", operand_type_str(mcu->operand_type), dataL | dataH << 8);
        MCU_Operand_Write(mcu, dataL | dataH << 8);
    }
    else
    {
        uint32_t data = mcu->MCU_ControlRegisterRead(opcode_reg, mcu->operand_size);
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "STC %s, CR%d  val=%04x", operand_type_str(mcu->operand_type), opcode_reg, data);
        MCU_Operand_Write(mcu, data);
    }
}

void MCU_Opcode_BSET(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    if (mcu->operand_type != GENERAL_IMMEDIATE)
    {
        uint32_t data = MCU_Operand_Read(mcu);
        uint32_t bit  = opcode_reg | ((opcode & 1) << 3);
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "BSET %s bit%d  val=%02x->%02x",
            operand_type_str(mcu->operand_type), bit, data, data | (1 << bit));
        mcu->MCU_SetStatus((data & (1 << bit)) == 0, STATUS_Z);
        data |= 1 << bit;
        MCU_Operand_Write(mcu, data);
    }
    else
        mcu->MCU_ErrorTrap();
}

void MCU_Opcode_BCLR(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    if (mcu->operand_type != GENERAL_IMMEDIATE)
    {
        uint32_t data = MCU_Operand_Read(mcu);
        uint32_t bit  = opcode_reg | ((opcode & 1) << 3);
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "BCLR %s bit%d  val=%02x->%02x",
            operand_type_str(mcu->operand_type), bit, data, data & ~(1 << bit));
        mcu->MCU_SetStatus((data & (1 << bit)) == 0, STATUS_Z);
        data &= ~(1 << bit);
        MCU_Operand_Write(mcu, data);
    }
    else
        mcu->MCU_ErrorTrap();
}

void MCU_Opcode_MOVG(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    if (mcu->opcode_extended)
    {
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "MOVG_EXT op=%02x  NOT_IMPL", opcode);
        mcu->MCU_ErrorTrap();
    }
    else
    {
        uint8_t d = (opcode & 2) != 0;
        uint32_t data;
        if (d)
        {
            if (mcu->operand_type == GENERAL_DIRECT)
            {
                if (mcu->operand_size)
                {
                    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
                        "XCH R%d, R%d  r%d=%04x r%d=%04x",
                        opcode_reg, mcu->operand_reg,
                        opcode_reg, mcu->mcu.r[opcode_reg],
                        mcu->operand_reg, mcu->mcu.r[mcu->operand_reg]);
                    uint32_t r1 = mcu->mcu.r[opcode_reg];
                    uint32_t r2 = mcu->mcu.r[mcu->operand_reg];
                    mcu->mcu.r[opcode_reg]   = r2;
                    mcu->mcu.r[mcu->operand_reg] = r1;
                }
                else
                    mcu->MCU_ErrorTrap();
            }
            else
            {
                data = mcu->mcu.r[opcode_reg];
                Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
                    "MOV.%c %s, R%d  val=%04x",
                    mcu->operand_size ? 'W' : 'B',
                    operand_type_str(mcu->operand_type), opcode_reg, data);
                MCU_Operand_Write(mcu, data);
                MCU_SetStatusCommon(mcu, data, mcu->operand_size);
            }
        }
        else
        {
            data = MCU_Operand_Read(mcu);
            Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
                "MOV.%c R%d, %s  val=%04x->r%d",
                mcu->operand_size ? 'W' : 'B',
                opcode_reg, operand_type_str(mcu->operand_type), data, opcode_reg);
            if (mcu->operand_size)
                mcu->mcu.r[opcode_reg] = data;
            else
            {
                mcu->mcu.r[opcode_reg] &= ~0xff;
                mcu->mcu.r[opcode_reg] |= data & 0xff;
            }
            MCU_SetStatusCommon(mcu, data, mcu->operand_size);
        }
    }
}

void MCU_Opcode_BTSTI(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    if (mcu->operand_type != GENERAL_IMMEDIATE)
    {
        uint32_t data = MCU_Operand_Read(mcu);
        uint32_t bit  = opcode_reg | ((opcode & 1) << 3);
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "BTSTI %s bit%d  val=%02x  Z=%d",
            operand_type_str(mcu->operand_type), bit, data, (data & (1 << bit)) == 0);
        mcu->MCU_SetStatus((data & (1 << bit)) == 0, STATUS_Z);
    }
    else
        mcu->MCU_ErrorTrap();
}

void MCU_Opcode_BNOTI(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    if (mcu->operand_type != GENERAL_IMMEDIATE)
    {
        uint32_t data = MCU_Operand_Read(mcu);
        uint32_t bit  = opcode_reg | ((opcode & 1) << 3);
        Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
            "BNOTI %s bit%d  val=%02x->%02x",
            operand_type_str(mcu->operand_type), bit, data, data ^ (1 << bit));
        mcu->MCU_SetStatus((data & (1 << bit)) == 0, STATUS_Z);
        data ^= (1 << bit);
        MCU_Operand_Write(mcu, data);
    }
    else
        mcu->MCU_ErrorTrap();
}

void MCU_Opcode_OR(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    uint32_t data = MCU_Operand_Read(mcu);
    uint32_t prev = mcu->mcu.r[opcode_reg];
    mcu->mcu.r[opcode_reg] |= data;
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "OR.%c R%d, %s  r%d=%04x|%04x=%04x",
        mcu->operand_size ? 'W' : 'B',
        opcode_reg, operand_type_str(mcu->operand_type),
        opcode_reg, prev, data, mcu->mcu.r[opcode_reg]);
    MCU_SetStatusCommon(mcu, mcu->mcu.r[opcode_reg], mcu->operand_size);
}

void MCU_Opcode_CMP(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    int32_t t1 = mcu->mcu.r[opcode_reg];
    int32_t t2 = MCU_Operand_Read(mcu);
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "CMP.%c R%d, %s  r%d=%04x op=%04x  sr=%04x",
        mcu->operand_size ? 'W' : 'B',
        opcode_reg, operand_type_str(mcu->operand_type),
        opcode_reg, (uint16_t)t1, (uint16_t)t2, mcu->mcu.sr);
    MCU_SUB_Common(mcu, t1, t2, 0, mcu->operand_size);
}

void MCU_Opcode_ADDQ(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    int32_t t1 = MCU_Operand_Read(mcu);
    int32_t t2 = 0;
    switch (opcode_reg)
    {
    case 0: t2 =  1; break;
    case 1: t2 =  2; break;
    case 4: t2 = -1; break;
    case 5: t2 = -2; break;
    default: mcu->MCU_ErrorTrap(); break;
    }
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "ADDQ.%c %s, #%d  val=%04x->%04x",
        mcu->operand_size ? 'W' : 'B',
        operand_type_str(mcu->operand_type), t2,
        (uint16_t)t1, (uint16_t)MCU_ADD_Common(mcu, t1, t2, 0, mcu->operand_size));
    t1 = MCU_ADD_Common(mcu, t1, t2, 0, mcu->operand_size);
    MCU_Operand_Write(mcu, t1);
}

void MCU_Opcode_ADD(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    int32_t t1   = mcu->mcu.r[opcode_reg];
    int32_t t2   = MCU_Operand_Read(mcu);
    int32_t res  = MCU_ADD_Common(mcu, t1, t2, 0, mcu->operand_size);
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "ADD.%c R%d, %s  r%d=%04x+%04x=%04x  sr=%04x",
        mcu->operand_size ? 'W' : 'B',
        opcode_reg, operand_type_str(mcu->operand_type),
        opcode_reg, (uint16_t)t1, (uint16_t)t2, (uint16_t)res, mcu->mcu.sr);
    if (mcu->operand_size)
        mcu->mcu.r[opcode_reg] = res;
    else
    {
        mcu->mcu.r[opcode_reg] &= ~0xff;
        mcu->mcu.r[opcode_reg] |= res & 0xff;
    }
}

void MCU_Opcode_SUB(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    int32_t t1  = mcu->mcu.r[opcode_reg];
    int32_t t2  = MCU_Operand_Read(mcu);
    int32_t res = MCU_SUB_Common(mcu, t1, t2, 0, mcu->operand_size);
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "SUB.%c R%d, %s  r%d=%04x-%04x=%04x  sr=%04x",
        mcu->operand_size ? 'W' : 'B',
        opcode_reg, operand_type_str(mcu->operand_type),
        opcode_reg, (uint16_t)t1, (uint16_t)t2, (uint16_t)res, mcu->mcu.sr);
    if (mcu->operand_size)
        mcu->mcu.r[opcode_reg] = res;
    else
    {
        mcu->mcu.r[opcode_reg] &= ~0xff;
        mcu->mcu.r[opcode_reg] |= res & 0xff;
    }
}

void MCU_Opcode_SUBS(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    int32_t t1  = mcu->mcu.r[opcode_reg];
    int32_t t2  = MCU_Operand_Read(mcu);
    int32_t res = mcu->operand_size ? t1 - t2 : t1 - (int8_t)t2;
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "SUBS.%c R%d, %s  r%d=%04x-%04x=%04x",
        mcu->operand_size ? 'W' : 'B',
        opcode_reg, operand_type_str(mcu->operand_type),
        opcode_reg, (uint16_t)t1, (uint16_t)t2, (uint16_t)res);
    mcu->mcu.r[opcode_reg] = res;
}

void MCU_Opcode_AND(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    uint32_t prev = mcu->mcu.r[opcode_reg];
    uint32_t data = prev & MCU_Operand_Read(mcu);
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "AND.%c R%d, %s  r%d=%04x&%04x=%04x",
        mcu->operand_size ? 'W' : 'B',
        opcode_reg, operand_type_str(mcu->operand_type),
        opcode_reg, prev, MCU_Operand_Read(mcu), data);
    if (mcu->operand_size)
        mcu->mcu.r[opcode_reg] = data;
    else
    {
        mcu->mcu.r[opcode_reg] &= ~0xff;
        mcu->mcu.r[opcode_reg] |= data & 0xff;
    }
    MCU_SetStatusCommon(mcu, mcu->mcu.r[opcode_reg], mcu->operand_size);
}

void MCU_Opcode_SHLR(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    if (mcu->operand_type == GENERAL_IMMEDIATE) { mcu->MCU_ErrorTrap(); return; }
    uint32_t data = MCU_Operand_Read(mcu);
    uint32_t C, res;
    const char* mnem = "?";

    if      (opcode_reg == 0x03) { mnem="SHLR";  C=data&1; res=data>>1; }
    else if (opcode_reg == 0x02) { mnem="SHLL";  C=mcu->operand_size?(data>>15)&1:(data>>7)&1; res=data<<1; }
    else if (opcode_reg == 0x06) {
        mnem="ROTXL";
        uint32_t bit=(mcu->mcu.sr&STATUS_C)!=0;
        C=mcu->operand_size?(data>>15)&1:(data>>7)&1; res=(data<<1)|bit;
    }
    else if (opcode_reg == 0x07) {
        mnem="ROTXR";
        uint32_t bit=(mcu->mcu.sr&STATUS_C)!=0;
        C=data&1; res=(data>>1)|(mcu->operand_size?bit<<15:bit<<7);
    }
    else if (opcode_reg == 0x04) { mnem="ROTL";  C=mcu->operand_size?(data>>15)&1:(data>>7)&1; res=(data<<1)|C; }
    else if (opcode_reg == 0x00) { mnem="SHAL";  C=mcu->operand_size?(data>>15)&1:(data>>7)&1; res=data<<1; }
    else if (opcode_reg == 0x01) {
        mnem="SHAR";
        C=data&1;
        uint32_t msb=mcu->operand_size?(data&0x8000):(data&0x80);
        res=((mcu->operand_size?data&0x7fff:data&0x7f)>>1)|msb;
    }
    else if (opcode_reg == 0x05) { mnem="ROTR";  C=data&1; res=(data>>1)|(mcu->operand_size?C<<15:C<<7); }
    else { mcu->MCU_ErrorTrap(); return; }

    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "%s.%c %s  val=%04x->%04x  C=%d",
        mnem, mcu->operand_size ? 'W' : 'B',
        operand_type_str(mcu->operand_type),
        data, res & (mcu->operand_size ? 0xffff : 0xff), C);

    MCU_Operand_Write(mcu, res);
    mcu->MCU_SetStatus(C, STATUS_C);
    MCU_SetStatusCommon(mcu, res, mcu->operand_size);
}

void MCU_Opcode_MULXU(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    uint32_t t1 = MCU_Operand_Read(mcu);
    uint32_t t2 = mcu->mcu.r[opcode_reg];
    if (!mcu->operand_size) t2 &= 0xff;
    uint32_t res = t1 * t2;
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "MULXU.%c R%d, %s  %04x*%04x=%08x",
        mcu->operand_size ? 'W' : 'B',
        opcode_reg, operand_type_str(mcu->operand_type),
        t2, t1, res);
    uint32_t N, Z;
    if (mcu->operand_size)
    {
        opcode_reg &= ~1;
        mcu->mcu.r[opcode_reg | 0] = res >> 16;
        mcu->mcu.r[opcode_reg | 1] = res;
        N = (res & 0x80000000UL) != 0;
    }
    else
    {
        res &= 0xffff;
        mcu->mcu.r[opcode_reg] = res;
        N = (res & 0x8000UL) != 0;
    }
    Z = res == 0;
    mcu->MCU_SetStatus(N, STATUS_N); mcu->MCU_SetStatus(Z, STATUS_Z);
    mcu->MCU_SetStatus(0, STATUS_V); mcu->MCU_SetStatus(0, STATUS_C);
}

void MCU_Opcode_DIVXU(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    uint32_t t1 = MCU_Operand_Read(mcu);
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "DIVXU.%c R%d, %s  divisor=%04x",
        mcu->operand_size ? 'W' : 'B',
        opcode_reg, operand_type_str(mcu->operand_type), t1);
    uint32_t R, Q;
    if (!t1)
    {
        mcu->MCU_ErrorTrap();
        mcu->MCU_SetStatus(0, STATUS_N); mcu->MCU_SetStatus(1, STATUS_Z);
        mcu->MCU_SetStatus(0, STATUS_V); mcu->MCU_SetStatus(0, STATUS_C);
        return;
    }
    if (mcu->operand_size)
    {
        opcode_reg &= ~1;
        uint32_t t2 = (mcu->mcu.r[opcode_reg|0]<<16)|mcu->mcu.r[opcode_reg|1];
        R = t2 % t1; Q = t2 / t1;
        if (Q > UINT16_MAX)
        {
            mcu->MCU_SetStatus(0,STATUS_N); mcu->MCU_SetStatus(0,STATUS_Z);
            mcu->MCU_SetStatus(1,STATUS_V); mcu->MCU_SetStatus(0,STATUS_C);
        }
        else
        {
            mcu->mcu.r[opcode_reg|0]=R; mcu->mcu.r[opcode_reg|1]=Q;
            MCU_SetStatusCommon(mcu,Q,OPERAND_WORD); mcu->MCU_SetStatus(0,STATUS_C);
        }
    }
    else
    {
        uint32_t t2 = mcu->mcu.r[opcode_reg];
        R = t2 % t1; Q = t2 / t1;
        if (Q > UINT8_MAX)
        {
            mcu->MCU_SetStatus(0,STATUS_N); mcu->MCU_SetStatus(0,STATUS_Z);
            mcu->MCU_SetStatus(1,STATUS_V); mcu->MCU_SetStatus(0,STATUS_C);
        }
        else
        {
            R &= 0xff; Q &= 0xff;
            mcu->mcu.r[opcode_reg] = (R<<8)|Q;
            MCU_SetStatusCommon(mcu,Q,OPERAND_BYTE); mcu->MCU_SetStatus(0,STATUS_C);
        }
    }
}

void MCU_Opcode_ADDS(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    uint32_t data = MCU_Operand_Read(mcu);
    uint32_t prev = mcu->mcu.r[opcode_reg];
    if (!mcu->operand_size) data = (int8_t)data;
    mcu->mcu.r[opcode_reg] += data;
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "ADDS.%c R%d, %s  r%d=%04x+%04x=%04x",
        mcu->operand_size ? 'W' : 'B',
        opcode_reg, operand_type_str(mcu->operand_type),
        opcode_reg, prev, data & 0xffff, mcu->mcu.r[opcode_reg]);
}

void MCU_Opcode_XOR(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    uint32_t data = MCU_Operand_Read(mcu);
    uint32_t prev = mcu->mcu.r[opcode_reg];
    mcu->mcu.r[opcode_reg] ^= data;
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "XOR.%c R%d, %s  r%d=%04x^%04x=%04x",
        mcu->operand_size ? 'W' : 'B',
        opcode_reg, operand_type_str(mcu->operand_type),
        opcode_reg, prev, data, mcu->mcu.r[opcode_reg]);
    MCU_SetStatusCommon(mcu, mcu->mcu.r[opcode_reg], mcu->operand_size);
}

void MCU_Opcode_ADDX(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    int32_t t1  = mcu->mcu.r[opcode_reg];
    int32_t t2  = MCU_Operand_Read(mcu);
    int32_t C   = (mcu->mcu.sr & STATUS_C) != 0;
    int32_t Z   = (mcu->mcu.sr & STATUS_Z) != 0;
    int32_t res = MCU_ADD_Common(mcu, t1, t2, C, mcu->operand_size);
    if (!Z) mcu->MCU_SetStatus(0, STATUS_Z);
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "ADDX.%c R%d, %s  r%d=%04x+%04x+C%d=%04x  sr=%04x",
        mcu->operand_size ? 'W' : 'B',
        opcode_reg, operand_type_str(mcu->operand_type),
        opcode_reg, (uint16_t)t1, (uint16_t)t2, C, (uint16_t)res, mcu->mcu.sr);
    if (mcu->operand_size)
        mcu->mcu.r[opcode_reg] = res;
    else
    {
        mcu->mcu.r[opcode_reg] &= ~0xff;
        mcu->mcu.r[opcode_reg] |= res & 0xff;
    }
}

void MCU_Opcode_SUBX(MCU *mcu, uint8_t opcode, uint8_t opcode_reg)
{
    int32_t t1  = mcu->mcu.r[opcode_reg];
    int32_t t2  = MCU_Operand_Read(mcu);
    int32_t C   = (mcu->mcu.sr & STATUS_C) != 0;
    int32_t res = MCU_SUB_Common(mcu, t1, t2, C, mcu->operand_size);
    Tracer::LogCPU(mcu->mcu.cycles, mcu->mcu.cp, (uint16_t)(mcu->mcu.pc - 1),
        "SUBX.%c R%d, %s  r%d=%04x-%04x-C%d=%04x  sr=%04x",
        mcu->operand_size ? 'W' : 'B',
        opcode_reg, operand_type_str(mcu->operand_type),
        opcode_reg, (uint16_t)t1, (uint16_t)t2, C, (uint16_t)res, mcu->mcu.sr);
    if (mcu->operand_size)
        mcu->mcu.r[opcode_reg] = res;
    else
    {
        mcu->mcu.r[opcode_reg] &= ~0xff;
        mcu->mcu.r[opcode_reg] |= res & 0xff;
    }
}

void (*MCU_Operand_Table[256])(MCU *_this, uint8_t operand) = {
    MCU_Operand_Nop,            // 00
    MCU_Jump_JMP,               // 01
    MCU_LDM,                    // 02
    MCU_Jump_PJSR,              // 03
    MCU_Operand_General,        // 04
    MCU_Operand_General,        // 05
    MCU_Jump_JMP,               // 06
    MCU_Jump_JMP,               // 07
    MCU_TRAPA,                  // 08
    MCU_Operand_NotImplemented, // 09
    MCU_Jump_RTE,               // 0A
    MCU_Operand_NotImplemented, // 0B
    MCU_Operand_General,        // 0C
    MCU_Operand_General,        // 0D
    MCU_Jump_BSR,               // 0E
    MCU_UNLK,                   // 0F
    MCU_Jump_JMP,               // 10
    MCU_Jump_JMP,               // 11
    MCU_STM,                    // 12
    MCU_Jump_PJMP,              // 13
    MCU_Jump_RTD,               // 14
    MCU_Operand_General,        // 15
    MCU_Operand_NotImplemented, // 16
    MCU_LINK,                   // 17
    MCU_Jump_JSR,               // 18
    MCU_Jump_RTS,               // 19
    MCU_Operand_Sleep,          // 1A
    MCU_Operand_NotImplemented, // 1B
    MCU_Jump_RTD,               // 1C
    MCU_Operand_General,        // 1D
    MCU_Jump_BSR,               // 1E
    MCU_LINK,                   // 1F
    MCU_Jump_Bcc, MCU_Jump_Bcc, MCU_Jump_Bcc, MCU_Jump_Bcc, // 20-23
    MCU_Jump_Bcc, MCU_Jump_Bcc, MCU_Jump_Bcc, MCU_Jump_Bcc, // 24-27
    MCU_Jump_Bcc, MCU_Jump_Bcc, MCU_Jump_Bcc, MCU_Jump_Bcc, // 28-2B
    MCU_Jump_Bcc, MCU_Jump_Bcc, MCU_Jump_Bcc, MCU_Jump_Bcc, // 2C-2F
    MCU_Jump_Bcc, MCU_Jump_Bcc, MCU_Jump_Bcc, MCU_Jump_Bcc, // 30-33
    MCU_Jump_Bcc, MCU_Jump_Bcc, MCU_Jump_Bcc, MCU_Jump_Bcc, // 34-37
    MCU_Jump_Bcc, MCU_Jump_Bcc, MCU_Jump_Bcc, MCU_Jump_Bcc, // 38-3B
    MCU_Jump_Bcc, MCU_Jump_Bcc, MCU_Jump_Bcc, MCU_Jump_Bcc, // 3C-3F
    MCU_Opcode_Short_CMP,  MCU_Opcode_Short_CMP,  MCU_Opcode_Short_CMP,  MCU_Opcode_Short_CMP,  // 40-43
    MCU_Opcode_Short_CMP,  MCU_Opcode_Short_CMP,  MCU_Opcode_Short_CMP,  MCU_Opcode_Short_CMP,  // 44-47
    MCU_Opcode_Short_CMP,  MCU_Opcode_Short_CMP,  MCU_Opcode_Short_CMP,  MCU_Opcode_Short_CMP,  // 48-4B
    MCU_Opcode_Short_CMP,  MCU_Opcode_Short_CMP,  MCU_Opcode_Short_CMP,  MCU_Opcode_Short_CMP,  // 4C-4F
    MCU_Opcode_Short_MOVE, MCU_Opcode_Short_MOVE, MCU_Opcode_Short_MOVE, MCU_Opcode_Short_MOVE, // 50-53
    MCU_Opcode_Short_MOVE, MCU_Opcode_Short_MOVE, MCU_Opcode_Short_MOVE, MCU_Opcode_Short_MOVE, // 54-57
    MCU_Opcode_Short_MOVI, MCU_Opcode_Short_MOVI, MCU_Opcode_Short_MOVI, MCU_Opcode_Short_MOVI, // 58-5B
    MCU_Opcode_Short_MOVI, MCU_Opcode_Short_MOVI, MCU_Opcode_Short_MOVI, MCU_Opcode_Short_MOVI, // 5C-5F
    MCU_Opcode_Short_MOVL, MCU_Opcode_Short_MOVL, MCU_Opcode_Short_MOVL, MCU_Opcode_Short_MOVL, // 60-63
    MCU_Opcode_Short_MOVL, MCU_Opcode_Short_MOVL, MCU_Opcode_Short_MOVL, MCU_Opcode_Short_MOVL, // 64-67
    MCU_Opcode_Short_MOVL, MCU_Opcode_Short_MOVL, MCU_Opcode_Short_MOVL, MCU_Opcode_Short_MOVL, // 68-6B
    MCU_Opcode_Short_MOVL, MCU_Opcode_Short_MOVL, MCU_Opcode_Short_MOVL, MCU_Opcode_Short_MOVL, // 6C-6F
    MCU_Opcode_Short_MOVS, MCU_Opcode_Short_MOVS, MCU_Opcode_Short_MOVS, MCU_Opcode_Short_MOVS, // 70-73
    MCU_Opcode_Short_MOVS, MCU_Opcode_Short_MOVS, MCU_Opcode_Short_MOVS, MCU_Opcode_Short_MOVS, // 74-77
    MCU_Opcode_Short_MOVS, MCU_Opcode_Short_MOVS, MCU_Opcode_Short_MOVS, MCU_Opcode_Short_MOVS, // 78-7B
    MCU_Opcode_Short_MOVS, MCU_Opcode_Short_MOVS, MCU_Opcode_Short_MOVS, MCU_Opcode_Short_MOVS, // 7C-7F
    MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, // 80-83
    MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, // 84-87
    MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, // 88-8B
    MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, // 8C-8F
    MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, // 90-93
    MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, // 94-97
    MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, // 98-9B
    MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, MCU_Opcode_Short_MOVF, // 9C-9F
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // A0-A3
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // A4-A7
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // A8-AB
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // AC-AF
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // B0-B3
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // B4-B7
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // B8-BB
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // BC-BF
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // C0-C3
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // C4-C7
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // C8-CB
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // CC-CF
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // D0-D3
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // D4-D7
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // D8-DB
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // DC-DF
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // E0-E3
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // E4-E7
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // E8-EB
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // EC-EF
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // F0-F3
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // F4-F7
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // F8-FB
    MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, MCU_Operand_General, // FC-FF
};

void (*MCU_Opcode_Table[32])(MCU *_this, uint8_t opcode, uint8_t opcode_reg) = {
    MCU_Opcode_MOVG_Immediate, // 00
    MCU_Opcode_ADDQ,           // 01
    MCU_Opcode_CLR,            // 02
    MCU_Opcode_SHLR,           // 03
    MCU_Opcode_ADD,            // 04
    MCU_Opcode_ADDS,           // 05
    MCU_Opcode_SUB,            // 06
    MCU_Opcode_SUBS,           // 07
    MCU_Opcode_OR,             // 08
    MCU_Opcode_BSET_ORC,       // 09
    MCU_Opcode_AND,            // 0A
    MCU_Opcode_BCLR_ANDC,      // 0B
    MCU_Opcode_XOR,            // 0C
    MCU_Opcode_NotImplemented, // 0D
    MCU_Opcode_CMP,            // 0E
    MCU_Opcode_BTST,           // 0F
    MCU_Opcode_MOVG,           // 10
    MCU_Opcode_LDC,            // 11
    MCU_Opcode_MOVG,           // 12
    MCU_Opcode_STC,            // 13
    MCU_Opcode_ADDX,           // 14
    MCU_Opcode_MULXU,          // 15
    MCU_Opcode_SUBX,           // 16
    MCU_Opcode_DIVXU,          // 17
    MCU_Opcode_BSET,           // 18
    MCU_Opcode_BSET,           // 19
    MCU_Opcode_BCLR,           // 1A
    MCU_Opcode_BCLR,           // 1B
    MCU_Opcode_BNOTI,          // 1C
    MCU_Opcode_BNOTI,          // 1D
    MCU_Opcode_BTSTI,          // 1E
    MCU_Opcode_BTSTI,          // 1F
};