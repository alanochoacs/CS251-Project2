#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

/**
 * Cleans a specific token by:
 * - Removing leading and trailing punctuation
 * - Converting to lowercase
 * If the token has no letters, returns the empty string.
 */
string cleanToken(string s) {
    int countLetters = 0;
    for (unsigned int i = 0; i < s.size(); i++) {
        if (isalpha(s.at(i)) != 0) {
            s.at(i) = tolower(s.at(i));
            countLetters++;
        }
    }
    
    if (countLetters == 0) {
        return "";
    }

    int index = 0;
    while(ispunct(s.at(index)) != 0) {
        s.erase(index, 1);
    }

    index = s.size() - 1;
    while(ispunct(s.at(index)) != 0) {
        s.erase(index, 1);
        index--;
    }

    return s;
}


/**
 * Returns a set of the cleaned tokens in the given text.
 */
set<string> gatherTokens(string text) {
    set<string> set;
    istringstream iss(text);
    string word;

    // Counting words by reading each word from the line
    while (iss >> word) {
        string clean = cleanToken(word);
        if (clean != "") {
            set.insert(clean);
        }
    }

    return set;
}


/**
 * Builds the inverted index from the given data file. The existing contents
 * of `index` are left in place.
 */
int buildIndex(string filename, map<string, set<string>>& index) {
    ifstream infile(filename);
    int count = 0;
    string url;
    string text;

    if (infile.is_open()) {
        while (getline(infile, url) && getline(infile, text)) {
            count++;

            set<string> words = gatherTokens(text);
            for (string word : words) {
                if (index.count(word) == 1) {
                    index.at(word).insert(url);
                } else {
                    index.emplace(word, set{url});
                }
            }
        }
    } else {
        cout << "Invalid filename." << endl;
    }

    return count;
}


/**
 * Runs a search query on the provided index, and returns a set of results.
 *
 * Search terms are processed using set operations (union, intersection, difference).
 */
set<string> findQueryMatches(map<string, set<string>>& index, string sentence) {
    vector<char> operations{' '};
    for (unsigned int i = 0; i < sentence.size(); i++) {
        if (sentence.at(i) == ' ') {
            if (sentence.at(i+1) == '+') {
                operations.push_back('+');
            } else if (sentence.at(i+1) == '-'){
                operations.push_back('-');
            } else {
                operations.push_back(' ');
            }
        }
    }

    vector<string> words;
    istringstream iss(sentence);
    string word;
    while (iss >> word) {
        string clean = cleanToken(word);
        if (clean != "") {
            words.push_back(clean);
        }
    }

    set<string> matches;
    if (index.count(words.at(0)) == 1) {
        matches = index.at(words.at(0));
    }
    
    for (unsigned int i = 1; i < words.size(); i++) {
        set<string> term;
        if (index.count(words.at(i)) == 1) {
            term = index.at(words.at(i));
        }

        set<string> temp;
        if (operations.at(i) == '+') {
            set_intersection(matches.begin(), matches.end(), term.begin(), term.end(), inserter(temp, temp.begin()));
        } else if (operations.at(i) == '-') {
            set_difference(matches.begin(), matches.end(), term.begin(), term.end(), inserter(temp, temp.begin()));
        } else {
            set_union(matches.begin(), matches.end(), term.begin(), term.end(), inserter(temp, temp.begin()));
        }
        matches = temp;
    }

    return matches;
}


/**
 * Runs the main command loop for the search program
 */
void searchEngine(string filename) {
    string input;
    map<string, set<string>> index;
    int pages = buildIndex(filename, index);

    cout << "Stand by while building index..." << endl;
    cout << "Indexed " << pages << " pages containing " << index.size() << " unique terms" << endl;
    cout << endl;
    cout << "Enter query sentence (press enter to quit): ";

    getline(cin, input);
    
    while (input != "") {
        set<string> matches = findQueryMatches(index, input);

        cout << "Found " << matches.size() << " matching pages" << endl;
        for (string str : matches) {
            cout << str << endl;
        }
        
        cout << "Enter query sentence (press enter to quit): ";
        getline(cin, input);
    }

    cout << "Thank you for searching!" << endl;
}