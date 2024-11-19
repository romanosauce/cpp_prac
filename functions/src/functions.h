#ifndef SRC_FUNCTIONS_H_
#define SRC_FUNCTIONS_H_

#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

class IFunction;
class IBasicFunction;
using TFunctionPtr = std::shared_ptr<IFunction>;
using TBasicFunctionPtr = std::shared_ptr<IBasicFunction>;


class IFunction {
public:
    virtual double evaluate(double) const = 0;
    virtual double deriv(double) const = 0;
    virtual std::string get_type() const = 0;

    double operator()(double x) const {
        return evaluate(x);
    }

    inline static const std::set<std::string> valid_types = {
        "polynomial",
        "ident",
        "const",
        "power",
        "exp",
        "add_func",
        "sub_func",
        "mult_func",
        "div_func"
    };
};


class IBasicFunction: public IFunction {
public:
    virtual std::string ToString() const = 0;
};


class TMadnessFunction: public IBasicFunction {
    TMadnessFunction(std::vector<double>) {}
    
public:
    double evaluate(double) const override;
    double deriv(double) const override;
    std::string get_type() const override {
        return "mad";
    }
    std::string ToString() const override {
        return "HAHAHAHA";
    }

    friend class TFunctionFactory;
};

class TPolynomialFunction: public IBasicFunction {
    std::vector<double> coef_;

public:
    TPolynomialFunction(std::vector<double> coef) : coef_(coef) {}

    double evaluate(double) const override;
    double deriv(double) const override;
    std::string ToString() const override;
    std::string get_type() const override {
        return "polynomial";
    }

    friend class TFunctionFactory;
};


class TIdentityFunction: public TPolynomialFunction {
    TIdentityFunction(const std::vector<double>& coef = {}): TPolynomialFunction({0, 1}) {}

public:
    std::string get_type() const override {
        return "ident";
    }

    friend class TFunctionFactory;
};


class TConstantFunction: public TPolynomialFunction {
    TConstantFunction(const std::vector<double>& coef): TPolynomialFunction({coef[0]}) {}

public:
    std::string get_type() const override {
        return "const";
    }

    friend class TFunctionFactory;
};


class TPowerFunction: public IBasicFunction {
    double pow_;
    TPowerFunction(const std::vector<double>& coef): pow_(coef[0]) {}

public:    
    double evaluate(double) const override;
    double deriv(double) const override;
    std::string ToString() const override;
    std::string get_type() const override {
        return "power";
    }

    friend class TFunctionFactory;
};


class TExponentialFunction: public IBasicFunction {
    double exp_;
    TExponentialFunction(const std::vector<double>& coef): exp_(coef[0]) {}

public:
    double evaluate(double) const override;
    double deriv(double) const override;
    std::string ToString() const override;
    std::string get_type() const override {
        return "exp";
    }

    friend class TFunctionFactory;
};


class TAddFunction: public IFunction {
    TFunctionPtr lhs_;
    TFunctionPtr rhs_;
public:
    TAddFunction(TFunctionPtr lhs, TFunctionPtr rhs):
            lhs_(lhs)
            , rhs_(rhs) {}

    double evaluate(double) const override;
    double deriv(double) const override;
    std::string get_type() const override {
        return "add_func";
    }
};


class TSubFunction: public IFunction {
    TFunctionPtr lhs_;
    TFunctionPtr rhs_;
public:
    TSubFunction(TFunctionPtr lhs, TFunctionPtr rhs):
            lhs_(lhs)
            , rhs_(rhs) {}

    double evaluate(double) const override;
    double deriv(double) const override;
    std::string get_type() const override {
        return "sub_func";
    }
};


class TMultFunction: public IFunction {
    TFunctionPtr lhs_;
    TFunctionPtr rhs_;
public:
    TMultFunction(TFunctionPtr lhs, TFunctionPtr rhs):
            lhs_(lhs)
            , rhs_(rhs) {}

    double evaluate(double) const override;
    double deriv(double) const override;
    std::string get_type() const override {
        return "mult_func";
    }
};

class TDivFunction: public IFunction {
    TFunctionPtr lhs_;
    TFunctionPtr rhs_;
public:
    TDivFunction(TFunctionPtr lhs, TFunctionPtr rhs):
            lhs_(lhs)
            , rhs_(rhs) {}

    double evaluate(double) const override;
    double deriv(double) const override;
    std::string get_type() const override {
        return "div_func";
    }
};

void check_operands(std::string lhs, std::string rhs);

TFunctionPtr operator+(TFunctionPtr lhs, TFunctionPtr rhs);
TFunctionPtr operator-(TFunctionPtr lhs, TFunctionPtr rhs);
TFunctionPtr operator*(TFunctionPtr lhs, TFunctionPtr rhs);
TFunctionPtr operator/(TFunctionPtr lhs, TFunctionPtr rhs);

class TFunctionFactory {
    class TImpl;
    std::unique_ptr<const TImpl> Impl;

public:
    TFunctionFactory();
    ~TFunctionFactory();

    TBasicFunctionPtr CreateObject(
            const std::string& type, const std::vector<double>& param
    ) const;

    TBasicFunctionPtr CreateObject(
            const std::string& type, double param
    ) const;

    TBasicFunctionPtr CreateObject(
            const std::string& type
    ) const;

    std::vector<std::string> GetAvailableObjects() const;
};

double newtons_method(TFunctionPtr, double, int=10000, double=1e-6);

#endif // SRC_FUNCTIONS_H_
