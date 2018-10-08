#include <iostream>

#define DEGREE 3

using namespace std;
/**
 * degree = min no of child ptrs per node
 * Min no of keys per node = degree - 1
 * Max no of keys per node = 2 * degree - 1
 *
 * Max no of child ptrs = 2 * degree
 */
struct BTreeNode {
    int keys[2 * DEGREE - 1];  // array of keys
    struct BTreeNode *children[2 * DEGREE];   // array of child pointers

    int degree = DEGREE;  // min degree
    int num_filled = 0;  // no of keys currently stored
    bool isleaf = true;
};

void remove(struct BTreeNode *node, int key);

void remove_helper(struct BTreeNode *node, int index);

void remove_from_leaf(struct BTreeNode *node, int index);

void remove_from_nonleaf(struct BTreeNode *node, int index);

struct BTreeNode *make_node(bool isleaf) {
    auto *node = (struct BTreeNode *) malloc(sizeof(BTreeNode));
    node->isleaf = isleaf;
    node->degree = DEGREE;
    return node;
}

void split_child_node(struct BTreeNode *parent, struct BTreeNode *child, int index) {
    // child will be split
    // for that we create a new node to store the right half(degree-1) keys of the original node(completely filled child node)
    // the original node(child) will contain left half(degree-1) keys and 1 key will move to parent node
    BTreeNode *temp = make_node(child->isleaf);
    int deg = child->degree;
    temp->num_filled = deg - 1;

    // copy the right half keys to temp
    for (int i = 0; i < temp->num_filled; i++) {
        temp->keys[i] = child->keys[i + deg];
    }

    if (!child->isleaf) {
        // copy right half children pointers of child node to temp. No. of child ptrs to copy = degree
        //       ______________________
        //      | 40 |50 | 60| 70 | 80 |
        //      /    \   \   /    \    \

        for (int i = 0; i < deg; i++) {
            temp->children[i] = child->children[i + deg];
        }
    }

    child->num_filled = deg - 1;

    // create space for new child node to be attached to parent move child ptrs in parent
    for (int i = parent->num_filled; i >= index + 1; i--) {
        parent->children[i + 1] = parent->children[i];
    }

    // link this new child node to parent
    parent->children[index + 1] = temp;

    // make space for the middle key of child which is to be moved to parent
    for (int i = parent->num_filled - 1; i >= index; i--)
        parent->keys[i + 1] = parent->keys[i];

    // copy the middle key of child to parent
    parent->keys[index] = child->keys[deg - 1]; // position of middle element in child is deg-1

    parent->num_filled += 1;    // increase number of filled keys in parent
}


/**
 * insert key into a NON FULL node.
 * @param node
 * @param key
 */
void insert_helper(struct BTreeNode *node, int key) {

    // if it's a leaf node insert at right position
    if (node->isleaf) {
        int index = node->num_filled - 1;   // index == right most element
        while (index >= 0 and node->keys[index] > key) {
            node->keys[index + 1] = node->keys[index];      // move elements to the right
            index -= 1;
        }
        // insert the key at found location
        node->keys[index + 1] = key;
        node->num_filled += 1;
    }

        // else recursively find the correct node where we should insert
    else {
        // find the child which should have this new key
        int i = node->num_filled - 1;   // i == right most element
        while (i >= 0 and node->keys[i] > key) {    // TODO: refactor: modify to go from 0 to n and not the other way
            i -= 1;
        }

        // if found child is full
        if (node->children[i + 1]->num_filled == 2 * node->degree - 1) {
            split_child_node(node, node->children[i + 1], i + 1);

            // after split, child node has been split into two children
            // and middle element of child has moved to this node
            // now find which child should contain this key
            if (node->keys[i + 1] < key)        // TODO: understand this
                i += 1;
        }

        // found the correct node where we should insert
        // recursively add to it
        insert_helper(node->children[i + 1], key);
    }
}

void print(struct BTreeNode *root) {
    int i;
    for (i = 0; i < root->num_filled; i++) {
        if (!root->isleaf)
            print(root->children[i]);

        cout << " " << root->keys[i];
    }

    // FIXME: last child was not getting traversed
    if (!root->isleaf)
        print(root->children[i]);
}


struct BTreeNode *insert(struct BTreeNode *root, int key) {
    // inserting into an empty tree
    if (root == nullptr) {
        root = make_node(true);
        root->keys[0] = key;
        root->num_filled = 1;
        return root;
    }

    // if root node is full
    if (root->num_filled == 2 * root->degree - 1) {
        struct BTreeNode *oldroot = root;
        // get a new root node
        root = make_node(false);
        // make oldroot a child of new root
        root->children[0] = oldroot;

        // split oldroot (bcoz it was full)
        split_child_node(root, oldroot, 0);

        // new root has two children now. Insert key in correct child
        if (root->keys[0] > key) {
            // insert into left child
            insert_helper(root->children[0], key);
        } else {
            // go right
            insert_helper(root->children[1], key);
        }
    } else {
        // root is not full
        insert_helper(root, key);
    };

    return root;
}

