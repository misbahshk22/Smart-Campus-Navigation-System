/*
    ============================================================
        Smart Campus Navigation System
        Language : C++
        Concepts : Graph (Adjacency List), Dijkstra's Algorithm,
                   BFS, DFS, Stack, Queue, BST
    ============================================================
*/

#include <iostream>
#include <string>
using namespace std;


// ============================================================
//                      EDGE NODE CLASS
//  Each EdgeNode represents one road connecting two locations.
//  It stores where the road goes (destination) and how long
//  it is (distance), plus a pointer to the next edge in the
//  adjacency list of that location.
// ============================================================

class EdgeNode {
public:
    int destination;   // index of the location this road leads to
    int distance;      // length of the road in meters
    EdgeNode* next;    // next road in the linked list

    // Constructor — initialise a new road with given destination and distance
    EdgeNode(int dest, int dist) {
        destination = dest;
        distance    = dist;
        next        = nullptr;
    }
};


// ============================================================
//                    LOCATION NODE CLASS
//  Represents a single campus location (e.g. Library, Cafe).
//  'head' is the start of the linked list of roads leaving
//  this location — basically the adjacency list for this node.
// ============================================================

class LocationNode {
public:
    string    name;   
    EdgeNode* head;   

    // Constructor — a fresh location starts with no connected roads
    LocationNode() {
        head = nullptr;
    }
};


// ============================================================
//                        STACK CLASS
//  A simple fixed-size stack (max 50 items) used to keep a
//  search history of paths the user has looked up.
//  Works on LIFO principle — last searched path shows first.
// ============================================================

class Stack {
private:
    string data[50];   // array holding the history entries
    int    top;        // index of the topmost item (-1 = empty)

public:
    // Start with an empty stack
    Stack() {
        top = -1;
    }

    // Push: add a new search entry to the top of the history
    void push(string value) {
        if (top < 49) {       // make sure we don't overflow
            data[++top] = value;
        }
    }

    // Pop: remove the most recent search entry
    void pop() {
        if (top >= 0) {
            top--;
        }
    }

    // Display: print all history entries from newest to oldest
    void display() {
        if (top == -1) {
            cout << "No Search History!" << endl;
            return;
        }
        cout << "Search History: ";
        for (int i = top; i >= 0; i--) {
            cout << data[i] << " ";
        }
        cout << endl;
    }
};


// ============================================================
//                        QUEUE CLASS
//  A simple fixed-size queue (max 50 items) used to manage
//  navigation requests in order — FIFO (first in, first out).
//  Requests are processed in the order they were added.
// ============================================================

class Queue {
private:
    string data[50];    // array holding pending requests
    int    front, rear; // front = next to process, rear = next empty slot

public:
    // Start with an empty queue
    Queue() {
        front = rear = 0;
    }

    // Enqueue: add a new request to the back of the line
    void enqueue(string value) {
        if (rear < 50) {
            data[rear++] = value;
        }
    }

    // Dequeue: process (remove) the oldest pending request
    void dequeue() {
        if (front < rear) {
            front++;
        }
    }

    // Display: show all requests that are still waiting
    void display() {
        if (front == rear) {
            cout << "No Pending Request!" << endl;
            return;
        }
        cout << "============ Pending Requests ===========" << endl;
        for (int i = front; i <= rear; i++) {
            cout << data[i] << " ";
        }
        cout << endl;
    }
};


// ============================================================
//                       BST NODE CLASS
//  A single node in the Binary Search Tree.
//  Stores the location name and pointers to left/right children.
// ============================================================

class BSTNode {
public:
    string   name;    // location name stored at this node
    BSTNode* left;    // pointer to left child (alphabetically smaller)
    BSTNode* right;   // pointer to right child (alphabetically larger)

    // Initialise with a name; children start as null
    BSTNode(string value) {
        name  = value;
        left  = right = nullptr;
    }
};


// ============================================================
//                         BST CLASS
//  Binary Search Tree for fast location lookup.
//  Locations are stored alphabetically — left subtree has names
//  that come before the current node, right has names that come
//  after. This makes searching much faster than a plain array.
// ============================================================

class BST {
private:
    BSTNode* root;   // top of the tree (null if tree is empty)

public:
    // Start with an empty tree
    BST() {
        root = nullptr;
    }

    // -------------------------------------------------------
    //                  
    //                    InsertNode
  
