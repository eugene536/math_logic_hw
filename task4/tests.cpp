#include <gtest/gtest.h>

#include <fstream>

#include "deduction.h"
#include "parser.h"

namespace {
    std::ofstream log_("log");
    Deduction deduction("in");
}

TEST(Parser, BadExpressions) {
  EXPECT_EQ(nullptr, parse("x+x", log_));
  EXPECT_EQ(nullptr, parse("@x(x+x)", log_));
  EXPECT_EQ(nullptr, parse("", log_));
  EXPECT_EQ(nullptr, parse("+x", log_));
  EXPECT_EQ(nullptr, parse("P(x", log_));
  EXPECT_EQ(nullptr, parse("P(x))", log_));
}

TEST(Parser, GoodExpressions) {
  EXPECT_NE(nullptr, parse("x+x=x'", log_));
  EXPECT_NE(nullptr, parse("@x(x+x)*y=a''''", log_));
  EXPECT_NE(nullptr, parse("P(x)&P(y)", log_));
  EXPECT_NE(nullptr, parse("y+x=0''''", log_));
  EXPECT_NE(nullptr, parse("A & B | C -> D & (x + 0''' = y) & P(x, y, z) & f(x, y, z) = d'''", log_));
  EXPECT_NE(nullptr, parse("P(x) | P", log_));
}

void forIsAxiom(std::string A, std::string B, std::string C) {
    EXPECT_TRUE(deduction.isAxiom(parse("(" + A + ")->(" + B + ")->(" + A + ")")));
    EXPECT_TRUE(deduction.isAxiom(parse("((" + A + ")->(" + B + "))->((" + A + ")->(" + B + ")->(" + C + "))->((" + A + ")->(" + C + "))")));
    EXPECT_TRUE(deduction.isAxiom(parse("(" + A + ")->(" + B + ") ->(" + A + ")&(" + B + ")")));
    EXPECT_TRUE(deduction.isAxiom(parse("(" + A + ")& (" + B + ") ->(" + A + ")")));
    EXPECT_TRUE(deduction.isAxiom(parse("(" + A + ")& (" + B + ") ->(" + B + ")")));
    EXPECT_TRUE(deduction.isAxiom(parse("(" + A + ")->(" + A + ") | (" + B + ")")));
    EXPECT_TRUE(deduction.isAxiom(parse("(" + B + ")->(" + A + ") | (" + B + ")")));
    EXPECT_TRUE(deduction.isAxiom(parse("((" + A + ")->(" + C + "))->((" + B + ")->(" + C + "))->((" + A + ")|(" + B + ")->(" + C + "))")));
    EXPECT_TRUE(deduction.isAxiom(parse("((" + A + ")->(" + B + "))->((" + A + ")->!(" + B + "))->!(" + A + ")")));
    EXPECT_TRUE(deduction.isAxiom(parse("!!(" + A + ")->(" + A + ")")));

    EXPECT_FALSE(deduction.isAxiom(parse("(" + B + ")->(" + B + ")->(" + A + ")")));
    EXPECT_FALSE(deduction.isAxiom(parse("(("+ B + ")->(" + B + "))->((" + A + ")->(" + B + ")->(" + C + "))->((" + A + ")->(" + C + "))")));
    EXPECT_FALSE(deduction.isAxiom(parse("(" + B + ")->(" + B + ") ->(" + A + ")&(" + B + ")")));
    EXPECT_FALSE(deduction.isAxiom(parse("(" + B + ")& (" + B + ") ->(" + A + ")")));
    EXPECT_FALSE(deduction.isAxiom(parse("(" + A + ")& (" + A + ") ->(" + B + ")")));
    EXPECT_FALSE(deduction.isAxiom(parse("(" + A + ")->(" + B + ") | (" + B + ")")));
    EXPECT_FALSE(deduction.isAxiom(parse("(" + C + ")->(" + A + ") | (" + B + ")")));
    EXPECT_FALSE(deduction.isAxiom(parse("((" + B + ")->(" + C + "))->((" + B + ")->(" + C + "))->((" + A + ")|(" + B + ")->(" + C + "))")));
    EXPECT_FALSE(deduction.isAxiom(parse("((" + B + ")->(" + B + "))->((" + A + ")->!(" + B + "))->!(" + A + ")")));
    EXPECT_FALSE(deduction.isAxiom(parse("!!(" + B + ")->(" + A + ")")));
}

TEST(Deduction, isAxiom) {
    forIsAxiom("A", "B", "C");
    forIsAxiom("x+0'''=f(a, b, c)", "@xx+x=x", "?yF(y, z, a)");
    forIsAxiom("P(x, y, z) & A & B & D", "P(x, y) & A & B & D", "A & B & C123 | f(a0, c123, 0'''') = 0");
}

