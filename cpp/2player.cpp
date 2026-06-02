#include <algorithm>
#include <iostream>
#include <random>
#include <string>
#include <vector>

// Check whether the provided string is a valid 4-digit number that does not start with zero.
bool isFourDigitNumber(const std::string& s) {
    return s.size() == 4 && std::all_of(s.begin(), s.end(), ::isdigit) && s[0] != '0';
}

// Compute feedback for a guess against a secret number.
// Returns a pair: {correct position count, wrong position count}.
std::pair<int, int> computeFeedback(const std::string& guess, const std::string& secret) {
    int correctPosition = 0;
    std::vector<int> secretCounts(10, 0);
    std::vector<int> guessCounts(10, 0);

    // Count exact matches and record remaining digits for wrong-position matching.
    for (int i = 0; i < 4; ++i) {
        if (guess[i] == secret[i]) {
            ++correctPosition;
        } else {
            ++secretCounts[secret[i] - '0'];
            ++guessCounts[guess[i] - '0'];
        }
    }

    // Count digits that are correct but in the wrong position.
    int wrongPosition = 0;
    for (int digit = 0; digit < 10; ++digit) {
        wrongPosition += std::min(secretCounts[digit], guessCounts[digit]);
    }

    return {correctPosition, wrongPosition};
}

// Read a valid 4-digit guess from the user.
std::string readFourDigitGuess() {
    while (true) {
        std::string guess;
        std::cout << "Enter your guess: ";
        std::getline(std::cin, guess);

        if (isFourDigitNumber(guess)) {
            return guess;
        }

        std::cout << "Please enter a valid 4-digit number between 1000 and 9999." << std::endl;
    }
}

// Read a non-negative integer from the user and validate it.
bool readNonNegativeInt(const std::string& prompt, int& value) {
    while (true) {
        std::string input;
        std::cout << prompt;
        std::getline(std::cin, input);

        try {
            size_t pos = 0;
            value = std::stoi(input, &pos);
            if (pos == input.size() && value >= 0) {
                return true;
            }
        } catch (...) {
            // Invalid input is handled by retrying.
        }

        std::cout << "Please enter a non-negative integer." << std::endl;
    }
}

// Build the full list of all possible 4-digit numbers from 1000 to 9999.
std::vector<std::string> buildAllCandidates() {
    std::vector<std::string> candidates;
    candidates.reserve(9000);
    for (int number = 1000; number <= 9999; ++number) {
        candidates.push_back(std::to_string(number));
    }
    return candidates;
}

int main() {
    // Random generator for Nicole's secret and for choosing from candidate guesses.
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> secretDist(1000, 9999);

    std::cout << "Sean vs Nicole: 4-digit guess game" << std::endl;
    std::cout << "Nicole has a secret number, and Sean has a secret number in his head." << std::endl;
    std::cout << "Sean and Nicole will alternate guesses until one of them wins." << std::endl;
    std::cout << std::endl;

    // Nicole's secret number is randomly chosen by the program.
    std::string nicoleSecret = std::to_string(secretDist(gen));

    int seanAttempts = 0;
    int nicoleAttempts = 0;
    bool seanWon = false;
    bool nicoleWon = false;

    std::cout << "Nicole has chosen her secret number." << std::endl;
    std::cout << "Sean, think of a secret 4-digit number between 1000 and 9999." << std::endl;
    std::cout << "Do not tell Nicole or enter it into the program." << std::endl;
    std::cout << "Press Enter when you are ready for the first guess." << std::endl;
    std::string dummy;
    std::getline(std::cin, dummy);

    // Build Nicole's possible candidate list for her guesses.
    std::vector<std::string> candidates = buildAllCandidates();

    while (!seanWon && !nicoleWon) {
        // Sean's turn: he guesses Nicole's secret.
        std::cout << "\nSean's turn:" << std::endl;
        std::string seanGuess = readFourDigitGuess();
        ++seanAttempts;

        if (seanGuess == nicoleSecret) {
            std::cout << "Sean guessed Nicole's secret in " << seanAttempts << " attempt" << (seanAttempts == 1 ? "" : "s") << "." << std::endl;
            seanWon = true;
            break;
        }

        // Give Sean feedback for his guess.
        auto [seanCorrectPosition, seanWrongPosition] = computeFeedback(seanGuess, nicoleSecret);
        std::cout << "Correct digits in the correct position: " << seanCorrectPosition << std::endl;
        std::cout << "Correct digits in the wrong position: " << seanWrongPosition << std::endl;

        // Nicole's turn: she guesses Sean's secret using the candidate list.
        if (candidates.empty()) {
            std::cout << "Nicole has no possible candidates left. The feedback may have been inconsistent." << std::endl;
            break;
        }

        std::uniform_int_distribution<size_t> pick(0, candidates.size() - 1);
        std::string nicoleGuess = candidates[pick(gen)];
        ++nicoleAttempts;

        std::cout << "\nNicole's turn:" << std::endl;
        std::cout << "Nicole guesses: " << nicoleGuess << std::endl;

        int nicoleCorrectPosition = 0;
        int nicoleWrongPosition = 0;
        readNonNegativeInt("Correct digits in the correct position: ", nicoleCorrectPosition);
        readNonNegativeInt("Correct digits in the wrong position: ", nicoleWrongPosition);

        if (nicoleCorrectPosition == 4) {
            std::cout << "Nicole guessed Sean's secret in " << nicoleAttempts << " attempt" << (nicoleAttempts == 1 ? "" : "s") << "." << std::endl;
            nicoleWon = true;
            break;
        }

        // Filter the list of possible secrets for Nicole's next guess.
        std::vector<std::string> nextCandidates;
        nextCandidates.reserve(candidates.size());
        for (const std::string& candidate : candidates) {
            auto [candidateCorrect, candidateWrong] = computeFeedback(nicoleGuess, candidate);
            if (candidateCorrect == nicoleCorrectPosition && candidateWrong == nicoleWrongPosition) {
                nextCandidates.push_back(candidate);
            }
        }

        candidates.swap(nextCandidates);
        std::cout << "Nicole has " << candidates.size() << " possible numbers left." << std::endl;

        if (candidates.empty()) {
            std::cout << "The feedback appears inconsistent with a valid 4-digit number." << std::endl;
            break;
        }
    }

    std::cout << std::endl;
    if (seanWon) {
        std::cout << "Sean wins!" << std::endl;
    } else if (nicoleWon) {
        std::cout << "Nicole wins!" << std::endl;
    } else {
        std::cout << "Game ended without a winner." << std::endl;
    }

    std::cout << "Thanks for playing Sean vs Nicole!" << std::endl;
    return 0;
}
