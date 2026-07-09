#pragma once
#include <map>
#include <string>
#include <vector>

using namespace std;

// Trie (medical term search and autocompletion) - Ruby McCandless


struct TrieNode {
    map<char, TrieNode*> children;
    bool isEnd = false;
};


class Trie {
public:

    // MY CONSTRUCTOR
    Trie() {
        root = new TrieNode();
    }

    // MY DESTRUCTOR
    ~Trie() {
        destroyNode(root);
    }

    // INSERT FUNCTION
    void insert(const std::string& term) {
        string lower = toLower(term);

        TrieNode* current = root;
        for(char ch : lower) {
            if(current->children.find(ch) == current->children.end()) {
                current->children[ch] = new TrieNode();
            }
            current = current->children[ch];
        }
        current->isEnd = true;
    }

    // EXACT SEARCH FUNCTION
    bool search(const std::string& term) const {
        std::string lower = toLower(term);
        TrieNode* node = walkTo(lower);
        return node != nullptr && node->isEnd;
    }

    // PREFIX SEARCH FUNCTION
    vector<string> autocomplete(const string& prefix) const {
        string lower = toLower(prefix);
        vector<string> results;

        TrieNode* node = walkTo(lower);
        if(node == nullptr) {
            return results;
        }

        collectAll(node, lower, results);
        return results;
    }

    int size() const {
        return countTerms(root);
    }

private:
    TrieNode* root;
    TrieNode* walkTo(const std::string& str) const {
        TrieNode* current = root;
        for(char ch : str) {
            if(current->children.find(ch) == current->children.end()) {
                return nullptr;  // dead end
            }
            current = current->children.at(ch);
        }
        return current;
    }

    // COLLECT ALL FUNCTION
    void collectAll(TrieNode* node, const string& prefix, vector<string>& results) const {
        if(node->isEnd) {
            results.push_back(prefix);
        }
        for(auto& [ch, child] : node->children) {
            collectAll(child, prefix + ch, results);
        }
    }

    // DESTROY NODE FUNCTION
    void destroyNode(TrieNode* node) {
        if(node == nullptr) return;
        for(auto& [ch, child] : node->children) {
            destroyNode(child);
        }
        delete node;
    }

    // COUNT NUMBER OF TERMS FUNCTION
    int countTerms(TrieNode* node) const {
        if(node == nullptr) return 0;
        int count = node->isEnd ? 1 : 0;
        for(auto& [ch, child] : node->children) {
            count += countTerms(child);
        }
        return count;
    }

    static string toLower(const string& s) {
        string result = s;
        for(char& c : result) c = std::tolower(c);
        return result;
    }
};