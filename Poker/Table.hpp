#pragma once
#include "Card.hpp"
#include "Player.hpp"
#include <array>
#include <iostream>


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

			for (int i = 0; i < MAX_PLAYERS; i++) {
				Players[i] = nullptr;
			}
		}

		void BetRound() {
			Player* currentPlayer;

			for (int i = DealearPlayerNumber;;) {
				if (Players[i] != nullptr) {
					continue;
				}

				currentPlayer = Players[i];

				Action act = currentPlayer->Act(); // put tablestate/gamestate data in Act()

				switch (act.action) {
				case Actions::Call:
					if (!currentPlayer->Pay(CurrentRaise)) {
						currentPlayer->InGame = false;
						currentPlayer->ResetHand();
					}

					currentPlayer->AddToCurrentBet(CurrentRaise);
					break;

				case Actions::Fold:
					currentPlayer->InGame = false;
					currentPlayer->ResetHand();
					break;

				case Actions::Raise:
					if (!currentPlayer->Pay(CurrentRaise)) {
						currentPlayer->InGame = false;
						currentPlayer->ResetHand();
					}

					currentPlayer->AddToCurrentBet(CurrentRaise);
					CurrentRaise += act.bet;
				}

				if (Players[NextPlayerIndex(i)]->GetCurrentBet() == currentPlayer->GetCurrentBet()) {
					return;
				}

				i = NextPlayerIndex(i);
			}

		}

		int NextPlayerIndex(double current) {
			if (current == MAX_PLAYERS - 1) {
				return 0;
			}

			return current + 1;
		}

		void Preflop() {
			int iCurrentPlayer = DealearPlayerNumber;
			int iPlayersToGo = MAX_PLAYERS;

			do {
				switch (iPlayersToGo) {
				case MAX_PLAYERS - 1:
					if (!Players[iCurrentPlayer]->Pay(SmallBlind)) {
						Players[iCurrentPlayer]->InGame = false;
						continue;
					}
					Pot += SmallBlind;
				case MAX_PLAYERS - 2:
					if (!Players[iCurrentPlayer]->Pay(BigBlind)) {
						Players[iCurrentPlayer]->InGame = false;
						continue;
					}
					Pot += BigBlind;
				}

				Players[iCurrentPlayer]->SetHand(Deck.DrawCard(), Deck.DrawCard());

				iPlayersToGo--;
				iCurrentPlayer = NextPlayerIndex(iCurrentPlayer);

			} while (iPlayersToGo != 0);

			// Deal two private cards to each player
			/*for (int fromDealerCounter = DealerPlayerIndex;;) {
				if (Players[i] == nullptr) {
					continue;
				}

				if (fromDealerCounter == 1) {
					if (!Players[i]->Pay(SmallBlind)) {
						Players[i]->InGame = false;
						continue;
					}
					Pot += SmallBlind;
				} else if (fromDealerCounter == 2) {
					if (!Players[i]->Pay(BigBlind)) {
						Players[i]->InGame = false;
						continue;
					}
					Pot += BigBlind;
				}

				Players[i]->SetHand(Deck.DrawCard(), Deck.DrawCard());

				fromDealerCounter++;
			}*/
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

		void LastRound() {}

		void KickPlayer(Player* player) {
			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (Players[i] == player) {
					Players[i] = nullptr;

					CurrentPlayersNumber--;
				}
			}
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

		// TODO: Delete
		void PrintDebugInfo() {
			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (Players[i] != nullptr) {
					std::cout << Players[i]->GetBalance() << '\n';
				}
			}
		}

	private:
		Deck52 Deck;
		int CurrentPlayersNumber;
		int DealearPlayerNumber;
		int DealerPlayerIndex;
		double SmallBlind;
		double BigBlind;
		double BuyIn;
		double Pot;
		double CurrentRaise;
		double CurrentBet;
		bool TradesStated;
		std::array<Card*, MAX_TABLE_CARDS> TableCards;
		std::array<Player*, MAX_PLAYERS> Players;
	};
}