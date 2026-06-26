#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// ==========================================
// CORE DATA STRUCTURES & ENTITIES
// ==========================================

// Central entity representing a Book
struct Book {
    int id;
    string name;
    string author;
    string category;
    int quantity;
    int available;
};

// Node for the Singly Linked List (Primary Storage)
struct ListNode {
    Book book;          
    ListNode* next;
    
    ListNode(Book b) : book(b), next(nullptr) {}
};

// Node for the Binary Search Tree (Search Index)
struct BSTNode {
    Book* bookPtr; // Points to the book in the Linked List to avoid data duplication
    BSTNode* left;
    BSTNode* right;
    
    BSTNode(Book* b) : bookPtr(b), left(nullptr), right(nullptr) {}
};

// Node for the Queue (Issue Requests)
struct QueueNode {
    int requestedBookId;
    string studentName;
    QueueNode* next;
    
    QueueNode(int id, string name) : requestedBookId(id), studentName(name), next(nullptr) {}
};

// Enum and Node for the Stack (Undo Feature)
enum ActionType { UNDO_DELETE, UNDO_ISSUE };

struct StackNode {
    ActionType action;
    Book savedBook;    // Stores full book details if undoing a delete
    int issuedBookId;  // Stores just the ID if undoing an issue
    StackNode* next;
    
    // Constructor for Delete Undo
    StackNode(Book b) : action(UNDO_DELETE), savedBook(b), issuedBookId(-1), next(nullptr) {}
    
    // Constructor for Issue Undo
    StackNode(int id) : action(UNDO_ISSUE), issuedBookId(id), next(nullptr) {}
};

// ==========================================
// CUSTOM DATA STRUCTURE CLASSES
// ==========================================

class IssueQueue {
private:
    QueueNode* front;
    QueueNode* rear;

public:
    IssueQueue() : front(nullptr), rear(nullptr) {}
    
    ~IssueQueue() {
        while (!isEmpty()) {
            dequeue();
        }
    }

    void enqueue(int bookId, const string& studentName) {
        QueueNode* newNode = new QueueNode(bookId, studentName);
        if (rear == nullptr) {
            front = rear = newNode;
            return;
        }
        rear->next = newNode;
        rear = newNode;
    }

    void dequeue() {
        if (isEmpty()) return;
        QueueNode* temp = front;
        front = front->next;
        if (front == nullptr) rear = nullptr;
        delete temp;
    }

    bool isEmpty() const {
        return front == nullptr;
    }

    void displayQueue() const {
        if (isEmpty()) {
            cout << "The issue queue is currently empty.\n";
            return;
        }
        QueueNode* temp = front;
        int position = 1;
        cout << left << setw(10) << "Pos" << setw(15) << "Book ID" << "Student Name\n";
        cout << string(45, '-') << "\n";
        while (temp != nullptr) {
            cout << left << setw(10) << position++ 
                 << setw(15) << temp->requestedBookId 
                 << temp->studentName << "\n";
            temp = temp->next;
        }
    }
};

class ActionStack {
private:
    StackNode* top;

public:
    ActionStack() : top(nullptr) {}
    
    ~ActionStack() {
        while (!isEmpty()) {
            pop();
        }
    }

    void pushDelete(Book b) {
        StackNode* newNode = new StackNode(b);
        newNode->next = top;
        top = newNode;
    }

    void pushIssue(int bookId) {
        StackNode* newNode = new StackNode(bookId);
        newNode->next = top;
        top = newNode;
    }

    StackNode pop() {
        if (isEmpty()) {
            throw runtime_error("Stack is empty");
        }
        StackNode* temp = top;
        StackNode poppedData = *temp; // Copy data
        top = top->next;
        delete temp;
        return poppedData;
    }

    bool isEmpty() const {
        return top == nullptr;
    }
};

// ==========================================
// MAIN LIBRARY MANAGEMENT SYSTEM
// ==========================================

class LibraryManager {
private:
    ListNode* head;      // Linked List for storing all books
    BSTNode* root;       // BST for fast ID searches
    IssueQueue queue;    // Queue for pending issues
    ActionStack stack;   // Stack for undo actions

