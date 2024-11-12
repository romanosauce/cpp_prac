#include "src/annealing.h"
#include <fstream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>

int main (int argc, char *argv[]) {
    std::string input_file = argv[1];

    std::string law_type = "mixed";
    if (argc >= 3) {
        law_type = argv[2];
    }

    int PROCS = 4;
    if (argc == 4) {
        PROCS = std::stoi(std::string(argv[3]));
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

    //k = 3;
    //tasks = std::vector({1, 10, 20, 222, 1, 50, 70, 23, 14});

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

    int max_iter_without_improvement = 10;
    int without_improvement = 0;
    int best_loss = ann->get_loss_metric();

    while (without_improvement < max_iter_without_improvement) {
        //std::cout << "SEND\n";
        //ann->print();
        std::vector<int> sons_sockets{};
        for (int i = 0; i < PROCS; ++i) {
            int descs[2];
            socketpair(AF_UNIX, SOCK_STREAM, 0, descs);

            pid_t pid = fork();
            if (pid == 0) {
                close(descs[0]);
                SimulateAnnealing sim = SimulateAnnealing(ann, best_ann, new_ann, mut, *law, 1000);
                sim.simulate_annealing();
                //std::cout << "PROC " << i << ":\n";
                //sim.print_res();
                sim.get_solution()->to_bytes(descs[1]);

                sim.clear();
                delete law;

                close(descs[1]);
                return 0;
            } else {
                close(descs[1]);
                sons_sockets.push_back(descs[0]);
            }
        }

        for (int i = 0; i < PROCS; ++i) {
            new_ann->from_bytes(sons_sockets[i]);
            close(sons_sockets[i]);
            if (int loss; (loss = new_ann->get_loss_metric()) < best_loss) {
                *ann = *new_ann;
                *best_ann = *new_ann;
                best_loss = loss;
                without_improvement = 0;
                std::cout << "NEW BEST SOLUTION\n";
                std::cout << loss << '\n';
            }
        }
        ++without_improvement;
        int status;
        while (wait(&status) != -1);
    }
    std::cout << ann->get_loss_metric() << '\n';

    delete ann;
    delete best_ann;
    delete new_ann;
    delete law;
    return 0;
}
