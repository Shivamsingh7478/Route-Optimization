# **Route Optimization using Genetic Algorithm and Ant Colony Optimization**

This project demonstrates a hybrid approach for solving a route optimization problem using Genetic Algorithm (GA) and Ant Colony Optimization (ACO). The objective is to find the shortest possible route visiting all cities in a given set, often referred to as the Traveling Salesman Problem (TSP).

## **How the Code Works**

The code combines the strengths of both Genetic Algorithm and Ant Colony Optimization. It first uses a Genetic Algorithm to generate an initial optimized route and then refines it using Ant Colony Optimization.

### **1. Genetic Algorithm (GA)**

- **Initialization**: Random routes (sequences of city visits) are created for the initial population.
- **Selection**: Routes are evaluated based on the total distance, and the best ones are selected for crossover.
- **Crossover**: Two parent routes are combined to produce a child route, inheriting parts from both parents.
- **Mutation**: Occasionally, the order of cities in a route is swapped to add diversity and prevent premature convergence.
- **Repetition**: This process repeats over several generations, gradually optimizing the routes.

At the end of the GA phase, the best route (shortest distance) found so far is selected as the initial solution for the Ant Colony Optimization phase.

### **2. Ant Colony Optimization (ACO)**

- **Pheromone Trails**: Pheromone levels on paths between cities guide ants to find better routes over time.
- **Ant Routes**: Each ant generates a route by probabilistically selecting cities based on pheromone intensity.
- **Pheromone Update**: Pheromone levels are increased on shorter routes and evaporate over time, encouraging ants to converge on the best routes.

This phase iterates several times to improve the initial GA-optimized route. Finally, the shortest route found is returned as the solution.

## **Input Generation**

The input for this project is a set of cities with their geographical coordinates (latitude and longitude). The cities are read from a text file (`cities.txt`) in the following format:

CityName1 latitude1 longitude1
CityName2 latitude2 longitude2
...

Example of cities.txt:

CityA 12.9715987 77.594566
CityB 13.0826802 80.2707184
CityC 28.6139391 77.2090212
CityD 19.0760 72.8777

User Input

Upon running the program, the user is prompted to:

    Enter the number of cities to include in the route (up to the total number of cities available in the input file).
    Provide the names of the cities they wish to include.

Example Console Input:

Enter the number of cities you want to include in the route (max 4): 3
Available cities:
- CityA
- CityB
- CityC
- CityD
Enter the names of the cities you want to include in the route:
City 1: CityA
City 2: CityB
City 3: CityC

Output

The output consists of:

    Optimized Route Distance: The total distance of the shortest route found by the hybrid GA-ACO approach.
    Optimized Route: The sequence of cities in the order they are visited in the optimized route.

Example Output:

After running the code, the output might look like:

Optimized Route Distance: 210.45 km
Optimized Route: CityA CityB CityC

Running the Code
Prerequisites

You need a C++ compiler such as GCC (e.g., MinGW on Windows) or MSVC (Microsoft Visual Studio).
Compilation and Execution

    Compile the Code:

    bash

g++ RouteOptimization.cpp -o RouteOptimization

Run the Code:

bash

    ./RouteOptimization

Project Parameters

    CITY_COUNT: Number of cities to optimize. This is set by user input.
    POP_SIZE: Number of routes in the Genetic Algorithm population. Default is 100.
    GENERATIONS: Number of iterations for GA and ACO. Default is 500.
    MUTATION_RATE: Probability of mutation in GA. Default is 0.01.
    NUM_ANTS: Number of ants used in the ACO phase. Default is 50.
    PHEROMONE_INTENSITY: Initial pheromone level. Default is 100.
    PHEROMONE_EVAPORATION: Rate at which pheromones evaporate. Default is 0.5.

Notes

    Each run may produce different results due to randomness in city coordinates and initial routes.
    This code serves as a demonstration of how GA and ACO can be combined for route optimization and may be further customized for different problem sizes or requirements.