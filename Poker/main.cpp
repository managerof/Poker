#include "PokerGame.hpp"
#include <thread>
#include <vector>

// Parallel task function
void SimulateTableRounds(PokerGame::Table& table, int numRounds) {
    for (int round = 0; round < numRounds; round++) {
        table.Play();
        table.Reset();  // Reset the table for the next round
    }
}

int main() {
    int numCores = std::thread::hardware_concurrency(); // Get the number of CPU cores
    int numRoundsPerTable = 1;  // Number of rounds to play for each table

    // Create a vector of tables, one for each CPU core
    std::vector<PokerGame::Table> tables(numCores, PokerGame::Table());

    // Add players to each table
    for (PokerGame::Table& table : tables) {
        for (int i = 0; i < PokerGame::MAX_PLAYERS; i++) {
            table.AddPlayer(new PokerGame::Player());
        }
    }

    // Create a thread pool with one thread for each table
    std::vector<std::thread> threads;
    for (PokerGame::Table& table : tables) {
        threads.push_back(std::thread(SimulateTableRounds, std::ref(table), numRoundsPerTable));
    }

    // Join threads to wait for all simulations to complete
    for (std::thread& thread : threads) {
        thread.join();
    }

    return 0;
}