    // Helper to insert into BST
    BSTNode* insertBST(BSTNode* node, Book* b) {
        if (node == nullptr) return new BSTNode(b);
        if (b->id < node->bookPtr->id)
            node->left = insertBST(node->left, b);
        else if (b->id > node->bookPtr->id)
            node->right = insertBST(node->right, b);
        return node;
    }

    // Helper to search BST
    BSTNode* searchBST(BSTNode* node, int id) {
        if (node == nullptr || node->bookPtr->id == id)
            return node;
        if (node->bookPtr->id < id)
            return searchBST(node->right, id);
        return searchBST(node->left, id);
    }

    // Helper to find minimum node in BST
    BSTNode* minValueNode(BSTNode* node) {
        BSTNode* current = node;
        while (current && current->left != nullptr)
            current = current->left;
        return current;
    }

    // Helper to remove from BST
    BSTNode* removeBST(BSTNode* node, int id) {
        if (node == nullptr) return node;

        if (id < node->bookPtr->id) {
            node->left = removeBST(node->left, id);
        } else if (id > node->bookPtr->id) {
            node->right = removeBST(node->right, id);
        } else {
            // Node with only one child or no child
            if (node->left == nullptr) {
                BSTNode* temp = node->right;
                delete node;
                return temp;
            } else if (node->right == nullptr) {
                BSTNode* temp = node->left;
                delete node;
                return temp;
            }
            // Node with two children: Get inorder successor
            BSTNode* temp = minValueNode(node->right);
            node->bookPtr = temp->bookPtr;
            node->right = removeBST(node->right, temp->bookPtr->id);
        }
        return node;
    }

    // Safely get an integer from user
    int getValidInt() {
        int x;
        while (!(cin >> x)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "  [!] Invalid input. Please enter a valid number: ";
        }
        cin.ignore(10000, '\n');
        return x;
    }

public:
    LibraryManager() : head(nullptr), root(nullptr) {}

    ~LibraryManager() {
        ListNode* current = head;
        while (current != nullptr) {
            ListNode* next = current->next;
            delete current;
            current = next;
        }
        // Freeing BST nodes (pointers to LL were already handled above, so we only delete BST structures)
        destroyBST(root);
    }

    void destroyBST(BSTNode* node) {
        if (node) {
            destroyBST(node->left);
            destroyBST(node->right);
            delete node;
        }
    }

    // 1. Add Book
    void addBook(int id, string name, string author, string category, int qty, int avail) {
        if (searchBST(root, id) != nullptr) {
            cout << "\n  [!] Error: Book with ID " << id << " already exists!\n";
            return;
        }

        Book newBook = {id, name, author, category, qty, avail};
        ListNode* newNode = new ListNode(newBook);
        
        // Insert into Linked List (Front insertion for O(1))
        newNode->next = head;
        head = newNode;

        // Insert reference into BST (O(log N))
        root = insertBST(root, &(head->book));

        cout << "\n  [+] Book '" << name << "' added successfully!\n";
    }

    void menuAddBook() {
        cout << "\n--- ADD NEW BOOK ---\n";
        cout << "Enter Book ID: "; int id = getValidInt();
        cout << "Enter Title: "; string title; getline(cin, title);
        cout << "Enter Author: "; string author; getline(cin, author);
        cout << "Enter Category: "; string category; getline(cin, category);
        cout << "Enter Quantity: "; int qty = getValidInt();
        
        addBook(id, title, author, category, qty, qty);
    }

    // 2. Display Books
    void displayBooks() {
        if (head == nullptr) {
            cout << "\n  [!] The library is currently empty.\n";
            return;
        }

        cout << "\n" << string(95, '=') << "\n";
        cout << left << setw(10) << "ID" 
             << setw(30) << "Title" 
             << setw(20) << "Author" 
             << setw(15) << "Category" 
             << setw(10) << "Total" 
             << setw(10) << "Available" << "\n";
        cout << string(95, '-') << "\n";

        ListNode* temp = head;
        while (temp != nullptr) {
            cout << left << setw(10) << temp->book.id 
                 << setw(30) << temp->book.name.substr(0, 28) 
                 << setw(20) << temp->book.author.substr(0, 18) 
                 << setw(15) << temp->book.category.substr(0, 13) 
                 << setw(10) << temp->book.quantity 
                 << setw(10) << temp->book.available << "\n";
            temp = temp->next;
        }
        cout << string(95, '=') << "\n";
    }

