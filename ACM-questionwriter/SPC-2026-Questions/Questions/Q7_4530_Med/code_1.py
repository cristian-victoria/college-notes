import sys

def can_feed(cakes, k, m, slice_len):
    # Check if the slice size can feed all people within the cut limit
    
    total_possible_slices = 0
    perfect_counts = []
    
    # find absolute max slices we can get then find which cakes divide by slice_len
    for cake in cakes:
        if cake >= slice_len:
            slices_from_cake = cake // slice_len
            total_possible_slices += slices_from_cake
            
            if cake % slice_len == 0:
                perfect_counts.append(slices_from_cake)

    # If not possible return False
    if total_possible_slices < k:
        return False

    # use perfectly divisible cakes. Sort to harvest the easiest free slices first
    perfect_counts.sort()
    
    slices_accumulated = 0
    fully_consumed_perfect_cakes = 0
    
    for p in perfect_counts:
        # Check if we can take all slices from this perfect cake 
        # without exceeding the number of people we need to feed.
        if slices_accumulated + p <= k:
            slices_accumulated += p
            fully_consumed_perfect_cakes += 1
        else:
            break
            
    min_cuts_needed = k - fully_consumed_perfect_cakes
    
    return min_cuts_needed <= m

def solve():
    # Read all input from standard in
    data = sys.stdin.read().split()
    if not data:
        return
        
    n = int(data[0])
    k = int(data[1])
    m = int(data[2])
    
    # Cakes start at index 3
    cakes = [int(data[i + 3]) for i in range(n)]

    low = 1 # Start at 1 to prevent division by zero
    high = max(cakes) if cakes else 0
    answer = 0

    while low <= high:
        mid = (low + high) // 2

        if can_feed(cakes, k, m, mid):
            answer = mid
            low = mid + 1 # Try to find a larger slice
        else:
            high = mid - 1 # Slice is too big or takes too many cuts

    print(answer)

if __name__ == "__main__":
    solve()