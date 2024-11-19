#include "../src_old/functions.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <stdexcept>


class BasicFunctionStr: public testing::Test {
protected:
    TFunctionFactory factory = TFunctionFactory();
};

TEST_F(BasicFunctionStr, TestPolynomialStr) {
    TBasicFunctionPtr f1 = factory.CreateObject("polynomial", {-1});
    TBasicFunctionPtr f2 = factory.CreateObject("polynomial", {0, 0, 0, 0, 0, 10.0});
    TBasicFunctionPtr f3 = factory.CreateObject("polynomial", {1, 0, 3, 4});
    TBasicFunctionPtr f4 = factory.CreateObject("polynomial", {-1, 2, -3, 4});
    TBasicFunctionPtr f5 = factory.CreateObject("polynomial", {1, 0, 3, -4});
    TBasicFunctionPtr f6 = factory.CreateObject("polynomial", {0, 1, 3, 1});

    EXPECT_EQ(f1->ToString(), "-1");
    EXPECT_EQ(f2->ToString(), "10x^5");
    EXPECT_EQ(f3->ToString(), "1+3x^2+4x^3");
    EXPECT_EQ(f4->ToString(), "-1+2x-3x^2+4x^3");
    EXPECT_EQ(f5->ToString(), "1+3x^2-4x^3");
    EXPECT_EQ(f6->ToString(), "x+3x^2+x^3");
}

TEST_F(BasicFunctionStr, TestOtherStr) {
    TBasicFunctionPtr f1 = factory.CreateObject("ident");
    TBasicFunctionPtr f2 = factory.CreateObject("const", 10);
    TBasicFunctionPtr f3 = factory.CreateObject("power", 0);
    TBasicFunctionPtr f4 = factory.CreateObject("exp", 9);

    EXPECT_EQ(f1->ToString(), "x");
    EXPECT_EQ(f2->ToString(), "10");
    EXPECT_EQ(f3->ToString(), "x^0.000000");
    EXPECT_EQ(f4->ToString(), "9.000000^x");
}

TFunctionFactory factory = TFunctionFactory();

TEST(ArithmeticOps, Addition) {
    TBasicFunctionPtr f1 = factory.CreateObject("polynomial", {15, 6, 0, 1});
    TBasicFunctionPtr f2 = factory.CreateObject("power", 0.5);

    TFunctionPtr s = f1 + f2;
    EXPECT_DOUBLE_EQ(s->evaluate(10), 1078.16227766016837);
}

TEST(ArithmeticOps, Subtraction) {
    TBasicFunctionPtr f1 = factory.CreateObject("const", 5.5);
    TBasicFunctionPtr f2 = factory.CreateObject("exp", 0.5);

    TFunctionPtr s = f1 - f2;
    EXPECT_DOUBLE_EQ(s->evaluate(10), 5.4990234375);
}

TEST(ArithmeticOps, Multiplication) {
    TBasicFunctionPtr f1 = factory.CreateObject("ident");
    TBasicFunctionPtr f2 = factory.CreateObject("polynomial", {0, 0, -1, 1, 2});

    TFunctionPtr s = f1 * f2;
    EXPECT_DOUBLE_EQ(s->evaluate(10), 209000);
}

TEST(ArithmeticOps, BasicDivision) {
    TBasicFunctionPtr f1 = factory.CreateObject("const", 7.5);
    TBasicFunctionPtr f2 = factory.CreateObject("exp", 2.4);

    TFunctionPtr s = f1 / f2;
    EXPECT_DOUBLE_EQ(s->evaluate(10), 0.0011829022184314877);
}

TEST(Exceptions, WrongType) {
    TBasicFunctionPtr f1 = factory.CreateObject("aaa");
    TBasicFunctionPtr f2 = factory.CreateObject("exp", 5);

    EXPECT_THROW(f1 + f2, std::logic_error);
    EXPECT_THROW(f1 - f2, std::logic_error);
    EXPECT_THROW(f1 * f2, std::logic_error);
    EXPECT_THROW(f1 / f2, std::logic_error);
}

