#pragma once

namespace PokerGame {
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

	private:
		double Balance;
		Card* Hand1;
		Card* Hand2;
	};
}