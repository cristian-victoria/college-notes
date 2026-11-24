import sys
import heapq

def solve():
    input = sys.stdin.read().split()
    it = iter(input)
    nxt = lambda: int(next(it))

    for _ in range(nxt()):
        N, Md, Mu = nxt(), nxt(), nxt()
        adj = [[] for _ in range(N + 1)]

        for _ in range(Md):
            u, v, w = nxt(), nxt(), nxt()
            adj[u].append((v, w))

        for _ in range(Mu):
            u, v, w = nxt(), nxt(), nxt()
            adj[u].append((v, w))
            adj[v].append((u, w))

        dist = [float('inf')] * (N + 1)
        dist[1] = 0
        heap = [(0, 1)]

        while heap:
            cost, u = heapq.heappop(heap)
            if cost > dist[u]:
                continue
            for v, w in adj[u]:
                if cost + w < dist[v]:
                    dist[v] = cost + w
                    heapq.heappush(heap, (dist[v], v))

        print(dist[N] if dist[N] < float('inf') else "Punt")

if __name__ == "__main__":
    solve()