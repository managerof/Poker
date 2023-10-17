#pragma once
#include "Card.hpp"
#include "Player.hpp"
#include <array>

namespace PokerGame {
	const int MAX_TABLE_CARDS = 5;
	const int MAX_PLAYERS = 10;

	class Table {
	public:
		Table() {
			CurrentPlayersNumber = 0;

			for (int i = 0; i < MAX_TABLE_CARDS; i++) {
				TableCards[i] = nullptr;
			}
		}

		void Preflop() {
			// Deal two private cards to each player
			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (Players[i] != nullptr) {
					Players[i]->SetHand(Deck.DrawCard(), Deck.DrawCard());
				}
			}
		}

		void Flop() {
			// Deal the first three community cards (the flop)
			TableCards[0] = Deck.DrawCard();
			TableCards[1] = Deck.DrawCard();
			TableCards[2] = Deck.DrawCard();
		}

		void Turn() {
			// Deal the fourth community card (the turn)
			TableCards[3] = Deck.DrawCard();
		}

		void River() {
			// Deal the fifth community card (the river)
			TableCards[4] = Deck.DrawCard();
		}

		void Trades() {

		}

		void AddPlayer(Player* player) {
			if (CurrentPlayersNumber < MAX_PLAYERS) {
				Players[CurrentPlayersNumber] = player;

				CurrentPlayersNumber++;
			}
		}

		void Reset() {
			for (int i = 0; i < MAX_TABLE_CARDS; i++) {
				TableCards[i] = nullptr;
			}

			Deck.Reset();

			for (int i = 0; i < MAX_PLAYERS; i++) {
				Players[i]->ResetHand();
			}
		}

	private:
		Deck52 Deck;
		std::array<Card*, MAX_TABLE_CARDS> TableCards;
		std::array<Player*, MAX_PLAYERS> Players;
		int CurrentPlayersNumber;
	};
}