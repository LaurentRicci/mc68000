#include <boost/test/unit_test.hpp>
#include "asmparser.h"
#include "util.h"
namespace elfTest
{
    BOOST_AUTO_TEST_SUITE(elfTest)

    BOOST_AUTO_TEST_CASE(elf_directives)
    {
        // Arrange
        auto sourceCode =
            "#NO_APP\n"
            "   .text\n"
            "   .align 2\n"
            "   .globl  add\n"
            "   .type   add, @function\n"
            "add:\n"
            "    move.l 4(%sp), %d0\n"
            "    add.l  8(%sp), %d0\n"
            "    rts\n"
            "   .size   add, . - add\n"
            " .ident     \"GCC: (GNU) 16.1.0\"\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_file)
    {
        // Arrange
        auto sourceCode =
            "#TEST file directive\n"
            "   .file  \"test.c\"\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_file_bad)
    {
        // Arrange
        auto sourceCode =
            "#TEST file directive\n"
            "   .file  \n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_NE(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_text)
    {
        // Arrange
        auto sourceCode =
            "#TEST text directive\n"
            "   .text\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_text2)
    {
        // Arrange
        auto sourceCode =
            "    .text\n"
            "\n"
            "# ------------------------------------------------------------\n"
            "# Transfert instructions\n"
            "# ------------------------------------------------------------\n"
            "            move.b %d0,%d1\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_text_bad)
    {
        // Arrange
        auto sourceCode =
            "#TEST text directive\n"
            "   .text dummy\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_NE(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_align)
    {
        // Arrange
        auto sourceCode =
            "#TEST align directive\n"
            "   .align 2\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_align_bad)
    {
        // Arrange
        auto sourceCode =
            "#TEST align directive\n"
            "   .align \n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_NE(0, error);
    }


    BOOST_AUTO_TEST_CASE(elf_directive_globl)
    {
        // Arrange
        auto sourceCode =
            "#TEST globl directive\n"
            "   .globl  add\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_globl_bad)
    {
        // Arrange
        auto sourceCode =
            "#TEST globl directive\n"
            "   .globl \n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_NE(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_type)
    {
        // Arrange
        auto sourceCode =
            "#TEST type directive\n"
            "   .type   add, @function\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_type_bad)
    {
        // Arrange
        auto sourceCode =
            "#TEST type directive\n"
            "   .type  add \n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_NE(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_size)
    {
        // Arrange
        auto sourceCode =
            "#TEST size directive\n"
            "   .size   add, .-add\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_size_bad)
    {
        // Arrange
        auto sourceCode =
            "#TEST size directive\n"
            "   .size  add, - add \n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_NE(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_ident)
    {
        // Arrange
        auto sourceCode =
            "#TEST ident directive\n"
            "   .ident   \"GCC: (GNU) 10.2.0\"\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_ident_bad)
    {
        // Arrange
        auto sourceCode =
            "#TEST ident directive\n"
            "   .ident \n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_NE(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_comment)
    {
        // Arrange
        auto sourceCode =
            "#COMMENT\n";
            "    move.l 4(sp), d0\n"
            "    # --- Labels locaux générés par GCC ---\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_dregister)
    {
        // Arrange
        auto sourceCode =
            "    move.l %d0, %d1\n"
            "    move.l %d2, %d3\n"
            "    move.l %d4, %d5\n"
            "    move.l %d6, %d7\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_aregister_syntax)
    {
        // Arrange
        auto sourceCode =
            "    move.l %a0, %a1\n"
            "    move.l %a2, %a3\n"
            "    move.l %a4, %a5\n"
            "    move.l %a6, %a7\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_aregister_code)
    {
        // Arrange
        auto sourceCode = "    move.l %a6, %a7\n";
        asmparser parser;
        // Act
        auto opcode = parser.parseText(sourceCode);
        // Assert
        validate_hasValue<uint16_t>(0b00'10'111'001'001'110, opcode);
    }

    BOOST_AUTO_TEST_CASE(elf_sp)
    {
        // Arrange
        auto sourceCode =
            "    move.l %d0, %sp\n"
            "    move.l %sp, sp\n"
            "    move.l d0, %SP\n"
            "    move.l %SP, SP\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_fp)
    {
        // Arrange
        auto sourceCode =
            "    link %fp,#-8\n"
            "    unlk %fp\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }
    BOOST_AUTO_TEST_CASE(elf_link_fp)
    {
        asmparser parser;
        auto opcode = parser.parseText("  link %fp, #-16\n");
        validate_hasValue<uint16_t>(0b0100'1110'0101'0'110, opcode);

        const auto& code = validate_codeSize(parser, 2);
        BOOST_CHECK_EQUAL(0xfff0, code[1]);
    }

    BOOST_AUTO_TEST_CASE(elf_dregister2)
    {
        asmparser parser;
        auto opcode = parser.parseText("  move.l %d0, %d1\n");
        validate_hasValue<uint16_t>(0b00'10'001'000'000'000, opcode);
    }

    BOOST_AUTO_TEST_CASE(elf_exg_dregister)
    {
        asmparser parser;
        auto opcode = parser.parseText("  exg %d0, %d1\n");
        validate_hasValue<uint16_t>(0b1100'000'1'01000'001, opcode);
    }

    BOOST_AUTO_TEST_CASE(elf_exg_aregister)
    {
        asmparser parser;
        auto opcode = parser.parseText("  exg %a0, %fp\n");
        validate_hasValue<uint16_t>(0b1100'000'1'01001'110, opcode);
    }

    BOOST_AUTO_TEST_CASE(elf_movem_syntax)
    {
        // Arrange
        auto sourceCode = "  movem.l #0x00ff, (a0)\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_movem_code)
    {
        // Arrange
        auto sourceCode = "  movem.l #0x00ff, (%a0)\n";
        asmparser parser;

        // Act
        auto opcode = parser.parseText(sourceCode);

        // Assert
        validate_hasValue<uint16_t>(0b01001'0'001'1'010'000, opcode);
        const auto& code = validate_codeSize(parser, 2);
        BOOST_CHECK_EQUAL(0b00000000'11111111, code[1]); // bitmask for d0-d7
    }

    BOOST_AUTO_TEST_CASE(elf_movem_toMemoryRange)
    {
        asmparser parser;
        auto opcode = parser.parseText("  movem.l #0x045f,-(a3)\n");
        validate_hasValue<uint16_t>(0b0100'1'0'001'1'100'011, opcode);

        const auto& code = validate_codeSize(parser, 2);
        BOOST_CHECK_EQUAL(0b11111010'00100000, code[1]); // bitmask for d0,d1,d2,d3,d4,d6,a2
    }

    BOOST_AUTO_TEST_CASE(elf_movem_decimal)
    {
        asmparser parser;
        auto opcode = parser.parseText("  movem.l #15360,-(%sp)\n"); // 15360 is 0011'1100'0000'0000 which is bitmask for a2,a3,a4,a5
        validate_hasValue<uint16_t>(0b01001'0'001'1'100'111, opcode);

        const auto& code = validate_codeSize(parser, 2);
        BOOST_CHECK_EQUAL(0b00000000'00111100, code[1]); // bitmask for a2,a3,a4,a5
    }

    BOOST_AUTO_TEST_CASE(elfmovem_fromMemoryRange)
    {
        asmparser parser;
        auto opcode = parser.parseText("  movem.l (a3)+, #0x045f\n");
        validate_hasValue<uint16_t>(0b0100'1'1'001'1'011'011, opcode);

        const auto& code = validate_codeSize(parser, 2);
        BOOST_CHECK_EQUAL(0b00000100'01011111, code[1]); // bitmask for d0,d1,d2,d3,d4,d6,a2
    }
    BOOST_AUTO_TEST_CASE(elf_addressing)
    {
        // Arrange
        auto sourceCode =
            "      move.l  (a0,d1.l),d0\n"
            "      move.l  (%a0,%d1.l),%d0\n";
        asmparser parser;

        // Act
        auto error = parser.checkSyntax(sourceCode);

        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_import)
    {
        // Arrange
        auto sourceCode =
            "#TEST import directive\n"
            "   .import  symbol\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_importmultiple)
    {
        // Arrange
        auto sourceCode =
            "#TEST import directive\n"
            "   .import  symbol1, symbol2, symbol3\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_export)
    {
        // Arrange
        auto sourceCode =
            "#TEST export directive\n"
            "   .export  symbol\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(elf_directive_exportmultiple)
    {
        // Arrange
        auto sourceCode =
            "#TEST export directive\n"
            "   .export  symbol1, symbol2, symbol3\n";
        asmparser parser;
        // Act
        auto error = parser.checkSyntax(sourceCode);
        // Assert
        BOOST_CHECK_EQUAL(0, error);
    }

    BOOST_AUTO_TEST_CASE(import_use_bsr)
    {
        // Arrange
        auto sourceCode =
            "#TEST import directive\n"
            " .import  symbol\n"
            "  bsr     symbol\n";

        asmparser parser;
        // Act
        std::any result = parser.parseText(sourceCode);
        // Assert no errors
        auto errs = parser.getErrors();
        BOOST_CHECK_EQUAL(0, errs.get().size());

        // Assert correct relocation
        auto relocations = parser.getRelocations();
        BOOST_CHECK_EQUAL(1, relocations.size());
        BOOST_CHECK_EQUAL("symbol", relocations[0].symbol);
        BOOST_CHECK_EQUAL(RelocationType::PCREL_16, relocations[0].type);

        // Assert correct code generated
        const auto& code = validate_codeSize(parser, 2);
        BOOST_CHECK_EQUAL(0x6100, code[0]); // bsr opcode
        BOOST_CHECK_EQUAL(0x0000, code[1]);

    }

    BOOST_AUTO_TEST_CASE(import_use_jsr)
    {
        // Arrange
        auto sourceCode =
            "#TEST import directive\n"
            " .import  symbol\n"
            "  jsr     symbol\n";

        asmparser parser;
        // Act
        std::any result = parser.parseText(sourceCode);
        // Assert no errors
        auto errs = parser.getErrors();
        BOOST_CHECK_EQUAL(0, errs.get().size());

        // Assert correct relocation
        auto relocations = parser.getRelocations();
        BOOST_CHECK_EQUAL(1, relocations.size());
        BOOST_CHECK_EQUAL("symbol", relocations[0].symbol);
        BOOST_CHECK_EQUAL(RelocationType::PCREL_16, relocations[0].type);

        // Assert correct code generated
        const auto& code = validate_codeSize(parser, 2);
        BOOST_CHECK_EQUAL(0x4eba, code[0]); // jsr opcode
        BOOST_CHECK_EQUAL(0x0000, code[1]);
    }

    BOOST_AUTO_TEST_CASE(import_use_jmp)
    {
        // Arrange
        auto sourceCode =
            "#TEST import directive\n"
            " .import  symbol\n"
            "  jmp     symbol\n";

        asmparser parser;
        // Act
        std::any result = parser.parseText(sourceCode);
        // Assert no errors
        auto errs = parser.getErrors();
        BOOST_CHECK_EQUAL(0, errs.get().size());

        // Assert correct relocation
        auto relocations = parser.getRelocations();
        BOOST_CHECK_EQUAL(1, relocations.size());
        BOOST_CHECK_EQUAL("symbol", relocations[0].symbol);
        BOOST_CHECK_EQUAL(RelocationType::PCREL_16, relocations[0].type);

        // Assert correct code generated
        const auto& code = validate_codeSize(parser, 2);
        BOOST_CHECK_EQUAL(0b0100'1110'11'111'010, code[0]); // jmp opcode
        BOOST_CHECK_EQUAL(0x0000, code[1]);
    }

    BOOST_AUTO_TEST_CASE(import_use_lea)
    {
        // Arrange
        auto sourceCode =
            "#TEST import directive\n"
            " .import  symbol\n"
            "  lea     symbol, a0\n";

        asmparser parser;
        // Act
        std::any result = parser.parseText(sourceCode);
        // Assert no errors
        auto errs = parser.getErrors();
        BOOST_CHECK_EQUAL(0, errs.get().size());

        // Assert correct relocation
        auto relocations = parser.getRelocations();
        BOOST_CHECK_EQUAL(1, relocations.size());
        BOOST_CHECK_EQUAL("symbol", relocations[0].symbol);
        BOOST_CHECK_EQUAL(RelocationType::PCREL_16, relocations[0].type);

        // Assert correct code generated
        const auto& code = validate_codeSize(parser, 2);
        BOOST_CHECK_EQUAL(0b0100'000'111'111'010, code[0]); // lea a0,opcode
        BOOST_CHECK_EQUAL(0x0000, code[1]);
    }

    BOOST_AUTO_TEST_CASE(import_use_bcs)
    {
        // Arrange
        auto sourceCode =
            "#TEST import directive\n"
            " .import  symbol\n"
            "  bcs     symbol\n";

        asmparser parser;
        // Act
        std::any result = parser.parseText(sourceCode);
        // Assert no errors
        auto errs = parser.getErrors();
        BOOST_CHECK_EQUAL(0, errs.get().size());

        // Assert correct relocation
        auto relocations = parser.getRelocations();
        BOOST_CHECK_EQUAL(1, relocations.size());
        BOOST_CHECK_EQUAL("symbol", relocations[0].symbol);
        BOOST_CHECK_EQUAL(RelocationType::PCREL_16, relocations[0].type);

        // Assert correct code generated
        const auto& code = validate_codeSize(parser, 2);
        BOOST_CHECK_EQUAL(0b0110'0101'0000'0000, code[0]); // bcs opcode
        BOOST_CHECK_EQUAL(0x0000, code[1]);
    }

    BOOST_AUTO_TEST_CASE(import_defined_after)
    {
        // Arrange
        auto sourceCode =
            " .import  symbol\n"
            "symbol nop\n";

        asmparser parser;
        // Act
        std::any result = parser.parseText(sourceCode);
        // Assert one error
        auto errs = parser.getErrors();
        BOOST_CHECK_EQUAL(1, errs.get().size());

        // Assert correct relocation
        auto relocations = parser.getRelocations();
        BOOST_CHECK_EQUAL(0, relocations.size());

        // Assert correct code generated
        const auto& code = validate_codeSize(parser, 1);
        BOOST_CHECK_EQUAL(0x4e71, code[0]); // nop
    }

    BOOST_AUTO_TEST_CASE(import_defined_before)
    {
        // Arrange
        auto sourceCode =
            "symbol nop\n"
            " .import  symbol\n";

        asmparser parser;
        // Act
        std::any result = parser.parseText(sourceCode);
        // Assert one error
        auto errs = parser.getErrors();
        BOOST_CHECK_EQUAL(1, errs.get().size());

        // Assert correct relocation
        auto relocations = parser.getRelocations();
        BOOST_CHECK_EQUAL(0, relocations.size());

        // Assert correct code generated
        const auto& code = validate_codeSize(parser, 1);
        BOOST_CHECK_EQUAL(0x4e71, code[0]); // nop
    }

    BOOST_AUTO_TEST_CASE(export_defined_before)
    {
        // Arrange
        auto sourceCode =
            "symbol nop\n"
            " .export  symbol\n";

        asmparser parser;
        // Act
        std::any result = parser.parseText(sourceCode);
        // Assert no errors
        auto errs = parser.getErrors();
        BOOST_CHECK_EQUAL(0, errs.get().size());

        // Assert correct relocation
        auto relocations = parser.getRelocations();
        BOOST_CHECK_EQUAL(1, relocations.size());
        auto it = relocations.begin();
        BOOST_CHECK_EQUAL("symbol", it->symbol);

        // Assert coorect label defined
        auto labels = parser.getLabels();
        BOOST_CHECK_EQUAL(1, labels.size());
        auto label = labels.find("symbol")->second;
        BOOST_CHECK_EQUAL(false, label.imported);
        BOOST_CHECK_EQUAL(true, label.defined);
        BOOST_CHECK_EQUAL(true, label.exported);

        // Assert correct code generated
        const auto& code = validate_codeSize(parser, 1);
        BOOST_CHECK_EQUAL(0x4e71, code[0]); // nop
    }

    BOOST_AUTO_TEST_CASE(export_defined_after)
    {
        // Arrange
        auto sourceCode =
            " .export  symbol\n"
            "symbol nop\n";

        asmparser parser;
        // Act
        std::any result = parser.parseText(sourceCode);
        // Assert no errors
        auto errs = parser.getErrors();
        BOOST_CHECK_EQUAL(0, errs.get().size());

        // Assert correct relocation
        auto relocations = parser.getRelocations();
        BOOST_CHECK_EQUAL(1, relocations.size());
        auto it = relocations.begin();
        BOOST_CHECK_EQUAL("symbol", it->symbol);

        // Assert coorect label defined
        auto labels = parser.getLabels();
        BOOST_CHECK_EQUAL(1, labels.size());
        auto label = labels.find("symbol")->second;
        BOOST_CHECK_EQUAL(false, label.imported);
        BOOST_CHECK_EQUAL(true, label.defined);
        BOOST_CHECK_EQUAL(true, label.exported);

        // Assert correct code generated
        const auto& code = validate_codeSize(parser, 1);
        BOOST_CHECK_EQUAL(0x4e71, code[0]); // nop
    }

    BOOST_AUTO_TEST_CASE(export_not_defined)
    {
        // Arrange
        auto sourceCode =
            " .export  symbol\n"
            " nop\n";

        asmparser parser;
        // Act
        std::any result = parser.parseText(sourceCode);
        // Assert one error
        auto errs = parser.getErrors();
        BOOST_CHECK_EQUAL(1, errs.get().size());

        // Assert correct relocation
        auto relocations = parser.getRelocations();
        BOOST_CHECK_EQUAL(1, relocations.size());
        auto it = relocations.begin();
        BOOST_CHECK_EQUAL("symbol", it->symbol);

        // Assert label not defined
        auto labels = parser.getLabels();
        BOOST_CHECK_EQUAL(0, labels.size());

        // Assert correct code generated
        const auto& code = validate_codeSize(parser, 1);
        BOOST_CHECK_EQUAL(0x4e71, code[0]); // nop
    }

    BOOST_AUTO_TEST_SUITE_END()
}
