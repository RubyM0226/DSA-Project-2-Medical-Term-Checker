#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include "Trie.h"
#include "HashMap.h"

using namespace std;
using namespace std::chrono;

// Benchmark: Trie vs custom Hash Map - Kaden Lendick


// Load every term from the file (one term per line, no blank lines)
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
    return terms;
}

vector<string> generateTerms(int n) {
    vector<string> terms;
    terms.reserve(n);
    for(int i = 0; i < n; i++) {
        string t = "term" + to_string(i);
        terms.push_back(t);
    }
    return terms;
}

int main() {
    vector<int> sizes = {1000, 10000, 50000, 100000};
    const int SEARCH_TRIALS = 100000;  // searches to average over per structure

    // Try to use the real dataset
    vector<string> allTerms = loadTerms("terms.txt");
    bool usingReal = ((int)allTerms.size() >= sizes.back());
    if(!usingReal) {
        cout << "terms.txt has only " << allTerms.size()
             << " terms; using GENERATED terms for the benchmark instead.\n\n";
        allTerms = generateTerms(sizes.back());
    } else {
        cout << "Loaded " << allTerms.size() << " real terms from terms.txt.\n\n";
    }

    // Write a CSV we can open in Excel for the charts
    ofstream csv("benchmark_results.csv");
    csv << "N,TrieInsert_ms,HashInsert_ms,TrieSearch_ns_avg,HashSearch_ns_avg,ResultsMatch\n";

    // Print a table to the screen too.
    cout << "N\tTrieIns(ms)\tHashIns(ms)\tTrieSrch(ns)\tHashSrch(ns)\tMatch\n";
    cout << "-------------------------------------------------------------------------------\n";

    mt19937 rng(12345);  // fixed seed => same random searches every run

    for(int N : sizes) {
        // Take the first N terms for this round
        vector<string> terms(allTerms.begin(), allTerms.begin() + N);

        // ---- INSERT PHASE ----
        Trie trie;
        auto t0 = high_resolution_clock::now();
        for(const string& term : terms) {
            trie.insert(term);
        }
        auto t1 = high_resolution_clock::now();
        double trieInsertMs = duration<double, milli>(t1 - t0).count();

        HashMap hashMap(N * 2 + 1);
        auto t2 = high_resolution_clock::now();
        for(const string& term : terms) {
            hashMap.insert(term);
        }
        auto t3 = high_resolution_clock::now();
        double hashInsertMs = duration<double, milli>(t3 - t2).count();

        // ---- BUILD A SEARCH LIST ----
        vector<string> queries;
        queries.reserve(SEARCH_TRIALS);
        uniform_int_distribution<int> pick(0, N - 1);
        for(int i = 0; i < SEARCH_TRIALS; i++) {
            if(i % 2 == 0) {
                queries.push_back(terms[pick(rng)]);
            } else {
                queries.push_back(terms[pick(rng)] + "_x"); // almost certainly missing
            }
        }

        // ---- SEARCH PHASE: Trie ----
        long long trieHits = 0;
        auto s0 = high_resolution_clock::now();
        for(const string& q : queries) {
            if(trie.search(q)) trieHits++;
        }
        auto s1 = high_resolution_clock::now();
        double trieSearchNs = duration<double, nano>(s1 - s0).count() / SEARCH_TRIALS;

        // ---- SEARCH PHASE: Hash Map ----
        long long hashHits = 0;
        auto s2 = high_resolution_clock::now();
        for(const string& q : queries) {
            if(hashMap.search(q)) hashHits++;
        }
        auto s3 = high_resolution_clock::now();
        double hashSearchNs = duration<double, nano>(s3 - s2).count() / SEARCH_TRIALS;

        // ---- CORRECTNESS: both structures must agree on every query ----
        bool match = (trieHits == hashHits);
        for(const string& q : queries) {
            if(trie.search(q) != hashMap.search(q)) {
                match = false;
                break;
            }
        }

        cout << N << "\t" << trieInsertMs << "\t\t" << hashInsertMs << "\t\t"
             << trieSearchNs << "\t\t" << hashSearchNs << "\t\t"
             << (match ? "YES" : "NO") << "\n";

        csv << N << "," << trieInsertMs << "," << hashInsertMs << ","
            << trieSearchNs << "," << hashSearchNs << ","
            << (match ? "YES" : "NO") << "\n";
    }

    csv.close();
    cout << "\nSaved results to benchmark_results.csv (open in Excel).\n";
    return 0;
}