TEST(Exceptions, ZeroDivision) {
    TBasicFunctionPtr f1 = factory.CreateObject("polynomial", {10, 20, 30, 40});
    TBasicFunctionPtr f2 = factory.CreateObject("const", 0);

    EXPECT_THROW((f1 / f2)->evaluate(1), std::invalid_argument);

    TBasicFunctionPtr f3 = factory.CreateObject("power", -1);

    EXPECT_THROW(f3->evaluate(0), std::invalid_argument);

    TBasicFunctionPtr f4 = factory.CreateObject("power", 0.5);
    EXPECT_THROW(f4->deriv(0), std::invalid_argument);
}

TEST(Derivs, BasicDerivs) {
    TBasicFunctionPtr f1 = factory.CreateObject("polynomial", {1, 5.5, 0, 0, 4});
    TBasicFunctionPtr f2 = factory.CreateObject("ident");
    TBasicFunctionPtr f3 = factory.CreateObject("const", 3.4);
    TBasicFunctionPtr f4 = factory.CreateObject("power", 0.4);
    TBasicFunctionPtr f5 = factory.CreateObject("exp", 2);

    EXPECT_DOUBLE_EQ(f1->deriv(10), 16005.5);
    EXPECT_DOUBLE_EQ(f2->deriv(10), 1);
    EXPECT_DOUBLE_EQ(f3->deriv(10), 0);
    EXPECT_DOUBLE_EQ(f4->deriv(10), 0.1004754572603832);
    EXPECT_DOUBLE_EQ(f5->deriv(10), 709.782712893383996843);
}

TEST(Derivs, OpDerivs) {
    TBasicFunctionPtr f1 = factory.CreateObject("polynomial", {0, 2, 5.5, -1.2});
    TBasicFunctionPtr f2 = factory.CreateObject("ident");
    TBasicFunctionPtr f3 = factory.CreateObject("const", 8);
    TBasicFunctionPtr f4 = factory.CreateObject("power", -0.4);
    TBasicFunctionPtr f5 = factory.CreateObject("exp", 1.1);

    TFunctionPtr op_sum = f1 + f2;
    TFunctionPtr op_sub = f3 - f5;
    TFunctionPtr op_mult = f4 * f5;
    TFunctionPtr op_div = f2 / f1;

    EXPECT_DOUBLE_EQ(op_sum->deriv(5), -32);
    EXPECT_DOUBLE_EQ(op_sub->deriv(5), -0.15349799767666342);
    EXPECT_DOUBLE_EQ(op_mult->deriv(5), 0.012952563055287247);
    EXPECT_DOUBLE_EQ(op_div->deriv(5), 26);
}

TEST(Derivs, ComplexFuncDerivs) {
    TBasicFunctionPtr f1 = factory.CreateObject("polynomial", {-1, 4, 0, 0.9});
    TBasicFunctionPtr f2 = factory.CreateObject("exp", 3);
    TBasicFunctionPtr f3 = factory.CreateObject("power", 0.1);

    TFunctionPtr c_sum = f1 + f2 + f3;
    TFunctionPtr c_mult = f1 * f2 * f3;
    TFunctionPtr mixed = f1 - f2 / f3;

    EXPECT_DOUBLE_EQ(c_sum->deriv(3), 57.999735899840097);
    EXPECT_DOUBLE_EQ(c_mult->deriv(3), 2056.9674320136696);
    EXPECT_DOUBLE_EQ(mixed->deriv(3), 2.5299663126918936);
}


TEST(NewtonMethods, BasicNewtonMethod) {
    TBasicFunctionPtr f1 = factory.CreateObject("polynomial", {3, 0, 1, -2.3});

    EXPECT_NEAR(newtons_method(f1, 1), 1.258425044, 1e-6);
}

TEST(NewtonMethods, ComplexNewtonMethod) {
    TBasicFunctionPtr f1 = factory.CreateObject("const", 2);
    TBasicFunctionPtr f2 = factory.CreateObject("exp", 1.1);

    EXPECT_NEAR(newtons_method(f1 - f2, 1, 100), 7.272540897341719, 1e-6);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
