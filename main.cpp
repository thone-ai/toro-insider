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

std::vector<std::string> generateNGrams(const std::vector<std::string>& tokens, int n) {
    std::vector<std::string> ngrams;
    int numTokens = tokens.size();

    if (numTokens < n) {
        return ngrams;
    }

    for (int i = 0; i <= numTokens - n; ++i) {
        std::string ngram;
        for (int j = 0; j < n; ++j) {
            if (j > 0) ngram += " ";
            ngram += tokens[i + j];
        }
        ngrams.push_back(ngram);
    }

    return ngrams;
}

std::string generateResponse(const std::map<std::string, std::vector<std::string>>& markovChain, int n) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::string response;
    std::vector<std::string> currentNgramTokens;
    int maxWords = 10000;

    while (currentNgramTokens.size() < n) {
        auto it = markovChain.find(".");
        if (it == markovChain.end()) {
            return response;
        }

        const auto& startingTokens = it->second;
        std::uniform_int_distribution<> dis(0, startingTokens.size() - 1);
        int index = dis(gen);
        std::string startingToken = startingTokens[index];
        currentNgramTokens = tokenize(startingToken);
    }

    while (currentNgramTokens.size() == n) {
        std::string currentNgram;
        for (const auto& token : currentNgramTokens) {
            currentNgram += token + " ";
        }
        auto it = markovChain.find(currentNgram);
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

        currentNgramTokens.erase(currentNgramTokens.begin());
        currentNgramTokens.push_back(nextToken);
    }

    return response;
}

std::map<std::string, std::vector<std::string>> buildMarkovChain(const std::vector<std::pair<std::string, std::string>>& data, int n) {
    std::map<std::string, std::vector<std::string>> markovChain;

    for (const auto& entry : data) {
        const std::string& question = entry.first;
        const std::string& answer = entry.second;

        std::istringstream iss(question);
        std::string token;
        std::vector<std::string> tokens;

        while (iss >> token) {
            tokens.push_back(token);
            if (tokens.size() == n) {
                std::string ngram;
                for (const auto& t : tokens) {
                    ngram += t + " ";
                }
                markovChain[ngram].push_back(answer);
                tokens.erase(tokens.begin());
            }
        }
    }

    return markovChain;
}

void processQuestion(const std::string& input, const std::map<std::string, std::vector<std::string>>& markovChain, int n) {
    std::vector<std::string> questions;
    std::string currentQuestion;
    std::istringstream inputStream(input);

    while (inputStream >> currentQuestion) {
        questions.push_back(preprocess(currentQuestion));
    }

    std::string combinedQuestion;
    for (const auto& q : questions) {
        combinedQuestion += q + " ";
    }

    std::string response = generateResponse(markovChain, n);
    if (!response.empty()) {
        std::cout << "Thone AI: " << response << std::endl;
    } else {
        std::cout << "Thone AI: Lo siento, no puedo entender tu pregunta. ¿Puedes intentar reformularla?" << std::endl;
    }
}

void chatWithAI(const std::string& datasetFilePath, int n) {
    std::ifstream dataset(datasetFilePath);

    if (!dataset.is_open()) {
        std::cout << "Error: no se pudo abrir '" << datasetFilePath << "'" << std::endl;
        return;
    }

    std::vector<std::pair<std::string, std::string>> data;
    std::string line;

    while (std::getline(dataset, line)) {
        std::istringstream iss(line);
        std::string question, answer;

        std::getline(iss, question, ',');
        std::getline(iss, answer);

        data.push_back(std::make_pair(preprocess(question), answer));
    }

    std::cout << "ThoneAI: ¡Me alegra verte! ¿Necesitas que te ayude en algo?" << std::endl;

    std::map<std::string, std::vector<std::string>> markovChain = buildMarkovChain(data, n);

    while (true) {
        std::cout << "User: ";
        std::string input;
        std::getline(std::cin, input);

        if (input == "exit") {
            std::cout << "Thone AI: ¡Adiós! Ten un buen día." << std::endl;
            break;
        }

        processQuestion(input, markovChain, n);
    }

    dataset.close();
}

int main() {
    std::string datasetFilePath = "dataset.csv";
    int n = 2;
    chatWithAI(datasetFilePath, n);

    return 0;
}
