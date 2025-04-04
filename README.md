# Dijkstra Algorithm on Luxembourg Map

This C++ application implements Dijkstra's algorithm to find the shortest path between two nodes on a map of Luxembourg. It visualizes the map, allowing the user to select two nodes and compute the shortest path based on the edges and costs provided.

## Key Features

### 1. **Graph Representation of Luxembourg**
- The map is represented as a graph where each node corresponds to a geographical location (latitude and longitude) and each edge represents a road between two locations.
- The map data is loaded from an XML file (`Harta_Luxemburg.xml`) containing nodes and arcs with associated costs (distances or travel times).

### 2. **Dijkstra's Algorithm**
- The application computes the shortest path between two nodes using **Dijkstra's algorithm**.
- It reads the graph data, builds an adjacency list, and applies Dijkstra's algorithm to find the minimum distance between the selected nodes.

### 3. **Interactive Map**
- The user can interact with the map, click to select two nodes (source and destination), and the application will compute and display the shortest path.
- The map is drawn in a **Qt GUI** where nodes are represented by circles, and edges by lines.
- The computed shortest path is highlighted on the map.

### 4. **File Handling**
- The adjacency list of the graph and the computed shortest path are saved to text files (`AdjacencyList.txt`, `NodesList.txt`) for further analysis.

### 5. **Graphical User Interface (GUI)**
- Built using **Qt** (with `QGraphicsScene`, `QGraphicsView`, and custom `QWidget` for the map).
- The application shows the map of Luxembourg with nodes and edges drawn, along with an interactive interface for selecting the nodes.

### 6. **Real-time Interaction**
- On selecting two nodes, the application computes and displays the shortest path in real-time.

## How It Works

### **Components:**

- **MainWindow**: The main window of the application, providing the user interface.
- **MapWidget**: Handles the map display and user interaction. It loads the map data from the XML file, visualizes nodes and edges, and computes the shortest path using Dijkstra’s algorithm.

### **Dijkstra’s Algorithm**:
- The algorithm uses a priority queue to explore the graph and compute the shortest path between the two selected nodes.
- The adjacency list and the edge costs are used for calculating the shortest path.

### **Map Data**:
- The map data is represented in an XML file containing `<node>` and `<arc>` elements.
- Each node has geographical coordinates (latitude and longitude), and each arc has a source, target, and cost (e.g., distance between nodes).

## Technologies Used
- **C++**
- **Qt Framework (Qt5/Qt6)**
- **Dijkstra’s Algorithm** for pathfinding
- **XML** for map data storage

