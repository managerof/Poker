#pragma once

namespace PokerGame {
	enum class Actions {Call, Fold, Raise};

	struct Action {
		Action(Actions action, double bet) : action(action), bet(bet) {}

		Actions action;
		double bet;
	};

	struct Hand {
		Hand(Card* a, Card* b) : First(a), Second(b) {};

		void Reset() {
			First = nullptr;
			Second = nullptr;
		}

		Card* First;
		Card* Second;
	};

	class Player {
	public:
		bool InGame;

		Player() : Balance(1000), CurrentBet(0), InGame(false), Hand(nullptr, nullptr) {
		}

		void SetHand(Card* hand1, Card* hand2) {
			this->Hand.First = hand1;
		}

		void ResetHand() {
			this->Hand.Reset();
		}

		Action Act() {
			// TODO: neural network action prediction
			return Action(Actions::Call, 0);
		}

		double GetBalance() {
			return Balance;
		}

		void AddToBalance(double amount) {
			Balance += amount;
		}
		
		bool Pay(double ammount) {
			if (ammount > Balance) {
				return false;
			}

			Balance -= ammount;
			return true;
		}

		void SetCurrentBet(double bet) {
			CurrentBet = bet;
		}

		void AddToCurrentBet(double bet) {
			CurrentBet += bet;
		}

		double GetCurrentBet() {
			return CurrentBet;
		}

		Hand GetHand() {
			return this->Hand;
		}

	private:
		double Balance;
		double CurrentBet;
		Hand Hand;
	};
}