#include "point2d.h"

#include <boost/graph/graph_concepts.hpp>
#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

#ifndef __clang__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

#include <boost/graph/stoer_wagner_min_cut.hpp>

#ifndef __clang__
#pragma GCC diagnostic pop
#endif

#include <range/v3/view/all.hpp>

#include <cassert>
#include <iostream>
#include <range/v3/view/drop.hpp>
#include <string>
#include <unordered_map>
#include <vector>

using Point = Gfx_2d::Point<int>;

using EdgeWeightProp = boost::property<boost::edge_weight_t, unsigned>;
using Graph = boost::adjacency_list<
    /*vertex storage*/ boost::vecS,
    /*edge storage*/ boost::vecS,
    /*graph type*/ boost::undirectedS,
    /*vertex properties*/ boost::no_property,
    /*edge properties*/ EdgeWeightProp>;
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

int main()
{
    Graph g;
    std::unordered_map<std::string, Vertex> node_to_vertex;

    auto get_vertex = [&g, &node_to_vertex](std::string const& name) -> Vertex {
        auto [it, inserted] = node_to_vertex.insert({name, Vertex{}});
        if (inserted) {
            it->second = boost::add_vertex(g);
        }
        return it->second;
    };

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(": "), boost::token_compress_on);

        Vertex from {get_vertex(parts.at(0))};

        for (auto const& dst : parts | ranges::views::drop(1)) {
            boost::add_edge(from, get_vertex(dst), EdgeWeightProp{1u}, g);
        }
    }

    auto parity = boost::make_one_bit_color_map(num_vertices(g), get(boost::vertex_index, g));
    const auto weights = get(boost::edge_weight, g);

    int w = boost::stoer_wagner_min_cut(g, weights, boost::parity_map(parity));
    assert(w == 3);

    unsigned left {0}, right {0};
    for (auto const& vx : boost::make_iterator_range(boost::vertices(g))) {
        if (boost::get(parity, vx)) {
            ++right;
        } else {
            ++left;
        }
    }

    fmt::print("1: {}\n", left * right);

    return 0;
}
