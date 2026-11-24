"""
Cristian Victoria
cjv22c
Due date: Feb 2 2026

The program in this file is the individual work of Cristian Victoria.
"""

def pattern(N) -> str:
    # Every digit until 9 accounts for 1 dot, after that it accounts for 2
    if(N > 9):
        num_dots: int = ((N - 9) * 2) + 8
    else:
        num_dots: int = N - 1

    left_nums: str = ""
    right_nums: str = ""
    for n in range(1, N+1):
        numbers = f"{left_nums}1{right_nums}"
        print("."*num_dots + numbers + "."*num_dots)
        left_nums = f"{n+1}{left_nums}"
        right_nums = f"{right_nums}{n+1}"
        if n < 9:
            num_dots = num_dots - 1
        else:
            num_dots = num_dots - 2

    return " "


if __name__ == "__main__":
    # check that N remains within 1 and 99
    within_bounds: bool = False
    while(within_bounds == False):
        user_input = int(input("Enter a number between 1 and 99: "))
        if user_input > 0 and user_input < 100:
            within_bounds = True    
    
    print(pattern(user_input))
