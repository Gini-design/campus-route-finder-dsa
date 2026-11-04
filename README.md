# CampusRoute: MANIT Bhopal Campus Route Finder

A graph-based campus navigation, connectivity verification, and route-distance calculation system built from scratch in C++ for our **Data Structures & Algorithms (DSA)** Mini-Project[cite: 1, 7].

## Overview
Navigating a large university campus requires knowing how buildings connect, which walkways are open, and the total walking distance between locations[cite: 1, 7]. **CampusRoute** models the **MANIT Bhopal** campus as a weighted undirected graph where buildings act as vertices and pathways act as bidirectional edges[cite: 1, 7].

## Core Data Structures Implemented (From Scratch in C++)
All data structures in `src/campus_route_engine.cpp` are implemented using raw structs, arrays, and pointers without relying on Standard Template Library (STL) containers[cite: 1, 7]:

| Data Structure | Implementation & Role in Project |
| :--- | :--- |
| **Graph (Adjacency List)** | Fixed-size array of `Building` vertices, each storing a `head` pointer to its connected pathways in $O(V + E)$ space[cite: 1, 7]. |
| **Singly Linked List** | `Edge` nodes storing `destID`, `distance` (meters), `isBlocked` status flag, color swatch metadata, and `next` pointer[cite: 1, 7]. |
| **Queue (FIFO)** | Custom pointer-based `RouteQueue` class executing **Breadth-First Search (BFS)** across open pathways to check connectivity and track parent nodes[cite: 1, 7]. |
| **Stack (LIFO)** | Custom pointer-based `RouteStack` class reversing the backtracked `parent[]` path from destination to source for forward route output[cite: 1, 7]. |

## Repository Structure
```text
campus-route-finder-dsa/
├── src/
│   └── campus_route_engine.cpp     # Core C++ CLI Engine (BFS + Dijkstra + Live DSA Inspector)
├── docs/                           # Project Synopsis files (.docx & .pptx)
├── index.html                      # Interactive Web Graph Visualizer (HTML/CSS/JS)
├── .gitignore
└── README.md