    // -------------------------------------------------------
    BSTNode* insertNode(BSTNode* node, string value) {
        // Base case: we've reached an empty spot — create the node here
        if (node == nullptr) {
            return new BSTNode(value);
        }

        // Go left if the new name comes before the current node's name
        if (value < node->name) {
            node->left = insertNode(node->left, value);
        }
        // Go right if it comes after
        else if (value > node->name) {
            node->right = insertNode(node->right, value);
        }
        // If equal, it's a duplicate — we just return without inserting

        return node;
    }

    // Insert a location name into the BST
    void insert(string value) {
        root = insertNode(root, value);
        cout << "Location Inserted in BST!" << endl;
    }

    // -------------------------------------------------------
    //  
    //                    SearchNode 
    
    // -------------------------------------------------------
    bool searchNode(BSTNode* node, string value) {
        // Reached the end without finding it
        if (node == nullptr) {
            return false;
        }

        // Found it!
        if (node->name == value) {
            return true;
        }

        // Search in the left or right subtree based on alphabetical order
        if (value < node->name) {
            return searchNode(node->left, value);
        }

        return searchNode(node->right, value);
    }

    //  Search for a location and print the result
    void search(string value) {
        if (searchNode(root, value)) {
            cout << "Location Found!" << endl;
        } else {
            cout << "Location Not Found!" << endl;
        }
    }

    // -------------------------------------------------------
    // inorder: visits nodes in alphabetical order (left → root → right)
    // This produces a sorted list of all location names
    // -------------------------------------------------------
    void inorder(BSTNode* node) {
        if (node == nullptr) {
            return;
        }
        inorder(node->left);
        cout << node->name << " ";
        inorder(node->right);
    }

    // Print all locations stored in the BST, sorted alphabetically
    void display() {
        cout << "\nBST Locations (Alphabetical Order): ";
        inorder(root);
        cout << endl;
    }
};


// ============================================================
//                        GRAPH CLASS
//  The heart of the navigation system.
//  Represents the campus as a weighted undirected graph where:
//    - Locations are vertices (nodes)
//    - Roads are edges with weights (distances in meters)
//  Internally uses an adjacency list for space efficiency.
// ============================================================

class Graph {
private:
    LocationNode location[50];   // array of all campus locations
    int          locationCount;  // how many locations have been added so far

public:
    // Start with zero locations
    Graph() {
        locationCount = 0;
    }

    // -------------------------------------------------------
    // getLocationIndex: search the locations array by name
    // Returns the array index if found, -1 if not found
    // -------------------------------------------------------
    int getLocationIndex(string name) {
        for (int i = 0; i < locationCount; i++) {
            if (location[i].name == name)
                return i;
        }
        return -1;   // location doesn't exist yet
    }

    // -------------------------------------------------------
    // addLocation: register a new campus location
    // Checks for duplicates and capacity before adding
    // -------------------------------------------------------
    void addLocation(string name) {
        if (locationCount >= 50) {
            cout << "Location Limit Exceeded!" << endl;
            return;
        }
        if (getLocationIndex(name) != -1) {
            cout << "Location Already Exists!" << endl;
            return;
        }
        location[locationCount].name = name;
        locationCount++;
        cout << "Location Added Successfully!" << endl;
    }

    // -------------------------------------------------------
    // addRoad: create a bidirectional road between two locations
    // Both locations must exist first.
    // Since roads go both ways, we add two EdgeNodes — one for
    // each direction (src→dest and dest→src)
    // -------------------------------------------------------
    void addRoad(string source, string destination, int distance) {
        int src  = getLocationIndex(source);
        int dest = getLocationIndex(destination);

        if (src == -1 || dest == -1) {
            cout << "Invalid Location! Add locations first." << endl;
            return;
        }

        // Add edge from source to destination
        EdgeNode* newNode1 = new EdgeNode(dest, distance);
        newNode1->next     = location[src].head;
        location[src].head = newNode1;

        // Add the reverse edge (destination to source) — undirected graph
        EdgeNode* newNode2  = new EdgeNode(src, distance);
        newNode2->next      = location[dest].head;
        location[dest].head = newNode2;

        cout << "Road Added Successfully!" << endl;
    }

    // -------------------------------------------------------
    // displayMap: print the full adjacency list of the campus
    // Shows every location and all the roads leaving it
    // -------------------------------------------------------
    void displayMap() {
        if (locationCount == 0) {
            cout << "No Locations Added Yet!" << endl;
            return;
        }
        cout << "\n====== Campus Map ======" << endl;
        for (int i = 0; i < locationCount; i++) {
            cout << location[i].name << " -> ";
            EdgeNode* temp = location[i].head;
            while (temp != nullptr) {
                cout << location[temp->destination].name
                     << " (" << temp->distance << "m)  ";
                temp = temp->next;
            }
            cout << endl;
        }
        cout << "========================" << endl;
    }

