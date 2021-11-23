/**
 * Project: Compiler IFJ21
 *
 * @file scannerTests.cpp
 * @authors Andrej Binovsky <xbinov00>
 *          Zdenek Lapes    <xlapes02>
 */

#include <gtest/gtest.h>
#include <iostream>
#include <cstdlib>

extern "C" {
#include "../src/scanner.h"
}

class Code : public ::testing::Test {
protected:
    std::string compiler = " ./compiler";
    Code() {
    }

    ~Code() {
    }

    void testCode(const std::string testingString, const int expectedCode) {
        const std::string testingStringF = "echo \"" + testingString + "\"" + compiler;
        int status = system(testingStringF.c_str());
        EXPECT_EQ(status, expectedCode) << "TEST FAILED: \t" << status << " != " << expectedCode;
    }
};

// 0 - should be ok
TEST_F(Code, Exit_Code_0) {
    const int expectedCode = 0;
    const std::string testCase1 = R"(
ahoj
helo ) (
dw\n\0
)";
    testCode(testCase1, expectedCode);
}

// 1 - chyba v programu v rámci lexikální analýzy (chybná struktura aktuálního lexému)
TEST_F(Code, Exit_Code_1) {
    const int expectedCode = 1;
    const std::string testCase1;
}

// 2 - chyba v programu v rámci syntaktické analýzy (chybná syntaxe programu)
TEST_F(Code, Exit_Code_2) {
    const int expectedCode = 2;
    const std::string testCase1;
    //    testCode(testCase1, expectedCode);
}

// 3 - sémantická chyba v programu – nedefinovaná funkce/proměnná, pokus o redefi- nici proměnné, atp
TEST_F(Code, Exit_Code_3) {
    const int expectedCode = 3;
    const std::string testCase1;
    //    testCode(testCase1, expectedCode);
}

// 5 - sémantická chyba v programu – špatný počet / typ parametrů či návratových hodnot u volání funkce či návratu z funkce.
TEST_F(Code, Exit_Code_5) {
    const int expectedCode = 5;
    const std::string testCase1;
    //    testCode(testCase1, expectedCode);
}

// 6 - sémantická chyba typové kompatibility v aritmetických, řetězcových a relačních výrazech.
TEST_F(Code, Exit_Code_6) {
    const int expectedCode = 6;
    const std::string testCase1;
//    testCode(testCase1, expectedCode);
}

// 7 - ostatní sémantické chyby.
TEST_F(Code, Exit_Code_7) {
    const int expectedCode = 7;
    const std::string testCase1;
    //    testCode(testCase1, expectedCode);
}

// 8 - běhová chyba při práci s neočekávanou hodnotou nil.
TEST_F(Code, Exit_Code_8) {
    const int expectedCode = 8;
    const std::string testCase1;
    //    testCode(testCase1, expectedCode);
}

// 9 - běhová chyba celočíselného dělení nulovou konstantou.
TEST_F(Code, Exit_Code_9) {
    const int expectedCode = 9;
    const std::string testCase1;
    //    testCode(testCase1, expectedCode);
}

// 99 - interní chyba překladače tj. neovlivněná vstupním programem (např. chyba alo- kace paměti atd.).
TEST_F(Code, Exit_Code_99) {
    const int expectedCode = 99;
    const std::string testCase1;
    //    testCode(testCase1, expectedCode);
}
