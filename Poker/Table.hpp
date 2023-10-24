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
			DealerPlayerIndex = 0;
			CurrentRaise = 0;
			CurrentBet = 0;

			for (int i = 0; i < MAX_TABLE_CARDS; i++) {
				TableCards[i] = nullptr;
			}

			for (int i = 0; i < MAX_PLAYERS; i++) {
				Players[i] = nullptr;
			}
		}

		void HandlePlayerAction(Player* player) {
			Action playerAction = player->Act();

			switch (playerAction.action) {
			case Actions::Call:
				if (!player->Pay(CurrentRaise)) {
					player->InGame = false;
					player->ResetHand();
					std::cout << " CALL PLAYER KICKED!" << '\n';
					break;
				}

				player->AddToCurrentBet(CurrentRaise);
				Pot += CurrentRaise;
				break;

			case Actions::Fold:
				player->InGame = false;
				player->ResetHand();
				std::cout << " FOLD PLAYER KICKED!" << '\n';
				break;

			case Actions::Raise:
				if (!player->Pay(CurrentRaise + playerAction.bet)) {
					player->InGame = false;
					player->ResetHand();
					std::cout << " RAISE PLAYER KICKED!" << '\n';
					break;
				}

				player->AddToCurrentBet(CurrentRaise + playerAction.bet);
				CurrentRaise += playerAction.bet;
				Pot += CurrentRaise;
			}
		}

		void BetRound() {
			Player* currPlayer;
			int i = DealerPlayerIndex;
			int playersPassed = 0;

			do {
				currPlayer = Players[i];

				if (currPlayer == nullptr) {
					i = NextPlayerIndex(i);
					continue;
				}

				HandlePlayerAction(currPlayer);
				playersPassed++;

				i = NextPlayerIndex(i);

			} while (CurrentBetsEqual() && playersPassed >= 9);

			ResetCurrentBets();
		}

		Player* NextPlayer(int current) {
			int PlayersToGo = MAX_PLAYERS;
			int i = NextPlayerIndex(current);

			do {
				if (Players[i] != nullptr) {
					return Players[i];
				}

				i = NextPlayerIndex(i);
				PlayersToGo--;
			} while (PlayersToGo != 0);
		}

		int NextPlayerIndex(int current) {
			if (current == MAX_PLAYERS - 1) {
				return 0;
			}

			return current + 1;
		}

		bool CurrentBetsEqual() {
			std::cout << "CurrentBetsEqual() iter; Dealer index: " << DealerPlayerIndex << " Current Raise: " << CurrentRaise << '\n';
			double lastPlayerBet;
			int iPlayer = 0;

			for (Player* player : Players) {
				if (player == nullptr) {
					iPlayer = NextPlayerIndex(iPlayer);
					continue;
				}

				lastPlayerBet = player->GetCurrentBet();
				iPlayer = NextPlayerIndex(iPlayer);
				break;
			}

			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (Players[iPlayer] == nullptr) {
					iPlayer = NextPlayerIndex(iPlayer);
					continue;
				}

				if (Players[iPlayer]->GetCurrentBet() != lastPlayerBet) {
					return false;
				} else {
					iPlayer = NextPlayerIndex(iPlayer);
				}
			}

			return true;
		}

		void ResetCurrentBets() {
			for (Player* player : Players) {
				if (player != nullptr) {
					player->SetCurrentBet(0);
				}
			}
		}

		void Preflop() {
			int iCurrentPlayer = DealerPlayerIndex;
			int iPlayersToGo = CurrentPlayersNumber;

			do {
				if (Players[iCurrentPlayer] == nullptr) {
					iCurrentPlayer = NextPlayerIndex(iCurrentPlayer);
					continue;
				}

				if (NextPlayerIndex(DealerPlayerIndex) == iCurrentPlayer) {
					if (!Players[iCurrentPlayer]->Pay(SmallBlind)) {
						Players[iCurrentPlayer]->InGame = false;
						continue;
					}
					Pot += SmallBlind;
				}

				if (NextPlayerIndex(NextPlayerIndex(DealerPlayerIndex)) == iCurrentPlayer) {
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
			
			ResetCurrentBets();
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

		int FreePlayerSlotIndex() {
			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (Players[i] == nullptr) {
					return i;
				}
			}
		}

		void AddPlayer(Player* player) {
			if (CurrentPlayersNumber < MAX_PLAYERS) {
				Players[FreePlayerSlotIndex()] = player;

				CurrentPlayersNumber++;
				return;
			}

			throw std::runtime_error("No space for a new player!");
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

			DealerPlayerIndex = NextPlayerIndex(DealerPlayerIndex);
		}

		// TODO: Delete
		void PrintDebugInfo() {
			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (Players[i] != nullptr) {
					std::cout << "Player " << i << " " << "Balance " << Players[i]->GetBalance() << '\n';
				}
			}

			std::cout << "Pot: " << Pot << '\n';
		}

	private:
		Deck52 Deck;
		int CurrentPlayersNumber;
		int DealerPlayerIndex;
		double SmallBlind;
		double BigBlind;
		double BuyIn;
		double Pot;
		double CurrentRaise;
		double CurrentBet;
		std::array<Card*, MAX_TABLE_CARDS> TableCards;
		std::array<Player*, MAX_PLAYERS> Players;
	};
}