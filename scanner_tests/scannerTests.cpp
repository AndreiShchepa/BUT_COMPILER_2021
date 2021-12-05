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

#define tokenTestNoErr(expectedTokenStr,expectedType) \
    EXPECT_EQ(NO_ERR, get_next_token(&token)) << "ExpectedTokenStr has value: " << expectedTokenStr; \
    tokenStr = token.attr.id.str; \
    EXPECT_TRUE(tokenStr == expectedTokenStr) \
                        << "tokenStr != expectedTokenStr: " << tokenStr << " != " << expectedTokenStr; \
    EXPECT_EQ(expectedType, token.type) << "TokenStr has value: " << tokenStr;

#define tokenTestNoErrKw(expectedTokenStr, expectedType, expectedKw) \
    EXPECT_EQ(NO_ERR, get_next_token(&token)) << "ExpectedTokenStr has value: " << expectedTokenStr; \
    tokenStr = token.attr.id.str; \
    EXPECT_TRUE(tokenStr == expectedTokenStr) \
                        << "tokenStr != expectedTokenStr: " << tokenStr << " != " << expectedTokenStr; \
    EXPECT_EQ(expectedType, token.type) << "TokenStr has value: " << tokenStr; \
    EXPECT_EQ(expectedKw, token.keyword) << "TokenStr has value: " << tokenStr;

#define tokenTestNoErrNumber(expectedType, nInt, nFloat) \
    EXPECT_EQ(NO_ERR, get_next_token(&token)); \
    if(expectedType == T_INT){ \
        EXPECT_EQ(nInt, token.attr.num_i); \
    } else{ \
        EXPECT_EQ(nFloat, token.attr.num_f);   \
    } \
    EXPECT_EQ(expectedType, token.type);

#define tokenTestErr(buffer) \
    createBuffer(buffer); \
    EXPECT_EQ(SCANNER_ERR, get_next_token(&token)) << "This token should be incorrect: " << buffer;

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
        stream = std::tmpfile(); // /tmp/filename
        std::fputs(input.c_str(), stream);
        std::rewind(stream);
        if (stream == nullptr)
            FAIL();
        set_source_file(stream);
    }
};

TEST_F(Token, Integer) {
    std::string tokenStr;
    createBuffer("0      1  -0 0999 012345678910");
    tokenTestNoErrNumber(T_INT, 0, 0.0)
    tokenTestNoErrNumber(T_INT, 1, 0.0)
    tokenTestNoErr("-", T_MINUS);
    tokenTestNoErrNumber(T_INT, 0, 0.0)
    tokenTestNoErrNumber(T_INT, 999, 0.0)
    tokenTestNoErrNumber(T_INT, 12345678910, 0.0)
    tokenTestErr("001")
}

TEST_F(Token, Double) {
    std::string tokenStr;
    createBuffer("1.0 0.45 9.88 0.001 0001.00100 -1.5e+07 +0.04E-00014 3.4e0 3.4e3");
    tokenTestNoErrNumber(T_FLOAT, 0, 1.0)
    tokenTestNoErrNumber(T_FLOAT, 0, 0.45)
    tokenTestNoErrNumber(T_FLOAT, 0, 9.88)
    tokenTestNoErrNumber(T_FLOAT, 0,0.001)
    tokenTestNoErrNumber(T_FLOAT, 0, 1.001)
    tokenTestNoErr("-", T_MINUS)
    tokenTestNoErrNumber(T_FLOAT, 0, 15000000.0)
    tokenTestNoErr("+", T_PLUS)
    tokenTestNoErrNumber(T_FLOAT, 0, 0.0000000000000004)
    tokenTestNoErrNumber(T_FLOAT, 0, 3.4)
    tokenTestNoErrNumber(T_FLOAT, 0, 3400.0)
    tokenTestErr("3.4e");
    tokenTestErr("3.a");
    tokenTestErr("0.04E-+0003");
}

TEST_F(Token, String) {
    std::string tokenStr;
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
    std::string tokenStr;
    createBuffer("do global require else if return end local then function nil while");
    tokenTestNoErrKw("do", T_KEYWORD, KW_DO);
    tokenTestNoErrKw("global", T_KEYWORD,KW_GLOBAL);
    tokenTestNoErrKw("require", T_KEYWORD, KW_REQUIRE);
    tokenTestNoErrKw("else", T_KEYWORD,KW_ELSE);
    tokenTestNoErrKw("if", T_KEYWORD, KW_IF);
    tokenTestNoErrKw("return", T_KEYWORD, KW_RETURN);
    tokenTestNoErrKw("end", T_KEYWORD, KW_END);
    tokenTestNoErrKw("local", T_KEYWORD, KW_LOCAL);
    tokenTestNoErrKw("then", T_KEYWORD, KW_THEN);
    tokenTestNoErrKw("function", T_KEYWORD, KW_FUNCTION);
    tokenTestNoErrKw("nil", T_KEYWORD, KW_NIL);
    tokenTestNoErrKw("while", T_KEYWORD, KW_WHILE);
}

TEST_F(Token, Operators) {
    std::string tokenStr;
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
    std::string tokenStr;
    createBuffer("global foo : function(string) : integer end");
    tokenTestNoErrKw("global", T_KEYWORD, KW_GLOBAL)
    tokenTestNoErr("foo", T_ID)
    tokenTestNoErr(":", T_COLON)
    tokenTestNoErrKw("function", T_KEYWORD, KW_FUNCTION)
    tokenTestNoErr("(", T_L_ROUND_BR)
    tokenTestNoErrKw("string", T_KEYWORD, KW_STRING)
    tokenTestNoErr(")", T_R_ROUND_BR)
    tokenTestNoErr(":", T_COLON)
    tokenTestNoErrKw("integer", T_KEYWORD, KW_INTEGER)
    tokenTestNoErrKw("end", T_KEYWORD, KW_END)
}

TEST_F(Token, Comment) {
    const std::string line_comment1 = "100 -- this is line comment\n";
    const std::string block_comment1 = R"(100 --[[comment
    this is comment]])";
    const std::string block_comment2 = R"(--[[comment
    this is comment]])";
    const std::string block_comment3 = R"(100 --[[comment
    this is comment]]
	code)";

    createBuffer(line_comment1);
    EXPECT_FALSE(get_next_token(&token));
    EXPECT_TRUE(token.type == T_INT);
    EXPECT_FALSE(get_next_token(&token));
    EXPECT_TRUE(token.type == T_EOF);

    createBuffer(block_comment1);
    EXPECT_FALSE(get_next_token(&token)); // 100
    EXPECT_TRUE(token.type == T_INT);
    EXPECT_FALSE(get_next_token(&token)); // comment
    EXPECT_TRUE(token.type == T_EOF); // comment

    createBuffer(block_comment2);
    EXPECT_FALSE(get_next_token(&token)); // 100
    EXPECT_FALSE(get_next_token(&token)); // comment
    EXPECT_TRUE(token.type == T_EOF); // comment

    createBuffer(block_comment3);
    EXPECT_FALSE(get_next_token(&token));
    EXPECT_TRUE(token.type == T_INT);
    EXPECT_FALSE(get_next_token(&token));
    EXPECT_TRUE(token.type == T_ID);
}
