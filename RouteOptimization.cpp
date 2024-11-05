#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <unordered_map>
#include <fstream>

using namespace std;

const int POP_SIZE = 100;        // Population size for GA
const int GENERATIONS = 500;     // Number of generations for GA
const double MUTATION_RATE = 0.01;
const int NUM_ANTS = 50;         // Number of ants for ACO
const double PHEROMONE_INTENSITY = 100;
const double PHEROMONE_EVAPORATION = 0.5;
const double EARTH_RADIUS = 6371.0; // Earth's radius in kilometers

struct City {
    string name;
    double lat, lon;
    City(string name = "", double lat = 0, double lon = 0) : name(name), lat(lat), lon(lon) {}
};

// Haversine distance calculation
double haversineDistance(const City& a, const City& b) {
    double lat1 = a.lat * M_PI / 180.0;
    double lon1 = a.lon * M_PI / 180.0;
    double lat2 = b.lat * M_PI / 180.0;
    double lon2 = b.lon * M_PI / 180.0;

    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;

    double aH = pow(sin(dlat / 2), 2) +
                 cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2);
    double c = 2 * atan2(sqrt(aH), sqrt(1 - aH));

    return EARTH_RADIUS * c; // Distance in kilometers
}

// Genetic Algorithm Functions
double routeDistance(const vector<City>& cities, const vector<int>& route) {
    double totalDistance = 0;
    for (size_t i = 0; i < route.size() - 1; ++i) {
        totalDistance += haversineDistance(cities[route[i]], cities[route[i + 1]]);
    }
    totalDistance += haversineDistance(cities[route.back()], cities[route[0]]);
    return totalDistance;
}

vector<int> createRoute(int numCities) {
    vector<int> route(numCities);
    for (int i = 0; i < numCities; ++i) route[i] = i;
    random_shuffle(route.begin(), route.end());
    return route;
}

void mutate(vector<int>& route) {
    if ((rand() / double(RAND_MAX)) < MUTATION_RATE) {
        int i = rand() % route.size();
        int j = rand() % route.size();
        swap(route[i], route[j]);
    }
}

vector<int> crossover(const vector<int>& parent1, const vector<int>& parent2) {
    vector<int> child(parent1.size(), -1);
    int start = rand() % parent1.size();
    int end = start + rand() % (parent1.size() - start);

    for (int i = start; i <= end; ++i) {
        child[i] = parent1[i];
    }

    int j = 0;
    for (int i = 0; i < parent2.size(); ++i) {
        if (find(child.begin(), child.end(), parent2[i]) == child.end()) {
            while (child[j] != -1) j++;
            child[j] = parent2[i];
        }
    }
    return child;
}

// Ant Colony Optimization Functions
void initializePheromones(vector<vector<double>>& pheromones, int numCities) {
    for (int i = 0; i < numCities; ++i)
        for (int j = 0; j < numCities; ++j)
            pheromones[i][j] = 1.0;
}

vector<int> antRoute(const vector<City>& cities, const vector<vector<double>>& pheromones) {
    vector<int> route;
    vector<bool> visited(cities.size(), false);
    int currentCity = rand() % cities.size();
    route.push_back(currentCity);
    visited[currentCity] = true;

    for (int i = 1; i < cities.size(); ++i) {
        double maxPheromone = 0;
        int nextCity = -1;
        for (int j = 0; j < cities.size(); ++j) {
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
    for (int i = 0; i < bestRoute.size() - 1; ++i) {
        int city1 = bestRoute[i];
        int city2 = bestRoute[i + 1];
        pheromones[city1][city2] += pheromoneContribution;
        pheromones[city2][city1] += pheromoneContribution;
    }

    for (int i = 0; i < pheromones.size(); ++i)
        for (int j = 0; j < pheromones.size(); ++j)
            pheromones[i][j] *= (1 - PHEROMONE_EVAPORATION);
}

int main() {
    srand(time(0));

    // Input file reading
    ifstream inputFile("cities.txt");
    if (!inputFile) {
        cerr << "Error opening input file!" << endl;
        return 1;
    }

    unordered_map<string, City> cityDatabase;
    string cityName;
    double lat, lon;

    // Read cities from the file
    while (inputFile >> cityName >> lat >> lon) {
        cityDatabase[cityName] = City(cityName, lat, lon);
    }
    inputFile.close();

    vector<City> cities;
    int numCities;
    cout << "Enter the number of cities you want to include in the route (max " << cityDatabase.size() << "): ";
    cin >> numCities;

    cout << "Available cities:\n";
    for (const auto& pair : cityDatabase) {
        cout << "- " << pair.first << endl;
    }

    cout << "Enter the names of the cities you want to include in the route:\n";
    for (int i = 0; i < numCities; ++i) {
        string cityName;
        cout << "City " << (i + 1) << ": ";
        cin >> cityName;
        cities.push_back(cityDatabase[cityName]);
    }

    // Step 2: Genetic Algorithm for Initial Route Optimization
    vector<vector<int>> population(POP_SIZE);
    for (int i = 0; i < POP_SIZE; ++i) {
        population[i] = createRoute(numCities);
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
    vector<vector<double>> pheromones(numCities, vector<double>(numCities));
    initializePheromones(pheromones, numCities);

    for (int iter = 0; iter < GENERATIONS; ++iter) {
        vector<int> newRoute = antRoute(cities, pheromones);
        double newDistance = routeDistance(cities, newRoute);

        if (newDistance < bestDistance) {
            bestDistance = newDistance;
            bestRoute = newRoute;
        }

        updatePheromones(pheromones, bestRoute, bestDistance);
    }

    // Output the final best route and distance to a file
    ofstream outputFile("optimized_route.txt");
    if (!outputFile) {
        cerr << "Error opening output file!" << endl;
        return 1;
    }

    // Output the final best route and distance
    cout << "Optimized Route Distance: " << bestDistance << " km" << endl;
    cout << "Optimized Route: ";
    for (int cityIdx : bestRoute) {
        cout << cities[cityIdx].name << " "; // Ensure it outputs the full route
    }
    cout << endl;

    // Write to the output file
    outputFile << "Optimized Route Distance: " << bestDistance << " km" << endl;
    outputFile << "Optimized Route: ";
    for (int cityIdx : bestRoute) {
        outputFile << cities[cityIdx].name << " "; // Ensure it outputs the full route
    }
    outputFile << endl;
    outputFile.close();

    return 0;
}
