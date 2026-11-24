import subprocess
import time
import os

STEPS = [
    {
        'name': 'Problem 1 - SVM with Polynomial and RBF Kernels',
        'script': 'p1_run1.py'
    },
    {
        'name': 'Problem 2 - K-Means Clustering',
        'script': 'p2_run1.py'
    }
]

print("\nThis pipeline will execute 2 scripts:")
for i, step in enumerate(STEPS, 1):
    print(f"  {i}. {step['name']}")

# Execute each step
for i, step in enumerate(STEPS, 1):
    print("\n==============================")
    print(f"STEP {i}/{len(STEPS)}: {step['name']}")
    print("==============================\n")
    
    try:
        # Run the script
        result = subprocess.run(
            ['python3', step['script']],
            capture_output=False,
            text=True,
            check=True
        )
                
    except subprocess.CalledProcessError as e:
        print(f"\n✗ ERROR in Step {i}: {step['name']}")
        print(f"Script: {step['script']}")
        print("Please check the error messages above and fix before continuing.")
        exit(1)
    except FileNotFoundError:
        print(f"\n✗ ERROR: Script not found: {step['script']}")
        print("Please ensure all scripts are in the current directory.")
        exit(1)