namespace {
    void checkInduction(const std::string& x, std::function<std::string(std::string)> genPhi) {
        std::string phi = genPhi(x);
        std::string phi1 = "(" + phi + "->" + genPhi(x + '\'') + ")";
        std::string phi0 = genPhi("0");

        std::string expr = phi0 + "& @" + x + phi1 + "->" + phi;
        EXPECT_TRUE(deduction.isFormalAxiom(parse(expr)));
    }
}
TEST(Deduction, isFormalAxiom) {
    EXPECT_TRUE(deduction.isFormalAxiom(parse("a=b->a'=b'")));
    EXPECT_TRUE(deduction.isFormalAxiom(parse("a=b->a=c->b=c")));
    EXPECT_TRUE(deduction.isFormalAxiom(parse("a'=b'->a=b")));
    EXPECT_TRUE(deduction.isFormalAxiom(parse("!a'=0")));
    EXPECT_TRUE(deduction.isFormalAxiom(parse("a+b'=(a+b)'")));
    EXPECT_TRUE(deduction.isFormalAxiom(parse("a+0=a")));
    EXPECT_TRUE(deduction.isFormalAxiom(parse("a*0=0")));
    EXPECT_TRUE(deduction.isFormalAxiom(parse("a*b'=a*b+a")));

    checkInduction(
        "x123123",
        [](const std::string& x) {
            return "(" + x + " = " + x + ")";
        });

    checkInduction(
        "x",
        [](const std::string& x) {
            return "(" + x + " + " + x + "'''' = f(y, z, u, " + x + ")'''')";
        });

    checkInduction(
        "u",
        [](const std::string& x) {
            return "(" + x + " + h123'''' = 0'''' & P(f," + x + ", x) -> 0'' = 0)";
        });


    EXPECT_FALSE(deduction.isFormalAxiom(parse("b=b->b'=b'")));
    EXPECT_FALSE(deduction.isFormalAxiom(parse("b=b->b=c->b=c")));
    EXPECT_FALSE(deduction.isFormalAxiom(parse("b'=b'->b=b")));
    EXPECT_FALSE(deduction.isFormalAxiom(parse("!b'=0")));
    EXPECT_FALSE(deduction.isFormalAxiom(parse("b+b'=(b+b)'")));
    EXPECT_FALSE(deduction.isFormalAxiom(parse("b+0=b")));
    EXPECT_FALSE(deduction.isFormalAxiom(parse("b*0=0")));
    EXPECT_FALSE(deduction.isFormalAxiom(parse("b*b'=b*b+b")));

    EXPECT_FALSE(deduction.isFormalAxiom(parse("e=g->e'=g'")));
    EXPECT_FALSE(deduction.isFormalAxiom(parse("e=g->e=f->g=f")));
    EXPECT_FALSE(deduction.isFormalAxiom(parse("e'=g'->e=g")));
    EXPECT_FALSE(deduction.isFormalAxiom(parse("!e'=0")));
    EXPECT_FALSE(deduction.isFormalAxiom(parse("e+g'=(e+g)'")));
    EXPECT_FALSE(deduction.isFormalAxiom(parse("e+0=e")));
    EXPECT_FALSE(deduction.isFormalAxiom(parse("e*0=0")));
    EXPECT_FALSE(deduction.isFormalAxiom(parse("e*g'=e*g+e")));

    EXPECT_FALSE(deduction.isFormalAxiom(parse("(0 = 0) & @x(x = y -> x' = x') -> (x = x)")));
    EXPECT_FALSE(deduction.isFormalAxiom(parse("(x = x) & @x(x = x -> x' = x') -> (x = x)")));
    EXPECT_FALSE(deduction.isFormalAxiom(parse("(0' = 0') & @x(x = x -> x' = x') -> (x = x)")));
}

TEST(Deduction, isForallExistAxiom) {
    EXPECT_TRUE(deduction.isExistAxiom(parse("P(x)->?xP(x)")));
    EXPECT_TRUE(deduction.isForallAxiom(parse("@xP(x)->P(x)")));

    Tree* expr = parse("@z?y((a + u''')'' + z = y & (a + u''') * z'' + a = b'' | P(a, b, (a + u'''), y, z))->?x@z?y(x'' + z = y & x * z'' + a = b'' | P(a, b, x, y, z))");
    EXPECT_TRUE(deduction.isExistAxiom(expr));

    expr = parse("@xP(x, x)->P(y, y)");
    EXPECT_TRUE(deduction.isForallAxiom(expr));

    EXPECT_TRUE(deduction.isExistAxiom(parse("@yP(z, y)->?x@yP(x, y)")));
    EXPECT_FALSE(deduction.isExistAxiom(parse("@yP(y, y)->?x@yP(x, y)")));

    EXPECT_TRUE(deduction.isForallAxiom(parse("@x?yP(x, y)->?yP(z, y)")));
    EXPECT_FALSE(deduction.isForallAxiom(parse("@x?yP(x, y)->?yP(y, y)")));
}

