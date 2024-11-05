#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>

using namespace std;

const int CITY_COUNT = 10;       // Number of cities
const int POP_SIZE = 100;        // Population size for GA
const int GENERATIONS = 500;     // Number of generations for GA
const double MUTATION_RATE = 0.01;
const int NUM_ANTS = 50;         // Number of ants for ACO
const double PHEROMONE_INTENSITY = 100;
const double PHEROMONE_EVAPORATION = 0.5;

struct City {
    int x, y;
    City(int x = 0, int y = 0) : x(x), y(y) {}
};


double calculateDistance(const City& a, const City& b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

// Genetic Algorithm Functions
double routeDistance(const vector<City>& cities, const vector<int>& route) {
    double totalDistance = 0;
    for (int i = 0; i < CITY_COUNT - 1; ++i) {
        totalDistance += calculateDistance(cities[route[i]], cities[route[i + 1]]);
    }
    totalDistance += calculateDistance(cities[route[CITY_COUNT - 1]], cities[route[0]]);
    return totalDistance;
}

vector<int> createRoute() {
    vector<int> route(CITY_COUNT);
    for (int i = 0; i < CITY_COUNT; ++i) route[i] = i;
    random_shuffle(route.begin(), route.end());
    return route;
}

void mutate(vector<int>& route) {
    if ((rand() / double(RAND_MAX)) < MUTATION_RATE) {
        int i = rand() % CITY_COUNT;
        int j = rand() % CITY_COUNT;
        swap(route[i], route[j]);
    }
}

vector<int> crossover(const vector<int>& parent1, const vector<int>& parent2) {
    vector<int> child(CITY_COUNT, -1);
    int start = rand() % CITY_COUNT;
    int end = start + rand() % (CITY_COUNT - start);

    for (int i = start; i <= end; ++i) {
        child[i] = parent1[i];
    }

    int j = 0;
    for (int i = 0; i < CITY_COUNT; ++i) {
        if (find(child.begin(), child.end(), parent2[i]) == child.end()) {
            while (child[j] != -1) j++;
            child[j] = parent2[i];
        }
    }
    return child;
}

// Ant Colony Optimization Functions
void initializePheromones(vector<vector<double>>& pheromones) {
    for (int i = 0; i < CITY_COUNT; ++i)
        for (int j = 0; j < CITY_COUNT; ++j)
            pheromones[i][j] = 1.0;
}

vector<int> antRoute(const vector<City>& cities, const vector<vector<double>>& pheromones) {
    vector<int> route;
    vector<bool> visited(CITY_COUNT, false);
    int currentCity = rand() % CITY_COUNT;
    route.push_back(currentCity);
    visited[currentCity] = true;

    for (int i = 1; i < CITY_COUNT; ++i) {
        double maxPheromone = 0;
        int nextCity = -1;
        for (int j = 0; j < CITY_COUNT; ++j) {
            if (!visited[j] && pheromones[currentCity][j] > maxPheromone) {
                maxPheromone = pheromones[currentCity][j];
                nextCity = j;
            }
        }
        route.push_back(nextCity);
        visited[nextCity] = true;
        currentCity = nextCity;
    }
    return route;
}

void updatePheromones(vector<vector<double>>& pheromones, const vector<int>& bestRoute, double bestDistance) {
    double pheromoneContribution = PHEROMONE_INTENSITY / bestDistance;
    for (int i = 0; i < CITY_COUNT - 1; ++i) {
        int city1 = bestRoute[i];
        int city2 = bestRoute[i + 1];
        pheromones[city1][city2] += pheromoneContribution;
        pheromones[city2][city1] += pheromoneContribution;
    }

    for (int i = 0; i < CITY_COUNT; ++i)
        for (int j = 0; j < CITY_COUNT; ++j)
            pheromones[i][j] *= (1 - PHEROMONE_EVAPORATION);
}

int main() {
    srand(time(0));

    // Step 1: Create cities with random coordinates
    vector<City> cities(CITY_COUNT);
    cout << "Cities and their coordinates:\n";
    for (int i = 0; i < CITY_COUNT; ++i) {
        cities[i] = City(rand() % 100, rand() % 100);
        cout << "City " << i << ": (" << cities[i].x << ", " << cities[i].y << ")\n";
    }
    
    // Step 2: Genetic Algorithm for Initial Route Optimization
    vector<vector<int>> population(POP_SIZE);
    for (int i = 0; i < POP_SIZE; ++i) {
        population[i] = createRoute();
    }

    for (int generation = 0; generation < GENERATIONS; ++generation) {
        sort(population.begin(), population.end(), [&](const vector<int>& a, const vector<int>& b) {
            return routeDistance(cities, a) < routeDistance(cities, b);
        });

        vector<vector<int>> newPopulation(POP_SIZE);
        for (int i = 0; i < POP_SIZE; ++i) {
            int parent1 = rand() % (POP_SIZE / 2);
            int parent2 = rand() % (POP_SIZE / 2);
            newPopulation[i] = crossover(population[parent1], population[parent2]);
            mutate(newPopulation[i]);
        }
        population = newPopulation;
    }

    // Select the best route from GA as the initial route for ACO
    vector<int> bestRoute = population[0];
    double bestDistance = routeDistance(cities, bestRoute);

    // Step 3: Ant Colony Optimization to refine the GA-optimized route
    vector<vector<double>> pheromones(CITY_COUNT, vector<double>(CITY_COUNT));
    initializePheromones(pheromones);

    for (int iter = 0; iter < GENERATIONS; ++iter) {
        vector<int> newRoute = antRoute(cities, pheromones);
        double newDistance = routeDistance(cities, newRoute);

        if (newDistance < bestDistance) {
            bestDistance = newDistance;
            bestRoute = newRoute;
        }

        updatePheromones(pheromones, bestRoute, bestDistance);
    }

    // Output the final best route and distance
    cout << "\nOptimized Route Distance: " << bestDistance << endl;
    cout << "Optimized Route: ";
    for (int city : bestRoute) cout << city << " ";
    cout << endl;

    return 0;
}
