# Author
Cristian Victoria
cjv22c
Due date: February 16 2026
The program in this file is the individual work of Cristian Victoria

# To run program:
bash 
```
python stats_parallel.py --n <int> --workers <int> --mode <"serial", "threads_bad", "threads_good", "mp"> --seed <int>
```

# Performance Evaluation

To evaluate the performance of my different implementations, I used the following inputs for n and seed:

n = 50000, seed = 123

The three worker counts I used were: 

100, 25000, 45000

I analyze the performance of each implementation below.


## Runtime results of Serial Implementation
### w = 100:
bash
```
python3 stats_parallel.py --n 50000 --workers 100 --mode serial --seed 123
```
TIME 0.0056

### w = 25000:
bash
```
python3 stats_parallel.py --n 50000 --workers 25000 --mode serial --seed 123
```
TIME 0.0054

### w = 45000:
bash
```
python3 stats_parallel.py --n 50000 --workers 45000 --mode serial --seed 123
```
TIME 0.0053

### Analysis:
Runtime stays almost identical since it does not use workers. Execution time only depends on N. This is expected since the computation is strictly sequential. However, it is not perfectly linear dueto cache hierarchy effects where data that does not fit into L1/L2 cache can spill into L3 cache and RAM, resulting in runtime curve bending upwards.

## Runtime results of Threaded Implementation (incorrect)
### w = 100:
bash
```
python3 stats_parallel.py --n 50000 --workers 100 --mode threads_bad --seed 123
```
TIME 0.0140

### w = 25000:
bash
```
python3 stats_parallel.py --n 50000 --workers 25000 --mode threads_bad --seed 123
```
TIME 0.8456

### w = 45000:
bash
```
python3 stats_parallel.py --n 50000 --workers 45000 --mode threads_bad --seed 123
```
TIME 1.5344

### Analysis:
There is a linear correlation: as the number of workers increases, the run time increases. Creating larger numbers of threads introduces greater scheduling and overhead.

## Runtime results of Corrected Threaded Implementation
### w = 100:
bash
```
python3 stats_parallel.py --n 50000 --workers 100 --mode threads_lock --seed 123
```
TIME 0.0101

### w = 25000:
bash
```
python3 stats_parallel.py --n 50000 --workers 25000 --mode threads_lock --seed 123
```
TIME 0.8517

### w = 45000:
bash
```
python3 stats_parallel.py --n 50000 --workers 45000 --mode threads_lock --seed 123
```
TIME 1.5184

### Analysis:
Even with correct synchronization, we still see a similar correlation between the number of workers and the runtime as with the incorrect implementation. 

## Runtime results of Multiprocessing Implementation
### w = 100:
bash
```
python3 stats_parallel.py --n 50000 --workers 100 --mode mp --seed 123
```
TIME 1.6046

### w = 25000:
bash
```
python3 stats_parallel.py --n 50000 --workers 25000 --mode mp --seed 123
```
BlockingIOError: [Errno 35] Resource temporarily unavailable

### w = 45000:
bash
```
python3 stats_parallel.py --n 50000 --workers 45000 --mode mp --seed 123
```
BlockingIOError: [Errno 35] Resource temporarily unavailable

### Analysis:
Multiprocessing uses real OS processes. It is slower for small problem sizes because process creation still induces massive overheads. There seems to be a point where the OS outright prevents the creation of more workers as it is bounded by limits in my current hardware. 

python3 stats_parallel.py --n 6000000 --workers 8 --mode mp