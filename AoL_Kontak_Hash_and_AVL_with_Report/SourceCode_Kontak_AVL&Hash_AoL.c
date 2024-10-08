//Jerry - 2602149533= Menu & File Processing	
//Calvin - 2602085583 = Insert , Delete & Display	
//Marcell - 2602084104 = Update & Search



*/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PHONE 22
#define MAX_NAME 32
#define TABLE_SIZE 64  // 2^n

typedef struct Node {
    int height;
    char phone[MAX_PHONE];  // max 20; primary key
    char name[MAX_NAME];    // max 30
    struct Node *left, *right;
} Node;

typedef struct Hashmap {
    Node *root;
} Hashmap;

static const Node EmptyStruct = {0};

Hashmap table[TABLE_SIZE] = {0};

int getKey(Node temp) {
    int key = 0;
    for (int i = 0; temp.phone[i] != '\0'; i++) {
        key += (int)temp.phone[i];
    }
    return key;
}

// return the number of bits in a number
 int countBits(int number)
 {
     int i = 0;
     while (number > 0)
     {
         i++;
         number >>= 1;
     }
     return i;
 }

// return the number of bits in a number
//int countBits(int number) {
//    if (number == 0) {
//        return 0;
//    }
//    return (int)log2(number) + 1;
//}

int middlesquare(int key) {
    // Get the square of the key
    long long int square = key * key;
    // Get how many shifts need to be done
    int shift = (countBits(square) - countBits(key)) / 2;
    // The mask used to only get the number inside the TABLE_SIZE range
    int mask = TABLE_SIZE - 1;
    // Shift the key and mask the key
    square = (square >> shift) & mask;
    return square;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int getHeight(Node *current) {
    if (current != NULL) {
        return current->height;
    }
    return 0;
}

int getBalance(Node *current) {
    if (current == NULL) {
        return 0;
    }
    return getHeight(current->left) - getHeight(current->right);
}

// Left rotation
Node *leftRotation(Node *current) {
    // right child of parrent node
    Node *T1 = current->right;
    // Left child of child node
    Node *T2 = T1->left;

    // Rotation
    T1->left = current;
    current->right = T2;

    // Update height
    current->height = 1 + max(getHeight(current->left), getHeight(current->right));
    T1->height = 1 + max(getHeight(T1->left), getHeight(T1->right));

    return T1;
}

// Right rotation
Node *rightRotation(Node *current) {
    // Left child of parrent node
    Node *T1 = current->left;
    // Right child of child node
    Node *T2 = T1->right;

    // Rotation
    T1->right = current;
    current->left = T2;

    // Update height
    current->height = 1 + max(getHeight(current->left), getHeight(current->right));
    T1->height = 1 + max(getHeight(T1->left), getHeight(T1->right));

    return T1;
}

// Create new node
Node *createNode(Node temp) {
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->height = 1;
    strcpy(new_node->phone, temp.phone);
    strcpy(new_node->name, temp.name);
    new_node->left = NULL;
    new_node->right = NULL;
    return new_node;
}

void writeNodeToFile(FILE *file, Node *current) {
    if (current != NULL) {
        writeNodeToFile(file, current->left);
        fprintf(file, "%s,%s\n", current->phone, current->name);
        writeNodeToFile(file, current->right);
    }
}

void writeToFile(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Failed to open the file.\n");
        return;
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        if (table[i].root != NULL) {
            writeNodeToFile(file, table[i].root);
        }
    }

    fclose(file);
}

// Push into AVL Tree
Node *push(Node *current, Node temp) {
    int cmp = 0;
    if (current != NULL) {
        cmp = strcmp(current->phone, temp.phone);
    }
    if (current == NULL) {
        return createNode(temp);
    } else if (cmp < 0) {
        current->left = push(current->left, temp);
    } else if (cmp > 0) {
        current->right = push(current->right, temp);
    }
    // if duplicate key exist, return the current node
    else {
        return current;
    }
    // Update the current node height
    current->height = 1 + max(getHeight(current->left), getHeight(current->right));
    int balance = getBalance(current);
    // Check if there a violation
    int cmpl = 0, cmpr = 0;
    if (current->left != NULL) {
        cmpl = strcmp(current->left->phone, temp.phone);
    }
    if (current->right != NULL) {
        cmpr = strcmp(current->right->phone, temp.phone);
    }

    // Left Left case
    if (balance > 1 && cmpl < 0) {
        return rightRotation(current);
    }
    // Right Right case
    if (balance < -1 && cmpr > 0) {
        return leftRotation(current);
    }
    // Left Right case
    if (balance > 1 && cmpl > 0) {
        current->left = leftRotation(current->left);
        return rightRotation(current);
    }
    // Right Left case
    if (balance < -1 && cmpr < 0) {
        current->right = rightRotation(current->right);
        return leftRotation(current);
    }

    // No modification
    return current;
}

