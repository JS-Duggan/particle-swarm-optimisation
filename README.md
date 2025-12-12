# Particle Swarm Optimization for Portfolio Optimization

A multi-objective Particle Swarm Optimization (PSO) implementation for finding optimal portfolio allocations using S&P 500 stock data. This project optimizes portfolios by simultaneously maximizing returns and minimizing variance (risk), producing a Pareto front of non-dominated solutions.

## Overview

This project implements a multi-objective PSO algorithm to solve the portfolio optimization problem. Given historical stock data from the S&P 500, it finds a set of optimal portfolio weight allocations that represent the best trade-offs between expected returns and portfolio risk (variance).

The workflow consists of three main stages:
1. **Data Preprocessing**: Convert raw stock price data into annualized returns and covariance matrices
2. **Optimization**: Run multi-objective PSO to find the Pareto front
3. **Results**: Output portfolio allocations with their corresponding returns and variance

## Features

- **Multi-objective Optimization**: Simultaneously optimizes for returns and variance
- **Pareto Front**: Generates a set of non-dominated solutions representing optimal risk-return trade-offs
- **Parallel Processing**: Utilizes 8 threads for efficient computation
- **Crowding Distance**: Maintains diversity in the Pareto front using crowding distance metrics
- **Portfolio Constraints**: Automatically normalizes weights to ensure valid portfolio allocations

## Requirements

### Python Dependencies
- Python 3.x
- pandas
- numpy

Install with:
```bash
pip install pandas numpy
```

### C++ Requirements
- C++ compiler with C++17 support (g++, clang++, etc.)
- Standard library support for threading (`<thread>`)

## Data Source

The project uses S&P 500 stock data from Kaggle:
- **Dataset**: [S&P 500 Stock Data](https://www.kaggle.com/datasets/camnugent/sandp500/discussion?sort=hotness)
- **Time Period**: 5 years of historical data
- **Format**: CSV file with columns including date, stock name (ticker), and closing prices

## Installation

1. Clone or download this repository
2. Install Python dependencies:
   ```bash
   pip install pandas numpy
   ```
3. Download the S&P 500 dataset from Kaggle and place it in the project directory as `sp500_data.csv`

## Usage

### Step 1: Prepare the Data

Run the preprocessing script to convert raw stock data into covariance and returns matrices:

```bash
python objective_calculation.py
```

This script:
- Loads the stock price data from `sp500_data.csv`
- Calculates daily log returns
- Computes the covariance matrix
- Annualizes both returns and covariance (assuming 252 trading days per year)
- Outputs two CSV files:
  - `mean_returns.csv`: Annualized mean returns for each stock
  - `covariance_matrix.csv`: Annualized covariance matrix

**Note**: Update the `data` variable in `objective_calculation.py` if your input file has a different name.

### Step 2: Compile the C++ Program

Compile the main optimization program:

```bash
g++ -std=c++17 -pthread -O2 main.cpp -o pso_portfolio
```

Or using clang++:

```bash
clang++ -std=c++17 -pthread -O2 main.cpp -o pso_portfolio
```

### Step 3: Run the Optimization

Execute the compiled program with the generated matrices as arguments:

```bash
./pso_portfolio mean_returns.csv covariance_matrix.csv
```

The program will:
- Load the returns and covariance matrices
- Initialize a swarm of 1000 particles in a 505-dimensional search space (one dimension per stock)
- Run 1000 iterations of the PSO algorithm
- Output progress to the console
- Save the Pareto front to `results.csv`

### Step 4: Analyze Results

The output file `results.csv` contains the Pareto front solutions. Each line represents one solution with the following format:

```
w1,w2,w3,...,w505,return,variance
```

Where:
- `w1, w2, ..., w505`: Portfolio weights (allocations) for each stock, summing to 1.0
- `return`: Negative annualized return (to be negated for actual return)
- `variance`: Portfolio variance (risk measure)

**Note**: The return value in the output is negated (since the algorithm minimizes negative returns). To get the actual return, multiply by -1.

The file contains portfolio weights followed by the fitness values (returns and variance) for each solution in the Pareto front.

## Algorithm Details

### Particle Swarm Optimization Parameters

- **Dimensions**: 505 (number of stocks in S&P 500)
- **Particles**: 1000
- **Iterations**: 1000
- **Inertial Weight (w)**: 0.7
- **Cognitive Coefficient (c1)**: 1.4
- **Social Coefficient (c2)**: 1.4
- **Search Space**: [0, 1] for each dimension (weights are normalized after each update)

### Multi-Objective Optimization

The algorithm optimizes two objectives:
1. **Maximize Returns**: Minimizes the negative of portfolio returns
   - Formula: `-Σ(weight_i × return_i)`
2. **Minimize Variance**: Minimizes portfolio variance
   - Formula: `Σ_i Σ_j (weight_i × weight_j × covariance_ij)`

### Pareto Front Management

- Maintains a maximum of 100 solutions in the Pareto front
- Uses crowding distance to ensure diversity
- Updates the front after each iteration by checking dominance relationships

### Parallel Processing

The algorithm uses 8 threads to process particles in parallel, dividing the swarm into 8 equal segments (125 particles each).

## Output Interpretation

The Pareto front represents the set of optimal portfolios where:
- No solution dominates another (no solution is better in both objectives)
- Each solution represents a different risk-return trade-off
- Solutions with higher returns typically have higher variance
- Solutions with lower variance typically have lower returns

You can visualize the Pareto front by plotting return (y-axis) vs. variance (x-axis) to see the efficient frontier.

## File Structure

```
particle-swarm-optimisation/
├── main.cpp                  # Main program entry point
├── particle.cpp              # Particle structure and operations
├── swarm.cpp                 # Swarm class and PSO algorithm
├── objective_calculation.py  # Data preprocessing script
├── sp500_data.csv            # Input stock data (user-provided)
├── mean_returns.csv          # Generated returns matrix
├── covariance_matrix.csv     # Generated covariance matrix
└── results.csv               # Output Pareto front
```

## Customization

### Adjusting PSO Parameters

Edit the parameters in `main.cpp` (line 91):

```cpp
Swarm swarm(505, 1000, 0.7, 1.4, 1.4, {returns, variance});
```

Parameters: `(dimensions, particles, w, c1, c2, objectives)`

### Changing Iterations

Modify the loop in `main.cpp` (line 95):

```cpp
for (int i = 0; i < 1000; i++) {
```

### Adjusting Pareto Front Size

Modify the maximum front size in `swarm.cpp` (line 205):

```cpp
while (pareto_front.size() > 100)
```

## Notes

- The algorithm assumes 252 trading days per year for annualization
- Portfolio weights are automatically normalized to sum to 1.0
- The search space is constrained to [0, 1] with boundary reflection
- Velocity is clamped to 10% of the search space range

## Author

Jack Duggan
