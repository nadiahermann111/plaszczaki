#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

// Function to create the LPS array for the KMP algorithm
std::vector<int> computeLPSArray(const std::string &pattern) {
    int m = pattern.length();
    std::vector<int> lps(m, 0);
    int length = 0;
    int i = 1;
    while (i < m) {
        if (pattern[i] == pattern[length]) {
            length++;
            lps[i] = length;
            i++;
        } else {
            if (length != 0) {
                length = lps[length - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

// KMP algorithm to find all occurrences of the pattern in the text
std::vector<int> KMPSearch(const std::string &text, const std::string &pattern) {
    int n = text.length();
    int m = pattern.length();
    std::vector<int> lps = computeLPSArray(pattern);
    std::vector<int> result;
    int i = 0; // index for modified text
    int j = 0; // index for pattern
    while (i < n) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }
        if (j == m) {
            result.push_back(i - j);
            j = lps[j - 1];
        } else if (i < n && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }
    return result;
}

// Function to replace all occurrences with kmp algorithm
std::string replaceSubstr(const std::string &text, const std::string &oldSubstr, const std::string &newSubstr) {
    std::string result = text;
    std::vector<int> positions = KMPSearch(text, oldSubstr);
    int offset = 0;
    for (int pos : positions) { //for loop modified by chatgpt
        result.replace(pos + offset, oldSubstr.length(), newSubstr);
        offset += newSubstr.length() - oldSubstr.length();
    }
    return result;
}

// encoding files
std::string encodeText(const std::string &text, const std::unordered_map<char, std::string> &encodingMap) {
    std::string encodedText;
    for (char c : text) {
        encodedText += encodingMap.at(c);
    }
    return encodedText;
}

//read text from a file
std::string readTextFromFile(const std::string &filename) {
    std::ifstream file(filename);
    std::string text((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return text;
}

int main() {
    std::unordered_map<char, std::string> encodingMap = {
        {'a', "00000"}, {'b', "00001"}, {'c', "00010"}, {'d', "00011"}, {'e', "00100"},
        {'f', "00101"}, {'g', "00110"}, {'h', "00111"}, {'i', "01000"}, {'j', "01001"},
        {'k', "01010"}, {'l', "01011"}, {'m', "01100"}, {'n', "01101"}, {'o', "01110"},
        {'p', "01111"}, {'q', "10000"}, {'r', "10001"}, {'s', "10010"}, {'t', "10011"},
        {'u', "10100"}, {'v', "10101"}, {'w', "10110"}, {'x', "10111"}, {'y', "11000"},
        {'z', "11001"}, {' ', "11010"}
    };

    //read text from file
    std::string text = readTextFromFile("input.txt");

    std::string oldSubstr = readTextFromFile("old_substr.txt");
    std::string newSubstr = readTextFromFile("new_substr.txt");

    //encoding the text
    std::string encodedText = encodeText(text, encodingMap);
    std::cout << "Encoded Text: " << encodedText << std::endl;

    //endoding replaced words
    std::string encodedOldSubstr = encodeText(oldSubstr, encodingMap);
    std::string encodedNewSubstr = encodeText(newSubstr, encodingMap);

    // Replace modified words with correct ones from the original
    std::string modifiedEncodedText = replaceSubstr(encodedText, encodedOldSubstr, encodedNewSubstr);
    std::cout << "Modified Encoded Text: " << modifiedEncodedText << std::endl;

    // Compare original and modified texts
    if (encodedText != modifiedEncodedText) {
        std::cout << "The text has been modified." << std::endl;
    } else {
        std::cout << "No modifications were made to the text." << std::endl;
    }

    return 0;
}
