//
// Created by Shan on 2020-10-19.
//

#include <ctime>
#include <omp.h>
#include <vector>
#include <fstream>
#include <filesystem>
#include <iterator>
#include <iostream>
#include <chrono>
#include <sstream>
#include <queue>

const char CSV_SEPARATOR = ';';

enum COLOR : int {
    UNINITIALIZED = -1,
    RED = 1,
    GREEN = 2
};

class Graph {

public:
    /// Constructor
    Graph();

    /// Shows graph's matrix
    void show_graph_matrix();

    /// Loads graph, based on edges and using CSV file format.
    /// \param input_csv_file path to csv file to load
    void load_graph_edges_from_csv(const std::string &input_csv_file);

    void load_graph_matrix_from_csv(const std::string &input_csv_file);

    /// Checks whether graph is fully connected (in other words - is every node connected to every other node)
    bool is_fully_connected();

    /// Checks whether graph is bipartite
    bool is_bipartite(int source_node);

private:
    /// Maximum size of the matrix being considered
    static const unsigned int MAX_MATRIX_SIZE = 30;
    /// Matrix to be searched
    int matrix[MAX_MATRIX_SIZE][MAX_MATRIX_SIZE] = {0};
    /// List of the edges and it's content
    int edges[(int) (MAX_MATRIX_SIZE * MAX_MATRIX_SIZE)][2] = {0};
    /// How many edges has already been loaded
    int edge_count = 0;
    /// ?
    int highest_node_present = 0;
    std::vector<int> nodes;

    /// Adds edge to graph, right now without taking into account direction of the graph
    /// \param vertex_u First vertex
    /// \param vertex_v Second vertex
    void add_edge(int vertex_u, int vertex_v);

    /// Function for traversing whole graph by specified vertex 'u'
    /// \param vertex_u vertex by which graph will be traversed
    /// \param visited reference to boolean list which contains information about whether specified vertex has been visited or not
    /// \returns by reference - visited vertexes
    void traverse_graph(int vertex_u, bool *visited);

    void store_highest_node_present(int vertex_u, int vertex_v);

    bool self_reference_in_matrix(int u) const;

    static std::vector<int> get_csv_row(const std::string &line) ;
};


void Graph::add_edge(int vertex_u, int vertex_v) {
    edges[edge_count][0] = vertex_u;
    edges[edge_count][1] = vertex_v;
    edge_count++;
    matrix[vertex_u][vertex_v] = 1;
    matrix[vertex_v][vertex_u] = 1;
    store_highest_node_present(vertex_u, vertex_v);
}

void Graph::store_highest_node_present(int vertex_u, int vertex_v) {
    highest_node_present = vertex_v > highest_node_present ? vertex_v : highest_node_present;
    highest_node_present = vertex_u > highest_node_present ? vertex_u : highest_node_present;
}

void Graph::show_graph_matrix() {
    int i, j;
    for (i = 0; i <= highest_node_present; i++) {
        for (j = 0; j <= highest_node_present; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

void Graph::load_graph_edges_from_csv(const std::string &input_csv_file) {
    std::ifstream data(input_csv_file);
    std::string line;
    while (std::getline(data, line)) {
        std::vector<int> row = get_csv_row(line);
        add_edge(row[0], row[1]);
    }
}

std::vector<int> Graph::get_csv_row(const std::string &line) {
    std::vector<int> row;
    std::stringstream basic_stringstream(line);
    std::string cell;
    while (std::getline(basic_stringstream, cell, CSV_SEPARATOR)) {
        row.push_back(std::stoi(cell));
    }
    return row;
}

void Graph::traverse_graph(int vertex_u, bool *visited) {
    visited[vertex_u] = true;
    for (int vertex_v = 0; vertex_v < edge_count; vertex_v++) {
        if (matrix[vertex_u][vertex_v]) {
            if (!visited[vertex_v])
                traverse_graph(vertex_v, visited);
        }
    }
}


bool Graph::is_fully_connected() {
    bool *already_visited = new bool[edge_count];
    for (int vertex_u = 0; vertex_u < edge_count; vertex_u++) {
        for (int i = 0; i < edge_count; i++) {
            already_visited[i] = false;
        }
        traverse_graph(vertex_u, already_visited);
        for (int i = 0; i < edge_count; i++) {
            if (!already_visited[i]) {
                return false;
            }
        }
    }
    return true;
}

bool Graph::is_bipartite(int source_node) {
    COLOR color_array[highest_node_present];
    for (int i=0; i<highest_node_present; i++) {
        color_array[i] = UNINITIALIZED;
    }
    color_array[source_node] = RED;
    std::queue<int> color_queue;
    color_queue.push(source_node);
    while (!color_queue.empty()) {
        int current_source = color_queue.front();
        color_queue.pop();
        if (self_reference_in_matrix(current_source)) {
            return false;
        }
        for (int current_target = 0; current_target < highest_node_present; current_target++) {
            printf("Matrix[current_source][current_target] = %d,"
                   "color_array[current_target] = %d,"
                   "color_array[current_source] = %d\n",
                   matrix[current_source][current_target],
                   color_array[current_target],
                   color_array[current_source]);
            if (matrix[current_source][current_target] && color_array[current_target] != UNINITIALIZED) {
                color_array[current_target] = color_array[current_source] == RED ? GREEN : RED;
                color_queue.push(current_target);
            }
            else if (matrix[current_source][current_target] && color_array[current_target] == color_array[current_source]) {
                return false;
            }
        }
    }
    return true;
}

bool Graph::self_reference_in_matrix(int u) const { return matrix[u][u] == 1; }

void Graph::load_graph_matrix_from_csv(const std::string &input_csv_file) {
    std::ifstream data(input_csv_file);
    std::string line;
    int current_row = 0;
    while (std::getline(data, line)) {
        std::vector<int> row = get_csv_row(line);
        for (int current_column=0; current_column<row.size(); current_column++) {
            matrix[current_row][current_column] = row[current_column];
        }
        current_row++;
    }

}


Graph::Graph() = default;


int main() {
    Graph graph = Graph();
    graph.load_graph_edges_from_csv(R"(W:\ZAiSD\Zad1\out.csv)");
    graph.show_graph_matrix();
    std::printf("Is fully connected?: %s\n", graph.is_fully_connected() ? "true" : "false");
    std::printf("Is bipartite?: %s", graph.is_bipartite(0) ? "true" : "false");
    return 0;
}