// Insert data to the table
void insertData(Node temp) {
    int hashKey = middlesquare(getKey(temp));
    // if the table is empty, create node at the root
    if (table[hashKey].root == NULL) {
        table[hashKey].root = createNode(temp);
    }
    // if the table is occupied, use avl to deal with the collision
    else {
        table[hashKey].root = push(table[hashKey].root, temp);
    }
}

void readFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        // printf("Failed to open the file.\n");
        fclose(file);
        file = fopen(filename, "w");
        fclose(file);
        return;
    }
    Node temp = {0};
    while (fscanf(file, "%[^,],%[^\n]\n", temp.phone, temp.name) == 2) {
        insertData(temp);
    }
    fclose(file);
}

Node *search(Node *current, Node temp) {
    int cmp = 0;
    if (current != NULL) {
        cmp = strcmp(current->phone, temp.phone);
    }
    if (current == NULL) {
        return NULL;
    } else if (cmp == 0) {
        return current;
    } else if (cmp < 0) {
        search(current->left, temp);
    } else if (cmp > 0) {
        search(current->right, temp);
    }
}

Node *minValueNode(Node *node) {
    Node *current = node;
    while (current->left != NULL) {
        current = current->left;
    }
    return current;
}

Node *pop(Node *current, Node temp) {
    int cmp = 0;
    if (current != NULL) {
        cmp = strcmp(current->phone, temp.phone);
    }
    if (current == NULL) {
        return current;
    } else if (cmp < 0) {
        current->left = pop(current->left, temp);
    } else if (cmp > 0) {
        current->right = pop(current->right, temp);
    } else {
        // check left and right child
        if (current->right == NULL || current->left == NULL) {
            Node *temp = (current->left) ? current->left : current->right;
            // leaf
            if (temp == NULL) {
                temp = current;
                current = NULL;
            }
            // 1 child
            else {
                *current = *temp;
            }
            free(temp);
        } else {
            Node *delete = minValueNode(current->right);
            Node temps = *current;
            *current = *delete;
            current->left = temps.left;
            current->right = temps.right;
            current->right = pop(current->right, *delete);
        }
    }
    // If the tree had only one node then return
    if (current == NULL) {
        return current;
    }
    // Update tree
    current->height = 1 + max(getHeight(current->left), getHeight(current->right));
    int balance = getBalance(current);

    // Check if there a violation
    int cmpl = 0, cmpr = 0;
    if (current->left != NULL) {
        cmpl = strcmp(current->left->phone, temp.phone);
    }
    if (current->right != NULL) {
        cmpr = strcmp(current->right->phone, temp.phone);
    }

    // Left Left case
    if (balance > 1 && cmpl < 0) {
        return rightRotation(current);
    }
    // Right Right case
    if (balance < -1 && cmpr > 0) {
        return leftRotation(current);
    }
    // Left Right case
    if (balance > 1 && cmpl > 0) {
        current->left = leftRotation(current->left);
        return rightRotation(current);
    }
    // Right Left case
    if (balance < -1 && cmpr < 0) {
        current->right = rightRotation(current->right);
        return leftRotation(current);
    }

    // No modification
    return current;
}

void updateData(Node updated, Node old) {
    // Remove old data
    int hashKey = middlesquare(getKey(old));
    table[hashKey].root = pop(table[hashKey].root, old);
    // Insert new data
    insertData(updated);
}

// print the data inorder
void inorderDisplay(Node *current) {
    if (current != NULL) {
        inorderDisplay(current->left);
        printf("| %-30s | %-20s |\n", current->name, current->phone);
        inorderDisplay(current->right);
    }
}

// display the data from the table
void display() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (table[i].root != NULL) {
            inorderDisplay(table[i].root);
        }
    }
}

