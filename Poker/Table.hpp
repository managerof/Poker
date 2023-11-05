#pragma once
#include "Card.hpp"
#include "Player.hpp"
#include <array>
#include <iostream>
#include <map>
#include <set>

namespace PokerGame {
	enum class Combination { HighCard, Pair, TwoPair, ThreeOfAKind, Straight, Flush, FullHouse, FourOfAKind, StraightFlush };

	struct PokerComb {
		PokerComb(Combination combination, Rank higherrank) : comb(combination), higherRank(higherrank) {}

		Combination comb;
		Rank higherRank;
	};

	class Table {
	public:
		int HandsPlayed = 0;

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

			// Sort the cards by rank in ascending order
			std::array<Card*, MAX_HAND_CARDS + MAX_TABLE_CARDS> sortedCards = cards;
			std::sort(sortedCards.begin(), sortedCards.end(), [](Card* a, Card* b) {
				return a->getRank() < b->getRank();
			});

			for (const Card* card : cards) {
				Suit suit = card->getSuit();
				suitCount[suit]++;
			}

			for (const auto& pair : suitCount) {
				if (pair.second >= 5) {
					return CombBool(true, cards[MAX_HAND_CARDS + MAX_TABLE_CARDS - 1]->getRankPointer()); // Flush found
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
					CurrentPlayersNumber--;
					break;
				}

				player->AddToCurrentBet(CurrentRaise);
				Pot += CurrentRaise;
				break;

			case Actions::Fold:
				player->InGame = false;
				player->ResetHand();
				CurrentPlayersNumber--;
				break;

			case Actions::Raise:
				if (!player->Pay(CurrentRaise + playerAction.bet)) {
					player->InGame = false;
					player->ResetHand();
					CurrentPlayersNumber--;
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

				if (!currPlayer->InGame) {
					playersPassed++;
					continue;
				}

				HandlePlayerAction(currPlayer);
				playersPassed++;

				i = NextPlayerIndex(i);

			} while (playersPassed < CurrentPlayersNumber && !CurrentBetsEqual());

			ResetCurrentBets();
		}

		/*Player* NextPlayer(int current) {
			int PlayersToGo = MAX_PLAYERS;
			int i = NextPlayerIndex(current);

			do {
				if (Players[i] != nullptr) {
					return Players[i];
				}

				i = NextPlayerIndex(i);
				PlayersToGo--;
			} while (PlayersToGo != 0);
		}*/

		int NextPlayerIndex(int current) {
			if (current == MAX_PLAYERS - 1) {
				return 0;
			}

			return current + 1;
		}

		int NextPlayerIndexWithoutNULLPTRs(int current) {

			int iter = NextPlayerIndex(current);

			while (Players[iter] == nullptr) {
				iter = NextPlayerIndex(iter);
			}

			return iter;
		}
		
		bool CurrentBetsEqual() {
			double lastPlayerBet = -1;
			int iPlayer = 0;
			int PlayersToGo = MAX_PLAYERS;

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

		void PlayerPayBet(int playerIndex, double amount) {
			if (!Players[playerIndex]->Pay(SmallBlind)) {
				Players[playerIndex]->InGame = false;
				return;
			}
			Pot += SmallBlind;
		}
		
		bool CanAllPlayerPay(double amount) {
			for (Player* player : Players) {
				if (player->GetBalance() < amount) {
					return false;
				}
			}

			return true;
		}

		/*void PreFlop() {
			int SmallBlindplayerIndex = NextPlayerIndexWithoutNULLPTRs(DealerPlayerIndex);
			int PlayersToGo = CurrentPlayersNumber;

			bool smallBlindPaid = false;

			do {

				if (Players[SmallBlindplayerIndex]->Pay(SmallBlind) == false) {
					Players[SmallBlindplayerIndex]->InGame = false;
					SmallBlindplayerIndex = NextPlayerIndexWithoutNULLPTRs(SmallBlindplayerIndex);
					PlayersToGo++;
				}
				else {
					smallBlindPaid = true;
				}


			} while(!smallBlindPaid and (PlayersToGo < 0 !CanAllPlayerPay(SmallBlind)));


			for (Player* player : Players) {
				if (player->InGame) {
					player->SetHand(Deck.DrawCard(), Deck.DrawCard());
				}
			}
		}*/

		void Preflop() {
			int iCurrentPlayer = DealerPlayerIndex;

			// Determine the small blind and big blind positions
			int smallBlindPosition = NextPlayerIndexWithoutNULLPTRs(DealerPlayerIndex);
			int bigBlindPosition = NextPlayerIndexWithoutNULLPTRs(smallBlindPosition);

			// Pay small blind
			PaySmallBlind(smallBlindPosition);

			// Pay big blind
			PayBigBlind(bigBlindPosition);

			// Deal cards to players
			DealCardsToPlayers();

			// Reset current bets
			ResetCurrentBets();
		}

		void PaySmallBlind(int playerIndex) {
			int originalIndex = playerIndex;

			do {
				if (Players[playerIndex] != nullptr) {
					if (Players[playerIndex]->Pay(SmallBlind)) {
						// Player can pay the small blind
						Pot += SmallBlind;
						return;
					}
					else {
						// Player can't pay the small blind
						Players[playerIndex]->InGame = false;
					}
				}

				playerIndex = NextPlayerIndexWithoutNULLPTRs(playerIndex);
			} while (playerIndex != originalIndex);

			// If no player can pay the small blind, you may want to take specific action or handle this situation accordingly.
			// For example, you can end the game or take appropriate measures.
		}

		void PayBigBlind(int playerIndex) {
			int originalIndex = playerIndex;

			do {
				if (Players[playerIndex] != nullptr) {
					if (Players[playerIndex]->Pay(BigBlind)) {
						// Player can pay the big blind
						Pot += BigBlind;
						return;
					}
					else {
						// Player can't pay the big blind
						Players[playerIndex]->InGame = false;
					}
				}

				playerIndex = NextPlayerIndexWithoutNULLPTRs(playerIndex);
			} while (playerIndex != originalIndex);

			// If no player can pay the big blind, you may want to take specific action or handle this situation accordingly.
			// For example, you can end the game or take appropriate measures.
		}

		void DealCardsToPlayers() {
			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (Players[i] != nullptr) {
					Players[i]->SetHand(Deck.DrawCard(), Deck.DrawCard());
				}
			}
		}