TEST(Deduction, isMP) {
    std::vector<Tree*> context{
                          parse("a=b->0'''''=0"),
                          parse("P(b)->(A->A->A)->P(b)"),
                          parse("(A->A->A)->P(b)"),
                          parse("((A->A->A)->P(b))->P(a)->((A->A->A)->P(b))"),
                          parse("P(a)->(A->A->A)"),
                          parse("(P(a)->(A->A->A))->P(b)"),
                          parse("A->A->A"),
                          parse("P(b)"),
                          parse("(a=b->0'''''=0)->@x?yP(x, y)"),
                          parse("?aP(a)->(A->A->A)->P(b)")};

    EXPECT_TRUE(deduction.isMP(parse("@x?yP(x, y)"), context));
    EXPECT_TRUE(deduction.isMP(parse("P(b)"), context));

    EXPECT_FALSE(deduction.isMP(parse("(P(a)->(A->A->A))->P(b)"), context));
    EXPECT_FALSE(deduction.isMP(parse("P(a)"), context));
    EXPECT_FALSE(deduction.isMP(parse("a=b->0'''''=0"), context));
    EXPECT_FALSE(deduction.isMP(parse("?aP(a)->(A->A->A)->P(b)"), context));
}

TEST(Deduction, isForallRule) {
    std::vector<Tree*> context{
                          parse("a=b->0'''''=0"),
                          parse("P(b)->(A->A->A)->P(b)"),
                          parse("(A->A->A)->P(b)"),
                          parse("((A->A->A)->P(b))->P(a)->((A->A->A)->P(b))"),
                          parse("P(a)->(A->A->A)"),
                          parse("(P(a)->(A->A->A))->P(b)"),
                          parse("A->A->A"),
                          parse("P(b)"),
                          parse("(a=b->0'''''=0)->@x?yP(x, y)"),
                          parse("(@a(a = b) & ?a(a * b = c'') | P(x, y))->(A->A->A)"),
                          parse("?aP(a)->(A->A->A)->P(b)")};

    EXPECT_TRUE(deduction.isForallRule(parse("A->@x(A->A)"), context));
    EXPECT_TRUE(deduction.isForallRule(parse("(A->A->A)->@bP(b)"), context));
    EXPECT_TRUE(deduction.isForallRule(parse("?aP(a)->@b((A->A->A)->P(b))"), context));
    EXPECT_TRUE(deduction.isForallRule(parse("?aP(a)->@a((A->A->A)->P(b))"), context));
    EXPECT_TRUE(deduction.isForallRule(parse("P(a)->@b(A->A->A)"), context));
    EXPECT_TRUE(deduction.isForallRule(parse("(@a(a = b) & ?a(a * b = c'') | P(x, y))->@a(A->A->A)"), context));

    EXPECT_FALSE(deduction.isForallRule(parse("P(a)->@a(A->A->A)"), context));
    EXPECT_FALSE(deduction.isForallRule(parse("P(a)->@b(A->A)"), context));
    EXPECT_FALSE(deduction.isForallRule(parse("(@a(a = b) & ?a(a * b = c'') | P(x, y))->@x(A->A->A)"), context));
    EXPECT_FALSE(deduction.isForallRule(parse("(@a(a = b) & ?a(a * b = c'') | P(x, y))->@b(A->A->A)"), context));
    EXPECT_FALSE(deduction.isForallRule(parse("(@a(a = b) & ?a(a * b = c'') | P(x, y))->@c(A->A->A)"), context));
}

TEST(Deduction, isExistRule) {
    std::vector<Tree*> context{
                          parse("a=b->0'''''=0"),
                          parse("P(b)->(A->A->A)->P(b)"),
                          parse("(A->A->A)->P(b)"),
                          parse("((A->A->A)->P(b))->P(a)->((A->A->A)->P(b))"),
                          parse("P(a)->(A->A->A)"),
                          parse("(P(a)->(A->A->A))->P(b)"),
                          parse("A->A->A"),
                          parse("P(b)"),
                          parse("(a=b->0'''''=0)->@x?yP(x, y)"),
                          parse("(@a(a = b) & ?a(a * b = c'') | P(x, y))->(A->A->A)"),
                          parse("?aP(a)->(A->A->A)->P(b)")};

    EXPECT_TRUE(deduction.isExistRule(parse("?a(A->A->A)->P(b)"), context));
    EXPECT_TRUE(deduction.isExistRule(parse("?x(a=b->0'''''=0)->@x?yP(x, y)"), context));
    EXPECT_TRUE(deduction.isExistRule(parse("?y(a=b->0'''''=0)->@x?yP(x, y)"), context));
    EXPECT_TRUE(deduction.isExistRule(parse("?u(P(a)->(A->A->A))->P(b)"), context));

    EXPECT_FALSE(deduction.isExistRule(parse("?u?x(P(a)->(A->A->A))->P(b)"), context));
    EXPECT_FALSE(deduction.isExistRule(parse("?b(P(a)->(A->A->A))->P(b)"), context));
    EXPECT_FALSE(deduction.isExistRule(parse("?b(A->A->A)->P(b)"), context));
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    return ret;
}
