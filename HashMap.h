#pragma once
#include <string>
#include <vector>

using namespace std;

// Custom Hash Map - Kaden Lendick


// One entry in a bucket's chain :3
struct HashNode {
    string term;
    HashNode* next = nullptr;
};


class HashMap {
public:

    // MY CONSTRUCTOR
    // bucketCount is FIXED for the life of the map so we dont ger rehashing and resizing during the benchmark
    HashMap(int bucketCount = 100003) {
        numBuckets = bucketCount;
        buckets.assign(numBuckets, nullptr);
        count = 0;
    }

    // MY COOL DESTRUCTOR
    ~HashMap() {
        for(int i = 0; i < numBuckets; i++) {
            HashNode* current = buckets[i];
            while(current != nullptr) {
                HashNode* next = current->next;
                delete current;
                current = next;
            }
        }
    }

    // INSERT FUNCTION
    void insert(const string& term) {
        string lower = toLower(term);
        int index = hashIndex(lower);

        // skip duplicates so size() matches the trie's term count
        for(HashNode* node = buckets[index]; node != nullptr; node = node->next) {
            if(node->term == lower) {
                return;
            }
        }

        // add new term to the FRONT of the chain
        HashNode* node = new HashNode();
        node->term = lower;
        node->next = buckets[index];
        buckets[index] = node;
        count++;
    }

    // EXACT SEARCH FUNCTION
    bool search(const string& term) const {
        string lower = toLower(term);
        int index = hashIndex(lower);

        for(HashNode* node = buckets[index]; node != nullptr; node = node->next) {
            if(node->term == lower) {
                return true;
            }
        }
        return false;
    }

    int size() const {
        return count;
    }

private:
    vector<HashNode*> buckets;
    int numBuckets;
    int count;

    // HASH FUNCTION mapped into the fixed bucket range
    int hashIndex(const string& str) const {
        unsigned long hash = 5381;
        for(char ch : str) {
            hash = ((hash << 5) + hash) + (unsigned char)ch;
        }
        return (int)(hash % numBuckets);
    }

    static string toLower(const string& s) {
        string result = s;
        for(char& c : result) c = tolower(c);
        return result;
    }
};