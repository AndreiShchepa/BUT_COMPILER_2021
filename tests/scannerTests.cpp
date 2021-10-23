/**
 * Project: Compiler IFJ21
 *
 * @file scannerTests.cpp
 * @authors Andrej Binovsky <xbinov00>
 *          Zdenek Lapes    <xlapes02>
 */

#include <gtest/gtest.h>
#include <iostream>
#include <filesystem>

extern "C" {
#include "../src/scanner.h"
#include "../src/error.h"
}

class Token : public ::testing::Test {
protected:
    token_t token;
    std::FILE *old_stdin;
    std::FILE *stream;

    Token() {
        str_init(&token.attr.id, 20);
        old_stdin = nullptr;
        stream = nullptr;
    }

    ~Token() {
        str_free(&token.attr.id);
    }

    void createBuffer(const std::string &input) {
        old_stdin = stdin;
        stream = std::tmpfile();
        std::fputs(input.c_str(), stream);
        std::rewind(stream);
        if (stream == nullptr)
            FAIL();
        set_source_file(stream);
    }

    void testingPrint() {
        for (int c = 0; c != EOF; c = fgetc(stream))
            printf("%c", c);
    }

    void closeBuffer() {
        stdin = old_stdin;
        fclose(stream);
    }

    void tokenTestNoErr(std::string expectedTokenStr, int TYPE) {
        EXPECT_EQ(NO_ERR, get_next_token(&token)) << "ExpectedTokenStr has value: " << expectedTokenStr;
        std::string tokenStr = token.attr.id.str;
        EXPECT_TRUE(tokenStr == expectedTokenStr)
                            << "tokenStr != expectedTokenStr: " << tokenStr << " != " << expectedTokenStr;
        EXPECT_EQ(TYPE, token.type) << "TokenStr has value: " << tokenStr;
    }

    void tokenTestErr(std::string buffer) {
        createBuffer(buffer);
        EXPECT_EQ(SCANNER_ERR, get_next_token(&token)) << "This token should be incorrect: " << buffer;
        closeBuffer();
    }
};

TEST_F(Token, Integer) {
    createBuffer("0      1  -0 0999 012345678910");
    tokenTestNoErr("0", T_INT);
    tokenTestNoErr("1", T_INT);
    tokenTestNoErr("-", T_MINUS);
    tokenTestNoErr("0", T_INT);
    tokenTestNoErr("0999", T_INT);
    tokenTestNoErr("012345678910", T_INT);
    tokenTestErr("001");
}

#if 1

TEST_F(Token, Double) {
    createBuffer("1.0 0.45 9.88 0.001 0001.001 -1.5e+07 +0.04E-00014 3.4e0 3.4e3");
    tokenTestNoErr("1.0", T_FLOAT);
    tokenTestNoErr("0.45", T_FLOAT);
    tokenTestNoErr("9.88", T_FLOAT);
    tokenTestNoErr("0.001", T_FLOAT);
    tokenTestNoErr("0001.001", T_FLOAT);
    tokenTestNoErr("-", T_MINUS);
    tokenTestNoErr("1.5e+07", T_FLOAT);
    tokenTestNoErr("+", T_PLUS);
    tokenTestNoErr("0.04E-00014", T_FLOAT);
    tokenTestNoErr("3.4e0", T_FLOAT);
    tokenTestNoErr("3.4e3", T_FLOAT);

    tokenTestErr("3.4e");
    tokenTestErr("3.4e");
    tokenTestErr("3.4e");
    tokenTestErr("3.a");
    tokenTestErr("0.04E-+0003");
}

TEST_F(Token, string) { // I cut next part of lexem after err - ("hello \a world") is  ("hello \a)
    createBuffer(R"("Hello' World"
                 "Printing some numbers: 0 1 33 3.e10"
                 "Using escape sequences: \" \n \t \\ "
                 "a is: \097")");
    tokenTestNoErr("Hello' World", T_STRING);
    tokenTestNoErr("Printing some numbers: 0 1 33 3.e10", T_STRING);
    tokenTestNoErr("Using escape sequences: \" \n \t \\ ", T_STRING);
    tokenTestNoErr("a is: a", T_STRING);

    tokenTestErr(R"("a is \257")");
    tokenTestErr(R"("Using bad escape sequences: \a")");
    tokenTestErr(R"("hello \n)");
}

