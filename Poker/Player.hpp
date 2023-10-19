#pragma once

namespace PokerGame {
	enum class Actions {Call, Fold, Raise};

	struct Action {
		Action(Actions action, double bet) : action(action), bet(bet) {}

		Actions action;
		double bet;
	};

	class Player {
	public:
		bool InGame;

		Player() : Balance(0), CurrentBet(0), InGame(false) {
			Hand1 = nullptr;
			Hand2 = nullptr;
		}

		void SetHand(Card* hand1, Card* hand2) {
			Hand1 = hand1;
			Hand2 = hand2;
		}

		void ResetHand() {
			Hand1, Hand2 = nullptr;
		}

		Action Act() {
			return Action(Actions::Call, 0);
		}

		double GetBalance() {
			return Balance;
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

	private:
		double Balance;
		double CurrentBet;
		Card* Hand1;
		Card* Hand2;
	};
}