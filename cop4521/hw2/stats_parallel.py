"""
Cristian Victoria
cjv22c
Due date: February 16 2026
The program in this file is the individual work of Cristian Victoria
"""

import random
import time
import threading
import argparse
import multiprocessing as mp

# Global variables shared by all threads
global_sum = 0
global_minimum = float('inf')
global_maximum = float('-inf')
global_mean = 0
global_variance = 0

"""
Serial Implementation
"""
def serial_imp(arr):
    total = 0
    minimum = arr[0]
    maximum = arr[0]
    
    for value in arr:
        total += value
        if value < minimum:
            minimum = value
        if value > maximum:
            maximum = value
    
    mean = total / len(arr)
    standard_dev = sum((x - mean) ** 2 for x in arr)
    variance = standard_dev / (len(arr) - 1)

    return total, minimum, maximum, mean, variance

"""
Threads bad Implementation
"""
def compute_chunk_bad(arr, start_index, end_index):
    #initialize global variables
    global global_sum, global_minimum, global_maximum, global_mean, global_variance
    
    #initialize local variables
    local_sum = 0
    local_min = arr[start_index]
    local_max = arr[start_index]
    
    # computes stats operations on local variables
    for i in range(start_index, end_index):
        value = arr[i]
        local_sum += value
        if value < local_min:
            local_min = value
        if value > local_max:
            local_max = value
    
    chunk_length = end_index - start_index
    local_mean = local_sum / chunk_length
    local_standard_dev = sum((x - local_mean) ** 2 for x in arr[start_index:end_index])
    local_variance = local_standard_dev / chunk_length - 1

    # update global variables accordingly
    global_sum += local_sum
    
    if local_min < global_minimum:
        global_minimum = local_min
    if local_max > global_maximum:
        global_maximum = local_max

    global_mean = local_mean
    global_variance = local_variance

def thread_bad(A, k):
    # If N = 10 and k = 3, then chunk_size = 3
    # [0-3], [4-6], [7-10]
    chunk_size = len(A) // k

    t_list = []    # list of threads to append to

    for i in range(k):
        start_index = i * chunk_size
        if i != k - 1:
            end_index = (i + 1) * chunk_size
        else:
            end_index = len(A)

        thread = threading.Thread(target=compute_chunk_bad, args=(A, start_index, end_index))
        thread.start()
        t_list.append(thread) # append to list of threads
        

    for thread in t_list:
        thread.join()

    return global_sum, global_minimum, global_maximum, global_mean, global_variance

"""
Threads good Implementation
"""
def compute_chunk_safe(arr, start_index, end_index, results, index):
    #initialize local variables
    local_sum = 0
    local_min = arr[start_index]
    local_max = arr[start_index]
    
    # computes stats operations on local variables
    for i in range(start_index, end_index):
        value = arr[i]
        local_sum += value
        if value < local_min:
            local_min = value
        if value > local_max:
            local_max = value
    
    results[index] = (local_sum, local_min, local_max)

def threads_lock(A, k):
    # If N = 10 and k = 3, then chunk_size = 3
    # [0-3], [4-6], [7-10]
    chunk_size = len(A) // k
    results = [None] * k

    t_list = []    # list of threads to append to

    for i in range(k):
        start_index = i * chunk_size
        if i != k - 1:
            end_index = (i + 1) * chunk_size
        else:
            end_index = len(A)

        thread = threading.Thread(target=compute_chunk_safe, args=(A, start_index, end_index, results, i))
        thread.start()
        t_list.append(thread) # append to list of threads
        

    for thread in t_list:
        thread.join()

    total_sum = 0
    global_min = float('inf')
    global_max = float('-inf')

    for partial_sum, partial_min, partial_max in results:
        total_sum += partial_sum
        global_min = min(global_min, partial_min)
        global_max = max(global_max, partial_max)

    local_mean = total_sum / len(A)

    local_standard_dev = sum((x - local_mean) ** 2 for x in A)
    local_variance = local_standard_dev / (len(A) - 1)

    return total_sum, global_min, global_max, local_mean, local_variance

"""
Multiprocessing
"""
def compute_chunk_mp(args):
    arr, start_index, end_index = args

    local_sum = 0
    local_min = arr[start_index]
    local_max = arr[start_index]

    for i in range(start_index, end_index):
        value = arr[i]
        local_sum += value

        if value < local_min:
            local_min = value
        if value > local_max:
            local_max = value

    return local_sum, local_min, local_max

def multiprocessing_imp(A, k):
    chunk_size = len(A) // k
    p_list = []

    for i in range(k):
        start_index = i * chunk_size

        if i != k - 1:
            end_index = (i + 1) * chunk_size
        else:
            end_index = len(A)

        p_list.append((A, start_index, end_index))

    with mp.Pool(processes=k) as pool:
        results = pool.map(compute_chunk_mp, p_list)

    total_sum = 0
    global_min = float('inf')
    global_max = float('-inf')

    for partial_sum, partial_min, partial_max in results:
        total_sum += partial_sum
        global_min = min(global_min, partial_min)
        global_max = max(global_max, partial_max)

    mean = total_sum / len(A)

    squared_diff_sum = sum((x - mean) ** 2 for x in A)
    variance = squared_diff_sum / (len(A) - 1)

    return total_sum, global_min, global_max, mean, variance

"""
Compute Checksum
"""
def compute_check(total, minimum, maximum):
    return (total+minimum+maximum) % 1000000011

"""
Argument Paser to run from command line, like "python file_name.py --n 6000000 --workers 10 --mode mp --seed 123"
"""
def parse_args():
    parser = argparse.ArgumentParser(description="Parallel statistics computation")

    # --n specifies how many integers to generate
    parser.add_argument("--n", type=int, required=True, help="Number of integers")
    # --w specifies how many threads or processes to use
    parser.add_argument("--workers", type=int, required=True, help="Number of threads or processes")
    # --mode selects the execution mode (serial, threads_bad, threads_lock, or mp)
    parser.add_argument("--mode", type=str, required=True,
                        choices=["serial", "threads_bad", "threads_lock", "mp"],
                        help="Execution mode")
    # --seed specifies the random seed for reproducibility
    parser.add_argument("--seed", type=int, default=50, help="Random seed")
    
    return parser.parse_args()


"""
MAIN
"""
def main():
    args = parse_args()

    N = args.n
    k = args.workers
    mode = args.mode
    seed = args.seed

    A = [random.randint(0, 100) for _ in range(N)]

    if N <= 2:
        raise ValueError("N must be greater than 2")

    if k <= 0 or k >= N:
        raise ValueError("workers must satisfy 0 < workers < n")

    random.seed(seed)

    A = [random.randint(0, 100) for _ in range(N)]

    start_time = time.time()

    if mode == "serial":
        result = serial_imp(A)

    elif mode == "threads_bad":
        result = thread_bad(A, k)

    elif mode == "threads_lock":
        result = threads_lock(A, k)

    elif mode == "mp":
        result = multiprocessing_imp(A, k)

    end_time = time.time()

    total, minimum, maximum, mean, variance = result
    check_value = compute_check(total, minimum, maximum)

    print(f"\nMODE {mode}")
    print(f"N {N} WORKERS {k}")
    print(f"SUM {total}")
    print(f"MIN {minimum} MAX {maximum}")
    print(f"MEAN {mean}")
    print(f"VAR {variance}")
    print(f"TIME {end_time - start_time:.4f}")
    print(f"CHECK {check_value}")


if __name__ == "__main__":
    main()