// Function that returns the index of the first key that is
// greater than or equal to k in this node
int findKey(struct BTreeNode *node, int key) {
    int i = 0;
    while (i < node->num_filled and node->keys[i] < key)
        i += 1;
    return i;
}

int get_predecessor(struct BTreeNode *node, int index) {
    struct BTreeNode *ptr = node->children[index];  // start in left child node

    // Keep moving to the right most node until we reach a leaf
    while (!ptr->isleaf) {
        ptr = ptr->children[ptr->num_filled];   // right most key = num_filled-1; right mode child ptr = num_filled
    }

    // return the last key of the leaf
    return ptr->keys[ptr->num_filled - 1];    // right most key = num_filled-1;
}

// index= index of the key whose successor is needed to be found
int get_successor(struct BTreeNode *node, int index) {
    struct BTreeNode *ptr = node->children[index + 1];    // start in right child node

    // Keep moving the left most node starting from children[index+1] until we reach a leaf
    while (!ptr->isleaf) {
        ptr = ptr->children[0];
    }

    // return the first key of the leaf
    return ptr->keys[0];
}

// merges child[index] and child[index+1]
// also frees child[index+1]
void merge(struct BTreeNode *parent, int index) {
    struct BTreeNode *leftchild = parent->children[index];
    struct BTreeNode *rightchild = parent->children[index + 1];

    // Insert the key from the parent node to left child node at (degree-1)th position of Child
    leftchild->keys[DEGREE - 1] = parent->keys[index];

    // shift all keys after index in the parent node one step left
    // to overwrite keys[index] element which was moved to leftchild
    for (int i = index + 1; i < parent->num_filled; ++i)
        parent->keys[i - 1] = parent->keys[i];

    // similarly shift all the child pointers after index+1 in the parent node one step left
    for (int i = index + 2; i <= parent->num_filled; ++i)
        parent->children[i - 1] = parent->children[i];

    parent->num_filled -= 1;    // one less key now

    // copy the keys from right child to left child
    for (int i = 0; i < rightchild->num_filled; i++) {
        leftchild->keys[i + DEGREE] = rightchild->keys[i];
    }

    // if leftchild is not a leaf node, copy the child pointers from right to left
    if (!leftchild->isleaf) {
        for (int i = 0; i <= rightchild->num_filled; ++i)
            leftchild->children[i + DEGREE] = rightchild->children[i];
    }

    leftchild->num_filled += rightchild->num_filled + 1;    // +1 for the element copied from parent

    // free space occupied by rightchild
    delete (rightchild);    // TODO: read diff between using free() and delete
}

// TODO: reafctor plag
// A function to borrow a key from C[idx-1] (sibling) and insert it
// into C[idx]  (child)
void borrow_from_prev(struct BTreeNode *node, int index) {
    struct BTreeNode *child = node->children[index];
    struct BTreeNode *sibling = node->children[index - 1];

    // The last key from C[idx-1] goes up to the parent
    // and key[idx-1] from parent is inserted as the first key in C[idx].
    // Thus, the sibling loses one key and child gains one key

    // Moving all key in C[idx] one step right to make space for incoming key
    for (int i = child->num_filled - 1; i >= 0; --i)
        child->keys[i + 1] = child->keys[i];

    // Setting child's first key equal to keys[idx-1] from the current node
    child->keys[0] = node->keys[index - 1];

    // If C[idx] is not a leaf, move all its child pointers one step ahead
    if (!child->isleaf) {
        for (int i = child->num_filled; i >= 0; --i)
            child->children[i + 1] = child->children[i];
    }

    // Moving sibling's last child as C[idx]'s first child
    if (!child->isleaf)
        child->children[0] = sibling->children[sibling->num_filled];


    // Moving the key from the sibling to the parent
    // This reduces the number of keys in the sibling
    node->keys[index - 1] = sibling->keys[sibling->num_filled - 1];

    child->num_filled += 1;
    sibling->num_filled -= 1;
}

// TODO: refactor plag
// A function to borrow a key from the C[idx+1] and place it in C[idx]
void borrow_from_next(struct BTreeNode *node, int index) {
    struct BTreeNode *child = node->children[index];
    struct BTreeNode *sibling = node->children[index + 1];

    // keys[idx] is inserted as the last key in C[idx]
    child->keys[child->num_filled] = node->keys[index];

    //The first key from sibling is inserted into keys[idx]
    node->keys[index] = sibling->keys[0];
    // Sibling's first child is inserted as the last child into C[idx]
    if (!(child->isleaf))
        child->children[child->num_filled + 1] = sibling->children[0];

    // Moving all keys in sibling one step left
    for (int i = 1; i < sibling->num_filled; ++i)
        sibling->keys[i - 1] = sibling->keys[i];
    // Moving the child pointers in sibling one step left
    if (!sibling->isleaf) {
        for (int i = 1; i <= sibling->num_filled; ++i)
            sibling->children[i - 1] = sibling->children[i];
    }

    child->num_filled += 1;
    sibling->num_filled -= 1;

}

