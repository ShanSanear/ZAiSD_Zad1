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
    RED = 0,
    GREEN = 1
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

    /// Loads matrix from CSV file
    void load_graph_matrix_from_csv(const std::string &input_csv_file);

    void load_graph_edges_from_stdin();

    void load_graph_matrix_from_stdin();

    /// Checks whether graph is fully connected (in other words - is every node connected to every other node)
    bool is_fully_connected();

    /// Checks whether graph is bipartite
    bool is_bipartite();

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
    /// \param bidirectional is edge bidirectional
    void add_edge(int vertex_u, int vertex_v, bool bidirectional);

    void store_highest_node_present(int vertex_u, int vertex_v);

    bool self_reference_in_matrix(int u) const;

    static std::vector<int> get_csv_row(const std::string &line);

    bool color_it(std::vector<COLOR> &color_array, int position, COLOR color);
};


void Graph::add_edge(int vertex_u, int vertex_v, bool bidirectional) {
    edges[edge_count][0] = vertex_u;
    edges[edge_count][1] = vertex_v;
    edge_count++;
    matrix[vertex_u][vertex_v] = 1;
    if (bidirectional) {
        matrix[vertex_v][vertex_u] = 1;
    }
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
        add_edge(row[0], row[1], false);
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

bool Graph::is_fully_connected() {
    for (int vertex_u = 0; vertex_u < highest_node_present; vertex_u++) {
        for (int vertex_v = 0; vertex_v < highest_node_present; vertex_v++) {
            if (vertex_v == vertex_u) {
                continue;
            }
            if (!(matrix[vertex_u][vertex_v] == 1 || matrix[vertex_v][vertex_u] == 1)) {
                return false;
            }
        }
    }
    return true;
}

bool Graph::color_it(std::vector<COLOR> &color_array, int position, COLOR color) {
    if (color_array[position] != UNINITIALIZED && color_array[position] != color) {
        return false;
    }
    color_array[position] = color;
    COLOR neighbours_color = color == RED ? GREEN : RED;
    bool answer = true;
    for (int neighbour_node = 0; neighbour_node <= highest_node_present; neighbour_node++) {
        if (matrix[position][neighbour_node]) {
            if (color_array[neighbour_node] == UNINITIALIZED) {
                answer &= color_it(color_array, neighbour_node, neighbours_color);
            }

            if (color_array[neighbour_node] != UNINITIALIZED
                && color_array[neighbour_node] != neighbours_color) {
                return false;
            }
        }
        if (!answer) {
            return false;
        }
    }

    return true;
}


bool Graph::is_bipartite() {
    std::vector<COLOR> color_array(highest_node_present+1);
    // No way for graph to be bipartite when it has self-reference
    for (int current_node_index = 0; current_node_index <= highest_node_present; current_node_index++) {
        if (self_reference_in_matrix(current_node_index)) {
            return false;
        }
    }
    // Initialize color array
    for (int i = 0; i < highest_node_present; i++) {
        color_array[i] = UNINITIALIZED;
    }
    bool answer = color_it(color_array, 0, GREEN);
    for (COLOR col : color_array) {
        printf("Color:%d\n", col);
    }
    return answer;
}

bool Graph::self_reference_in_matrix(int u) const { return matrix[u][u] == 1; }


void Graph::load_graph_matrix_from_csv(const std::string &input_csv_file) {
    std::ifstream data(input_csv_file);
    std::string line;
    int current_row = 0;
    while (std::getline(data, line)) {
        std::vector<int> row = get_csv_row(line);
        for (int current_column = 0; current_column < row.size(); current_column++) {
            matrix[current_row][current_column] = row[current_column];
        }
        current_row++;
    }
}

void Graph::load_graph_matrix_from_stdin() {
    std::string line;
    std::vector<std::vector<int>> input_data;
    int num_of_vectors;
    std::getline(std::cin, line);
    num_of_vectors = std::stoi(line);
    for (int i = 0; i < num_of_vectors; i++) {
        std::vector<int> row;
        std::getline(std::cin, line);
        std::stringstream basic_stringstream(line);
        std::string provided_number;
        int j = 0;
        while (std::getline(basic_stringstream, provided_number, ' ')) {
            int number = std::stoi(provided_number);
            matrix[i][j] = number;
            if (number == 1) {
                add_edge(i, j, false);
            }
            j++;
        }
    }
}

void Graph::load_graph_edges_from_stdin() {
    std::string line;

    std::vector<std::vector<int>> input_data;
    int num_of_lines;
    std::getline(std::cin, line);
    num_of_lines = std::stoi(line);
    for (int i = 0; i < num_of_lines; i++) {
        std::vector<int> row;
        std::getline(std::cin, line);
        std::stringstream basic_stringstream(line);
        std::string provided_number;
        while (std::getline(basic_stringstream, provided_number, ' ')) {
            row.push_back(std::stoi(provided_number));
        }
        add_edge(row[0], row[1], false);
    }
}


Graph::Graph() = default;


int main(int argc, char **argv) {
    Graph graph = Graph();
//    graph.load_graph_edges_from_csv(R"(W:\ZAiSD\Zad1\out.csv)");
//    graph.load_graph_matrix_from_stdin();
    graph.load_graph_edges_from_stdin();
    graph.show_graph_matrix();
    std::printf("Is fully connected?: %s\n", graph.is_fully_connected() ? "true" : "false");
    std::printf("Is bipartite?: %s", graph.is_bipartite() ? "true" : "false");
    return 0;
}