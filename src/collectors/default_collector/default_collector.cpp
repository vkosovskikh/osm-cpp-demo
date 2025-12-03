#include <charconv>
#include <iostream>
#include <string>
#include <vector>

#include "./default_collector.hpp"

static constexpr std::size_t BUFFER_SIZE = 4096;

DefaultCollector::DefaultCollector() {
    collected_relations.reserve(20'000);
    collected_ways.reserve(200'000);
    collected_nodes.reserve(2'000'000);
}

std::string_view
DefaultCollector::get_tag(const osmium::TagList &tags, const char *key) const noexcept {
    if (auto *p = tags.get_value_by_key(key)) {
        return std::string_view{p};
    }
    return {};
}

std::string_view DefaultCollector::get_relation_member_role(const osmium::RelationMember &rel_member
) const noexcept {
    if (auto *p = rel_member.role()) {
        return std::string_view{p};
    }
    return {};
}

osmium::memory::Buffer DefaultCollector::build_filtered_relation_buffer(const osmium::Relation &rel
) const noexcept {
    osmium::memory::Buffer buffer{BUFFER_SIZE, osmium::memory::Buffer::auto_grow::yes};

    {
        // scope for relation_builder
        osmium::builder::RelationBuilder relation_builder{buffer};

        /* copy metadata */
        relation_builder.set_id(rel.id());
        relation_builder.set_version(rel.version());
        relation_builder.set_changeset(rel.changeset());
        relation_builder.set_uid(rel.uid());
        relation_builder.set_timestamp(rel.timestamp());
        relation_builder.set_visible(rel.visible());

        if (!rel.user_is_anonymous()) {
            relation_builder.set_user(rel.user());
        }

        /* copy tags */
        {
            osmium::builder::TagListBuilder tag_builder{relation_builder};

            for (const auto &tag : rel.tags()) {
                tag_builder.add_tag(tag.key(), tag.value());
            }
        }

        /* copy filtered members */
        {
            osmium::builder::RelationMemberListBuilder member_builder{relation_builder};

            for (const auto &member : rel.members()) {
                if (member.type() == osmium::item_type::way) {

                    if (!contains_id(collected_ways, member.ref()))
                        continue;

                    const char *role = member.role();
                    if (!role)
                        role = "";

                    member_builder.add_member(osmium::item_type::way, member.ref(), role);
                    continue;
                }

                if (member.type() == osmium::item_type::relation) {

                    if (!contains_id(collected_relations, member.ref()))
                        continue;

                    const char *role = member.role();
                    if (!role)
                        role = "";

                    member_builder.add_member(osmium::item_type::relation, member.ref(), role);
                    continue;
                }
            }
        }
    }

    buffer.commit();

    return buffer;
}

void DefaultCollector::relation(const osmium::Relation &rel) noexcept {
    if (phase == CollectPhase::WriteAll && writer) {
        if (contains_id(collected_relations, rel.id())) {
            auto buffer = build_filtered_relation_buffer(rel);

            (*writer)(std::move(buffer));
        }

        return;
    }

    if (phase != CollectPhase::ReadRelations) {
        return;
    }

    /* Фильтруем по type=boundary */
    const auto type = get_tag(rel.tags(), "type");
    if (type != "boundary") {
        return;
    }

    /* Фильтрурем по boundary = "administrative" */
    const auto boundary = get_tag(rel.tags(), "boundary");
    if (boundary != "administrative") {
        return;
    }

    /* Фильтрурем по admin_level = 2-4 */
    const auto admin_level_str = get_tag(rel.tags(), "admin_level");
    if (admin_level_str.empty()) {
        return;
    }

    int admin_level = 0;
    const auto result = std::from_chars(
        admin_level_str.data(),
        admin_level_str.data() + admin_level_str.size(),
        admin_level
    );

    if (result.ec != std::errc{}) {
        return;
    }

    if (admin_level < 2 || admin_level > 4) {
        return;
    }

    /* Cохраняем relation и его ways */
    collected_relations.push_back(rel.id());

    for (const auto &rel_member : rel.members()) {
        // Фильтруем members по type=way
        if (rel_member.type() != osmium::item_type::way) {
            continue;
        }

        const auto role = get_relation_member_role(rel_member);

        // Фильтруем по role = "outer" | "inner" | ""
        if (!role.empty() && role != "outer" && role != "inner") {
            continue;
        }

        collected_ways.push_back(rel_member.ref());
    }
}

void DefaultCollector::way(const osmium::Way &way) noexcept {
    if (phase == CollectPhase::WriteAll && writer) {
        if (contains_id(collected_ways, way.id())) {
            (*writer)(way);
        }

        return;
    }

    if (phase != CollectPhase::ReadWays) {
        return;
    }

    if (!contains_id(collected_ways, way.id())) {
        return;
    }

    for (const auto &way_node : way.nodes()) {
        collected_nodes.push_back(way_node.ref());
    }
}

void DefaultCollector::node(const osmium::Node &node) noexcept {
    if (phase == CollectPhase::WriteAll && writer) {
        if (contains_id(collected_nodes, node.id())) {
            (*writer)(node);
        }
    }
}
