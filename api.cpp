#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#include <WinSock2.h>
#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cctype>
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
    size_t minLength = (((str1.length()) < (str2.length())) ? (str1.length()) : (str2.length()));

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

std::string chatWithAI(const std::string& datasetFilePath, const std::string& user_input) {
    std::ifstream dataset(datasetFilePath);

    if (!dataset.is_open()) {
        return "Error: no se pudo abrir '" + datasetFilePath + "'";
    }

    std::string preprocessedInput = preprocess(user_input);

    std::string bestMatchAnswer = "Lo siento, no encuentro respuesta para esa pregunta.";
    double bestMatchScore = 0.0;

    std::string line;
    while (std::getline(dataset, line)) {
        std::istringstream iss(line);
        std::string question, answer;

        std::getline(iss, question, ',');
        std::getline(iss, answer);

        std::string preprocessedQuestion = preprocess(question);

        size_t pos = preprocessedInput.find(preprocessedQuestion);
        if (pos != std::string::npos && preprocessedQuestion.length() > 2) {
            double overlapRatio = static_cast<double>(preprocessedQuestion.length()) / preprocessedInput.length();
            if (overlapRatio > bestMatchScore) {
                bestMatchScore = overlapRatio;
                bestMatchAnswer = answer;
            }
        }
    }

    dataset.close();
    return bestMatchAnswer;
}

std::string generateHTMLResponse(const std::string& content) {
    std::stringstream ss;
    ss << "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: " << content.size() << "\n\n" << content;
    return ss.str();
}

std::string url_decode(const std::string& str) {
    std::string result;
    char ch;
    int i, ii;
    for (i = 0; i < str.length(); ++i) {
        if (int(str[i]) == 37) {
            sscanf(str.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<char>(ii);
            result += ch;
            i = i + 2;
        } else {
            result += str[i];
        }
    }
    return result;
}
