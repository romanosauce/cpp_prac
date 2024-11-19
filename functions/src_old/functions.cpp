#include "functions.h"
#include <algorithm>
#include <cmath>
#include <exception>
#include <memory>
#include <stdexcept>
#include <string>

class TFunctionFactory::TImpl {
    class ICreator {
    public:
        virtual TBasicFunctionPtr Create(const std::vector<double>&) const = 0;

        virtual ~ICreator() {}
    };

    using TCreatorPtr = std::shared_ptr<ICreator>;
    using TRegisteredCreators = std::map<std::string, TCreatorPtr>;
    TRegisteredCreators RegisteredCreators;

public:
    template <typename TCurrentObject>
    class TCreator: public ICreator {
        TBasicFunctionPtr Create(const std::vector<double>& v) const override {
            struct TCurrentObjectSharedEnabler: public TCurrentObject {
                TCurrentObjectSharedEnabler(const std::vector<double>& coef):
                    TCurrentObject(coef) {}
            };
            return std::make_shared<TCurrentObjectSharedEnabler>(v);
        }
    };

    TImpl() {
        RegisterAll();
    }

    template <typename T>
    void RegisterCreator(const std::string& name) {
        RegisteredCreators[name] = std::make_shared<TCreator<T>>();
    }

    void RegisterAll() {
        RegisterCreator<TPolynomialFunction>("polynomial");
        RegisterCreator<TIdentityFunction>("ident");
        RegisterCreator<TConstantFunction>("const");
        RegisterCreator<TPowerFunction>("power");
        RegisterCreator<TExponentialFunction>("exp");
        RegisterCreator<TMadnessFunction>("mad");
    }

    TBasicFunctionPtr CreateObject(
            const std::string& type
            , const std::vector<double>& v
        ) const {
        std::map<std::string, TCreatorPtr>::const_iterator creator = RegisteredCreators.find(type);
        if (creator == RegisteredCreators.end()) {
            creator = RegisteredCreators.find("mad");
        }
        return creator->second->Create(v);
    }

    std::vector<std::string> GetAvailableObjects() const {
        std::vector<std::string> result;
        for (
                std::map<std::string, TCreatorPtr>::const_iterator it = RegisteredCreators.begin();
                it != RegisteredCreators.end();
                ++it
        ) {
            result.push_back(it->first);
        }
        return result;
    }
};

TBasicFunctionPtr TFunctionFactory::CreateObject(
        const std::string& type
        , const std::vector<double>& param
    ) const {
    return Impl->CreateObject(type, param);
}

TBasicFunctionPtr TFunctionFactory::CreateObject(
        const std::string& type
        , double param
    ) const {
    return Impl->CreateObject(type, {param});
}

TBasicFunctionPtr TFunctionFactory::CreateObject(
        const std::string& type
    ) const {
    return Impl->CreateObject(type, {});
}

TFunctionFactory::TFunctionFactory():
        Impl(std::make_unique<TFunctionFactory::TImpl>()) {}
TFunctionFactory::~TFunctionFactory() {}

std::vector<std::string> TFunctionFactory::GetAvailableObjects() const {
    return Impl->GetAvailableObjects();
}

double TMadnessFunction::evaluate(double) const {
    std::cout << "HAHAHAHA\n";
    std::terminate();
}

double TPolynomialFunction::evaluate(double x) const {
    double res = 0;
    double x_pow = 1;
    for (double coef : coef_) {
        res += coef * x_pow;
        x_pow *= x;
    }
    return res;
}

double TPowerFunction::evaluate(double x) const {
    if (pow_ < 0 and x == 0) {
        throw std::invalid_argument("Division by zero");
    }
    return std::pow(x, pow_);
}

double TExponentialFunction::evaluate(double x) const {
    return std::pow(exp_, x);
}

double TAddFunction::evaluate(double x) const {
    return lhs_->evaluate(x) + rhs_->evaluate(x);
}

double TSubFunction::evaluate(double x) const {
    return lhs_->evaluate(x) - rhs_->evaluate(x);
}

double TMultFunction::evaluate(double x) const {
    return lhs_->evaluate(x) * rhs_->evaluate(x);
}

double TDivFunction::evaluate(double x) const {
    double num = lhs_->evaluate(x);
    double denom = rhs_->evaluate(x);
    if (denom == 0) {
        throw std::invalid_argument("Division by zero");
    }
    return lhs_->evaluate(x) / rhs_->evaluate(x);
}

double TMadnessFunction::deriv(double x) const {
    std::cout << "HAHAHAH DERIV\n";
    std::terminate();
}

double TPolynomialFunction::deriv(double x) const {
    double res = 0;
    double x_pow = 1;
    for (size_t i = 1; i < coef_.size(); ++i) {
        res += i * coef_[i] * x_pow;
        x_pow *= x;
    }
    return res;
}

double TPowerFunction::deriv(double x) const {
    if (pow_ == 0) {
        return 0;
    }
    if (x == 0 && pow_ - 1 < 0) {
        throw std::invalid_argument("Division by zero");
    }
    return pow_ * std::pow(x, pow_ - 1);
}

double TExponentialFunction::deriv(double x) const {
    return evaluate(x) * std::log(exp_);
}

double TAddFunction::deriv(double x) const {
    return lhs_->deriv(x) + rhs_->deriv(x);
}

double TSubFunction::deriv(double x) const {
    return lhs_->deriv(x) - rhs_->deriv(x);
}

double TMultFunction::deriv(double x) const {
    return lhs_->deriv(x) * rhs_->evaluate(x)
            + lhs_->evaluate(x) * rhs_->deriv(x);
}

