#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <locale>

// Helper function to remove accents from characters
std::string removeAccents(const std::string& str) {
    std::string noAccents;
    std::locale loc;
    for (char c : str) {
        if (std::isalnum(c, loc) || std::isspace(c, loc)) {
            noAccents += c;
        }
    }
    return noAccents;
}

// Helper function to combine two strings partially
std::string combineStrings(const std::string& str1, const std::string& str2) {
    std::string result;
    size_t minLength = std::min(str1.length(), str2.length());

    for (size_t i = 0; i < minLength; ++i) {
        if (str1[i] == str2[i]) {
            result += str1[i];
        } else {
            break;
        }
    }

    result += str2.substr(minLength);

    return result;
}

std::vector<std::string> tokenize(const std::string& sentence) {
    std::vector<std::string> tokens;
    std::istringstream iss(sentence);
    std::string token;

    while (iss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

std::string preprocess(const std::string& s) {
    std::string withoutAccents = removeAccents(s);
    std::string result;

    for (char c : withoutAccents) {
        if (!std::ispunct(c)) {
            result += std::tolower(c);
        }
    }

    return result;
}

void chatWithAI(const std::string& datasetFilePath) {
    std::ifstream dataset(datasetFilePath);

    if (!dataset.is_open()) {
        std::cout << "Error: no se pudo abrir '" << datasetFilePath << "'" << std::endl;
        return;
    }

    std::vector<std::string> questions;
    std::vector<std::string> answers;
    std::string line;

    while (std::getline(dataset, line)) {
        std::istringstream iss(line);
        std::string question, answer;

        std::getline(iss, question, ',');
        std::getline(iss, answer);

        questions.push_back(preprocess(question));
        answers.push_back(answer);
    }

    std::string input;
    std::cout << "ThoneAI: ¡Me alegra verte! ¿Necesitas que te ayude en algo?" << std::endl;

    while (true) {
        std::cout << "User: ";
        std::getline(std::cin, input);

        if (input == "exit") {
            std::cout << "Thone AI: ¡Adiós! Ten un buen día." << std::endl;
            break;
        }

        std::string preprocessedInput = preprocess(input);

        double bestMatchScore = 0.0;
        int bestMatchIndex = -1;

        for (size_t i = 0; i < questions.size(); ++i) {
            std::vector<std::string> tokens1 = tokenize(preprocessedInput);
            std::vector<std::string> tokens2 = tokenize(questions[i]);

            int commonWords = 0;
            for (const auto& token1 : tokens1) {
                for (const auto& token2 : tokens2) {
                    if (token1 == token2) {
                        commonWords++;
                        break;
                    }
                }
            }

            double overlapRatio = static_cast<double>(commonWords) / tokens1.size();
            if (overlapRatio > bestMatchScore) {
                bestMatchScore = overlapRatio;
                bestMatchIndex = i;
            } else if (overlapRatio == bestMatchScore && answers[i].length() > answers[bestMatchIndex].length()) {
                // If two questions have the same match score, choose the one with the longer answer
                bestMatchIndex = i;
            }
        }

        // Adjust the matching threshold as needed (e.g., 0.5 means 50% overlap)
        const double matchingThreshold = 0.5;
        if (bestMatchIndex != -1 && bestMatchScore > matchingThreshold) {
            std::cout << "Thone AI: " << answers[bestMatchIndex] << std::endl;
        } else {
            std::cout << "Thone AI: Lo siento, no encuentro respuesta a su pregunta." << std::endl;
        }
    }

    dataset.close();
}

int main() {
    std::string datasetFilePath = "dataset.csv";
    chatWithAI(datasetFilePath);

    return 0;
}