    // 3. Search Book
    void searchBook() {
        cout << "\n--- SEARCH BOOK ---\n";
        cout << "1. Search by ID (O(log N) using BST)\n";
        cout << "2. Search by Name (O(N) using Linked List)\n";
        cout << "Choose an option: ";
        int choice = getValidInt();

        if (choice == 1) {
            cout << "Enter Book ID: ";
            int id = getValidInt();
            BSTNode* result = searchBST(root, id);
            if (result) {
                cout << "\n  [✓] Book Found!\n";
                cout << "  Title: " << result->bookPtr->name << " | Author: " << result->bookPtr->author 
                     << " | Available: " << result->bookPtr->available << "/" << result->bookPtr->quantity << "\n";
            } else {
                cout << "\n  [!] Book not found.\n";
            }
        } else if (choice == 2) {
            cout << "Enter Book Name: ";
            string name; getline(cin, name);
            
            ListNode* temp = head;
            bool found = false;
            while (temp != nullptr) {
                if (temp->book.name == name) {
                    cout << "\n  [✓] Book Found!\n";
                    cout << "  ID: " << temp->book.id << " | Author: " << temp->book.author 
                         << " | Available: " << temp->book.available << "/" << temp->book.quantity << "\n";
                    found = true;
                    break;
                }
                temp = temp->next;
            }
            if (!found) cout << "\n  [!] Book not found.\n";
        } else {
            cout << "  [!] Invalid choice.\n";
        }
    }

    // 4. Update Book
    void updateBook() {
        cout << "\n--- UPDATE BOOK ---\n";
        cout << "Enter Book ID to update: ";
        int id = getValidInt();

        BSTNode* node = searchBST(root, id);
        if (!node) {
            cout << "  [!] Book not found.\n";
            return;
        }

        cout << "  Updating '" << node->bookPtr->name << "'...\n";
        cout << "Enter New Title (or press Enter to skip): ";
        string title; getline(cin, title);
        if (!title.empty()) node->bookPtr->name = title;

        cout << "Enter New Author (or press Enter to skip): ";
        string author; getline(cin, author);
        if (!author.empty()) node->bookPtr->author = author;

        cout << "\n  [+] Book updated successfully!\n";
    }

    // 5. Delete Book
    void deleteBook() {
        cout << "\n--- DELETE BOOK ---\n";
        cout << "Enter Book ID to delete: ";
        int id = getValidInt();

        BSTNode* bstNode = searchBST(root, id);
        if (!bstNode) {
            cout << "  [!] Book not found.\n";
            return;
        }

        // Save to stack for undo
        stack.pushDelete(*(bstNode->bookPtr));

        // 1. Remove from BST
        root = removeBST(root, id);

        // 2. Remove from Linked List
        if (head->book.id == id) {
            ListNode* temp = head;
            head = head->next;
            delete temp;
        } else {
            ListNode* curr = head;
            while (curr->next && curr->next->book.id != id) {
                curr = curr->next;
            }
            if (curr->next) {
                ListNode* temp = curr->next;
                curr->next = curr->next->next;
                delete temp;
            }
        }
        cout << "  [-] Book deleted successfully. (You can Undo this action)\n";
    }

    // 6. Issue Book
    void issueBook() {
        cout << "\n--- ISSUE BOOK ---\n";
        cout << "Enter Book ID: ";
        int id = getValidInt();

        BSTNode* node = searchBST(root, id);
        if (!node) {
            cout << "  [!] Book does not exist in the library.\n";
            return;
        }

        if (node->bookPtr->available > 0) {
            node->bookPtr->available--;
            stack.pushIssue(id); // Save for undo
            cout << "  [+] Book '" << node->bookPtr->name << "' issued successfully! Copies left: " << node->bookPtr->available << "\n";
        } else {
            cout << "  [!] Book is currently out of stock.\n";
            cout << "Enter Student Name to join the waiting queue: ";
            string name; getline(cin, name);
            queue.enqueue(id, name);
            cout << "  [+] Added " << name << " to the waiting queue for this book.\n";
        }
    }

