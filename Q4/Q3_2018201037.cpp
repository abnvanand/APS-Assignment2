#include <iostream>

#define THRESHOLD (0.70)

using namespace std;

template<typename K, typename V>
class node {
public:
    K key;
    V value;
    node<K, V> *next;

    node(K key, V value) {
        this->key = key;
        this->value = value;
    }

    ~node() {
        // first delete the pointed node // leads to a chain to delete the entire ll
        delete next;
    }
};

template<typename K, typename V>
class HashMap {
private:
    int n;   // Number of elements currently stored
    int m;   // Number of buckets
    node<K, V> **buckets;

    int hashcode(int key) {
        return key % m;
    }

    int hashcode(string key) {
        const int PRIME = 43;


        int res = 0;
        int pow = 1;

        // hash calculation formula:-
        // eg str = Apple
        // hash = A*PRIME^0 + p* PRIME^1 + p*PRIME^2 +...e*PRIME^4
        for (char ch : key) {
            res += ch * pow;
            pow = pow * PRIME;

            pow %= m;  // prevent integer overflow
            res %= m;  // prevent integer overflow
        }

        return res;
    }

    void resize() {
        node<K, V> **oldbuckets = buckets;
        int num_old_buckets = m;

        // TODO: set m to next closest prime
        m = (m << 1) | 1;  // make almost twice as large; odd number is more likely to be a prime
        n = 0;

        buckets = new node<K, V> *[m];
        for (int i = 0; i < m; i++)
            buckets[i] = nullptr;


        // Insert old data into new table
        for (int i = 0; i < num_old_buckets; i++) {
            auto temp = oldbuckets[i];

            if (temp != nullptr) {
                // ll exists in current bucket
                while (temp != nullptr) {
                    insert(temp->key, temp->value);
                    temp = temp->next;
                }

                delete oldbuckets[i];   // calls the destructor of node
            }
        }

        delete[] oldbuckets;   // delete the old buckets array
    }

public:
    HashMap(int bucket_count = 11) {
        this->n = 0;
        this->m = bucket_count;

        this->buckets = new node<K, V> *[this->m];

        for (int i = 0; i < m; i++)
            this->buckets[i] = nullptr;   // cleanup and garbage references
    }


    void insert(K key, V value) {
        int i = hashcode(key);
        auto *temp = new node<K, V>(key, value);

        temp->next = this->buckets[i];    // set to the head of ll that the bucket points to.
        this->buckets[i] = temp;          // temp is now the new head of ll

        this->n += 1;    // increase the number of elements in our hashmap

        float load_factor = (float) this->n / this->m;

        // check to see if we need rehashing
        if (load_factor > THRESHOLD) {
            resize();
        }
    }

    // TODO: remove this function before submission
    // Print it
    void print() {
        ///Iterate over buckets array
        for (int i = 0; i < m; i++) {
            ///Print the LL for each buckets
            node<K, V> *temp = buckets[i];
            cout << "Bucket " << i << "->";
            while (temp != nullptr) {
                cout << temp->key << ",";
                temp = temp->next;
            }
            cout << endl;
        }
    }

    V *find(K key) {
        int idx = hashcode(key);    // idx = index of bucket possibly containing the key

        node<K, V> *ptr = buckets[idx];
        while (ptr != nullptr) {
            if (ptr->key == key)
                return &(ptr->value);

            ptr = ptr->next;
        }

        return nullptr;
    }

    bool erase(K key) {
        int idx = hashcode(key);

        node<K, V> *prev = nullptr;
        node<K, V> *ptr = buckets[idx];

        while (ptr != nullptr) {
            if (ptr->key == key)
                break;

            prev = ptr;
            ptr = ptr->next;
        }

        if (ptr == nullptr) {
            // key not found
            return false;
        }

        // else found the key to be deleted

        if (prev == nullptr) {
            // node to be deleted is at the head of the ll of the bucket
            buckets[idx] = ptr->next;
        } else {
            prev->next = ptr->next;
        }

        // can't use delete here
        // bcoz it will call the destructor which will delete entire ll starting from ptr
        free(ptr);

        n -= 1;
        return true;
    }

};

int main() {
    HashMap<string, string> strStrMap;
    strStrMap.insert("Abhinav", "Anand");
    strStrMap.insert("Anurag", "Anand");
    strStrMap.insert("Lokesh", "Singh");
    strStrMap.insert("Neeraj", "Dhiman");
    strStrMap.insert("Ankit", "Popli");
    strStrMap.insert("Amogh", "Banta");
//    strStrMap.insert("Nishant", "Goyal");
//    strStrMap.insert("Suchi", "IDK?");

    strStrMap.print();

    strStrMap.erase("Ankit");

    strStrMap.print();

    HashMap<int, int> int_int_map;
    int_int_map.insert(11, 100);
    int_int_map.insert(22, 527);
    int_int_map.insert(33, 127);
    int_int_map.insert(44, 99);
    int_int_map.insert(55, 4);
    int_int_map.insert(66, 44);
//    int_int_map.insert(77, 55);
//    int_int_map.insert(88, 55);
//    int_int_map.insert(99, 55);

    int_int_map.print();
    auto it = int_int_map.find(22);
    if (it != nullptr)
        cout << *it << endl;
}
