#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <cctype>
#include <climits>
#include <cstdlib>
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#endif

using namespace std;

//numeric variables
const int wordLength = 5;
const int totalGuesses = 6;
const int matched = 2;
const int partialMatched = 1;
const int notMatched = 0;

//ansi color codes
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define WHITE "\033[1;37m"
#define RESET "\033[0m"

//enabling ansi on windows 
void enableANSI() {
    #ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    #endif
}

//get 5-letter random word
string getFiveLetterWord() {
    string word;
    system("curl -s \"https://random-word-api.herokuapp.com/word?length=5\" > word.txt");
    
    ifstream file("word.txt");
    if (!file || file.peek() == EOF) {
        cerr << "Error fetching word. Ensure internet connection & curl is installed.\n";
        return "";
    }
    
    file >> word;
    file.close();
    
    //extract the word correctly
    if (word.size()>4 && word.front()=='[' && word.back()==']') {
        word = word.substr(2, word.size() - 4);
    }
    
    return (word.length()==wordLength) ? word : "";
}

//check for matching of letter
void inputMatch(string& input, string& actualWord, vector<int>& ans) {
    unordered_map<char, int> actualFreq;
    for (char c : actualWord) actualFreq[c]++;

    //match
    for (int i=0; i<wordLength; i++) {
        if (input[i]==actualWord[i]) {
            ans[i] = matched;
            actualFreq[input[i]]--;        
        }
    }

    //partial match
    for (int i=0; i<wordLength; i++) {
        if (ans[i]!=matched && actualFreq[input[i]]>0) {
            ans[i] = partialMatched;
            actualFreq[input[i]]--;     
        }
    }
}

//colour-coding of each letter
void printColoredWord(string input, vector<int> ans) {
    for (int i=0; i<wordLength; i++) {
        if (ans[i]==matched)
            cout << GREEN << input[i] << RESET;
        else if (ans[i]==partialMatched)
            cout << YELLOW << input[i] << RESET;
        else
            cout << WHITE << input[i] << RESET;
    }
    cout << endl;
}

int main() {
    enableANSI();
    string actualWord = getFiveLetterWord();

    if (actualWord.empty()) {
        cout << "Error fetching a valid word. Try again later." << endl;
        return 1;
    }

    cout << "Word selected! Start guessing..." << endl;

    int guess = totalGuesses;

    //run a loop for the guesses
    while (guess>0) {
        string input;
        cout << "Enter a 5-letter word: ";
        cin >> input;

        //ensure input is 5 characters long
        if (input.length()!=wordLength) {
            cout << "Invalid input! Please enter a 5-letter word." << endl;
            continue;
        }
        
        //convert word to lowercase
        for (char& c : input) c = tolower(c);

        vector<int> ans(wordLength, notMatched);
        inputMatch(input, actualWord, ans);

        bool match = true;

        //if input is correct, mark match=false
        for (int i=0; i<wordLength; i++) {
            if (ans[i]!=matched) {
                match = false;
                break;
            }
        }
        
        //if input is correct, end game
        if (match) {
            cout << "You won! The word was: ";
            printColoredWord(input, ans);
            return 0;
        }
        
        //else, make another guess
        cout << "Check for Match again: ";
        printColoredWord(input, ans);
        guess--;
        cout << endl;
    }
    
    //guesses exhausted
    cout << "You lost! The correct word was: " << actualWord << endl;
    return 0;
}
