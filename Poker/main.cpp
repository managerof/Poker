#include "PokerGame.hpp"

int main() {
	PokerGame::Table table;
	PokerGame::Player player;

	table.AddPlayer(&player);

	table.PrintDebugInfo();

	table.KickPlayer(&player);

	table.PrintDebugInfo();

	return 0;
}