    // -------------------------------------------------------
    // shortestPath: find the shortest route between two locations
    //
    // Uses Dijkstra's Algorithm:
    //   1. Set distance of start = 0, all others = infinity
    //   2. Repeatedly pick the unvisited node with smallest distance
    //   3. Relax (update) distances of its neighbours
    //   4. Track 'previous' node so we can reconstruct the path
    // -------------------------------------------------------
    void shortestPath(string start, string end) {
        int src  = getLocationIndex(start);
        int dest = getLocationIndex(end);

        if (src == -1 || dest == -1) {
            cout << "Invalid Location!" << endl;
            return;
        }

        int  distance[50];    // shortest known distance from src to each node
        bool visited[50];     // true once a node's shortest distance is finalised
        int  previous[50];    // which node did we come from on the shortest path

        // Step 1: initialise everything
        for (int i = 0; i < locationCount; i++) {
            distance[i] = 99999999;   // treat as infinity
            visited[i]  = false;
            previous[i] = -1;         // no predecessor yet
        }
        distance[src] = 0;            // distance to ourselves is zero

        // Step 2 & 3: main Dijkstra loop — runs once per location
        for (int i = 0; i < locationCount; i++) {

            // Pick the closest unvisited node
            int minDist     = 99999999;
            int currentNode = -1;

            for (int j = 0; j < locationCount; j++) {
                if (!visited[j] && distance[j] < minDist) {
                    minDist     = distance[j];
                    currentNode = j;
                }
            }

            // If no reachable unvisited node remains, we're done
            if (currentNode == -1) break;

            visited[currentNode] = true;

            // Relax all neighbours of the current node
            EdgeNode* temp = location[currentNode].head;
            while (temp != nullptr) {
                int neighbor    = temp->destination;
                int newDistance = distance[currentNode] + temp->distance;

                // If we found a shorter route to this neighbour, update it
                if (newDistance < distance[neighbor]) {
                    distance[neighbor] = newDistance;
                    previous[neighbor] = currentNode;   // remember how we got here
                }
                temp = temp->next;
            }
        }

        // If destination is still "infinity", there's no path
        if (distance[dest] == 99999999) {
            cout << "No Path Found Between " << start << " and " << end << "!" << endl;
            return;
        }

        // Print the total shortest distance
        cout << "\nShortest Distance from "
             << start << " to " << end
             << ": " << distance[dest] << "m" << endl;

        // Step 4: trace back from destination to source using 'previous' array
        int path[50];
        int pathCount = 0;
        int current   = dest;

        while (current != -1) {
            path[pathCount++] = current;
            current = previous[current];
        }

        // Print the path in the correct order (source → destination)
        cout << "Path: ";
        for (int i = pathCount - 1; i >= 0; i--) {
            cout << location[path[i]].name;
            if (i != 0) cout << " -> ";
        }
        cout << endl;
    }


    // -------------------------------------------------------
    // BFS: Breadth-First Search traversal from a given location
    //
    // Visits locations level by level — explores all immediate
    // neighbours first, then their neighbours, and so on.
    // Uses an internal array as a simple queue.
    // -------------------------------------------------------
    void BFS(string start) {
        int src = getLocationIndex(start);
        if (src == -1) {
            cout << "Invalid Location!" << endl;
            return;
        }

        bool visited[50] = {false};  // track which locations we've already seen
        int  queue[50];
        int  front = 0, rear = 0;

        // Kick off BFS from the starting location
        queue[rear++] = src;
        visited[src]  = true;

        while (front < rear) {
            int current = queue[front++];  // dequeue the next location
            cout << location[current].name << " ";

            // Enqueue all unvisited neighbours
            EdgeNode* temp = location[current].head;
            while (temp != nullptr) {
                int neighbor = temp->destination;
                if (!visited[neighbor]) {
                    visited[neighbor]  = true;
                    queue[rear++]      = neighbor;
                }
                temp = temp->next;
            }
        }
        cout << endl;
    }


