#define BOOST_TEST_MODULE noopcputest
#include <boost/test/included/unit_test.hpp> //single-header
#include "../core/core.h"
#include "../core/noopcpu.h"
#include "../core/instructions.h"

namespace noopcputest
{
    using namespace mc68000;

    void validateDecode(unsigned short int instruction, int expected, const wchar_t* instructionName)
    {
        // Arrange
        core<NoOpCpu> cpu;

        // act
        auto actual = cpu(instruction);

        // Assert
        BOOST_CHECK_EQUAL(expected, actual);
        BOOST_CHECK_EQUAL(instructions::names[actual], instructionName);
    }
    
    BOOST_AUTO_TEST_CASE(abcd)
    {
        validateDecode(0b1100000100000000u, instructions::ABCD, L"ABCD");
    }

    BOOST_AUTO_TEST_CASE(add)
    {
        validateDecode(0b1101000000000000u, instructions::ADD, L"ADD");
    }

    BOOST_AUTO_TEST_CASE(adda)
    {
        validateDecode(0b1101'0000'1100'0000u, instructions::ADDA, L"ADDA");
    }

    BOOST_AUTO_TEST_CASE(addi)
    {
        validateDecode(0b0000'0110'0000'0000u, instructions::ADDI, L"ADDI");
    }

    BOOST_AUTO_TEST_CASE(addq)
    {
        validateDecode(0b0101'1110'0000'0000u, instructions::ADDQ, L"ADDQ");
    }

    BOOST_AUTO_TEST_CASE(addx)
    {
        validateDecode(0b1101'0101'0100'1010u, instructions::ADDX, L"ADDX");
    }

    BOOST_AUTO_TEST_CASE(and_)
    {
        validateDecode(0b1100'1110'1001'0010u, instructions::AND, L"AND"); // AND.L (A2), D7
    }

    BOOST_AUTO_TEST_CASE(andi)
    {
        validateDecode(0b0000'0010'1001'0010u, instructions::ANDI, L"ANDI"); // ANDI.L #xxx, (A2)
    }

    BOOST_AUTO_TEST_CASE(andi_to_ccr)
    {
        validateDecode(0b0000'0010'0011'1100u, instructions::ANDI2CCR, L"ANDI2CCR"); // ANDI.L #xxx, CCR
    }


    BOOST_AUTO_TEST_CASE(asl)
    {
        validateDecode(0b1110'001'1'01'1'00'110u, instructions::ASL, L"ASL"); // ASL.W D1, D6
        validateDecode(0b1110'000'1'11'010'010u, instructions::ASL, L"ASL");  // ASL.W (A2)

    }

    BOOST_AUTO_TEST_CASE(asr)
    {
        validateDecode(0b1110'001'0'10'1'00'100u, instructions::ASR, L"ASR"); // ASR.L D1, D4
        validateDecode(0b1110'000'0'11'010'010u, instructions::ASR, L"ASR");  // ASR.W (A2)
    }

    BOOST_AUTO_TEST_CASE(bhi)
    {
        validateDecode(0b0110'0010'1110'0100u, instructions::BHI, L"BHI");
    }

    BOOST_AUTO_TEST_CASE(bls)
    {
        validateDecode(0b0110'0011'1110'0100u, instructions::BLS, L"BLS");
    }

    BOOST_AUTO_TEST_CASE(bcc)
    {
        validateDecode(0b0110'0100'1110'0100u, instructions::BCC, L"BCC");
    }
    BOOST_AUTO_TEST_CASE(bcs)
    {
        validateDecode(0b0110'0101'1110'0100u, instructions::BCS, L"BCS");
    }

    BOOST_AUTO_TEST_CASE(bne)
    {
        validateDecode(0b0110'0110'1110'0100u, instructions::BNE, L"BNE");
    }

    BOOST_AUTO_TEST_CASE(beq)
    {
        validateDecode(0b0110'0111'1110'0100u, instructions::BEQ, L"BEQ");
    }

    BOOST_AUTO_TEST_CASE(bvc)
    {
        validateDecode(0b0110'1000'1110'0100u, instructions::BVC, L"BVC");
    }

    BOOST_AUTO_TEST_CASE(bvs)
    {
        validateDecode(0b0110'1001'1110'0100u, instructions::BVS, L"BVS");
    }

    BOOST_AUTO_TEST_CASE(bpl)
    {
        validateDecode(0b0110'1010'1110'0100u, instructions::BPL, L"BPL");
    }

    BOOST_AUTO_TEST_CASE(bmi)
    {
        validateDecode(0b0110'1011'1110'0100u, instructions::BMI, L"BMI");
    }

    BOOST_AUTO_TEST_CASE(bge)
    {
        validateDecode(0b0110'1100'1110'0100u, instructions::BGE, L"BGE");
    }

    BOOST_AUTO_TEST_CASE(blt)
    {
        validateDecode(0b0110'1101'1110'0100u, instructions::BLT, L"BLT");
    }

    BOOST_AUTO_TEST_CASE(bgt)
    {
        validateDecode(0b0110'1110'1110'0100u, instructions::BGT, L"BGT");
    }

    BOOST_AUTO_TEST_CASE(ble)
    {
        validateDecode(0b0110'1111'1111'0100u, instructions::BLE, L"BLE");
    }

    BOOST_AUTO_TEST_CASE(bchg)
    {
        validateDecode(0b0000'111'101'010'010u, instructions::BCHG_R, L"BCHG"); // BCHG D7, (A2)
        validateDecode(0b0000'100'001'010'010u, instructions::BCHG_I, L"BCHG"); // BCHG #xx, (A2)
    }

    BOOST_AUTO_TEST_CASE(bclr)
    {
        validateDecode(0b0000'111'110'010'010u, instructions::BCLR_R, L"BCLR"); // BCLR D7, (A2)
        validateDecode(0b0000'100'010'010'010u, instructions::BCLR_I, L"BCLR"); // BCLR #xx, (A2)
    }

    BOOST_AUTO_TEST_CASE(bra)
    {
        validateDecode(0b0110'0000'1010'1010u, instructions::BRA, L"BRA");
    }

    BOOST_AUTO_TEST_CASE(bset)
    {
        validateDecode(0b0000'111'111'010'010u, instructions::BSET_R, L"BSET"); // BSET D7, (A2)
        validateDecode(0b0000'100'011'010'010u, instructions::BSET_I, L"BSET"); // BSET #xx, (A2)
    }

    BOOST_AUTO_TEST_CASE(bsr)
    {
        validateDecode(0b0110'0001'0101'0101u, instructions::BSR, L"BSR"); // BSR <label>
    }

    BOOST_AUTO_TEST_CASE(btst)
    {
        validateDecode(0b0000'111'100'010'010u, instructions::BTST_R, L"BTST"); // BTST D7, (A2)
        validateDecode(0b0000'100'000'010'010u, instructions::BTST_I, L"BTST"); // BTST #xx, (A2)
    }

    BOOST_AUTO_TEST_CASE(chk)
    {
        validateDecode(0b0100'111'11'0'010'010u, instructions::CHK, L"CHK"); // CHK (A2), D7
    }

    BOOST_AUTO_TEST_CASE(clr)
    {
        validateDecode(0b0100'0010'10'010'010u, instructions::CLR, L"CLR"); // CLR.L (A2)
    }

    BOOST_AUTO_TEST_CASE(cmp)
    {
        validateDecode(0b1011'000'001'010'010u, instructions::CMP, L"CMP"); // CMP.W D0, (A2)
    }

    BOOST_AUTO_TEST_CASE(cmpa)
    {
        validateDecode(0b1011'000'011'010'010u, instructions::CMPA, L"CMPA"); // CMPA.W A0, (A2)
    }

    BOOST_AUTO_TEST_CASE(cmpi)
    {
        validateDecode(0b0000'1100'01'010'010u, instructions::CMPI, L"CMPI"); // CMPI.W xxx, (A2) 
    }

    BOOST_AUTO_TEST_CASE(cmpm)
    {
        validateDecode(0b1011'000'101'001'010u, instructions::CMPM, L"CMPM"); // CMPM.W (A0)+, (A2)+
    }

    BOOST_AUTO_TEST_CASE(dbcc)
    {
        validateDecode(0b0101'0111'11001'010u, instructions::DBCC, L"DBCC"); // DBEQ D2
    }

    BOOST_AUTO_TEST_CASE(divs)
    {
        validateDecode(0b1000'011'111'010'010u, instructions::DIVS, L"DIVS"); //DIVS.W (A2), D3
        // DIVS.L is 68020. 68030, 68040 only
    }

    BOOST_AUTO_TEST_CASE(divsl)
    {
        // DIVSL is 68020. 68030, 68040 only
    }

    BOOST_AUTO_TEST_CASE(divu)
    {
        validateDecode(0b1000'011'011'010'010u, instructions::DIVU, L"DIVU"); //DIVU.W (A2), D3
        // DIVU.L is 68020. 68030, 68040 only
    }

    BOOST_AUTO_TEST_CASE(divul)
    {
        // DIVUL is 68020. 68030, 68040 only
    }

    BOOST_AUTO_TEST_CASE(eor)
    {
        validateDecode(0b1011'010'101'010'010u, instructions::EOR, L"EOR"); // EOR.W D2, (A2)
    }

    BOOST_AUTO_TEST_CASE(eori)
    {
        validateDecode(0b0000'1010'01'010'010u, instructions::EORI, L"EORI"); // EORI.W xx, (A2)
    }

    BOOST_AUTO_TEST_CASE(eori2ccr)
    {
        validateDecode(0b0000'1010'0011'1100u, instructions::EORI2CCR, L"EORI2CCR");
    }

    BOOST_AUTO_TEST_CASE(exg)
    {
        validateDecode(0b1100'100'1'01000'010u, instructions::EXG, L"EXG"); // EXG D4,D2
        validateDecode(0b1100'100'1'01001'010u, instructions::EXG, L"EXG"); // EXG A4,A2
        validateDecode(0b1100'100'1'10001'010u, instructions::EXG, L"EXG"); // EXG D4,A2
    }

    BOOST_AUTO_TEST_CASE(ext)
    {
        validateDecode(0b0100'100'010'000'010u, instructions::EXT, L"EXT"); // EXT.W D2
        validateDecode(0b0100'100'011'000'110u, instructions::EXT, L"EXT"); // EXT.L D6
    }

    BOOST_AUTO_TEST_CASE(extb)
    {
        validateDecode(0b0100'100'111'000'110u, instructions::UNKNOWN, L"UNKNOWN"); // EXTB.L D6 only MC68020+
    }

    BOOST_AUTO_TEST_CASE(illegal)
    {
        validateDecode(0b0100'1010'1111'1100u, instructions::ILLEGAL, L"ILLEGAL");
    }

    BOOST_AUTO_TEST_CASE(jmp)
    {
        validateDecode(0b0100'1110'11'010'010u, instructions::JMP, L"JMP"); // JMP (A2)
    }

    BOOST_AUTO_TEST_CASE(jsr)
    {
        validateDecode(0b0100'1110'10'010'010u, instructions::JSR, L"JSR"); // JSR (A2)
    }

    BOOST_AUTO_TEST_CASE(lea)
    {
        validateDecode(0b0100'011'111'010'010u, instructions::LEA, L"LEA"); // LEA (A2), A3
    }

    BOOST_AUTO_TEST_CASE(link)
    {
        validateDecode(0b0100'1110'0101'0'010u, instructions::LINK, L"LINK"); // LINK.W xxx, A2
    }

    BOOST_AUTO_TEST_CASE(lsl)
    {
        validateDecode(0b1110'011'1'00'0'01'010u, instructions::LSL, L"LSL"); // LSL.B #3, D2
        validateDecode(0b1110'001'1'11'010'010u, instructions::LSL, L"LSL"); // LSL.B #1, (A2)
    }

    BOOST_AUTO_TEST_CASE(lsr)
    {
        validateDecode(0b1110'011'0'00'1'01'010u, instructions::LSR, L"LSR"); // LSR.B D3, D2
        validateDecode(0b1110'001'0'11'010'010u, instructions::LSR, L"LSR"); // LSL.B #1, (A2)
    }

    BOOST_AUTO_TEST_CASE(move)
    {
        validateDecode(0b00'01'000'000'000'010u, instructions::MOVE, L"MOVE"); // MOVE.B D2,D0
        validateDecode(0b00'11'100'000'010'010u, instructions::MOVE, L"MOVE"); // MOVE.L (A2),D4
        validateDecode(0b00'01'001'011'000'000u, instructions::MOVE, L"MOVE"); // MOVE.b d0,(a1)+
    }

    BOOST_AUTO_TEST_CASE(movea)
    {
        validateDecode(0b00'10'000'001'000'010u, instructions::MOVEA, L"MOVEA"); // MOVEA.W D2,A0
        validateDecode(0b00'11'100'001'010'010u, instructions::MOVEA, L"MOVEA"); // MOVEA.L (A2),A4
    }

    BOOST_AUTO_TEST_CASE(moveccr)
    {
        // 68010 and up only
        validateDecode(0b0100'0010'11'010'010u, instructions::UNKNOWN, L"UNKNOWN");
    }

    BOOST_AUTO_TEST_CASE(move2ccr)
    {
        validateDecode(0b0100'0100'11'010'010u, instructions::MOVE2CCR, L"MOVE2CCR"); // MOVE (A2), CCR
    }

    BOOST_AUTO_TEST_CASE(movesr)
    {
        validateDecode(0b0100'0000'11'010'010u, instructions::MOVESR, L"MOVESR");  // MOVE SR,(A2)
    }

    BOOST_AUTO_TEST_CASE(movem)
    {
        validateDecode(0b0100'1'0'001'0'010'010u, instructions::MOVEM, L"MOVEM"); // MOVEM.W ...,(A2)
        validateDecode(0b0100'1'0'001'1'010'010u, instructions::MOVEM, L"MOVEM"); // MOVEM.L ...,(A2)
        validateDecode(0b0100'1'1'001'0'010'010u, instructions::MOVEM, L"MOVEM"); // MOVEM.W (A2),...
        validateDecode(0b0100'1'1'001'1'010'010u, instructions::MOVEM, L"MOVEM"); // MOVEM.L (A2),...
    }

    BOOST_AUTO_TEST_CASE(movep)
    {
        validateDecode(0b0000'110'100'001'010u, instructions::MOVEP, L"MOVEP"); // MOVEP.W (x,A2), D6
        validateDecode(0b0000'110'111'001'010u, instructions::MOVEP, L"MOVEP"); // MOVEP.L D6,(x,A2)
    }

    BOOST_AUTO_TEST_CASE(moveq)
    {
        validateDecode(0b0111'101'0'0000'0110u, instructions::MOVEQ, L"MOVEQ"); // MOVEQ.L #6, D5
    }

    BOOST_AUTO_TEST_CASE(muls)
    {
        validateDecode(0b1100'101'111'010'011u, instructions::MULS, L"MULS"); // MULS.W D5, (A3)
    }

    BOOST_AUTO_TEST_CASE(mulu)
    {
        validateDecode(0b1100'011'011'010'010u, instructions::MULU, L"MULU"); // MULU.W D3, (A2)
    }

    BOOST_AUTO_TEST_CASE(nbcd)
    {
        validateDecode(0b0100'1000'00'010'010u, instructions::NBCD, L"NBCD"); // NBCD.B (A2)
    }

    BOOST_AUTO_TEST_CASE(neg)
    {
        validateDecode(0b0100'0100'00'010'010u, instructions::NEG, L"NEG"); // NEG.B (A2) 
    }

    BOOST_AUTO_TEST_CASE(negx)
    {
        validateDecode(0b0100'0000'00'010'010u, instructions::NEGX, L"NEGX"); // NEGX.W (A2) 
    }

    BOOST_AUTO_TEST_CASE(nop)
    {
        validateDecode(0b0100'1110'0111'0001u, instructions::NOP, L"NOP");
    }

    BOOST_AUTO_TEST_CASE(not_)
    {
        validateDecode(0b0100'0110'10'010'010u, instructions::NOT, L"NOT"); // NOT.L (A2)
    }

    BOOST_AUTO_TEST_CASE(or_)
    {
        validateDecode(0b1000'100'000'010'010u, instructions::OR, L"OR"); // OR.B (A2),D0
        validateDecode(0b1000'100'101'010'010u, instructions::OR, L"OR"); // OR.W D0,(A2)
    }

    BOOST_AUTO_TEST_CASE(ori)
    {
        validateDecode(0b0000'0000'0000'0010u, instructions::ORI, L"ORI");
    }

    BOOST_AUTO_TEST_CASE(ori2ccr)
    {
        validateDecode(0b0000'0000'0011'1100u, instructions::ORI2CCR, L"ORI2CCR");
    }

    BOOST_AUTO_TEST_CASE(pea)
    {
        validateDecode(0b0100'1000'01'010'010u, instructions::PEA, L"PEA"); // PEA (A2)
    }

    BOOST_AUTO_TEST_CASE(rol)
    {
        validateDecode(0b1110'010'1'00'0'11'011u, instructions::ROL, L"ROL"); // ROL.B #2, D3
        validateDecode(0b1110'011'1'11'010'010u, instructions::ROL, L"ROL");  // ROL.W #1, (A2)
    }

    BOOST_AUTO_TEST_CASE(ror)
    {
        validateDecode(0b1110'010'0'00'0'11'011u, instructions::ROR, L"ROR"); // ROR.B #2, D3
        validateDecode(0b1110'011'0'11'010'010u, instructions::ROR, L"ROR");  // ROR.W #1, (A2)
    }

    BOOST_AUTO_TEST_CASE(roxl)
    {
        validateDecode(0b1110'010'1'00'0'10'011u, instructions::ROXL, L"ROXL"); // ROXL.B #2, D3
        validateDecode(0b1110'010'1'11'010'010u, instructions::ROXL, L"ROXL");  // ROXL.W #1, (A2)
    }

    BOOST_AUTO_TEST_CASE(roxr)
    {
        validateDecode(0b1110'010'0'00'0'10'011u, instructions::ROXR, L"ROXR"); // ROXR.B #2, D3
        validateDecode(0b1110'010'0'11'010'010u, instructions::ROXR, L"ROXR");  // ROXR.W #1, (A2)
    }

    BOOST_AUTO_TEST_CASE(rtr)
    {
        validateDecode(0b0100'1110'0111'0111u, instructions::RTR, L"RTR");
    }

    BOOST_AUTO_TEST_CASE(rts)
    {
        validateDecode(0b0100'1110'0111'0101u, instructions::RTS, L"RTS");
    }

    BOOST_AUTO_TEST_CASE(sbcd)
    {
        validateDecode(0b1000'010'10000'0'011u, instructions::SBCD, L"SBCD"); // SBCD D3, D2
        validateDecode(0b1000'010'10000'1'011u, instructions::SBCD, L"SBCD"); // SBCD -(A3), -(A2)
    }

    BOOST_AUTO_TEST_CASE(scc)
    {
        validateDecode(0b0101'0010'11'010'010u, instructions::SCC, L"SCC"); // SHI (A2)
    }

    BOOST_AUTO_TEST_CASE(sub)
    {
        validateDecode(0b1001'001'001'010'010u, instructions::SUB, L"SUB"); // SUB.W (A2), D1
        validateDecode(0b1001'001'110'010'010u, instructions::SUB, L"SUB"); // SUB.L D1, (A2)
    }

    BOOST_AUTO_TEST_CASE(suba)
    {
        validateDecode(0b1001'001'111'000'010u, instructions::SUBA, L"SUBA"); // SUBA.L (A2), A1
    }

    BOOST_AUTO_TEST_CASE(subi)
    {
        validateDecode(0b0000'0100'10'010'010u, instructions::SUBI, L"SUBI"); // SUBI.L xxx, (A2)
    }

    BOOST_AUTO_TEST_CASE(subq)
    {
        validateDecode(0b0101'111'1'00'010'010u, instructions::SUBQ, L"SUBQ");
    }

    BOOST_AUTO_TEST_CASE(subx)
    {
        validateDecode(0b1001'010'1'01'00'0'010u, instructions::SUBX, L"SUBX");
    }

    BOOST_AUTO_TEST_CASE(swap)
    {
        validateDecode(0b0100'1000'0100'0'011u, instructions::SWAP, L"SWAP"); // SWAP D3
    }

    BOOST_AUTO_TEST_CASE(tas)
    {
        validateDecode(0b0100'1010'11'010'010u, instructions::TAS, L"TAS"); // TAS (A2)
    }

    BOOST_AUTO_TEST_CASE(trap)
    {
        validateDecode(0b0100'1110'0100'0101u, instructions::TRAP, L"TRAP"); // TRAP #5
    }

    BOOST_AUTO_TEST_CASE(trapv)
    {
        validateDecode(0b0100'1110'0111'0110u, instructions::TRAPV, L"TRAPV");
    }

    BOOST_AUTO_TEST_CASE(tst)
    {
        validateDecode(0b0100'1010'10'010'010u, instructions::TST, L"TST"); // TST.L (A2)
    }

    BOOST_AUTO_TEST_CASE(unlk)
    {
        validateDecode(0b0100'1110'0101'1'010u, instructions::UNLK, L"UNLK"); // UNLK A2
    }
    
}