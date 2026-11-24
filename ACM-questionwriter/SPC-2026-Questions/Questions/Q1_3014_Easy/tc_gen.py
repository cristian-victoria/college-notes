"""
tc_gen makes 4 test case files and output-solution files:

* Make sure to build the solution first.
To run: python tc_gen.py <line_count>

Ex: python tc_gen.py 50
Makes 4 files in the order: all-valid, all-invalid, 2 * mixed.

>> ./Input/in{1-4}.txt
>> ./Output/out{1-4}.txt

"""

import os
import random
import sys
import subprocess
from pathlib import Path

def run_generator(path: Path, count: int, func):
    with open(path, "w+") as file:
        for _ in range(count):
            id = func()
            file.write(id + "\n")

def valid_case() -> str:
    digits = [str(random.randint(0, 9)) for _ in range(21)]
    odd_sum, even_sum = 0, 0
    for idx, num in enumerate(digits):
        if idx % 2 == 1:
            odd_sum += int(num)
        else:
            even_sum += int(num)
    total_sum = (odd_sum * 3) + even_sum
    final_digit = (10 - (total_sum % 10)) % 10
    digits.append(str(final_digit))
    return "".join(digits)

def invalid_case() -> str:
    base = valid_case()
    if base[21] == "9":
        base = base[:21] + "0"
    else:
        base = base[:21] + str(int(base[21]) + 1)
    return base

def mix_case() -> str:
    rand = random.random()
    if rand <= 0.49:
        return valid_case()
    else:
        return invalid_case()

if __name__ == "__main__":
    if os.path.exists("./solution.out") == False:
        print("Build the solution first")
        sys.exit(1)

    args = sys.argv
    count = int(args[1])
    order = ["valid", "invalid", "mix", "mix"]

    for i in range(len(order)):
        path = Path(f"./Input/in{i+1}.txt")
        match order[i]:
            case "valid":
                run_generator(path, count, valid_case)
            case "invalid":
                run_generator(path, count, invalid_case)
            case "mix":
                run_generator(path, count, mix_case)
        subprocess.run(
            f"./solution.out < ./Input/in{i+1}.txt > ./Output/out{i+1}.txt", shell=True
        )
