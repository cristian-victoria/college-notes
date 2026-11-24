class Tower:
    def __init__(self):
        self.blocks = []
        self.maxHeight = 0
        self.currentHeight = 0
        self.current_streak = 0
        self.longest_streak = 0

    def place_block(self, x):
        self.blocks.append(x)
        self.currentHeight += x
        self.maxHeight = max(self.maxHeight, self.currentHeight)

    def remove_block(self):
        self.currentHeight -= self.blocks[-1]
        self.blocks.pop()

    def increase_streak(self):
        self.current_streak += 1
        self.longest_streak = max(self.longest_streak, self.current_streak)

    def reset_streak(self):
        self.current_streak = 0

def main():
    N = int(input())

    towers = {'K': Tower(), 'D': Tower()}

    for i in range(N):
        input_line = input().split()
        cmd = input_line[0]
        t = input_line[1]

        if cmd == "PLACE":
            x = int(input_line[2])
            towers[t].place_block(x)
            towers[t].increase_streak()
        elif cmd == "REMOVE":
            towers[t].remove_block()
            towers[t].reset_streak()

    print(f"Max Height K: {towers['K'].maxHeight}")
    print(f"Max Height D: {towers['D'].maxHeight}")
    print(f"Longest Streak K: {towers['K'].longest_streak}")
    print(f"Longest Streak D: {towers['D'].longest_streak}")

if __name__ == "__main__":
    main()