#include <iostream>
#include <random>

using namespace std;

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int get_rand_in_range(int low, int high) {
    // better random generator
    // refer: http://www.cplusplus.com/reference/random/mersenne_twister_engine/
    // TODO: refactor to seed the generator only once
    random_device rd; // obtain a random number from hardware
    mt19937 mt_engine(rd()); // seed the generator

    uniform_int_distribution<> distro(low, high); // define the range
    return distro(mt_engine);
//    return  (rand() % (high - low + 1)) + low; // not truly random
}

int partition(int arr[], int low, int high) {
    int rand_idx = get_rand_in_range(low, high);
    swap(&arr[rand_idx], &arr[low]);

    int pivot = arr[low];
    int i = low, j = low + 1;

    while (j <= high) {
        if (arr[j] < pivot) {
            i += 1;
            swap(&arr[i], &arr[j]);
        }
        j += 1;
    }

    swap(&arr[low], &arr[i]);
    return i;
}

int nth_smallest(int arr[], int low, int high, int k) {
    while (low <= high) {
        int piv_idx = partition(arr, low, high);

        if (piv_idx == k - 1) {
            return arr[piv_idx];
        } else if (piv_idx > k - 1) {
            // find in left half
            high = piv_idx - 1;
        } else {
            low = piv_idx + 1;
        }
    }

    return -1;
}

int main() {
    int N, Q;
    cin >> N >> Q;
    int arr[N];
    for (int i = 0; i < N; i++) {
        cin >> arr[i];
    }
    while (Q--) {
        int k;
        cin >> k;
        if (k == 0 or k > N) {
            cout << "Out of valid range\n";
        } else {
            cout << k << "th smallest number in arr is " << nth_smallest(arr, 0, N - 1, k) << endl;
        }
    }
}
