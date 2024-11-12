#include "annealing.h"
#include <unistd.h>
#include <random>
#include <algorithm>

namespace CONFIG {
    int MAX_ITER_WITHOUT_IMPROVEMENT = 1000;
    int STEPS_WITHOUT_TEMP_DECREASE = 5;
}


void SimulateAnnealing::annealing_step() {
    for (int i = 0; i < CONFIG::STEPS_WITHOUT_TEMP_DECREASE; ++i) {
        mutation(solution, new_solution);
        if (long long loss = 0; (loss = new_solution->get_loss_metric()) <= cur_loss) {
            replace_solution(loss);
        } else {
            long long df = loss - smallest_loss;
            double p = std::exp(-df / cur_temp);
            if (dis(gen) < p) {
                replace_solution(loss);
            }
        }
    }
}

void SimulateAnnealing::replace_solution(long long loss) {
    if (loss < smallest_loss) {
        iter_with_improvement = iter;
        smallest_loss = loss;
        *best_solution = *new_solution;
        //std::cout << "New loss " << smallest_loss << '\n';
    }
    cur_loss = loss;
    AnnealingSolution* temp = solution;
  solution = new_solution;
    new_solution = temp;
}

void SimulateAnnealing::simulate_annealing() {
    gen.seed(rd());
    while (1) {
        ++iter;
        annealing_step();
        if (iter - iter_with_improvement > CONFIG::MAX_ITER_WITHOUT_IMPROVEMENT) {
            break;
        }
        cur_temp = temperature_decrease_law(start_temp, iter);
    }
}

long long ImplAnnealingSolution::get_loss_metric() const {
    long long metric = 0;
    long long start = 0;
    for (const std::vector<int>& v : schedule) {
        start = 0;
        for (int work : v) {
            metric += start + works[work];
            start += works[work];
        }
    }
    return metric;
}

AnnealingSolution& ImplAnnealingSolution::operator=(const AnnealingSolution& other) {
    // Похоже, что без этой перегрузки оператора работать не будет.
    const ImplAnnealingSolution& temp = dynamic_cast<const ImplAnnealingSolution&>(other);
    k = temp.k;
    works = temp.works;
    schedule = temp.schedule;
    works_binding = temp.works_binding;
    return *this;
}

ImplAnnealingSolution& ImplAnnealingSolution::operator=(const ImplAnnealingSolution& other) {
    k = other.k;
    works = other.works;
    schedule = other.schedule;
    works_binding = other.works_binding;
    return *this;
}
    

void ImplAnnealingSolution::print() const {
    for (int i = 0; i < schedule.size(); ++i) {
        std::cout << i << ": ";
        for (int j = 0; j < schedule[i].size(); ++j) {
            std::cout << schedule[i][j]  << ':' << works[schedule[i][j]] << ' ';
        }
        std::cout << '\n';
    }
}

void ImplAnnealingSolution::to_bytes(int fd) const {
    for (int i = 0; i < schedule.size(); ++i) {
        size_t proc_queue_size = schedule[i].size();
        write(fd, &proc_queue_size, sizeof(proc_queue_size));
        for (const int& task : schedule[i]) {
            write(fd, &task, sizeof(task));
        }
    }
}

void ImplAnnealingSolution::from_bytes(int fd) {
    for (int i = 0; i < k; ++i) {
        size_t proc_queue_size;
        read(fd, &proc_queue_size, sizeof(proc_queue_size));
        schedule[i] = std::vector<int>();
        for (int j = 0; j < proc_queue_size; ++j) {
            int task;
            read(fd, &task, sizeof(task));
            schedule[i].push_back(task);
            works_binding[task] = i;
        }
    }
}


double BoltzmannLaw::operator()(double temp, int iter) const {
    return temp / std::log(1+iter);
}

double CauchyLaw::operator()(double temp, int iter) const {
    return temp / (1 + iter);
}

double MixedLaw::operator()(double temp, int iter) const {
    return temp * std::log(1 + iter) / (1 + iter);
}

ImplAnnealingSolution* ImplMutateSolution::operator()(AnnealingSolution* solution,
                                                      AnnealingSolution* new_solution) {
    *new_solution = *solution;
    // dynamic_cast'ы хоть и не способствуют разделению абстракций и реализации,
    // но можно добавить чисто виртуальные методы в AnnealingSolution для изменения
    // решения и оперировать только ими.
    ImplAnnealingSolution* i_new_solution = dynamic_cast<ImplAnnealingSolution*>(new_solution);
    ImplAnnealingSolution* i_solution = dynamic_cast<ImplAnnealingSolution*>(solution);

    int n = i_new_solution->works.size();
    int k = i_new_solution->k;

    gen.seed(rd());
    std::uniform_int_distribution<> dis_works(0, n-1);
    std::uniform_int_distribution<> dis_procs(0, k-1);
    
    int work_num = dis_works(gen);
    int proc_num = dis_procs(gen);

    int old_proc = i_new_solution->works_binding[work_num];
    i_new_solution->works_binding[work_num] = proc_num;
    std::vector<int>& old_proc_jobs = i_new_solution->schedule[old_proc];
    old_proc_jobs.erase(std::find(old_proc_jobs.begin(),
                                  old_proc_jobs.end(),
                                  work_num));
    i_new_solution->schedule[proc_num].push_back(work_num);
    return i_new_solution;
}
