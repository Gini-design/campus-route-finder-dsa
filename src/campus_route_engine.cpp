#include <iostream>
#include <string>
#include <iomanip>
#include <cmath>

using namespace std;

// ============================================================================
// CAMPUS ROUTE FINDER — MANIT BHOPAL (CORE C++ LOGIC ENGINE)
// Mirrors index.html 1-to-1 using raw structs & pointers (Zero STL Containers):
//   1. EdgeNode       : Singly Linked List node (destID, distance, isBlocked,
//                       low-contrast UI color swatch, next pointer)
//   2. BuildingVertex : Graph Adjacency List vertex (id, name, doodle category,
//                       draggable 2D canvas x/y coordinates, head pointer)
//   3. RouteQueue     : Pointer-based FIFO Queue with live BFS trace logging
//   4. RouteStack     : Pointer-based LIFO Stack with path reversal trace
//   5. CampusGraph    : Complete graph engine + Live Memory Address Inspector
// ============================================================================

const int MAX_BUILDINGS = 50;
const int INF = 99999999;

struct ColorSwatch {
    string name;
    string hex;
};

const ColorSwatch EDGE_SWATCHES[6] = {
    {"Soft Slate",      "#9AA3AB"},
    {"Muted Sage",      "#7D9D8C"},
    {"Warm Ochre",      "#C8A27A"},
    {"Dusty Blue",      "#7A93AC"},
    {"Muted Lavender",  "#9B8EA9"},
    {"Soft Terracotta", "#C28B82"}
};

// ============================================================================
// 1. POINTER-BASED FIFO QUEUE (Used for BFS Connectivity & Route Discovery)
// ============================================================================
struct QNode {
    int buildingID;
    QNode* next;
};

class RouteQueue {
private:
    QNode* frontPtr;
    QNode* rearPtr;
    string traceLog;

public:
    RouteQueue() {
        frontPtr = NULL;
        rearPtr = NULL;
        traceLog = "";
    }

    ~RouteQueue() {
        while (!isEmpty()) {
            dequeue();
        }
    }

    bool isEmpty() const {
        return frontPtr == NULL;
    }

    int peek() const {
        return isEmpty() ? -1 : frontPtr->buildingID;
    }

    void enqueue(int id, const string& bName = "") {
        QNode* newNode = new QNode();
        newNode->buildingID = id;
        newNode->next = NULL;

        if (rearPtr == NULL) {
            frontPtr = rearPtr = newNode;
        } else {
            rearPtr->next = newNode;
            rearPtr = newNode;
        }

        if (!bName.empty()) {
            traceLog += "  [ENQUEUE] -> [" + to_string(id) + "] " + bName + "\n";
        }
    }

    int dequeue(const string& bName = "") {
        if (isEmpty()) return -1;

        QNode* temp = frontPtr;
        int id = temp->buildingID;
        frontPtr = frontPtr->next;
        if (frontPtr == NULL) {
            rearPtr = NULL;
        }
        delete temp;

        if (!bName.empty()) {
            traceLog += "  [DEQUEUE] <- [" + to_string(id) + "] " + bName + "\n";
        }
        return id;
    }

    string getTraceLog() const {
        return traceLog;
    }
};

// ============================================================================
// 2. POINTER-BASED LIFO STACK (Used to Reverse & Output Discovered Route)
// ============================================================================
struct SNode {
    int buildingID;
    SNode* next;
};

class RouteStack {
private:
    SNode* topPtr;
    string traceLog;

public:
    RouteStack() {
        topPtr = NULL;
        traceLog = "";
    }

    ~RouteStack() {
        while (!isEmpty()) {
            pop();
        }
    }

    bool isEmpty() const {
        return topPtr == NULL;
    }

    int peek() const {
        return isEmpty() ? -1 : topPtr->buildingID;
    }

    void push(int id, const string& bName = "") {
        SNode* newNode = new SNode();
        newNode->buildingID = id;
        newNode->next = topPtr;
        topPtr = newNode;

        if (!bName.empty()) {
            traceLog += "  [PUSH]    -> [" + to_string(id) + "] " + bName + "\n";
        }
    }