double TDivFunction::deriv(double x) const {
   double denom = rhs_->evaluate(x);
   if (denom == 0) {
       throw std::invalid_argument("Division by zero");
   }
   return (lhs_->deriv(x) * denom - rhs_->deriv(x) * lhs_->evaluate(x)) / (denom * denom);
}

void check_operands(std::string lhs, std::string rhs) {
    const std::set<std::string>& type_set = IFunction::valid_types;
    if (std::find(type_set.begin(), type_set.end(), lhs) == type_set.end()) {
        throw std::logic_error("Unknown type");
    }
    if (std::find(type_set.begin(), type_set.end(), rhs) == type_set.end()) {
        throw std::logic_error("Unknown type");
    }
}

TFunctionPtr operator+(TFunctionPtr lhs, TFunctionPtr rhs) {
    check_operands(lhs->get_type(), rhs->get_type());
    return std::make_shared<TAddFunction>(lhs, rhs);
}

TFunctionPtr operator-(TFunctionPtr lhs, TFunctionPtr rhs) {
    check_operands(lhs->get_type(), rhs->get_type());
    return std::make_shared<TSubFunction>(lhs, rhs);
}

TFunctionPtr operator*(TFunctionPtr lhs, TFunctionPtr rhs) {
    check_operands(lhs->get_type(), rhs->get_type());
    return std::make_shared<TMultFunction>(lhs, rhs);
}

TFunctionPtr operator/(TFunctionPtr lhs, TFunctionPtr rhs) {
    check_operands(lhs->get_type(), rhs->get_type());
    return std::make_shared<TDivFunction>(lhs, rhs);
}

std::string double_to_str(double x) {
    if (floor(x) == x) {
        return std::to_string(static_cast<int>(x));
    }
    return std::to_string(x);
}

std::string TPolynomialFunction::ToString() const {
    std::string res = "";
    int pow = 0;
    bool first = true;
    for (double coef : coef_) {
        if (coef) {
            if (!pow) {
                res += double_to_str(coef);
            } else if (coef > 0) {
                res += (first ? "" : "+")
                        + (coef == 1 ? "" : double_to_str(coef))
                        + "x"
                        + (pow == 1 ? "" : "^" + double_to_str(pow));
            } else {
                res += double_to_str(coef)
                        + "x"
                        + (pow == 1 ? "" : "^" + double_to_str(pow));
            }
            first = false;
        }
        pow += 1;
    }
    return first ? "0" : res;
}

std::string TPowerFunction::ToString() const {
    std::string res = "x^" + std::to_string(pow_);
    return res;
}

std::string TExponentialFunction::ToString() const {
    std::string res = std::to_string(exp_) + "^x";
    return res;
}

double newtons_method(TFunctionPtr func
        , double initial_guess
        , int iter_num
        , double eps) {
    double x_cur = initial_guess;
    double x_new;
    for (int i = 0; i < iter_num; ++i) {
        double val = func->evaluate(x_cur);
        double deriv = func->deriv(x_cur);

        if (std::abs(deriv) < 1e-10) {
            break;
        }

        x_new = x_cur - val / deriv;

        if (std::abs(x_new) < eps) {
            return x_new;
        }

        x_cur = x_new;
    }

    return x_cur;
}

//int main() {
    //TFunctionFactory factory;
    //auto objects = factory.GetAvailableObjects();
    //for (const auto& obj : objects) {
        //std::cout << obj << '\n';
    //}
    ////factory.CreateObject("polynomial", {1, 2, 3})->evaluate();
    ////factory.CreateObject("const", 10)->evaluate();
    ////factory.CreateObject("exp", 2)->evaluate();
    ////factory.CreateObject("ident")->evaluate();

    //TBasicFunctionPtr f1 = factory.CreateObject("polynomial", {-1});
    //TBasicFunctionPtr i1 = factory.CreateObject("polynomial", {0, 0, 0, 0, 0, 10.0});
    //TBasicFunctionPtr f2 = factory.CreateObject("polynomial", {1, 0, 3, 4});
    //TBasicFunctionPtr f3 = factory.CreateObject("polynomial", {-1, 2, -3, 4});
    //TBasicFunctionPtr f4 = factory.CreateObject("polynomial", {1, 0, 3, -4});
    //TBasicFunctionPtr f5 = factory.CreateObject("polynomial", {0, 1, 3, 1});
    //TBasicFunctionPtr g1 = factory.CreateObject("ident");
    //TBasicFunctionPtr h1 = factory.CreateObject("const", 0.12345);
    //TBasicFunctionPtr h2 = factory.CreateObject("power", 20);
    //TBasicFunctionPtr k1 = factory.CreateObject("exp", 2.5);
    //TBasicFunctionPtr k2 = factory.CreateObject("polynomial", {-1, -2, -3, -4.223, 0, 7.8});
    //TBasicFunctionPtr t = factory.CreateObject("polynomial", {-5, 2, 1});
    //auto s = h2 + k1;
    //s->evaluate(10);
    //std::cout << i1->ToString() << '\n';
    //std::cout << f1->ToString() << '\n';
    //std::cout << f2->ToString() << '\n';
    //std::cout << f3->ToString() << '\n';
    //std::cout << f4->ToString() << '\n';
    //std::cout << f5->ToString() << '\n';
    //std::cout << g1->ToString() << '\n';
    //std::cout << h1->ToString() << '\n';
    //std::cout << h2->ToString() << '\n';
    //std::cout << k1->ToString() << '\n';
    //std::cout << k2->ToString() << '\n';
    //std::cout << newtons_method(t) << '\n';
//}
