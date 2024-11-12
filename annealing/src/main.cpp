#include "annealing.h"
#include <vector>

int main() {
    ImplAnnealingSolution* ann = new ImplAnnealingSolution(10, std::vector({53, 95, 68, 81, 70, 84, 78, 51, 88, 83, 89, 66, 64, 74, 78, 92,
            51, 59, 62, 89, 60, 66, 84, 93, 98, 78, 52, 99, 93, 63, 76, 71,
            53, 59, 65, 94, 59, 55, 93, 54, 57, 68, 64, 70, 71, 93, 88, 71,
            50, 81, 59, 99, 56, 72, 62, 61, 76, 52, 57, 67, 57, 81, 55, 60,
            64, 94, 67, 80, 56, 98, 60, 87, 81, 84, 53, 75, 65, 87, 54, 89,
            64, 69, 63, 91, 67, 56, 72, 75, 72, 52, 51, 61, 70, 65, 73, 65,
            93, 93, 97, 74, 61, 62, 58, 84, 98, 76, 76, 85, 81, 84, 75, 87,
            87, 63, 67, 84, 77, 64, 83, 75, 83, 85, 66, 57, 84, 73, 98, 62}));
    ImplAnnealingSolution* best_ann = new ImplAnnealingSolution(*ann);
    ImplAnnealingSolution* new_ann = new ImplAnnealingSolution(*ann);
    ImplMutateSolution mut = ImplMutateSolution();
    CauchyLaw law = CauchyLaw();
    SimulateAnnealing sim = SimulateAnnealing(ann, best_ann, new_ann, mut, law, 1000);
    sim.simulate_annealing();
    sim.print_res();
    sim.clear();
}
