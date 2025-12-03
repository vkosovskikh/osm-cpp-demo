#pragma once

#include "defs/collect_phase.hpp"
#include "defs/osm_object_type.hpp"
#include "defs/osmium.hpp"

class DefaultCollector : public osmium::handler::Handler {
  public:
    DefaultCollector();

    void set_phase(CollectPhase p) noexcept { phase = p; };
    void set_writer(osmium::io::Writer *w) noexcept { writer = w; }

    void prepare_relations() noexcept {
        prepare_vector(collected_relations);
        collected_relations_prepared = true;
    }

    void prepare_ways() noexcept {
        prepare_vector(collected_ways);
        collected_ways_prepared = true;
    }

    void prepare_nodes() noexcept {
        prepare_vector(collected_nodes);
        collected_nodes_prepared = true;
    }

    bool check_relations_prepared() const noexcept { return collected_relations_prepared; }
    bool check_ways_prepared() const noexcept { return collected_ways_prepared; }
    bool check_nodes_prepared() const noexcept { return collected_nodes_prepared; }

    const auto &get_relations() const noexcept { return collected_relations; }
    const auto &get_ways() const noexcept { return collected_ways; }
    const auto &get_nodes() const noexcept { return collected_nodes; }

    void relation(const osmium::Relation &r) noexcept;
    void way(const osmium::Way &w) noexcept;
    void node(const osmium::Node &n) noexcept;

  private:
    CollectPhase phase = CollectPhase::ReadRelations;

    osmium::io::Writer *writer = nullptr;

    bool collected_relations_prepared = false; // true when unique and sorted
    bool collected_ways_prepared = false;      // same
    bool collected_nodes_prepared = false;     // same

    std::vector<osmium::object_id_type> collected_relations;
    std::vector<osmium::object_id_type> collected_ways;
    std::vector<osmium::object_id_type> collected_nodes;

    template <class T> static inline void prepare_vector(std::vector<T> &v) noexcept {
        std::sort(v.begin(), v.end());
        v.erase(std::unique(v.begin(), v.end()), v.end());
    }

    template <typename T> static inline bool contains_id(const std::vector<T> &vec, T id) noexcept {
        return std::binary_search(vec.begin(), vec.end(), id);
    }

    osmium::memory::Buffer build_filtered_relation_buffer(const osmium::Relation &rel
    ) const noexcept;
    std::string_view get_tag(const osmium::TagList &tags, const char *key) const noexcept;
    std::string_view get_relation_member_role(const osmium::RelationMember &rel_member
    ) const noexcept;
};
