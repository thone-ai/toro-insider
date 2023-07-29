#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <locale>
#include <map>
#include <random>

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

std::vector<std::string> generateNGrams(const std::string& text, int n) {
    std::vector<std::string> ngrams;
    int numChars = text.size();

    if (numChars < n) {
        return ngrams;
    }

    for (int i = 0; i <= numChars - n; ++i) {
        std::string ngram = text.substr(i, n);
        ngrams.push_back(ngram);
    }

    return ngrams;
}

double calculateSimilarity(const std::vector<std::string>& ngrams1, const std::vector<std::string>& ngrams2) {
    int commonNgrams = 0;
    for (const auto& ngram1 : ngrams1) {
        for (const auto& ngram2 : ngrams2) {
            if (ngram1 == ngram2) {
                commonNgrams++;
                break;
            }
        }
    }

    double overlapRatio = static_cast<double>(commonNgrams) / ngrams1.size();
    return overlapRatio;
}

std::string generateResponse(const std::map<std::vector<std::string>, std::vector<std::string>>& markovChain, const std::string& input, int n) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::string response;
    std::vector<std::string> currentNgrams = generateNGrams(input, n);
    int maxWords = 10000;

    while (!currentNgrams.empty()) {
        auto it = markovChain.find(currentNgrams);
        if (it == markovChain.end()) {
            break;
        }

        const auto& nextTokens = it->second;
        std::uniform_int_distribution<> dis(0, nextTokens.size() - 1);
        int index = dis(gen);
        std::string nextToken = nextTokens[index];

        if (response.size() + nextToken.size() + 1 > maxWords) {
            break;
        }

        response += nextToken + " ";

        currentNgrams.erase(currentNgrams.begin());
        if (nextToken.size() >= n) {
            std::string newNgram = nextToken.substr(nextToken.size() - n, n);
            currentNgrams.push_back(newNgram);
        } else {
            break;
        }
    }

    return response;
}

std::map<std::vector<std::string>, std::vector<std::string>> buildMarkovChain(const std::vector<std::string>& questions, const std::vector<std::string>& answers, int n) {
    std::map<std::vector<std::string>, std::vector<std::string>> markovChain;

    for (size_t i = 0; i < questions.size(); ++i) {
        std::string preprocessedQuestion = preprocess(questions[i]);
        std::vector<std::string> ngrams = generateNGrams(preprocessedQuestion, n);
        markovChain[ngrams].push_back(answers[i]);
    }

    return markovChain;
}

void chatWithAI(const std::string& datasetFilePath, int n) {
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

        questions.push_back(question);
        answers.push_back(answer);
    }

    std::string input;
    std::cout << "ThoneAI: ¡Me alegra verte! ¿Necesitas que te ayude en algo?" << std::endl;

    std::map<std::vector<std::string>, std::vector<std::string>> markovChain = buildMarkovChain(questions, answers, n);

    while (true) {
        std::cout << "User: ";
        std::getline(std::cin, input);

        if (input == "exit") {
            std::cout << "Thone AI: ¡Adiós! Ten un buen día." << std::endl;
            break;
        }

        std::string preprocessedInput = preprocess(input);
        double bestMatchScore = 0.0;
        std::string bestMatchAnswer;

        for (const auto& question : questions) {
            std::string preprocessedQuestion = preprocess(question);
            double similarity = calculateSimilarity(generateNGrams(preprocessedInput, n), generateNGrams(preprocessedQuestion, n));
            if (similarity > bestMatchScore) {
                bestMatchScore = similarity;
                bestMatchAnswer = answers[&question - &questions[0]];
            }
        }

        const double matchingThreshold = 0.5;
        if (bestMatchScore > matchingThreshold) {
            std::cout << "Thone AI: " << bestMatchAnswer << std::endl;
        } else {
            std::string response = generateResponse(markovChain, preprocessedInput, n);
            if (!response.empty()) {
                std::cout << "Thone AI: " << response << std::endl;
                        } else {
                std::cout << "Thone AI: Lo siento, no puedo entender tu pregunta. ¿Puedes intentar reformularla?" << std::endl;
            }
        }
    }

    dataset.close();
}

int main() {
    std::string datasetFilePath = "dataset.csv";
    int n = 3;
    chatWithAI(datasetFilePath, n);

    return 0;
}
