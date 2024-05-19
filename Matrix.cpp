#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sstream>

using namespace std;

class Graph { // Currently directional, I'll add a parameter later to allow for undirected graphs
private:
    vector<vector<int>> adjacency_matrix;
    // To change node name to index (the nodes are inputted as strings, so we can refer
    // to them easily, e.g names. The matrix stores them as integers for efficiency,
    // hence the map and vector, to translate between the two.)
    map<string, int> node_to_index; // e.g. node_to_index["Krakow"] = 5
    vector<string> index_to_node; // e.g. index_to_node[5] = "Krakow"

    vector<bool> deleted_vertices; // Not added yet
    int number_of_vertices;

public:
    Graph() : number_of_vertices(0) {}

    void read() {   // Input node in cin, allows to enter edges via terminal or file (the latter is more convenient for large graphs/ tests)
        string input_mode, trash;
        cout << "Input mode (TER/FILE): ";
        cin >> input_mode;
        getline(cin, trash); // clearing the buffer
        if (input_mode == "TER") {
            readTerminal();
        } else if (input_mode == "FILE") {
            string filename;
            cout << "Filename: ";
            cin >> filename;
            readFile(filename);
        } else {
            cout << "Invalid input mode!" << endl;
            return;
        }
    }

    void readTerminal() { // Important: input is in the form of "node1 node2 weight" (e.g. "Krakow Warszawa 300"). It is terminated by a single 0 at the last line.
        string line;
        while (getline(cin, line)) {
            istringstream iss(line);
            string a, b;
            int weight;
            iss >> a >> b >> weight;

            if (iss.eof() && a == "0" && b.empty()) {
                break;
            }

            addEdge(a, b, weight);
        }
    }

    void readFile(const string& filename) { // Important: input is in the form of "node1 node2 weight" (e.g. "Krakow Warszawa 300"). It is terminated by a single 0 at the last line.
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Unable to open file!" << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            istringstream iss(line);
            string a, b;
            int weight;
            iss >> a >> b >> weight;

            if (iss.eof() && a == "0" && b.empty()) {
                break;
            }

            addEdge(a, b, weight);
        }
        file.close();
    }

    void addEdge(const string& a, const string& b, int weight) {
        if (node_to_index.find(a) == node_to_index.end()) { // If the node is new, add it
            node_to_index[a] = number_of_vertices++;
            index_to_node.push_back(a);
            for (auto & i : adjacency_matrix) {
                i.push_back(0);
            }
            adjacency_matrix.emplace_back(number_of_vertices, 0);
            deleted_vertices.push_back(false);
        }
        if (node_to_index.find(b) == node_to_index.end()) { // If the node is new, add it
            node_to_index[b] = number_of_vertices++;
            index_to_node.push_back(b);
            for (auto & i : adjacency_matrix) {
                i.push_back(0);
            }
            adjacency_matrix.emplace_back(number_of_vertices, 0);
            deleted_vertices.push_back(false);
        }
        adjacency_matrix[node_to_index[a]][node_to_index[b]] = weight;
        // TODO: Update for undirected graphs (based on parameter)
    }

    void printMatrix() {    // Formatted so it only prints the first letter of the node name. I'll probbably change it later, so you can pass in parameters to change it to node index or set the character amount.
        if(adjacency_matrix.empty()) {
            cout << "Empty matrix!" << endl;
            return;
        }
        cout << "Adjacency Matrix:" << endl;
        cout << "  ";
        for (const auto & i : index_to_node) {
            cout << " " << i[0];
        }
        cout << endl;
        for (int i = 0; i < adjacency_matrix.size(); ++i) {
            cout << index_to_node[i][0] << ": ";
            for (int j : adjacency_matrix[i]) {
                cout << j << " ";
            }
            cout << endl;
        }
    }

    int getNumberOfVertices() {
        return number_of_vertices;
    }

    int getWeight(const string& a, const string& b) {
        if (node_to_index.find(a) == node_to_index.end() || node_to_index.find(b) == node_to_index.end()) {
            return -1;
        }
        return adjacency_matrix[node_to_index[a]][node_to_index[b]];
    }

    int getVertexIndex(const string& node) {
        if (node_to_index.find(node) == node_to_index.end()) {
            return -1;
        }
        return node_to_index[node];
    }

    string getVertexName(int index) {
        if (index < 0 || index >= index_to_node.size()) {
            return "";
        }
        return index_to_node[index];
    }

    vector<vector<int>> getAdjacencyMatrix() {
        return adjacency_matrix;
    }
};

 // test:
int main() {
    Graph graph;
    graph.read();
    graph.printMatrix();
    return 0;
}