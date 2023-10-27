#pragma once
#include "Card.hpp"
#include "Player.hpp"
#include <array>
#include <iostream>
#include <map>
#include <set>

namespace PokerGame {
	const int MAX_TABLE_CARDS = 5;
	const int MAX_HAND_CARDS = 2;
	const int MAX_PLAYERS = 10;

	enum class Combination { HighCard, Pair, TwoPair, ThreeOfAKind, Straight, Flush, FullHouse, FourOfAKind, StraightFlush };

	struct PokerComb {
		PokerComb(Combination combination, Rank higherrank) : comb(combination), higherRank(higherrank) {}

		Combination comb;
		Rank higherRank;
	};

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

		struct CombBool {
			CombBool(bool isCombinationFound, Rank* combinationCardRank) : isFound(isCombinationFound), rank(combinationCardRank) {}

			CombBool(bool isCombinationFound, const Rank* combinationCardRank) : isFound(isCombinationFound), rank(const_cast<Rank*>(combinationCardRank)) {}

			CombBool(bool isCombinationFound, std::nullptr_t) : isFound(isCombinationFound), rank(nullptr) {}

			bool operator<(const CombBool& other) const {
				return *rank < *(other.rank);
			}

			bool isFound;
			Rank* rank;
		};


		CombBool HasPair(const std::array<Card*, MAX_HAND_CARDS + MAX_TABLE_CARDS>& cards) {
			std::map<Rank, int> rankCount;

			for (const Card* card : cards) {
				Rank rank = card->getRank();
				rankCount[rank]++;
			}

			// Check for two pair
			bool firstPairFound = false;
			for (const auto& pair : rankCount) {
				if (pair.second == 2) {
					if (firstPairFound) {
						CombBool result(true, &pair.first);
						return result;
					}
					else {
						firstPairFound = true;
					}
				}
			}

			// Check for a pair
			for (const auto& pair : rankCount) {
				if (pair.second == 2) {
					return CombBool(true, &pair.first);
				}
			}

			// Determine high card
			Rank currRank = cards[0]->getRank();
			for (int i = 1; i < MAX_HAND_CARDS + MAX_TABLE_CARDS; i++) {
				if (cards[i]->getRank() > currRank) {
					currRank = cards[i]->getRank();
				}
			}

			return CombBool(false, &currRank); // default, high card
		}

		// Function to check for two pairs
		CombBool HasTwoPairs(const std::array<Card*, MAX_HAND_CARDS + MAX_TABLE_CARDS>& cards) {
			std::map<Rank, int> rankCount;

			for (const Card* card : cards) {
				Rank rank = card->getRank();
				rankCount[rank]++;
			}

			Rank firstPairRank = Rank::Two; // Initialize with a valid rank (lowest rank)
			Rank secondPairRank = Rank::Two; // Initialize with a valid rank (lowest rank)

			for (const auto& pair : rankCount) {
				if (pair.second == 2) {
					if (firstPairRank == Rank::Two) {
						firstPairRank = pair.first;
					}
					else if (secondPairRank == Rank::Two) {
						secondPairRank = pair.first;
					}
				}
			}

			if (firstPairRank != Rank::Two && secondPairRank != Rank::Two) {
				return CombBool(true, firstPairRank > secondPairRank ? &firstPairRank : &secondPairRank); // Two pairs found
			}

			return CombBool(false, nullptr); // No two pairs found
		}


		CombBool HasThreeOfAKind(const std::array<Card*, MAX_HAND_CARDS + MAX_TABLE_CARDS>& cards) {
			std::map<Rank, int> rankCount;

			for (const Card* card : cards) {
				Rank rank = card->getRank();
				rankCount[rank]++;
			}

			for (const auto& pair : rankCount) {
				if (pair.second == 3) {
					return CombBool(true, &pair.first); // Three of a kind found
				}
			}

			return CombBool(false, nullptr); // No three of a kind found
		}
		
		// Function to check for a straight
		CombBool HasStraight(const std::array<Card*, MAX_HAND_CARDS + MAX_TABLE_CARDS>& cards) {
			// Sort the cards by rank in ascending order
			std::array<Card*, MAX_HAND_CARDS + MAX_TABLE_CARDS> sortedCards = cards;
			std::sort(sortedCards.begin(), sortedCards.end(), [](Card* a, Card* b) {
				return a->getRank() < b->getRank();
				});

			// Check for a straight
			int consecutiveRanks = 1;
			Rank currentRank = sortedCards[0]->getRank();
			Rank straightRank = currentRank;

			for (int i = 1; i < MAX_HAND_CARDS + MAX_TABLE_CARDS; i++) {
				if (sortedCards[i]->getRank() == NextRank(currentRank)) {
					consecutiveRanks++;
					currentRank = sortedCards[i]->getRank();
				}
				else if (sortedCards[i]->getRank() != currentRank) {
					consecutiveRanks = 1;
					currentRank = sortedCards[i]->getRank();
					straightRank = currentRank;
				}
			}

			if (consecutiveRanks >= 5) {
				return CombBool(true, &straightRank); // Straight found
			}

			return CombBool(false, nullptr); // No straight found
		}

		// Function to check for a flush
		CombBool HasFlush(const std::array<Card*, MAX_HAND_CARDS + MAX_TABLE_CARDS>& cards) {
			std::map<Suit, int> suitCount;

			for (const Card* card : cards) {
				Suit suit = card->getSuit();
				suitCount[suit]++;
			}

			for (const auto& pair : suitCount) {
				if (pair.second >= 5) {
					return CombBool(true, nullptr); // Flush found
				}
			}

			return CombBool(false, nullptr); // No flush found
		}