    int pop(const string& bName = "") {
        if (isEmpty()) return -1;

        SNode* temp = topPtr;
        int id = temp->buildingID;
        topPtr = topPtr->next;
        delete temp;

        if (!bName.empty()) {
            traceLog += "  [POP]     <- [" + to_string(id) + "] " + bName + "\n";
        }
        return id;
    }

    string getTraceLog() const {
        return traceLog;
    }
};

// ============================================================================
// 3. SINGLY LINKED LIST EDGE NODE & ADJACENCY LIST BUILDING VERTEX
// ============================================================================
struct EdgeNode {
    int destID;
    int distance;       // Pathway length in meters
    bool isBlocked;     // true = Closed/Blocked pathway, false = Open
    string colorName;   // Low-contrast UI swatch name
    string colorHex;    // Low-contrast UI hex code (e.g., #9AA3AB)
    EdgeNode* next;     // Pointer to next adjacent road on this building
};

struct BuildingVertex {
    int id;
    string name;
    string category;    // Doodle style: Gate, Admin, Library, Academic, Canteen, Hostel, Amenity
    int x;              // 2D map X coordinate (mirrors draggable doodle position)
    int y;              // 2D map Y coordinate
    EdgeNode* head;     // Head pointer to Singly Linked List of EdgeNodes
};

// ============================================================================
// 4. MAIN GRAPH ENGINE CLASS (Mirrors index.html State & Operations)
// ============================================================================
class CampusGraph {
private:
    BuildingVertex campus[MAX_BUILDINGS];
    int buildingCount;

    // O(1) head insertion into Singly Linked List (or updates existing edge)
    void upsertDirectedEdge(int src, int dest, int dist, const string& cName, const string& cHex) {
        EdgeNode* curr = campus[src].head;
        while (curr != NULL) {
            if (curr->destID == dest) {
                curr->distance = dist;
                curr->colorName = cName;
                curr->colorHex = cHex;
                return;
            }
            curr = curr->next;
        }

        EdgeNode* newEdge = new EdgeNode();
        newEdge->destID = dest;
        newEdge->distance = dist;
        newEdge->isBlocked = false;
        newEdge->colorName = cName;
        newEdge->colorHex = cHex;
        newEdge->next = campus[src].head;
        campus[src].head = newEdge;
    }

