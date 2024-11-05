import input.generator as generator
import subprocess
import time
import pickle
import numpy as np
import matplotlib.pyplot as plt


def experiment1():
    print("Experiment 1: finding parameters with running time > 60s")

    for N in [200, 400, 800, 1600, 3200, 6400]:
        file_name = f'input/input_ex1_k20_n{N}.csv'
        generator.generator(20, N, 10, 100, file_name)
        st = time.time()
        p = subprocess.run(f"./experiment {file_name}", shell=True, capture_output=True,
                           encoding='utf8')
        print(f"{N}: time {time.time()-st:.2f}")

    print(f"With N (number of tasks) == {heavy_N} and k (number of processor units) == {heavy_k} \
simulated annealing was working for roughly 66 seconds")


heavy_N = 6000
heavy_k = 20


def experiment2():
    print(f"Experiment 2: comparison of temperature lowering laws, k = {heavy_k}, N = {heavy_N}")

    mean_runs = 3

    for law in ['boltzmann', 'cauchy', 'mixed']:
        file_name = f'input/input_ex2_k{heavy_k}_n{heavy_N}_{law}.csv'
        generator.generator(heavy_k, heavy_N, 10, 100, file_name)
        loss = 0
        run_time = 0
        for i in range(mean_runs):
            print(f"Run {i}: {law}")
            st = time.time()
            p = subprocess.run(f"./experiment {file_name} {law}", shell=True, capture_output=True,
                               encoding='utf8')
            dur = time.time() - st
            run_time += dur
            print(f"    time: {dur:.2f}")
            print(f"    loss: {p.stdout.split()[0]}")
            loss += int(p.stdout.split()[0])

        print(f"mean time for law {law} over {mean_runs} runs: {run_time / mean_runs:.2f}")
        print(f"mean loss for law {law} over {mean_runs} runs: {loss / mean_runs:.2f}")

    print(f"Both best loss and running time were achieved when using Boltzmann law.\n\
Worst mean running time was with mixed law. Although this didn't decrease overall loss")


def experiment3():
    print(f"Experiment 3: heatmap, k = 2..{heavy_k}, N = 300..{heavy_N}, law = boltzmann")

    mean_runs = 5

    procs = [2, 4, 6, 8, 10, 12, 14, 16, 18, 20]
    tasks = [300 * i for i in range(1, 21)]

    # procs = [2, 4, 6]
    # tasks = [300 * i for i in range(1, 3)]


    res = np.zeros(len(procs) * len(tasks)).reshape(len(procs), len(tasks))
    for i, proc in enumerate(procs):
        for j, task_num in enumerate(tasks):
            run_time = 0
            file_name = f'input/input_ex3_k{proc}_n{task_num}.csv'
            generator.generator(proc, task_num, 10, 100, file_name)

            print(f"Runs for proc_num {proc}, task_num {task_num}")
            for k in range(mean_runs):
                print(f"  Run {k}")
                st = time.time()
                p = subprocess.run(f"./experiment {file_name} boltzmann", shell=True, capture_output=True,
                                   encoding='utf8')
                dur = time.time() - st
                run_time += dur
                print(f"    time: {dur:.2f}")

            mean_run_time = run_time / mean_runs
            res[i, j] = mean_run_time

            print(f"mean time over {mean_runs} runs: {mean_run_time:.2f}")
            print()

    with open('output/heat_map_data.pkl', 'wb') as f:
        pickle.dump(res, f)

    print("Experiment 3 finished!")
    print("You can find data for heat map in output/heat_map_data.pkl file")


def draw_heat_map(in_file, out_file, labels_x, labels_y):
    with open(in_file, 'rb') as f:
        arr = pickle.load(f)
    fig, ax = plt.subplots()
    im = ax.imshow(arr, cmap='inferno', interpolation='nearest')

    ax.set_yticks(np.arange(len(labels_x)), labels=labels_x)
    ax.set_xticks(np.arange(len(labels_y)), labels=labels_y, rotation=90)
    ax.invert_yaxis()

    plt.colorbar(im, shrink=0.55)
    plt.title("Algorithm execution time, s")
    plt.xlabel("Number of tasks")
    plt.ylabel("Number of processor unit")

    plt.savefig(out_file)

# experiment3()

procs = [2, 4, 6, 8, 10, 12, 14, 16, 18, 20]
tasks = [300 * i for i in range(1, 21)]
draw_heat_map('output/heat_map_data.pkl', 'output/heat_map.png', procs, tasks)
