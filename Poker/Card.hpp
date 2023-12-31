#pragma once
#include <array>
#include <random>
#include <algorithm>

namespace PokerGame {
    const int MAX_TABLE_CARDS = 5;
    const int MAX_HAND_CARDS = 2;
    const int MAX_PLAYERS = 10;

    const int DECK_52_SIZE = 52;
    const int DECK_36_SIZE = 36;

    enum class Rank { Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Jack, Queen, King, Ace };
    enum class Suit { Hearts, Diamonds, Clubs, Spades };

    class Card {
    public:
        Card(Rank rank, Suit suit) : _rank(rank), _suit(suit) {

        }

        Rank getRank() const { return _rank; }
        const Rank* getRankPointer() const {
            return &_rank;
        }
        Suit getSuit() const { return _suit; }

    private:
        Rank _rank;
        Suit _suit;
    };

    Rank MaxRank(Rank a, Rank b) { return a > b ? a : b; }

    Rank MaxRankThree(Rank a, Rank b, Rank c) {
        return std::max({ a, b, c });
    }

    Rank MaxRank(std::array<Card*, MAX_HAND_CARDS + MAX_TABLE_CARDS> cards) {
        std::array<Rank, MAX_HAND_CARDS + MAX_TABLE_CARDS> ranks;
        
        for (int i = 0; i < MAX_HAND_CARDS + MAX_TABLE_CARDS; i++) {
            ranks[i] = cards[i]->getRank();
        }

        return MaxRank(MaxRankThree(ranks[0], ranks[1], ranks[2]), MaxRank(MaxRankThree(ranks[3], ranks[4], ranks[5]), ranks[6]));
    }

    Rank NextRank(Rank rank) {
        if (rank == Rank::Ace) {
            return Rank::Two;
        }
        else {
            return static_cast<Rank>(static_cast<int>(rank) + 1);
        }
    }

    const std::array<Card, DECK_52_SIZE> full52deck = {
        Card(Rank::Two, Suit::Clubs),
        Card(Rank::Two, Suit::Diamonds),
        Card(Rank::Two, Suit::Hearts),
        Card(Rank::Two, Suit::Spades),

        Card(Rank::Three, Suit::Clubs),
        Card(Rank::Three, Suit::Diamonds),
        Card(Rank::Three, Suit::Hearts),
        Card(Rank::Three, Suit::Spades),

        Card(Rank::Four, Suit::Clubs),
        Card(Rank::Four, Suit::Diamonds),
        Card(Rank::Four, Suit::Hearts),
        Card(Rank::Four, Suit::Spades),

        Card(Rank::Five, Suit::Clubs),
        Card(Rank::Five, Suit::Diamonds),
        Card(Rank::Five, Suit::Hearts),
        Card(Rank::Five, Suit::Spades),

        Card(Rank::Six, Suit::Clubs),
        Card(Rank::Six, Suit::Diamonds),
        Card(Rank::Six, Suit::Hearts),
        Card(Rank::Six, Suit::Spades),

        Card(Rank::Seven, Suit::Clubs),
        Card(Rank::Seven, Suit::Diamonds),
        Card(Rank::Seven, Suit::Hearts),
        Card(Rank::Seven, Suit::Spades),

        Card(Rank::Eight, Suit::Clubs),
        Card(Rank::Eight, Suit::Diamonds),
        Card(Rank::Eight, Suit::Hearts),
        Card(Rank::Eight, Suit::Spades),

        Card(Rank::Nine, Suit::Clubs),
        Card(Rank::Nine, Suit::Diamonds),
        Card(Rank::Nine, Suit::Hearts),
        Card(Rank::Nine, Suit::Spades),

        Card(Rank::Ten, Suit::Clubs),
        Card(Rank::Ten, Suit::Diamonds),
        Card(Rank::Ten, Suit::Hearts),
        Card(Rank::Ten, Suit::Spades),

        Card(Rank::Jack, Suit::Clubs),
        Card(Rank::Jack, Suit::Diamonds),
        Card(Rank::Jack, Suit::Hearts),
        Card(Rank::Jack, Suit::Spades),

        Card(Rank::Queen, Suit::Clubs),
        Card(Rank::Queen, Suit::Diamonds),
        Card(Rank::Queen, Suit::Hearts),
        Card(Rank::Queen, Suit::Spades),

        Card(Rank::King, Suit::Clubs),
        Card(Rank::King, Suit::Diamonds),
        Card(Rank::King, Suit::Hearts),
        Card(Rank::King, Suit::Spades),

        Card(Rank::Ace, Suit::Clubs),
        Card(Rank::Ace, Suit::Diamonds),
        Card(Rank::Ace, Suit::Hearts),
        Card(Rank::Ace, Suit::Spades),
    };

    class Deck52 {
    public:
        Deck52(bool random = true) : Cards(full52deck) {
            DrawCardCounter = 0;

            if (random) {
                shuffle();
            }
        }

        void shuffle() {
            std::random_device rd;
            std::mt19937 rng(rd());
            std::shuffle(Cards.begin(), Cards.end(), rng);
        }

        Card* DrawCard() {
            DrawCardCounter++;
            
            return &Cards[DrawCardCounter];
        }

        void Reset() {
            DrawCardCounter = 0;

            shuffle();
        }

        Card* operator[](int index) {
            return &Cards[index];
        }

    private:
        int DrawCardCounter;
        std::array<Card, DECK_52_SIZE> Cards;
    };
}
