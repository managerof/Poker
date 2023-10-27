#include "PokerGame.hpp"


int main() {
	PokerGame::Table table;
	PokerGame::Player player1;
	PokerGame::Player player2;
	PokerGame::Player player3;
	PokerGame::Player player4;
	PokerGame::Player player5;
	PokerGame::Player player6;

	table.AddPlayer(&player1);
	table.AddPlayer(&player2);
	table.AddPlayer(&player3);
	table.AddPlayer(&player4);
	table.AddPlayer(&player5);
	table.AddPlayer(&player6);

	table.PrintDebugInfo(); // DEBUG

	table.Play();

	table.PrintDebugInfo(); // DEBUG

	return 0;
}