    bool modifyDirectedEdge(int u, int v, bool blocked, const string& cName, const string& cHex, int newDist) {
        EdgeNode* curr = campus[u].head;
        while (curr != NULL) {
            if (curr->destID == v) {
                curr->isBlocked = blocked;
                curr->colorName = cName;
                curr->colorHex = cHex;
                if (newDist > 0) {
                    curr->distance = newDist;
                }
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    bool unlinkDirectedEdge(int u, int v) {
        EdgeNode* curr = campus[u].head;
        EdgeNode* prev = NULL;

        while (curr != NULL && curr->destID != v) {
            prev = curr;
            curr = curr->next;
        }
        if (curr == NULL) return false;

        if (prev == NULL) {
            campus[u].head = curr->next;
        } else {
            prev->next = curr->next;
        }
        delete curr;
        return true;
    }

    int getEdgeDistance(int u, int v) const {
        EdgeNode* curr = campus[u].head;
        while (curr != NULL) {
            if (curr->destID == v) return curr->distance;
            curr = curr->next;
        }
        return 0;
    }

    void clearAllMemory() {
        for (int i = 0; i < buildingCount; i++) {
            EdgeNode* curr = campus[i].head;
            while (curr != NULL) {
                EdgeNode* toDelete = curr;
                curr = curr->next;
                delete toDelete;
            }
            campus[i].head = NULL;
        }
        buildingCount = 0;
    }

    void printRoutePopupBox(const string& algoTitle, int startID, int endID,
                            int parent[], const string& qTrace, bool showQTrace) {
        RouteStack stack;
        int trace = endID;
        int totalDistance = 0;

        // Backtrack from endID to startID using parent[] and push onto Stack
        while (trace != -1) {
            stack.push(trace, campus[trace].name);
            if (parent[trace] != -1) {
                totalDistance += getEdgeDistance(parent[trace], trace);
            }
            trace = parent[trace];
        }

        // Pop Stack to reverse trail into forward start -> destination route
        int pathIDs[MAX_BUILDINGS];
        int pathLen = 0;
        while (!stack.isEmpty()) {
            int topID = stack.peek();
            pathIDs[pathLen++] = stack.pop(campus[topID].name);
        }

        int walkMinutes = (totalDistance > 0) ? max(1, (int)round(totalDistance / 80.0)) : 0;

        cout << "\n+======================================================================+\n";
        cout << "|                    DISCOVERED CAMPUS ROUTE OUTPUT                    |\n";
        cout << "+======================================================================+\n";
        cout << "  Start Building      : [" << startID << "] " << campus[startID].name << "\n";
        cout << "  Destination         : [" << endID << "] " << campus[endID].name << "\n";
        cout << "  Connectivity Status : CONNECTED\n";
        cout << "  Algorithm           : " << algoTitle << "\n";
        cout << "  Total Distance      : " << totalDistance << " meters (~" << walkMinutes << " min walk)\n";
        cout << "  Stops Highlighted   : " << pathLen << " buildings (" << (pathLen - 1) << " pathway segments)\n";
        cout << "------------------------------------------------------------------------\n";
        cout << "  HIGHLIGHTED WALKING ROUTE:\n  ";
        for (int i = 0; i < pathLen; i++) {
            cout << "[" << pathIDs[i] << "] " << campus[pathIDs[i]].name;
            if (i < pathLen - 1) {
                int segDist = getEdgeDistance(pathIDs[i], pathIDs[i + 1]);
                cout << " --(" << segDist << "m)--> ";
            }
        }
        cout << "\n------------------------------------------------------------------------\n";

        if (showQTrace) {
            cout << "  DSA TRACE — RouteQueue (FIFO Breadth-First Exploration):\n";
            cout << qTrace;
            cout << "------------------------------------------------------------------------\n";
        }

        cout << "  DSA TRACE — RouteStack (LIFO Backtrack Push & Forward Pop):\n";
        cout << stack.getTraceLog();
        cout << "+======================================================================+\n";
    }

public:
    CampusGraph() {
        buildingCount = 0;
    }

    ~CampusGraph() {
        clearAllMemory();
    }

    bool isValidBuilding(int id) const {
        return id >= 0 && id < buildingCount;
    }

    // Mirrors "+ Add Building" Dialog in index.html
    int addBuilding(const string& name, const string& category, int x, int y) {
        if (buildingCount >= MAX_BUILDINGS) {
            cout << ">> Error: Maximum building capacity (" << MAX_BUILDINGS << ") reached.\n";
            return -1;
        }
        int id = buildingCount;
        campus[id].id = id;
        campus[id].name = name;
        campus[id].category = category;
        campus[id].x = x;
        campus[id].y = y;
        campus[id].head = NULL;
        buildingCount++;
        return id;
    }

    // Mirrors Drag-and-Drop Building Positioning on Canvas in index.html
    void moveBuildingCoordinates(int id, int newX, int newY) {
        if (!isValidBuilding(id)) {
            cout << ">> Error: Invalid building ID.\n";
            return;
        }
        campus[id].x = newX;
        campus[id].y = newY;
        cout << ">> Moved [" << id << "] " << campus[id].name
             << " to canvas coordinates (" << newX << ", " << newY << ").\n";
    }

    // Mirrors Right-Click Building -> Create Edge & Connections Tab
    void addPathway(int u, int v, int dist,
                    const string& colorName = "Soft Slate",
                    const string& colorHex = "#9AA3AB") {
        if (!isValidBuilding(u) || !isValidBuilding(v) || u == v || dist <= 0) {
            cout << ">> Error: Invalid building IDs or distance.\n";
            return;
        }
        upsertDirectedEdge(u, v, dist, colorName, colorHex);
        upsertDirectedEdge(v, u, dist, colorName, colorHex);
    }

    EdgeNode* findDirectEdge(int u, int v) const {
        if (!isValidBuilding(u) || !isValidBuilding(v)) return NULL;
        EdgeNode* curr = campus[u].head;
        while (curr != NULL) {
            if (curr->destID == v) return curr;
            curr = curr->next;
        }
        return NULL;
    }

    // Mirrors Right-Click Pathway Line -> Toggle isBlocked, Change Color Swatch, Edit Distance
    void updatePathwaySettings(int u, int v, bool isBlocked,
                               const string& colorName, const string& colorHex, int newDist) {
        if (!isValidBuilding(u) || !isValidBuilding(v)) {
            cout << ">> Error: Invalid building IDs.\n";
            return;
        }
        if (modifyDirectedEdge(u, v, isBlocked, colorName, colorHex, newDist) &&
            modifyDirectedEdge(v, u, isBlocked, colorName, colorHex, newDist)) {
            cout << ">> Updated Pathway [" << campus[u].name << " <-> " << campus[v].name << "]: "
                 << (isBlocked ? "BLOCKED" : "OPEN")
                 << " | Swatch: " << colorName << " (" << colorHex << ")"
                 << " | Distance: " << newDist << "m\n";
        } else {
            cout << ">> Error: No direct pathway exists between those buildings.\n";
        }
    }

    // Permanently deletes EdgeNode from both Singly Linked Lists
    void removePathway(int u, int v) {
        if (!isValidBuilding(u) || !isValidBuilding(v)) {
            cout << ">> Error: Invalid building IDs.\n";
            return;
        }
        if (unlinkDirectedEdge(u, v) && unlinkDirectedEdge(v, u)) {
            cout << ">> Permanently removed pathway between [" << campus[u].name
                 << "] and [" << campus[v].name << "].\n";
        } else {
            cout << ">> Error: Pathway not found.\n";
        }
    }

    void listBuildingsCompact() const {
        cout << "\n--- MANIT BHOPAL BUILDINGS ON CANVAS ---\n";
        for (int i = 0; i < buildingCount; i++) {
            cout << "  [" << setw(2) << campus[i].id << "] "
                 << left << setw(32) << campus[i].name
                 << " | Doodle: " << setw(9) << campus[i].category
                 << " | Pos: (" << campus[i].x << ", " << campus[i].y << ")\n" << right;
        }
    }

    // Mirrors the "Live DSA View" Drawer in index.html (displays actual C++ pointer addresses)
    void inspectLiveAdjacencyList() const {
        cout << "\n================== LIVE DSA ADJACENCY LIST INSPECTOR ==================\n";
        for (int i = 0; i < buildingCount; i++) {
            cout << "campus[" << i << "] \"" << campus[i].name << "\" "
                 << "[" << campus[i].category << " @ (" << campus[i].x << "," << campus[i].y << ")]\n"
                 << "   head (" << static_cast<const void*>(campus[i].head) << ") ===> ";

            EdgeNode* curr = campus[i].head;
            if (curr == NULL) {
                cout << "NULL\n";
                continue;
            }
            while (curr != NULL) {
                cout << "[dest:" << curr->destID << " (" << campus[curr->destID].name << ")"
                     << " | " << curr->distance << "m"
                     << " | " << (curr->isBlocked ? "BLOCKED" : "OPEN")
                     << " | " << curr->colorHex
                     << " | next:" << static_cast<const void*>(curr->next) << "] -> ";
                curr = curr->next;
            }
            cout << "NULL\n";
        }
        cout << "=======================================================================\n";
    }

    // Connectivity Verification via Queue-based BFS
    bool checkConnectivity(int startID, int endID) const {
        if (!isValidBuilding(startID) || !isValidBuilding(endID)) return false;

        bool visited[MAX_BUILDINGS];
        for (int i = 0; i < buildingCount; i++) visited[i] = false;

        RouteQueue q;
        visited[startID] = true;
        q.enqueue(startID);

        while (!q.isEmpty()) {
            int curr = q.dequeue();
            if (curr == endID) return true;

            EdgeNode* edge = campus[curr].head;
            while (edge != NULL) {
                if (!edge->isBlocked && !visited[edge->destID]) {
                    visited[edge->destID] = true;
                    q.enqueue(edge->destID);
                }
                edge = edge->next;
            }
        }
        return false;
    }

    // Core BFS Route Finder (Queue Traversal + Stack Route Reconstruction)
    void findRouteBFS(int startID, int endID) {
        if (!isValidBuilding(startID) || !isValidBuilding(endID)) {
            cout << ">> Error: Invalid building IDs.\n";
            return;
        }

        bool visited[MAX_BUILDINGS];
        int parent[MAX_BUILDINGS];
        for (int i = 0; i < buildingCount; i++) {
            visited[i] = false;
            parent[i] = -1;
        }

        RouteQueue q;
        visited[startID] = true;
        q.enqueue(startID, campus[startID].name);

        while (!q.isEmpty()) {
            int frontID = q.peek();
            int curr = q.dequeue(campus[frontID].name);
            if (curr == endID) break;

            EdgeNode* edge = campus[curr].head;
            while (edge != NULL) {
                int nxt = edge->destID;
                if (!edge->isBlocked && !visited[nxt]) {
                    visited[nxt] = true;
                    parent[nxt] = curr;
                    q.enqueue(nxt, campus[nxt].name);
                }
                edge = edge->next;
            }
        }

        if (!visited[endID]) {
            cout << "\n+======================================================================+\n";
            cout << "|  Connectivity Status : NOT CONNECTED                                 |\n";
            cout << "|  No open pathway route exists between the selected buildings.        |\n";
            cout << "+======================================================================+\n";
            return;
        }

        printRoutePopupBox("BFS (RouteQueue + RouteStack — Fewest Hops)", startID, endID,
                           parent, q.getTraceLog(), true);
    }

    // Weighted Shortest Distance Route Finder (Dijkstra + Stack Reconstruction)
    void findRouteDijkstra(int startID, int endID) {
        if (!isValidBuilding(startID) || !isValidBuilding(endID)) {
            cout << ">> Error: Invalid building IDs.\n";
            return;
        }

        int minDist[MAX_BUILDINGS];
        bool finalized[MAX_BUILDINGS];
        int parent[MAX_BUILDINGS];

        for (int i = 0; i < buildingCount; i++) {
            minDist[i] = INF;
            finalized[i] = false;
            parent[i] = -1;
        }
        minDist[startID] = 0;

        for (int step = 0; step < buildingCount; step++) {
            int u = -1;
            int best = INF;
            for (int i = 0; i < buildingCount; i++) {
                if (!finalized[i] && minDist[i] < best) {
                    best = minDist[i];
                    u = i;
                }
            }
            if (u == -1 || u == endID) break;
            finalized[u] = true;

            EdgeNode* edge = campus[u].head;
            while (edge != NULL) {
                int v = edge->destID;
                if (!edge->isBlocked && !finalized[v] && minDist[u] + edge->distance < minDist[v]) {
                    minDist[v] = minDist[u] + edge->distance;
                    parent[v] = u;
                }
                edge = edge->next;
            }
        }

        if (minDist[endID] == INF) {
            cout << "\n+======================================================================+\n";
            cout << "|  Connectivity Status : NOT CONNECTED                                 |\n";
            cout << "|  No open pathway route exists between the selected buildings.        |\n";
            cout << "+======================================================================+\n";
            return;
        }

        printRoutePopupBox("Dijkstra + RouteStack (Shortest Walking Meters)", startID, endID,
                           parent, "", false);
    }

    // Preloads the exact 12 MANIT Bhopal buildings, coordinates & 17 pathways from index.html
    void resetManitBhopalDefaults() {
        clearAllMemory();

        addBuilding("Main Gate (Link Rd 3)",         "Gate",     130, 130); // 0
        addBuilding("Admin Block",                   "Admin",    350, 170); // 1
        addBuilding("Central Library (LRC)",         "Library",  560, 230); // 2
        addBuilding("CSE Department",                "Academic", 560, 100); // 3
        addBuilding("New Teaching Block (NTB)",      "Academic", 770, 180); // 4
        addBuilding("ROLTA Incubation Centre",       "Academic", 820,  90); // 5
        addBuilding("Student Activity Centre (SAC)", "Amenity",  870, 360); // 6
        addBuilding("Campus Canteen (Nescafe)",      "Canteen",  640, 360); // 7
        addBuilding("Sports Complex & Dispensary",   "Amenity",  820, 520); // 8
        addBuilding("Boys Hostel Zone (Hostel 10)",  "Hostel",  1060, 460); // 9
        addBuilding("Kalpana Chawla Bhawan (H-7)",   "Hostel",   340, 430); // 10
        addBuilding("SBI Bank & Post Office",        "Amenity",  160, 310); // 11

        addPathway(0,  11, 180);
        addPathway(0,  1,  350);
        addPathway(11, 1,  220);
        addPathway(11, 10, 300);
        addPathway(1,  2,  160);
        addPathway(1,  3,  240);
        addPathway(2,  4,  190);
        addPathway(2,  7,  140);
        addPathway(3,  4,  170);
        addPathway(3,  5,  210);
        addPathway(4,  7,  150);
        addPathway(5,  6,  260);
        addPathway(7,  6,  230);
        addPathway(6,  8,  200);
        addPathway(6,  9,  320);
        addPathway(8,  9,  280);
        addPathway(10, 2,  340);
    }
};

// ============================================================================
// 5. INTERACTIVE CLI CONTROLLER (Mirrors Every Dialog & Action in index.html)
// ============================================================================
int main() {
    CampusGraph manit;
    manit.resetManitBhopalDefaults();

    int choice;
    while (true) {
        cout << "\n=======================================================================\n";
        cout << "   MANIT BHOPAL — CAMPUS ROUTE FINDER (C++ CORE DSA LOGIC ENGINE)      \n";
        cout << "=======================================================================\n";
        cout << "  1. Live DSA View (Inspect campus[i].head & Linked List Addresses)\n";
        cout << "  2. List Buildings & 2D Canvas Coordinates\n";
        cout << "  3. Add Building Node (+ Name, Doodle Category, X/Y Position)\n";
        cout << "  4. Move / Drag Building Coordinates on Canvas\n";
        cout << "  5. Connect Two Buildings (Create / Update Pathway)\n";
        cout << "  6. Pathway Settings (Toggle isBlocked, Change Color Swatch, Distance)\n";
        cout << "  7. Remove Pathway Permanently (Unlink EdgeNode)\n";
        cout << "  8. Check Connectivity Between Two Buildings (BFS)\n";
        cout << "  9. Find Route — BFS (RouteQueue + RouteStack Trace & Output)\n";
        cout << " 10. Find Route — Dijkstra (Shortest Meters + RouteStack Trace)\n";
        cout << " 11. Reset Map to Default MANIT Bhopal Network\n";
        cout << "  0. Exit\n";
        cout << "-----------------------------------------------------------------------\n";
        cout << "Select operation: ";

        if (!(cin >> choice)) break;
        cin.ignore();

        if (choice == 0) {
            cout << "Exiting MANIT Bhopal C++ DSA Engine. Memory cleaned up.\n";
            break;
        }

        if (choice == 1) {
            manit.inspectLiveAdjacencyList();
        }
        else if (choice == 2) {
            manit.listBuildingsCompact();
        }
        else if (choice == 3) {
            string name, category;
            int x, y;
            cout << "Enter Building Name: ";
            getline(cin, name);
            cout << "Enter Doodle Category (Academic/Hostel/Library/Gate/Canteen/Amenity): ";
            getline(cin, category);
            if (category.empty()) category = "Academic";
            cout << "Enter X and Y canvas coordinates (e.g., 480 300): ";
            cin >> x >> y;
            int newID = manit.addBuilding(name, category, x, y);
            if (newID != -1) {
                cout << ">> Added Building [" << newID << "] '" << name << "' at (" << x << ", " << y << ").\n";
            }
        }
        else if (choice == 4) {
            int id, x, y;
            manit.listBuildingsCompact();
            cout << "Enter Building ID to move: ";
            cin >> id;
            cout << "Enter new X and Y coordinates: ";
            cin >> x >> y;
            manit.moveBuildingCoordinates(id, x, y);
        }
        else if (choice == 5) {
            int u, v, dist;
            manit.listBuildingsCompact();
            cout << "Enter Source Building ID: ";
            cin >> u;
            cout << "Enter Destination Building ID: ";
            cin >> v;
            cout << "Enter Distance (meters): ";
            cin >> dist;
            manit.addPathway(u, v, dist);
            cout << ">> Connected Building [" << u << "] <---> Building [" << v << "] (" << dist << "m).\n";
        }
        else if (choice == 6) {
            int u, v;
            manit.listBuildingsCompact();
            cout << "Enter Building ID 1 of pathway: ";
            cin >> u;
            cout << "Enter Building ID 2 of pathway: ";
            cin >> v;

            EdgeNode* existing = manit.findDirectEdge(u, v);
            if (existing == NULL) {
                cout << ">> Error: No direct pathway exists between [" << u << "] and [" << v << "].\n";
                continue;
            }

            cout << "\nCurrent Settings -> Status: " << (existing->isBlocked ? "BLOCKED" : "OPEN")
                 << " | Swatch: " << existing->colorName << " (" << existing->colorHex << ")"
                 << " | Distance: " << existing->distance << "m\n";

            int blockChoice;
            cout << "Set isBlocked? (1 = Blocked/Closed, 0 = Open): ";
            cin >> blockChoice;

            cout << "Available Low-Contrast Color Swatches:\n";
            for (int i = 0; i < 6; i++) {
                cout << "  " << (i + 1) << ". " << EDGE_SWATCHES[i].name
                     << " (" << EDGE_SWATCHES[i].hex << ")\n";
            }
            int swatchIdx;
            cout << "Choose Swatch (1-6, or 0 to keep current): ";
            cin >> swatchIdx;

            string cName = existing->colorName;
            string cHex = existing->colorHex;
            if (swatchIdx >= 1 && swatchIdx <= 6) {
                cName = EDGE_SWATCHES[swatchIdx - 1].name;
                cHex = EDGE_SWATCHES[swatchIdx - 1].hex;
            }

            int newDist;
            cout << "Enter Distance in meters (or 0 to keep " << existing->distance << "m): ";
            cin >> newDist;
            if (newDist <= 0) newDist = existing->distance;

            manit.updatePathwaySettings(u, v, (blockChoice == 1), cName, cHex, newDist);
        }
        else if (choice == 7) {
            int u, v;
            manit.listBuildingsCompact();
            cout << "Enter Building ID 1 to unlink: ";
            cin >> u;
            cout << "Enter Building ID 2 to unlink: ";
            cin >> v;
            manit.removePathway(u, v);
        }
        else if (choice == 8) {
            int u, v;
            manit.listBuildingsCompact();
            cout << "Enter Start Building ID: ";
            cin >> u;
            cout << "Enter Destination Building ID: ";
            cin >> v;
            if (manit.checkConnectivity(u, v)) {
                cout << ">> Connectivity Status: CONNECTED (Open route available).\n";
            } else {
                cout << ">> Connectivity Status: NOT CONNECTED.\n";
            }
        }
        else if (choice == 9) {
            int u, v;
            manit.listBuildingsCompact();
            cout << "Enter Start Building ID: ";
            cin >> u;
            cout << "Enter Destination Building ID: ";
            cin >> v;
            manit.findRouteBFS(u, v);
        }
        else if (choice == 10) {
            int u, v;
            manit.listBuildingsCompact();
            cout << "Enter Start Building ID: ";
            cin >> u;
            cout << "Enter Destination Building ID: ";
            cin >> v;
            manit.findRouteDijkstra(u, v);
        }
        else if (choice == 11) {
            manit.resetManitBhopalDefaults();
            cout << ">> Reset MANIT Bhopal campus network to defaults.\n";
        }
        else {
            cout << ">> Invalid selection.\n";
        }
    }

    return 0;
}