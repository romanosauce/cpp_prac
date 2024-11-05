#include "annealing.h"
#include <vector>

int main() {
    ImplAnnealingSolution* ann = new ImplAnnealingSolution(5, std::vector({1, 2, 3, 4, 10, 12, 3, 100, 3, 5, 17, 2, 5, 4}));
    ImplAnnealingSolution* best_ann = new ImplAnnealingSolution(*ann);
    ImplAnnealingSolution* new_ann = new ImplAnnealingSolution(*ann);
    ImplMutateSolution mut = ImplMutateSolution();
    BoltzmannLaw law = BoltzmannLaw();
    SimulateAnnealing sim = SimulateAnnealing(ann, best_ann, new_ann, mut, law, 1000);
    sim.simulate_annealing();
    sim.print_res();
    sim.clear();
}
