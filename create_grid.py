import random

# Dimensions for the large M x N file
M = 100  # Number of rows
N = 100  # Number of columns

# Function to generate a random grid with mostly 0s
def generate_large_grid(m, n):
    grid = []
    for i in range(m):
        row = [random.choice([0, 1]) if random.random() < 0.3 else 0 for _ in range(n)]
        grid.append(row)
    return grid

# Generate the large grid
large_grid = generate_large_grid(M, N)

print("Hello")
# Writing the grid data to a file
file_path = './large_grid1.txt'
with open(file_path, 'w') as f:
    for row in large_grid:
        f.write(' '.join(map(str, row)) + '\n')

file_path
