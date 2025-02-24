//
// Created by bruno on 04/04/2021.
//

#ifndef GRAPPHS_ADJACENCY_MATRIX_H
#define GRAPPHS_ADJACENCY_MATRIX_H

#include <grapphs/graph.h>
#include <stdexcept>

namespace gpp {
    template<typename t_vertex, typename t_edge, typename t_index = default_graph_index>
    class adjacency_matrix : public graph<t_vertex, t_edge, t_index> {
    public:

        using vertex_type = typename graph<t_vertex, t_edge, t_index>::vertex_type;
        using edge_type = typename graph<t_vertex, t_edge, t_index>::edge_type;
        using index_type = typename graph<t_vertex, t_edge, t_index>::index_type;

    private:
        std::vector<vertex_type> _vertices;
        std::vector<edge_type> _edges;
    public:

        explicit adjacency_matrix(index_type size) : _vertices(size), _edges(size * size) {
        }

        explicit adjacency_matrix(graph<vertex_type, edge_type, index_type>* other)
            : _vertices(), _edges() {
            auto size = static_cast<index_type>(other->size());
            _vertices.resize(size);
            _edges.resize(size * size);
            for (index_type i = 0; i < size; ++i) {
                vertex_type vertex;
                if (other->try_get_vertex(i, vertex)) {
                    _vertices[i] = vertex;
                }
            }
            for (index_type x = 0; x < size; ++x) {
                for (index_type y = 0; y < size; ++y) {
                    edge_type edge;
                    if (other->try_get_edge(x, y, edge)) {
                        index_type i = index(x, y);
                        _edges[i] = edge;
                    }
                }
            }
        }

        index_type size() const final {
            return _vertices.size();
        }

        bool empty() const final {
            return size() == 0;
        }

        vertex_type* vertex(index_type index) final {
            return static_cast<vertex_type*>(&_vertices[index]);
        }

        const vertex_type* vertex(index_type index) const final {
            return static_cast<const vertex_type*>(&_vertices[index]);
        }

        vertex_type& operator[](index_type index) {
            return _vertices[index];
        }

        const vertex_type& operator[](index_type index) const {
            return _vertices[index];
        }

        index_type index(index_type from, index_type to) {
            return from * _vertices.size() + to;
        }

        edge_type* edge(index_type from, index_type to) final {
            return &_edges[index(from, to)];
        }

        void connect(index_type from, index_type to, edge_type edge) final {
            _edges[index(from, to)] = edge;
        }

        bool disconnect(index_type from, index_type to) final {
            _edges[index(from, to)] = edge_type();
            return true;
        }
    };

}
#endif
