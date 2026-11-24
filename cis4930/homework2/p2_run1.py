import numpy as np
from scipy.spatial.distance import euclidean, cdist

# ------------------------------ 
# Function Definitions
# ------------------------------

def load_seeds_data(filepath):
    """Load the seeds dataset"""

    print("Loading Seeds dataset...")

    data = np.loadtxt(filepath)
    print(f"\n{data.shape[0]} samples, {data.shape[1]} features")
    return data

def initialize_centroids(data, k, random_state=None):
    """ Randomly initialize k centroids by selecting k random samples from the data
    Returns centroids : numpy array of shape (k, n_features)"""
    if random_state is not None:
        np.random.seed(random_state)
    
    # Randomly select k samples as initial centroids
    n_samples = data.shape[0]
    indices = np.random.choice(n_samples, size=k, replace=False)
    centroids = data[indices].copy()
    
    return centroids

def assign_clusters(data, centroids):
    """ Assign each data point to the nearest centroid
    Returns labels : numpy array of shape (n_samples,) = Cluster assignment for each sample
    """
    # Compute distances from each point to each centroid using euclidean distance
    distances = cdist(data, centroids, metric='euclidean')
    
    # Assign each point to nearest centroid
    labels = np.argmin(distances, axis=1)
    
    return labels

def update_centroids(data, labels, k):
    """ Update centroids as the mean of all points assigned to each cluster
    Returns new_centroids : numpy array of shape (k, n_features) """
    n_features = data.shape[1]
    new_centroids = np.zeros((k, n_features))
    
    for i in range(k):
        # Get all points assigned to cluster i
        cluster_points = data[labels == i]
        
        if len(cluster_points) > 0:
            # Compute mean of all points in cluster
            new_centroids[i] = cluster_points.mean(axis=0)
        else:
            # If cluster is empty, keep the old centroid
            print(f"Warning: Cluster {i} is empty!")
            # Reinitialize to a random point
            new_centroids[i] = data[np.random.choice(data.shape[0])]
    
    return new_centroids

def compute_sse(data, labels, centroids):
    """ Compute Sum of Squared Errors (SSE)
    Returns sse : float"""
    sse = 0.0
    
    for i in range(len(centroids)):
        # Get all points in cluster i
        cluster_points = data[labels == i]
        
        if len(cluster_points) > 0:
            # Compute squared distances to centroid
            centroid = centroids[i]
            squared_distances = np.sum((cluster_points - centroid) ** 2, axis=1)
            sse += np.sum(squared_distances)
    
    return sse

def kmeans(data, k, max_iterations=100, tolerance=0.001, random_state=None, verbose=False):
    """ K-Means clustering algorithm
    Returns: 
    labels : numpy array of shape (n_samples,) = Final cluster assignments
    centroids : numpy array of shape (k, n_features) = Final centroid positions
    sse : float, final sum of squared errors
    n_iterations : int, number of iterations until convergence
    sse_history : list, SSE value at each iteration """
    # Initialize centroids
    centroids = initialize_centroids(data, k, random_state)
    
    sse_history = []
    prev_sse = float('inf')
    
    for iteration in range(max_iterations):
        # Step 1: Assign clusters
        labels = assign_clusters(data, centroids)
        
        # Step 2: Compute SSE
        current_sse = compute_sse(data, labels, centroids)
        sse_history.append(current_sse)
        
        if verbose:
            print(f"  Iteration {iteration + 1}: SSE = {current_sse:.4f}")
        
        # Step 3: Check convergence
        sse_change = abs(prev_sse - current_sse)
        if sse_change < tolerance:
            if verbose:
                print(f"  Converged after {iteration + 1} iterations (SSE change < {tolerance})")
            break
        
        # Step 4: Update centroids
        centroids = update_centroids(data, labels, k)
        
        prev_sse = current_sse
    
    # Final assignment
    labels = assign_clusters(data, centroids)
    final_sse = compute_sse(data, labels, centroids)
    
    return labels, centroids, final_sse, iteration + 1, sse_history

def run_multiple_kmeans(data, k, n_runs=10, max_iterations=100, tolerance=0.001, verbose=True):
    """ Run k-means multiple times with different random initializations    
    Returns:
    results : 
        dict containing:
        - all_sse : list of final SSE values for each run
        - avg_sse : average SSE across all runs"""
    
    all_sse = []
    
    print(f"\nRunning k-means for k={k} with {n_runs} different initializations...")
    
    for run in range(n_runs):
        if verbose:
            print(f"\nRun {run + 1}/{n_runs}:")
        
        labels, centroids, sse, n_iterations, sse_history = kmeans(
            data, k,
            max_iterations=max_iterations,
            tolerance=tolerance,
            random_state=run,  # Different seed for each run
            verbose=verbose
        )
        
        all_sse.append(sse)
        
        if verbose:
            print(f"  Final SSE: {sse:.4f} (converged in {n_iterations} iterations)")
    
    results = {
        'all_sse': all_sse,
        'avg_sse': np.mean(all_sse),
    }
    
    print(f"\nAverage SSE for k={k}:    {results['avg_sse']:.4f}")
    print("\n------------------------------\n")

    return results

# ------------------------------ 
# Main Execution 
# ------------------------------

# Load data
data = load_seeds_data('DataProblem2/seeds.txt')

# Parameters
k_values = [3, 5, 7]
n_runs = 10
max_iterations = 100
tolerance = 0.001

# Run k-means for each k value
all_results = {}

for k in k_values:
    results = run_multiple_kmeans(
        data, k,
        n_runs=n_runs,
        max_iterations=max_iterations,
        tolerance=tolerance,
        verbose=False  # Set to True for detailed output
    )
    all_results[k] = results