		//void Preflop() {
		//	int iCurrentPlayer = DealerPlayerIndex;
		//	int iPlayersToGo = CurrentPlayersNumber;

		//	do {
		//		if (Players[iCurrentPlayer] == nullptr) {
		//			iCurrentPlayer = NextPlayerIndex(iCurrentPlayer);
		//			continue;
		//		}
		//			
		//		// small bind
		//		if (iCurrentPlayer == NextPlayerIndexWithoutNULLPTRs(DealerPlayerIndex)) {
		//			if (!Players[iCurrentPlayer]->Pay(SmallBlind)) {
		//				Players[iCurrentPlayer]->InGame = false;
		//				iPlayersToGo--;
		//				continue;
		//			}
		//			Pot += SmallBlind;
		//		}

		//		// big bind
		//		if (iCurrentPlayer == NextPlayerIndexWithoutNULLPTRs(NextPlayerIndexWithoutNULLPTRs(DealerPlayerIndex))) {
		//			if (!Players[iCurrentPlayer]->Pay(BigBlind)) {
		//				Players[iCurrentPlayer]->InGame = false;
		//				iPlayersToGo--;
		//				continue;
		//			}
		//			Pot += BigBlind;
		//		}

		//		Players[iCurrentPlayer]->SetHand(Deck.DrawCard(), Deck.DrawCard());

		//		iPlayersToGo--;
		//		iCurrentPlayer = NextPlayerIndex(iCurrentPlayer);

		//	} while (iPlayersToGo != 0);
		//	
		//	ResetCurrentBets();
		//}

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

		void IdentifyWinners() {
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
					player->InGame = false;
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
				player->InGame = true;

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

		void Check(std::string label) {
			for (Player* player : Players) {
				if (player == nullptr) {
					continue;
				}

				if (!player->InGame) {
					continue;
				}

				Hand hand = player->GetHand();

				if (hand.First == nullptr or hand.Second == nullptr) {
					std::cout << label << " " << "HandsPlayed: " << HandsPlayed << '\n';
					throw std::runtime_error("Card is nullptr!");
				}
			}
		}

		void Play() {
			ReCountPlayers();

			if (CurrentPlayersNumber < 2) {
				return;
			}

			for (Player* player : Players) {
				if (player != nullptr) {
					player->InGame = true;
				}
			}

			Preflop();

			Check("After Preflop");

			BetRound();

			Check("After BetRound");

			Flop();

			Check("After Flop");

			BetRound();

			Check("After BetRound");

			Turn();

			Check("After Turn");
			
			BetRound();

			Check("After BetRound");

			River();

			Check("After River");

			BetRound();

			Check("After BetRound");

			IdentifyWinners();

			ReCountPlayers();

			Deck.Reset();

			DealerPlayerIndex = NextPlayerIndex(DealerPlayerIndex);

			HandsPlayed++;
		}

		void ReCountPlayers() {
			CurrentPlayersNumber = 0;

			for (Player* player : Players) {
				if (player != nullptr) {
					CurrentPlayersNumber++;
				}
			}
		}

		// TODO: Delete
		void PrintDebugInfo() {
			for (int i = 0; i < MAX_PLAYERS; i++) {
				if (Players[i] != nullptr) {
					std::cout << "Player " << i+1 << " " << "Balance " << Players[i]->GetBalance() << '\n';
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