#include <iostream>

#include "collectors/default_collector/default_collector.hpp"
#include "defs/osmium.hpp"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: ./osm_cpp_demo <input.osm.pbf> <output.osm.pbf>" << std::endl;
        return 1;
    }

    const char *input_file_name = argv[1];
    const char *output_file_name = argv[2];

    DefaultCollector collector;

    // -----------------------
    // PASS 1: Relations phase
    // -----------------------
    {
        collector.set_phase(CollectPhase::ReadRelations);

        osmium::io::Reader reader{input_file_name};
        osmium::apply(reader, collector);
        reader.close();

        collector.prepare_relations();
        collector.prepare_ways();
    }

    // -----------------------
    // PASS 2: Ways phase
    // -----------------------
    if (collector.check_relations_prepared() && collector.check_ways_prepared()) {
        collector.set_phase(CollectPhase::ReadWays);

        osmium::io::Reader reader{input_file_name};
        osmium::apply(reader, collector);
        reader.close();

        collector.prepare_nodes();
    }

    // -----------------------
    // PASS 3: Write result to .pbf
    // -----------------------
    if (collector.check_nodes_prepared()) {
        osmium::io::Header header;
        header.set("generator", "osm_cpp_demo");

        osmium::io::File outfile{output_file_name, "pbf"};

        osmium::io::Writer writer{outfile, header, osmium::io::overwrite::allow};
        collector.set_writer(&writer);

        collector.set_phase(CollectPhase::WriteAll);

        osmium::io::Reader reader{input_file_name};
        osmium::apply(reader, collector);
        reader.close();

        writer.close();
        collector.set_writer(nullptr);
    }

    std::cout << "Unique Relations collected: " << collector.get_relations().size() << std::endl;
    std::cout << "Unique Ways from relations: " << collector.get_ways().size() << std::endl;
    std::cout << "Unique Nodes from ways: " << collector.get_nodes().size() << std::endl;

    return 0;
}
