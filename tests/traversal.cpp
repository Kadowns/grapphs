#include <grapphs/adjacency_list.h>
#include <grapphs/algorithms/bfs_traversal.h>
#include <grapphs/algorithms/dfs_traversal.h>
#include <grapphs/algorithms/rlo_traversal.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <cstdint>
#include <chrono>
#include <random>
#include <fstream>
#include <ostream>
#include <grapphs/tests/traversal_order.h>

typedef std::pair<std::size_t, std::size_t> edge_identifier;

void print_order(const std::vector<size_t>& order) {
    std::stringstream vertices;
    for (int i = 0; i < order.size(); ++i) {
        vertices << order[i];
        if (i != order.size() - 1) {
            vertices << ", ";
        }
    }
    GTEST_LOG_(INFO) << vertices.str();
}

void print_order(const std::vector<std::pair<size_t, size_t>>& order) {
    std::stringstream vertices;
    for (int i = 0; i < order.size(); ++i) {
        const std::pair<size_t, size_t>& pair = order[i];
        vertices << pair.first << "->" << pair.second;
        if (i != order.size() - 1) {
            vertices << ", ";
        }
    }
    GTEST_LOG_(INFO) << vertices.str();
}

gpp::adjacency_list<int, bool> build_traversal_graph() {
    gpp::adjacency_list<int, bool> graph;
    for (int i = 0; i <= 9; ++i) {
        graph.push(0);
    }
    graph.connect(0, 1, true);
    graph.connect(1, 2, true);
    graph.connect(1, 3, true);
    graph.connect(2, 6, true);
    graph.connect(3, 4, true);
    graph.connect(3, 5, true);
    graph.connect(6, 7, true);
    graph.connect(6, 8, true);
    graph.connect(6, 9, true);
    return graph;
}

template<gpp::traversal_order order, typename t_entry_point>
void test_graph_order(
    gpp::expected_traversal_order& expected,
    const gpp::adjacency_list<int, bool>& graph,
    t_entry_point entryPoint
) {
    size_t vertexIndex = 0;
    size_t edgeIndex = 0;

    std::vector<size_t> receivedVertices;
    std::vector<edge_identifier> receivedEdges;

    gpp::traverse<gpp::adjacency_list<int, bool>, order>(
        graph,
        entryPoint,
        [&](size_t vertex) {
            bool isCorrect = expected.pop(vertex);
            EXPECT_TRUE(isCorrect)
                            << "Vertex " << vertex << " was not allowed for step #" << vertexIndex
                            << " (" << expected.list_available() << ")";
            receivedVertices.emplace_back(vertex);
            vertexIndex++;
        },
        [&](size_t from, size_t to) {
            receivedEdges.emplace_back(from, to);
            edgeIndex++;
        }
    );
}

template<gpp::traversal_order order, typename t_entry_point = std::size_t>
void test_order(gpp::expected_traversal_order& expected, t_entry_point entryPoint = 0) {
    gpp::adjacency_list<int, bool> graph = build_traversal_graph();
    test_graph_order<order, t_entry_point>(expected, graph, entryPoint);
}

TEST(grapphs, traversal_breadth_order) {
    gpp::expected_traversal_order order = {
        0
    };
    order.then({1})
         ->then({2, 3})
         ->then({4, 5, 6})
         ->then({7, 8, 9});

    test_order<gpp::traversal_order::BREADTH>(order, 0);
}

TEST(grapphs, traversal_breadth_order_multiple_entrypoints) {
    gpp::expected_traversal_order order = {
        0,
        1
    };
    order.then({2, 3})
         ->then({4, 5, 6})
         ->then({7, 8, 9});
    gpp::adjacency_list<int, bool> graph;
    for (int i = 0; i <= 9; ++i) {
        graph.push(0);
    }
    graph.connect(0, 2, true);
    graph.connect(1, 2, true);
    graph.connect(1, 3, true);
    graph.connect(2, 6, true);
    graph.connect(3, 4, true);
    graph.connect(3, 5, true);
    graph.connect(6, 7, true);
    graph.connect(6, 8, true);
    graph.connect(6, 9, true);

    std::set<std::size_t> startingPoints = {
        0,
        1
    };
    test_graph_order<gpp::traversal_order::BREADTH>(order, graph, startingPoints);
}

TEST(grapphs, traversal_depth_order) {
    gpp::expected_traversal_order order = {
        0
    };
    order.then({1})
         ->then({2})
         ->then({6})
         ->then({7, 8, 9})
         ->then({3})
         ->then({4, 5});

    test_order<gpp::traversal_order::DEPTH>(order, 0);
}

TEST(grapphs, reverse_level_order_traversal) {
    gpp::adjacency_list<int, bool> graph = build_traversal_graph();
    gpp::expected_traversal_order expected = {
        7, 8, 9, 4, 5
    };

    expected.then({6, 3})
            ->then({2})
            ->then({1})
            ->then({0});
    std::size_t vertexIndex = 0;
    gpp::reverse_level_order_traverse<decltype(graph)>(
        graph,
        0,
        [&](size_t vertex) {
            bool isCorrect = expected.pop(vertex);
            EXPECT_TRUE(isCorrect) << "Vertex " << vertex << " was not allowed for step #"
                                   << vertexIndex
                                   << "(" << expected.list_available() << ")";
            GTEST_LOG_(INFO) << "#" << vertexIndex << ": " << vertex;
            vertexIndex++;
        },
        [&](size_t from, size_t to) {
            GTEST_LOG_(INFO) << from << " -> " << to;
        }
    );
}
