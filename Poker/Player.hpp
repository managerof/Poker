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
		Player() {
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

		}

	private:
		double Balance;
		Card* Hand1;
		Card* Hand2;
	};
}