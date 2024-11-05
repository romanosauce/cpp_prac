#ifndef SRC_ANNEALING_H_
#define SRC_ANNEALING_H_
#include <iostream>
#include <limits>
#include <numeric>
#include <vector>
#include <map>
#include <random>
#include <string>


class AnnealingSolution {
public:
    virtual long long get_loss_metric() const = 0;
    virtual void print() const = 0;

    virtual AnnealingSolution& operator=(const AnnealingSolution& other) = 0;

    virtual ~AnnealingSolution() = default;
};

class MutateSolution {
public:
    virtual AnnealingSolution* operator()(AnnealingSolution*, AnnealingSolution*) = 0;

    virtual ~MutateSolution() = default;
};

class LowerTemperature {
public:
    virtual double operator()(double, int) const = 0;

    virtual ~LowerTemperature() = default;
};

class SimulateAnnealing {
    AnnealingSolution* solution;
    AnnealingSolution* best_solution;
    AnnealingSolution* new_solution;
    MutateSolution& mutation;
    LowerTemperature& temperature_decrease_law;

    double start_temp;
    double cur_temp;
    long long cur_loss;
    long long smallest_loss;
    long long iter_with_improvement = 0;
    long long iter = 0;

    void annealing_step();
    void replace_solution(long long);

    std::random_device rd;
    std::mt19937 gen = std::mt19937(rd());
    std::uniform_real_distribution<> dis = std::uniform_real_distribution(0.0, 1.0);

public:
    // Чтобы избежать лишних вызовов new при операции изменения решения,
    // передаём в конструктор три динамически созданных объекта расписания:
    // текущее решение, лучшее найденное решение и измененное решение.
    // Потом меняем указатели или копируем содержимое в зависимости от значения метрики.
    // Можно оптимизировать и дальше (избежать одного копирования из текущего решения
    // в изменяемое решение)
    SimulateAnnealing(AnnealingSolution* solution
                     , AnnealingSolution* best_solution
                     , AnnealingSolution* new_solution
                     , MutateSolution& mutation
                     , LowerTemperature& temperature_decrease_law
                     , double start_temp):
            solution(solution)
            , best_solution(best_solution)
            , new_solution(new_solution)
            , mutation(mutation)
            , temperature_decrease_law(temperature_decrease_law)
            , start_temp(start_temp)
            , cur_temp(start_temp)
            , cur_loss(solution->get_loss_metric())
            , smallest_loss(cur_loss) {}

    void simulate_annealing();
    void print_res() {
        best_solution->print();
        std::cout << "Best metric: " << smallest_loss << '\n';
    }
    
    void print_loss() {
        std::cout << smallest_loss << '\n';
        std::cout << iter;
    }

    void clear() {
        delete solution;
        delete best_solution;
        delete new_solution;
    }
};

class BoltzmannLaw: public LowerTemperature {
public:
    double operator()(double, int) const override;
    
    ~BoltzmannLaw() override = default;
};

class CauchyLaw: public LowerTemperature {
public:
    double operator()(double, int) const override;

    ~CauchyLaw() override = default;
};

class MixedLaw: public LowerTemperature {
public:
    double operator()(double, int) const override;

    ~MixedLaw() override = default;
};

class ImplAnnealingSolution: public AnnealingSolution {
    // Более быстрая реализация - использовать вектор связных списков
    int k;
    std::vector<int> works;
    std::vector<std::vector<int>> schedule;
    std::map<int, int> works_binding;

public:
    ImplAnnealingSolution(int k, const std::vector<int>& works): k(k)
            , works(works)
            , schedule(k, std::vector<int>()) {
        std::vector<int> v(works.size());
        std::iota(v.begin(), v.end(), 0);
        schedule[0] = v;
        for (int i = 0; i < works.size(); ++i) {
            works_binding[i] = 0;
        }
    }

    long long get_loss_metric() const override;
    // Похоже на костыль, поведение при перегрузке оператора присваивания мне
    // не до конца понятно.
    AnnealingSolution& operator=(const AnnealingSolution& other) override;

    void print() const override;

    ~ImplAnnealingSolution() override = default;

    friend class ImplMutateSolution;
};

class ImplMutateSolution: public MutateSolution {
    std::random_device rd;
    std::mt19937 gen = std::mt19937(rd());
    
public:
     ImplAnnealingSolution* operator()(AnnealingSolution*, AnnealingSolution*) override;

    ~ImplMutateSolution() override = default;
};

#endif // SRC_ANNEALING_H_
