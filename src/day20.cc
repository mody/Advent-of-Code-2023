#include <fmt/core.h>
#include <fmt/ranges.h>

#include <boost/algorithm/string/constants.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <range/v3/algorithm/all_of.hpp>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/view/all.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/map.hpp>

#include <array>
#include <deque>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

struct Command
{
    std::string to;
    std::string from;
    unsigned value{0};
};

struct Invertor
{
    bool state{false};
    std::vector<std::string> destinations;
};

struct Conjunction
{
    std::unordered_map<std::string, unsigned> signals;
    std::vector<std::string> destinations;
};

struct Processor
{
    void initialize() noexcept;
    void start(std::function<void(Command const& cmd)> callback) noexcept;

    std::deque<Command> queue;
    std::array<uint64_t, 2> signals{0};

    std::vector<std::string> broadcaster;
    std::unordered_map<std::string, Invertor> invertors;
    std::unordered_map<std::string, Conjunction> conjunctions;
};

void Processor::initialize() noexcept
{
    auto check = [](std::vector<std::string> const& r, std::string const& id) -> bool {
        return ranges::find(r, id) != r.end();
    };

    for (auto& [id, c] : conjunctions) {
        for (auto const& [dst, i] : invertors) {
            if (check(i.destinations, id)) {
                c.signals.insert({dst, 0});
            }
        }
        for (auto const& [dst, i] : conjunctions) {
            if (check(i.destinations, id)) {
                c.signals.insert({dst, 0});
            }
        }
    }
}

void Processor::start(std::function<void(Command const& cmd)> callback) noexcept
{
    queue.clear();
    queue.push_front({"broadcaster", "button", 0});
    signals[0] += 1;
    // fmt::print("button -0-> broadcaster\n");

    while(!queue.empty()) {
        auto cmd = queue.front();
        queue.pop_front();

        if (callback) {
            callback(cmd);
        }

        if (cmd.to == "broadcaster") {
            for (auto dst : broadcaster) {
                // fmt::print("{} -{}-> {}\n", cmd.to, cmd.value, dst);
                queue.push_back({std::move(dst), cmd.to, cmd.value});
            }
            signals.at(0) += broadcaster.size();
        } else if (auto iit = invertors.find(cmd.to); iit != invertors.end()) {
            if (cmd.value == 0) {
                Invertor& i = iit->second;
                i.state = !i.state;
                const unsigned val = i.state ? 1 : 0;
                for (auto dst : i.destinations) {
                    // fmt::print("{} -{}-> {}\n", cmd.to, val, dst);
                    queue.push_back({std::move(dst), cmd.to, val});
                }
                signals.at(val) += i.destinations.size();
            }
        } else if (auto cit = conjunctions.find(cmd.to); cit != conjunctions.end()) {
            Conjunction& c = cit->second;
            c.signals.insert({cmd.from, 0}).first->second = cmd.value;
            unsigned val = ranges::all_of(c.signals, [](auto&& p) { return p.second == 1; }) ? 0 : 1;
            for (auto dst : c.destinations) {
                // fmt::print("{} -{}-> {}\n", cmd.to, val, dst);
                queue.push_back({std::move(dst), cmd.to, val});
            }
            signals.at(val) += c.destinations.size();
        }
    }
}

void part1(Processor cpu)
{
    for (unsigned i = 0; i < 1000; ++i) {
        cpu.start({});
    }
    fmt::print("1: {}\n", cpu.signals.at(0) * cpu.signals.at(1));
}

void part2(Processor cpu)
{
    std::string node;

    auto check = [](std::vector<std::string> const& r, std::string const& id) -> bool {
        return ranges::find(r, id) != r.end();
    };
    for (auto const& [id, c] : cpu.conjunctions) {
        if (check(c.destinations, "rx")) {
            node = id;
            break;
        }
    }

    const auto max_size {cpu.conjunctions.at(node).signals.size()};
    std::unordered_map<std::string, uint64_t> counters;

    uint64_t i {1};
    for (; counters.size() != max_size; ++i) {
        cpu.start([&i, &node, &counters](Command const& cmd) {
            if (cmd.value == 0 || cmd.to != node) [[likely]]
                return;
            auto it = counters.find(cmd.from);
            if (it == counters.end()) {
                counters[cmd.from] = i;
            }
        });
    }

    using namespace ranges;
    fmt::print("2: {}\n", accumulate(counters | views::drop(1), counters.begin()->second, [](uint64_t a, auto&& b) {
                   return std::lcm(a, b.second);
               }));
}

int main()
{
    Processor cpu;

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" ,"), boost::token_compress_on);

        if (parts.at(0).at(0) == 'b') {
            // broadcaster
            cpu.broadcaster = std::vector(std::next(parts.begin(), 2), parts.end());
        } else if (parts.at(0).at(0) == '%') {
            // invertors
            cpu.invertors.insert({parts.at(0).substr(1), {}}).first->second.destinations =
                std::vector(std::next(parts.begin(), 2), parts.end());
        } else if (parts.at(0).at(0) == '&') {
            // conjunctions
            cpu.conjunctions.insert({parts.at(0).substr(1), {}}).first->second.destinations =
                std::vector(std::next(parts.begin(), 2), parts.end());
        }
    }
    cpu.initialize();

    part1(cpu);
    part2(cpu);

    return 0;
}