    // -------------------------------------------------------
    // DFS: Depth-First Search traversal from a given location
    //
    // Goes as deep as possible down one path before backtracking
    // and exploring another branch.
    // Uses an internal array as a simple stack.
    // -------------------------------------------------------
    void DFS(string start) {
        int src = getLocationIndex(start);
        if (src == -1) {
            cout << "Invalid Location!" << endl;
            return;
        }

        bool visited[50] = {false};  // track which locations we've already visited
        int  stack[50];
        int  top = -1;

        // Push the starting location onto the stack
        stack[++top] = src;

        while (top >= 0) {
            int current = stack[top--];  // pop the top location

            // Only process if we haven't visited it before
            // (a location can be pushed multiple times by different paths)
            if (!visited[current]) {
                visited[current] = true;
                cout << location[current].name << " ";

                // Push all unvisited neighbours — they'll be explored next
                EdgeNode* temp = location[current].head;
                while (temp != nullptr) {
                    int neighbor = temp->destination;
                    if (!visited[neighbor]) {
                        stack[++top] = neighbor;
                    }
                    temp = temp->next;
                }
            }
        }
        cout << endl;
    }
};


// ============================================================
//                           MAIN
//  Entry point — shows the menu and handles user interaction
//  in a loop until the user chooses to exit (option 13).
// ============================================================

int main() {
    Graph campus;     // the campus map (graph)
    Stack history;    // search history (stack)
    Queue requests;   // pending navigation requests (queue)
    BST   location;   // location lookup tree (BST)
    int   choice;

    do {
        // Display the main menu
        cout << "\n===== SMART CAMPUS NAVIGATION SYSTEM =====\n";
        cout << "1.  Add Location\n";
        cout << "2.  Add Road\n";
        cout << "3.  Display Map\n";
        cout << "4.  Find Shortest Path\n";
        cout << "5.  BFS Traversal\n";
        cout << "6.  DFS Traversal\n";
        cout << "7.  Search History\n";
        cout << "8.  Add Request\n";
        cout << "9.  Process Request\n";
        cout << "10. Display Requests\n";
        cout << "11. Search Location\n";
        cout << "12. Display Locations (BST)\n";
        cout << "13. Exit\n";
        cout << "Enter Choice: ";
        cin >> choice;

        if (choice == 1) {
            // Add a new location to both the graph and the BST
            string name;
            cout << "Enter Location Name: ";
            cin >> name;
            campus.addLocation(name);
            location.insert(name);
        }
        else if (choice == 2) {
            // Connect two existing locations with a road
            string source, destination;
            int    distance;
            cout << "Enter Source Location: ";
            cin >> source;
            cout << "Enter Destination Location: ";
            cin >> destination;
            cout << "Enter Distance (in meters): ";
            cin >> distance;
            campus.addRoad(source, destination, distance);
        }
        else if (choice == 3) {
            // Show the full campus map as an adjacency list
            campus.displayMap();
        }
        else if (choice == 4) {
            // Run Dijkstra and also record the search in history
            string start, end;
            cout << "Enter Start Location: ";
            cin >> start;
            cout << "Enter End Location: ";
            cin >> end;
            campus.shortestPath(start, end);
            history.push(start + " -> " + end);   // save to history stack
        }
        else if (choice == 5) {
            // Breadth-First traversal from a chosen starting point
            string start;
            cout << "Enter Starting Location for BFS: ";
            cin >> start;
            cout << "BFS Traversal: ";
            campus.BFS(start);
        }
        else if (choice == 6) {
            // Depth-First traversal from a chosen starting point
            string start;
            cout << "Enter Starting Location for DFS: ";
            cin >> start;
            cout << "DFS Traversal: ";
            campus.DFS(start);
        }
        else if (choice == 7) {
            // Print the search history (most recent first)
            history.display();
        }
        else if (choice == 8) {
            // Add a new request to the waiting queue
            string request;
            cout << "Enter Your Request: ";
            cin.ignore();               // flush leftover newline from previous cin
            getline(cin, request);      // read the full line (supports spaces)
            requests.enqueue(request);
            cout << "Request Added!" << endl;
        }
        else if (choice == 9) {
            // Process (remove) the oldest request from the queue
            requests.dequeue();
            cout << "Request Processed!" << endl;
        }
        else if (choice == 10) {
            // Show all requests still waiting in the queue
            requests.display();
        }
        else if (choice == 11) {
            // Search for a location by name using the BST
            string name;
            cout << "Enter Location to Search: ";
            cin >> name;
            location.search(name);
        }
        else if (choice == 12) {
            // Display all locations stored in the BST (alphabetical order)
            location.display();
        }
        else if (choice == 13) {
            cout << "Exiting Program. Goodbye!\n";
        }
        else {
            cout << "Invalid Choice! Try Again.\n";
        }

    } while (choice != 13);

    return 0;
}