		// Function to check for four of a kind
		CombBool HasFourOfAKind(const std::array<Card*, MAX_HAND_CARDS + MAX_TABLE_CARDS>& cards) {
			std::map<Rank, int> rankCount;

			for (const Card* card : cards) {
				Rank rank = card->getRank();
				rankCount[rank]++;
			}

			for (const auto& pair : rankCount) {
				if (pair.second == 4) {
					return CombBool(true, &pair.first); // Four of a kind found
				}
			}

			return CombBool(false, nullptr); // No four of a kind found
		}

		// Function to check for a full house
		CombBool HasFullHouse(const std::array<Card*, MAX_HAND_CARDS + MAX_TABLE_CARDS>& cards) {
			std::map<Rank, int> rankCount;

			for (const Card* card : cards) {
				Rank rank = card->getRank();
				rankCount[rank]++;
			}

			Rank threeOfAKindRank = Rank::Two; // Initialize with a valid rank (lowest rank)
			Rank pairRank = Rank::Two; // Initialize with a valid rank (lowest rank)

			for (const auto& pair : rankCount) {
				if (pair.second == 3) {
					threeOfAKindRank = pair.first;
				}
				else if (pair.second == 2) {
					pairRank = pair.first;
				}
			}

			if (threeOfAKindRank != Rank::Two && pairRank != Rank::Two) {
				return CombBool(true, &threeOfAKindRank); // Full house found
			}

			return CombBool(false, nullptr); // No full house found
		}

		PokerComb IdentifyPokerHand(Player* player, const std::array<Card*, MAX_TABLE_CARDS>& tableCards) {
			std::array<Card*, MAX_HAND_CARDS + MAX_TABLE_CARDS> allCards = { player->GetHand().First, player->GetHand().Second, tableCards[0], tableCards[1], tableCards[2], tableCards[3], tableCards[4] };

			// Check for flush
			CombBool flushResult = HasFlush(allCards);
			if (flushResult.isFound) {
				// Check for a straight within the flush
				CombBool straightResult = HasStraight(allCards);
				if (straightResult.isFound) {
					return PokerComb(Combination::StraightFlush, *straightResult.rank);
				}
				return PokerComb(Combination::Flush, *flushResult.rank);
			}

			// Check for a straight
			CombBool straightResult = HasStraight(allCards);
			if (straightResult.isFound) {
				return PokerComb(Combination::Straight, *straightResult.rank);
			}

			// Check for four of a kind
			CombBool fourOfAKindResult = HasFourOfAKind(allCards);
			if (fourOfAKindResult.isFound) {
				return PokerComb(Combination::FourOfAKind, *fourOfAKindResult.rank);
			}

			// Check for a full house
			CombBool fullHouseResult = HasFullHouse(allCards);
			if (fullHouseResult.isFound) {
				return PokerComb(Combination::FullHouse, *fullHouseResult.rank);
			}

			// Check for three of a kind
			CombBool threeOfAKindResult = HasThreeOfAKind(allCards);
			if (threeOfAKindResult.isFound) {
				return PokerComb(Combination::ThreeOfAKind, *threeOfAKindResult.rank);
			}

			// Check for a pair
			CombBool twoPairResult = HasTwoPairs(allCards);
			if (twoPairResult.isFound) {
				return PokerComb(Combination::Pair, *twoPairResult.rank);
			}

			// Check for a pair
			CombBool pairResult = HasPair(allCards);
			if (pairResult.isFound) {
				return PokerComb(Combination::Pair, *pairResult.rank);
			}

			// If none of the above combinations are found, it's a high card
			return PokerComb(Combination::HighCard, allCards[0]->getRank() > allCards[1]->getRank() ? allCards[0]->getRank() : allCards[1]->getRank());
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

			} while (playersPassed < CurrentPlayersNumber);

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
			double lastPlayerBet = -1; // Initialize with an impossible value
			int iPlayer = 0;

			for (Player* player : Players) {
				if (player != nullptr) {
					if (lastPlayerBet == -1) {
						lastPlayerBet = player->GetCurrentBet();
					}
					else if (player->GetCurrentBet() != lastPlayerBet) {
						return false;
					}
				}
				iPlayer = NextPlayerIndex(iPlayer);
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

		void LastRound() {
			std::vector<Player*> winners;
			PokerComb bestPokerComb = PokerComb(Combination::HighCard, Rank::Two);

			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (Players[i] != nullptr && Players[i]->InGame) {
					PokerComb playerPokerComb = IdentifyPokerHand(Players[i], TableCards);

					if (playerPokerComb.comb > bestPokerComb.comb || (playerPokerComb.comb == bestPokerComb.comb && playerPokerComb.higherRank > bestPokerComb.higherRank)) {
						winners.clear();
						bestPokerComb = playerPokerComb;
					}

					if (playerPokerComb.comb == bestPokerComb.comb && playerPokerComb.higherRank == bestPokerComb.higherRank) {
						winners.push_back(Players[i]);
					}
				}
			}

			// Distribute the pot to the winners
			double potPerWinner = Pot / static_cast<double>(winners.size());

			for (Player* winner : winners) {
				winner->AddToBalance(potPerWinner);
			}

			// Reset the pot and prepare for the next hand
			Pot = 0;
		}

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

			return -1; // no space for a new player
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