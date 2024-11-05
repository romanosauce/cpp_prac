#include "src/annealing.h"
#include <fstream>
#include <string>

int main (int argc, char *argv[]) {
    std::string input_file = argv[1];

    std::string law_type = "mixed";
    if (argc == 3) {
        law_type = argv[2];
    }

    std::ifstream my_file(input_file);

    if (!my_file.is_open()) {
        std::cerr << "Can't open file\n";
        exit(1);
    }

    int k;
    int task;
    char buf;
    my_file >> k;
    my_file >> buf;

    std::vector<int> tasks;

    while (my_file >> task) {
        tasks.push_back(task);
        my_file >> buf;
    }

    ImplAnnealingSolution* ann = new ImplAnnealingSolution(k, tasks);
    ImplAnnealingSolution* best_ann = new ImplAnnealingSolution(*ann);
    ImplAnnealingSolution* new_ann = new ImplAnnealingSolution(*ann);
    ImplMutateSolution mut = ImplMutateSolution();

    LowerTemperature* law;
    if (law_type == "boltzmann") {
        law = new BoltzmannLaw();
    } else if (law_type == "cauchy") {
        law = new CauchyLaw();
    } else {
        law = new MixedLaw();
    }
    SimulateAnnealing sim = SimulateAnnealing(ann, best_ann, new_ann, mut, *law, 1000);
    sim.simulate_annealing();
    sim.print_loss();
    sim.clear();

    return 0;
}
