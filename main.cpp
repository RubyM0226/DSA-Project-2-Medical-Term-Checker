#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Trie.h"
#include "HashMap.h"


using namespace std;


// Main function that connects everything and runs the app

vector<string> loadTerms(const string& filename) {
    vector<string> terms;
    ifstream file(filename);

    if(!file.is_open()) {
        cerr << "Error: could not open file '" << filename << "'\n";
        return terms;
    }

    string line;
    while(getline(file, line)) {
        if(!line.empty()) {
            terms.push_back(line);
        }
    }

    cout << "Loaded " << terms.size() << " terms from " << filename << "\n";
    return terms;
}


void printMenu() {
    cout << "\n----------------------------------\n";
    cout << "  Medical Term Search Tool\n";
    cout << "\n----------------------------------\n";
    cout << "  1. Exact search     (Hash Map)\n";
    cout << "  2. Autocomplete     (Trie)\n";
    cout << "  3. Run benchmarks   (Kaden's module)\n";
    cout << "  4. Exit\n";
    cout << "\n----------------------------------\n";
    cout << "  Enter choice: ";
}


// MENU OPTION 1
void exactSearch(HashMap& hashMap) {
    cout << "\nEnter a term to look up: ";
    string query;
    cin.ignore();
    getline(cin, query);

    bool found = hashMap.search(query);

    if(found) {
        cout << "  [FOUND] \"" << query << "\" found in database.\n";
    } else {
        cout << "  [NOT FOUND] \"" << query << "\" not found.\n";
        cout << "    (Tip: try autocomplete to find similar terms)\n";
    }
}


//  MENU OPTION 2
void autocompleteSearch(Trie& trie) {
    cout << "\nEnter a prefix to autocomplete: ";
    string prefix;
    cin.ignore();
    getline(cin, prefix);

    vector<string> results = trie.autocomplete(prefix);

    if(results.empty()) {
        cout << "  No terms found starting with \"" << prefix << "\".\n";
    } else {
        cout << "  Found " << results.size() << " match(es):\n";
        // SHOW MAX 20 RESULTS
        int limit = min((int)results.size(), 20);
        for(int i = 0; i < limit; i++) {
            cout << "    - " << results[i] << "\n";
        }
        if((int)results.size() > 20) {
            cout << "    ... and " << results.size() - 20 << " more.\n";
        }
    }
}


// MAIN FUNCTION TO LOAD THE DATA
int main() {
    string dataFile = "terms.txt";
    vector<string> terms = loadTerms(dataFile);

    if(terms.empty()) {
        cerr << "No terms loaded. Make sure terms.txt is in the same folder.\n";
        return 1;
    }

    cout << "Building Trie...\n";
    Trie trie;
    for(const string& term : terms) {
        trie.insert(term);
    }
    cout << "Trie built with " << trie.size() << " terms.\n";


    cout << "Building Hash Map...\n";
    HashMap hashMap;
    for(const string& term : terms) {
         hashMap.insert(term);
    }
    cout << "Hash Map built.\n";


    int choice = 0;
    while(choice != 4) {
        printMenu();
        cin >> choice;

        switch(choice) {
            case 1:
                exactSearch(hashMap);
                break;
            case 2:
                autocompleteSearch(trie);
                break;
            case 3:
                cout << "\n  Benchmarking is handled by Kaden's module.\n";
                cout << "  Run benchmark.cpp separately.\n";
                break;
            case 4:
                cout << "\n  Goodbye!\n";
                break;
            default:
                cout << "\n  Invalid choice — enter 1, 2, 3, or 4.\n";
        }
    }
    return 0;
}