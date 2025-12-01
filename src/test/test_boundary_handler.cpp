#include <iostream>
#include <osmium/handler.hpp>

#include <osmium/io/pbf_input.hpp>
#include <osmium/io/reader.hpp>
#include <osmium/visitor.hpp>

#include "../relations/boundary/RelationBoundaryHandler.hpp"

int main() {
    const char *filename = "osm_source/liechtenstein-251130.osm.pbf";

    try {
        osmium::io::Reader reader{filename};

        RelationBoundaryHandler handler;

        osmium::apply(reader, handler);
        reader.close();

        const auto &boundaries = handler.get_boundaries();

        std::cout << "Found boundaries: " << boundaries.size() << "\n";

        for (const auto &b : boundaries) {
            std::cout << "Relation " << b.id << " with " << b.relation_members.size() << " way-members\n";
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