// Fills the node.children[index] with more nodes
// bcoz it has less than DEG-1 keys
// adds atleast one key.
// (one when borrow happens)
// (more than one when merge happens)
void fill(struct BTreeNode *node, int index) {

    // If the previous child(C[idx-1]) has more than DEG-1 keys, borrow a key from that child
    if (index != 0 and node->children[index - 1]->num_filled >= DEGREE)
        borrow_from_prev(node, index);

        // else if next child(C[idx+1]) has more than DEG-1 keys, borrow from that child
    else if (index != node->num_filled and node->children[index + 1]->num_filled >= DEGREE) {
        borrow_from_next(node, index);
    }

        // else merge child[idx] with one of its siblings
    else {
        // If C[idx] is the last child, merge it with with its previous sibling
        if (index == node->num_filled)
            merge(node, index - 1);

            // else merge it with its next sibling
        else
            merge(node, index);
    }
}

void remove(struct BTreeNode *node, int key) {
    int index = findKey(node, key);

    // if the key to be removed is present in this node
    if (index < node->num_filled and node->keys[index] == key) {
        remove_helper(node, index);
    }
        // else key is not present in this node
        // maybe it's in one of the children
    else {
        if (node->isleaf) {
            cout << "Key: " << key << " not found." << endl;
            return;
        }


        // The key to be removed is present in the sub-tree rooted with this node
        // The islastchild flag indicates whether the key is present in the sub-tree rooted
        // with the last child of this node
        bool islastchild = index == node->num_filled;

        // if the child where the key is probably present has minimum number of keys i.e., less than DEGREE keys
        // we fill more keys into that child because after removing a key from it, it will underflow.
        if (node->children[index]->num_filled < DEGREE) {
            fill(node, index);
        }


        if (islastchild and index > node->num_filled)
            // If the last child has been merged, it must have merged with the previous child    TODO:understand
            // and so we recurse on the (idx-1)th child.
            remove(node->children[index - 1], key);
        else
            // Else, we recurse on the
            // (idx)th child which now has atleast t keys
            remove(node->children[index], key);
    }
}

void remove_from_leaf(struct BTreeNode *node, int index) {
    // shift all keys after the index to be deleted one place left
    // so as to overwrite the key to be deleted
    for (int i = index + 1; i < node->num_filled; i++) {
        node->keys[i - 1] = node->keys[i];
    }

    node->num_filled -= 1;
}

void remove_from_nonleaf(struct BTreeNode *node, int index) {
    int key_to_delete = node->keys[index];

    // If the child that precedes key to be delete (node.children[index])has atleast `degree` no. of keys
    if (node->children[index]->num_filled >= DEGREE) {
        // find the predecessor of key at index
        int pred = get_predecessor(node, index);
        // Replace key by predecessor.
        node->keys[index] = pred;
        // Recursively delete predecessor in the node children[index]
        remove(node->children[index], pred);
    }

        // else if child at index has less than `degree` keys
        // check if child at index+1 has atleast degree keys
    else if (node->children[index + 1]->num_filled >= DEGREE) {
        // find the successor of key at index
        int successor = get_successor(node, index);
        // Replace key by its successor.
        node->keys[index] = successor;
        // Recursively delete successor in the node children[index+1]
        remove(node->children[index + 1], successor);
    }

        // else both children[index] and children[index+1] have less than degree keys
    else {
        // merge key(from parent node) and all elements of children[index+1](right node) into children[index](left)
        merge(node, index);
        // now children[index] contains 2t-1 keys ((t-1) keys from left + (1)(key) + (t-1) keys from right)
        // free the node children[index+1] <= handled by merge() function


        // recursively delete key from children[index]
        remove(node->children[index], key_to_delete);
    }
}

void remove_helper(struct BTreeNode *node, int index) {
    if (node->isleaf) {
        remove_from_leaf(node, index);
    } else {
        remove_from_nonleaf(node, index);
    }
}

bool search(struct BTreeNode *root, int key) {
    if (root == nullptr)
        return false;

    int i = 0;
    while (i < root->num_filled and root->keys[i] < key) {      // find first element greater than or equal to key
        i += 1;
    }

    if (root->keys[i] == key)
        return true;

    if (root->isleaf)    // key is not found and it's a leaf node, i.e., we have nowhere to search further
        return false;

    // else go to child
    return search(root->children[i], key);
}

int main() {
    int Q, operation, operand;
    struct BTreeNode *root = nullptr;
    cin >> Q;
    while (Q--) {
        cin >> operation >> operand;

        switch (operation) {
            case 1:
                root = insert(root, operand);
                cout << "Tree content after inserting: " << operand << endl;
                print(root);
                cout << endl;

                break;
            case 2:
                if (search(root, operand))
                    cout << "Key: " << operand << " found." << endl;
                else
                    cout << "Key: " << operand << " NOT found." << endl;

                break;
            case 3:
                remove(root, operand);
                cout << "Tree content after removing: " << operand << endl;
                print(root);
                cout << endl;

                break;
            default:
                cout << "Invalid operation." << endl;
                cout << "Current tree content:- " << endl;
                print(root);
                cout << endl;
        }
    }
    return 0;
}