    // 7. Return Book
    void returnBook() {
        cout << "\n--- RETURN BOOK ---\n";
        cout << "Enter Book ID: ";
        int id = getValidInt();

        BSTNode* node = searchBST(root, id);
        if (!node) {
            cout << "  [!] Invalid Book ID.\n";
            return;
        }

        if (node->bookPtr->available < node->bookPtr->quantity) {
            node->bookPtr->available++;
            cout << "  [+] Book returned successfully!\n";
        } else {
            cout << "  [!] All copies of this book are already in the library.\n";
        }
    }

    // 8. View Issue Queue
    void viewQueue() {
        cout << "\n--- PENDING BOOK REQUESTS ---\n";
        queue.displayQueue();
    }

    // 9. Undo Last Delete/Issue
    void undoAction() {
        cout << "\n--- UNDO LAST ACTION ---\n";
        if (stack.isEmpty()) {
            cout << "  [!] No actions to undo.\n";
            return;
        }

        StackNode lastAction = stack.pop();

        if (lastAction.action == UNDO_DELETE) {
            Book b = lastAction.savedBook;
            addBook(b.id, b.name, b.author, b.category, b.quantity, b.available);
            cout << "  [✓] Undo successful: Restored deleted book '" << b.name << "'.\n";
        } 
        else if (lastAction.action == UNDO_ISSUE) {
            BSTNode* node = searchBST(root, lastAction.issuedBookId);
            if (node) {
                node->bookPtr->available++;
                cout << "  [✓] Undo successful: Reverted issue of book '" << node->bookPtr->name << "'.\n";
            }
        }
    }

    // 10. Statistics
    void displayStatistics() {
        cout << "\n--- LIBRARY STATISTICS ---\n";
        int totalTitles = 0;
        int totalCopies = 0;
        int availableCopies = 0;
        int issuedCopies = 0;

        ListNode* temp = head;
        while (temp != nullptr) {
            totalTitles++;
            totalCopies += temp->book.quantity;
            availableCopies += temp->book.available;
            temp = temp->next;
        }
        issuedCopies = totalCopies - availableCopies;

        cout << "  Total Unique Titles : " << totalTitles << "\n";
        cout << "  Total Physical Books: " << totalCopies << "\n";
        cout << "  Books Available     : " << availableCopies << "\n";
        cout << "  Books Issued        : " << issuedCopies << "\n";
    }
};

// ==========================================
// MAIN ENTRY POINT
// ==========================================

void displayHeader() {
    cout << "\n";
    cout << "  ======================================================\n";
    cout << "               LIBRARY MANAGEMENT SYSTEM\n";
    cout << "               Data Structures Showcase\n";
    cout << "  ======================================================\n";
}

int main() {
    LibraryManager library;
    int choice;

    // Pre-populate some dummy data for ease of testing
    library.addBook(101, "Introduction to Algorithms", "Thomas H. Cormen", "CS", 5, 5);
    library.addBook(102, "Clean Code", "Robert C. Martin", "SE", 3, 3);
    library.addBook(103, "Design Patterns", "Erich Gamma", "SE", 4, 4);

    do {
        displayHeader();
        cout << "  1. Add Book\n";
        cout << "  2. Display Books\n";
        cout << "  3. Search Book\n";
        cout << "  4. Update Book\n";
        cout << "  5. Delete Book\n";
        cout << "  6. Issue Book\n";
        cout << "  7. Return Book\n";
        cout << "  8. View Issue Queue\n";
        cout << "  9. Undo Last Delete/Issue\n";
        cout << "  10. Statistics\n";
        cout << "  11. Exit\n";
        cout << "  ------------------------------------------------------\n";
        cout << "  Select an option (1-11): ";
        
        while (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "  [!] Invalid input. Enter a number: ";
        }
        cin.ignore(10000, '\n');

        switch (choice) {
            case 1: library.menuAddBook(); break;
            case 2: library.displayBooks(); break;
            case 3: library.searchBook(); break;
            case 4: library.updateBook(); break;
            case 5: library.deleteBook(); break;
            case 6: library.issueBook(); break;
            case 7: library.returnBook(); break;
            case 8: library.viewQueue(); break;
            case 9: library.undoAction(); break;
            case 10: library.displayStatistics(); break;
            case 11: cout << "\n  Exiting system. Goodbye!\n"; break;
            default: cout << "\n  [!] Invalid choice! Please select between 1 and 11.\n";
        }
    } while (choice != 11);

    return 0;
}