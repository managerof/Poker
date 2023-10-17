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
			Pot = 0;
			SmallBlind = 10;
			BigBlind = 20;
			BuyIn = BigBlind * 70;
			DealearPlayerNumber = 0;

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

		void BetRound() {
			Player* currentPlayer;

			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (Players[i] != nullptr) {
					currentPlayer = Players[i];
				}

				Action act = currentPlayer->Act(); // put table / gamestate data in Act()

				switch (act.action) {
				case Actions::Call: 
					///
				case Actions::Fold:
					///
				case Actions::Raise:
					///
				}
			}
		}

		void LastRound() {}

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

		void Play() {
			Preflop();
			BetRound();
			Flop();
			BetRound();
			Turn();
			BetRound();
			River();
			LastRound();

			if (DealearPlayerNumber == MAX_PLAYERS - 1) {
				DealearPlayerNumber = 0;
			}
			else {
				DealearPlayerNumber++;
			}
		}

	private:
		Deck52 Deck;
		int CurrentPlayersNumber;
		int DealearPlayerNumber;
		double SmallBlind;
		double BigBlind;
		double BuyIn;
		double Pot;
		std::array<Card*, MAX_TABLE_CARDS> TableCards;
		std::array<Player*, MAX_PLAYERS> Players;
	};
}