int main() {
    readFromFile("output.txt");
    int menu;
    Node temp;
    do {
        temp = EmptyStruct;
        int namelen = 0, phonelen = 0, hashKey = 0;
        printf("****************************************\n");
        printf("*          Contact List                *\n");
        printf("****************************************\n");
        printf("* 1. Insert                            *\n");
        printf("* 2. View All                          *\n");
        printf("* 3. Search                            *\n");
        printf("* 4. Update                            *\n");
        printf("* 5. Delete                            *\n");
        printf("* 6. Exit                              *\n");
        printf("****************************************\n");
        printf(">> ");
        scanf("%d", &menu);
        getchar();
        switch (menu) {
            case 1:
                // Check input
                do {
                    printf("Input Name [string|max:30]: ");
                    fgets(temp.name, MAX_NAME, stdin);
                    fflush(stdin);
                    temp.name[MAX_NAME - 1] = '\0';
                    temp.name[strcspn(temp.name, "\n")] = '\0';
                    namelen = strlen(temp.name);
                } while (namelen > 30 || namelen < 0);
                do {
                    printf("Input Phone Number [string|max:20]: ");
                    fgets(temp.phone, MAX_PHONE, stdin);
                    fflush(stdin);
                    temp.phone[MAX_PHONE - 1] = '\0';
                    temp.phone[strcspn(temp.phone, "\n")] = '\0';
                    phonelen = strlen(temp.phone);
                } while (phonelen > 20 || phonelen < 0);
                insertData(temp);
                printf("Successfully added data!\n");
                break;
            case 2:
                printf("---------------------------------------------------------\n");
                printf("| %-30s | %-20s |\n", "Name", "Phone Number");
                printf("---------------------------------------------------------\n");
                display();
                printf("---------------------------------------------------------\n");
                break;
            case 3:
                do {
                    printf("Input Phone Number [string|max:20]: ");
                    fgets(temp.phone, MAX_PHONE, stdin);
                    fflush(stdin);
                    temp.phone[MAX_PHONE - 1] = '\0';
                    temp.phone[strcspn(temp.phone, "\n")] = '\0';
                    phonelen = strlen(temp.phone);
                } while (phonelen > 20 || phonelen < 0);
                hashKey = middlesquare(getKey(temp));
                if (table[hashKey].root != NULL) {
                    Node *src = search(table[hashKey].root, temp);
                    if (src == NULL) {
                        printf("Data not in the table");
                    } else {
                        printf("Name: %s\n", src->name);
                        printf("Phone Number: %s\n", src->phone);
                    }
                } else {
                    printf("Data not in the table");
                }
                break;
            case 4:
                // Phone number to be updated
                do {
                    printf("Input Phone Number [string|max:20]: ");
                    fgets(temp.phone, MAX_PHONE, stdin);
                    fflush(stdin);
                    temp.phone[MAX_PHONE - 1] = '\0';
                    temp.phone[strcspn(temp.phone, "\n")] = '\0';
                    phonelen = strlen(temp.phone);
                } while (phonelen > 20 || phonelen < 0);
                hashKey = middlesquare(getKey(temp));
                if (table[hashKey].root == NULL) {
                    printf("Phone number not found!\n");
                    break;
                }
                Node *src = search(table[hashKey].root, temp);
                if (src == NULL) {
                    printf("Phone number not found!\n");
                    break;
                }
                // Copy src to temp
                temp = *src;
                // What to update
                int updateMenu;
                do {
                    printf("Update: \n");
                    printf("1. Name [Current: %s]\n", temp.name);
                    printf("2. Phone Number [Current: %s]\n", temp.phone);
                    printf("3. Save Change(s)\n");
                    do {
                        printf(">>");
                        scanf("%d", &updateMenu);
                        getchar();
                    } while (updateMenu > 3 || updateMenu < 0);
                    // Update name
                    if (updateMenu == 1) {
                        do {
                            printf("Input Name [string|max:30]: ");
                            fgets(temp.name, MAX_NAME, stdin);
                            fflush(stdin);
                            temp.name[MAX_NAME - 1] = '\0';
                            temp.name[strcspn(temp.name, "\n")] = '\0';
                            namelen = strlen(temp.name);
                        } while (namelen > 30 || namelen < 0);
                    }
                    // Update phone number
                    else if (updateMenu == 2) {
                        do {
                            printf("Input Phone Number [string|max:20]: ");
                            fgets(temp.phone, MAX_PHONE, stdin);
                            fflush(stdin);
                            temp.phone[MAX_PHONE - 1] = '\0';
                            temp.phone[strcspn(temp.phone, "\n")] = '\0';
                            phonelen = strlen(temp.phone);
                        } while (phonelen > 20 || phonelen < 0);
                    }
                    // Save Changes
                    else {
                        // Check if anything change
                        if (strcmp(temp.name, src->name) == 0 && strcmp(temp.phone, src->phone) == 0) {
                            printf("No change(s) detected\n");
                        } else {
                            updateData(temp, *src);
                        }
                    }
                } while (updateMenu != 3);
                break;
            case 5:
                do {
                    printf("Input Phone Number [string|max:20]: ");
                    fgets(temp.phone, MAX_PHONE, stdin);
                    fflush(stdin);
                    temp.phone[MAX_PHONE - 1] = '\0';
                    temp.phone[strcspn(temp.phone, "\n")] = '\0';
                    phonelen = strlen(temp.phone);
                } while (phonelen > 20 || phonelen < 0);
                hashKey = middlesquare(getKey(temp));
                if (table[hashKey].root == NULL) {
                    printf("Phone number not found!\n");
                    break;
                } else {
                    Node *src = search(table[hashKey].root, temp);
                    if (src == NULL) {
                        printf("Phone number not found!\n");
                        break;
                    }
                    table[hashKey].root = pop(table[hashKey].root, temp);
                }
                break;
            case 6:
                break;
            default:
                printf("Invalid Input\n");
                break;
        }
    } while (menu != 6);
    writeToFile("output.txt");
    return 0;
}

/*
Test Case

Samantha Jones
+1 (555) 123-4567

Ethan Sullivan
+1 (555) 987-6543

Olivia Martinez
+1 (555) 246-8135

Benjamin Thompson
+1 (555) 369-2584

Ava Johnson
+1 (555) 741-8520
*/
