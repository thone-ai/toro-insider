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

double evaluateExpression(const std::string& expression) {
    std::istringstream iss(expression);
    double result = 0.0;
    char op = '+';

    while (true) {
        double num;
        iss >> num;

        if (iss.fail()) {
            break;
        }

        switch (op) {
            case '+':
                result += num;
                break;
            case '-':
                result -= num;
                break;
            case '*':
                result *= num;
                break;
            case '/':
                if (num != 0.0) {
                    result /= num;
                } else {
                    std::cerr << "Thone AI: No puedes dividir entre 0." << std::endl;
                    return 0.0;
                }
                break;
        }

        iss >> op;
    }

    return result;
}

std::string generateResponse(const std::map<std::vector<std::string>, std::vector<std::string>>& markovChain, const std::string& input, int n) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::string response;
    std::vector<std::string> currentNgrams = generateNGrams(input, n);
    int maxWords = 10000;

    if (input.find("cuanto es ") == 0) {
        std::string mathExpression = input.substr(10);
        double result = evaluateExpression(mathExpression);
        return "El resultado es: " + std::to_string(result);
    }

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

std::string trim(const std::string& str) {
    std::string result = str;
    result.erase(std::remove_if(result.begin(), result.end(), ::isspace), result.end());
    return result;
}

void chatWithAI(const std::string& datasetFilePathEs, const std::string& datasetFilePathEn, int n) {
    std::ifstream datasetEs(datasetFilePathEs);
    std::ifstream datasetEn(datasetFilePathEn);

    if (!datasetEs.is_open() || !datasetEn.is_open()) {
        std::cout << "Error: Could not open dataset files." << std::endl;
        return;
    }

    std::vector<std::string> questionsEs;
    std::vector<std::string> answersEs;
    std::vector<std::string> questionsEn;
    std::vector<std::string> answersEn;

    std::string line;

    while (std::getline(datasetEs, line)) {
        std::istringstream iss(line);
        std::string question, answer;

        std::getline(iss, question, ',');
        std::getline(iss, answer);

        questionsEs.push_back(question);
        answersEs.push_back(answer);
    }

    while (std::getline(datasetEn, line)) {
        std::istringstream iss(line);
        std::string question, answer;

        std::getline(iss, question, ',');
        std::getline(iss, answer);

        questionsEn.push_back(question);
        answersEn.push_back(answer);
    }

    datasetEs.close();
    datasetEn.close();

    std::map<std::vector<std::string>, std::vector<std::string>> markovChainEs = buildMarkovChain(questionsEs, answersEs, n);
    std::map<std::vector<std::string>, std::vector<std::string>> markovChainEn = buildMarkovChain(questionsEn, answersEn, n);

    std::cout << "Training completed with " << questionsEs.size() << " questions in Spanish and " << questionsEn.size() << " questions in English." << std::endl;

    std::string input;
    std::cout << "ThoneAI: ¡Me alegra verte! ¿Necesitas que te ayude en algo?" << std::endl;

    while (true) {
        std::cout << "User: ";
        std::getline(std::cin, input);
        std::string inputTrimmed = trim(input);
        if (inputTrimmed == "") {
            std::cout << "Thone AI: No me has preguntado nada." << std::endl;
            break;
        }

        if (input == "exit") {
            std::cout << "Thone AI: ¡Adiós! Ten un buen día." << std::endl;
            break;
        }

        std::string preprocessedInput = preprocess(input);
        double bestMatchScore = 0.0;
        std::string bestMatchAnswer;

        for (const auto& question : questionsEs) {
            std::string preprocessedQuestion = preprocess(question);
            double similarity = calculateSimilarity(generateNGrams(preprocessedInput, n), generateNGrams(preprocessedQuestion, n));
            if (similarity > bestMatchScore) {
                bestMatchScore = similarity;
                bestMatchAnswer = answersEs[&question - &questionsEs[0]];
            }
        }

        for (const auto& question : questionsEn) {
            std::string preprocessedQuestion = preprocess(question);
            double similarity = calculateSimilarity(generateNGrams(preprocessedInput, n), generateNGrams(preprocessedQuestion, n));
            if (similarity > bestMatchScore) {
                bestMatchScore = similarity;
                bestMatchAnswer = answersEn[&question - &questionsEn[0]];
            }
        }

        const double matchingThreshold = 0.5;
        if (bestMatchScore > matchingThreshold) {
            std::cout << "Thone AI: " << bestMatchAnswer << std::endl;
        } else if (preprocessedInput.substr(0, 9) == "cuanto es") {
            std::string mathExpression = preprocessedInput.substr(10);
            double result = evaluateExpression(mathExpression);
            std::cout << "Thone AI: El resultado es: " << result << std::endl;
        } else {
            std::string response = generateResponse(markovChainEs, preprocessedInput, n);
            if (!response.empty()) {
                std::cout << "Thone AI: " << response << std::endl;
            } else {
                std::cout << "Thone AI: Lo siento, no puedo entender tu pregunta. ¿Puedes intentar reformularla?" << std::endl;
            }
        }
    }
}

int main() {
    std::string datasetFilePathEs = "dataset_es.csv";
    std::string datasetFilePathEn = "dataset_en.csv";
    int n = 3;

    chatWithAI(datasetFilePathEs, datasetFilePathEn, n);

    return 0;
}