TEST_F(Token, Keywords) {
    createBuffer("do global require else if return end local then function nil while");
    tokenTestNoErr("do", T_ID);
    tokenTestNoErr("global", T_ID);
    tokenTestNoErr("require", T_ID);
    tokenTestNoErr("else", T_ID);
    tokenTestNoErr("if", T_ID);
    tokenTestNoErr("return", T_ID);
    tokenTestNoErr("end", T_ID);
    tokenTestNoErr("local", T_ID);
    tokenTestNoErr("then", T_ID);
    tokenTestNoErr("function", T_ID);
    tokenTestNoErr("nil", T_ID);
    tokenTestNoErr("while", T_ID);
}

TEST_F(Token, Operators) {
    createBuffer("opr1 <<opr2+opr3..opr4<==>=opr5*///-==~= #\"x\\nz\"");
    tokenTestNoErr("opr1", T_ID);
    tokenTestNoErr("<", T_LT);
    tokenTestNoErr("<", T_LT);
    tokenTestNoErr("opr2", T_ID);
    tokenTestNoErr("+", T_PLUS);
    tokenTestNoErr("opr3", T_ID);
    tokenTestNoErr("..", T_CONCAT);
    tokenTestNoErr("opr4", T_ID);
    tokenTestNoErr("<=", T_LE);
    tokenTestNoErr("=", T_ASSIGN);
    tokenTestNoErr(">=", T_GE);
    tokenTestNoErr("opr5", T_ID);
    tokenTestNoErr("*", T_MUL);
    tokenTestNoErr("//", T_DIV_INT);
    tokenTestNoErr("/", T_DIV);
    tokenTestNoErr("-", T_MINUS);
    tokenTestNoErr("==", T_EQ);
    tokenTestNoErr("~=", T_NEQ);
    tokenTestNoErr("#", T_LENGTH);
    tokenTestNoErr("x\nz", T_STRING);
}

TEST_F(Token, Functions) {
    createBuffer("global foo : function(string) : string end");
    tokenTestNoErr("global", T_ID);
    tokenTestNoErr("foo", T_ID);
    tokenTestNoErr(":", T_COLON);
    tokenTestNoErr("function", T_ID);
    tokenTestNoErr("(", T_L_ROUND_BR);
    tokenTestNoErr("string", T_ID);
    tokenTestNoErr(")", T_R_ROUND_BR);
    tokenTestNoErr(":", T_COLON);
    tokenTestNoErr("string", T_ID);
    tokenTestNoErr("end", T_ID);
}

TEST_F(Token, Comment) {
    // testing strings
    const std::string line_comment1 = "100 -- this is line comment\n";
    const std::string block_comment1 = R"(100 --[[comment
toto je komentar]])";
    const std::string block_comment2 = R"(--[[comment
toto je komentar]])";

    createBuffer(line_comment1);
    EXPECT_FALSE(get_next_token(&token));
    EXPECT_TRUE(token.type == T_INT);
    EXPECT_FALSE(get_next_token(&token));
    EXPECT_TRUE(token.type == T_EOF);
    closeBuffer();

    createBuffer(block_comment1);
    EXPECT_FALSE(get_next_token(&token)); // 100
    EXPECT_TRUE(token.type == T_INT);
    EXPECT_FALSE(get_next_token(&token)); // comment
    EXPECT_TRUE(token.type == T_EOF); // comment
    closeBuffer();

    createBuffer(block_comment2);
    EXPECT_FALSE(get_next_token(&token)); // 100
    EXPECT_FALSE(get_next_token(&token)); // comment
    EXPECT_TRUE(token.type == T_EOF); // comment
    closeBuffer();
}

#endif

