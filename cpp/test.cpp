#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>

bool isFourDigitNumber(const std::string& s) {
    return s.size() == 4 && std::all_of(s.begin(), s.end(), ::isdigit) && s[0] != '0';
}

int main() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1000, 9999);
    std::string secret = std::to_string(dist(gen));

    std::cout << "Welcome to the 4-digit guessing game!" << std::endl;
    std::cout << "I have chosen a random 4-digit number. Try to guess it." << std::endl;
    std::cout << "After each guess, I will tell you how many digits are correct in the right position," << std::endl;
    std::cout << "and how many digits are present but in the wrong position." << std::endl;

    int attempts = 0;
    while (true) {
        std::string guess;
        std::cout << "\nEnter your guess: ";
        std::getline(std::cin, guess);

        if (!isFourDigitNumber(guess)) {
            std::cout << "Please enter a valid 4-digit number between 1000 and 9999." << std::endl;
            continue;
        }

        ++attempts;
        if (guess == secret) {
            std::cout << "Correct! You guessed the number in " << attempts << " attempt" << (attempts == 1 ? "" : "s") << "." << std::endl;
            break;
        }

        int correctPosition = 0;
        int wrongPosition = 0;
        std::vector<int> secretCounts(10, 0);
        std::vector<int> guessCounts(10, 0);

        for (int i = 0; i < 4; ++i) {
            if (guess[i] == secret[i]) {
                ++correctPosition;
            } else {
                ++secretCounts[secret[i] - '0'];
                ++guessCounts[guess[i] - '0'];
            }
        }

        for (int digit = 0; digit < 10; ++digit) {
            wrongPosition += std::min(secretCounts[digit], guessCounts[digit]);
        }

        std::cout << "Correct digits in the correct position: " << correctPosition << std::endl;
        std::cout << "Correct digits in the wrong position: " << wrongPosition << std::endl;
    }

    std::cout << "Thanks for playing!" << std::endl;
    return 0;
}
