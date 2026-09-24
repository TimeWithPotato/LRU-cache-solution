#include <iostream>
#include <unordered_map>
#include <string>
#include <chrono>
#include <stdexcept>
using namespace std;
using namespace chrono;

// Node of the doubly linked list
struct Node {
    string key;
    int value;
    long long expiry;   
    Node* prev;
    Node* next;

    Node(string k, int v) {
        key = k;
        value = v;
        expiry = 0;
        prev = nullptr;
        next = nullptr;
    }
};

// current time in milliseconds
long long currentTimeMs() {
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

class LRUCache {
private:
    int capacity;
    unordered_map<string, Node*> mp;   // key -> node
    Node* head;                        // dummy node, most recently used side
    Node* tail;                        // dummy node, least recently used side

    // remove a node from the list
    void removeNode(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void addToFront(Node* node) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }

public:
    LRUCache(int cap) {
        if (cap <= 0) {
            throw invalid_argument("capacity must be positive");
        }
        capacity = cap;
        head = new Node("", -1);
        tail = new Node("", -1);
        head->next = tail;
        tail->prev = head;
    }

    ~LRUCache() {
        Node* cur = head;
        while (cur != nullptr) {
            Node* nextNode = cur->next;
            delete cur;
            cur = nextNode;
        }
    }

    int get(string key) {
        // key not present
        if (mp.find(key) == mp.end()) {
            return -1;
        }

        Node* node = mp[key];

        // check if the key has expired (only if it has a ttl)
        if (node->expiry != 0 && currentTimeMs() > node->expiry) {
            removeNode(node);
            mp.erase(key);
            delete node;
            return -1;
        }

        removeNode(node);
        addToFront(node);
        return node->value;
    }

    // ttl is in milliseconds, 0 means no expiry
    void put(string key, int value, long long ttl = 0) {
        // key already exists -> update it
        if (mp.find(key) != mp.end()) {
            Node* node = mp[key];
            node->value = value;
            node->expiry = (ttl > 0) ? currentTimeMs() + ttl : 0;
            removeNode(node);
            addToFront(node);
            return;
        }


        if ((int)mp.size() == capacity) {
            Node* lru = tail->prev;
            cout << "cache is full, removing " << lru->key << endl;
            mp.erase(lru->key);
            removeNode(lru);
            delete lru;
        }

        Node* newNode = new Node(key, value);
        if (ttl > 0) {
            newNode->expiry = currentTimeMs() + ttl;
        }
        addToFront(newNode);
        mp[key] = newNode;
    }
};

int main() {
    cout << "Test 1 - example from the task" << endl;
    LRUCache cache(2);

    cout << "put(A, 10)" << endl;
    cache.put("A", 10);
    cout << "put(B, 20)" << endl;
    cache.put("B", 20);

    cout << "get(A) = " << cache.get("A") << endl;

    cout << "put(C, 30)" << endl;
    cache.put("C", 30);

    cout << "get(B) = " << cache.get("B") << endl;
    cout << "get(C) = " << cache.get("C") << endl;
    cout << "get(A) = " << cache.get("A") << endl;

    cout << endl << "Test 2 - updating a key" << endl;
    LRUCache cache2(2);
    cout << "put(X, 1)" << endl;
    cache2.put("X", 1);
    cout << "put(Y, 2)" << endl;
    cache2.put("Y", 2);
    cout << "put(X, 100)" << endl;
    cache2.put("X", 100);
    cout << "put(Z, 3)" << endl;
    cache2.put("Z", 3);

    cout << "get(Y) = " << cache2.get("Y") << endl;
    cout << "get(X) = " << cache2.get("X") << endl;
    cout << "get(Z) = " << cache2.get("Z") << endl;

    cout << endl << "Test 3 - TTL (bonus)" << endl;
    LRUCache cache3(2);
    cout << "put(T, 99) with ttl 1000 ms" << endl;
    cache3.put("T", 99, 1000);
    cout << "get(T) = " << cache3.get("T") << endl;

    cout << "waiting 1500 ms..." << endl;
    long long start = currentTimeMs();
    while (currentTimeMs() - start < 1500) {

    }
    cout << "get(T) = " << cache3.get("T") << endl;

    return 0;
}