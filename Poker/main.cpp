#include <iostream>
#include "PokerGame.hpp"

int main() {
	PokerGame::Table table;
	
	for (int i = 0; i < 5; i++) {
		PokerGame::Player* player = new PokerGame::Player();

		table.AddPlayer(player);
	}

	table.PrintDebugInfo();

	table.Play();

	table.PrintDebugInfo();

	table.Play();

	table.PrintDebugInfo();

	return 0;
}