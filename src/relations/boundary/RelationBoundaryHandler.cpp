#include <charconv>
#include <string>

#include "./RelationBoundaryHandler.hpp"

std::string_view RelationBoundaryHandler::get_tag(const osmium::TagList &tags, const char *key) const noexcept {
    if (auto *p = tags.get_value_by_key(key)) {
        return std::string_view{p};
    }
    return {};
}

std::string_view RelationBoundaryHandler::get_relation_member_role(const osmium::RelationMember &rel_member
) const noexcept {
    if (auto *p = rel_member.role()) {
        return std::string_view{p};
    }
    return {};
}

void RelationBoundaryHandler::relation(const osmium::Relation &rel) noexcept {
    /* Фильтруем по type=boundary */
    auto type = get_tag(rel.tags(), "type");
    if (type != "boundary") {
        return;
    }

    /* Фильтрурем по boundary = "administrative" */
    auto boundary = get_tag(rel.tags(), "boundary");
    if (boundary != "administrative") {
        return;
    }

    /* Фильтрурем по admin_level = 2-4 */
    auto admin_level_str = get_tag(rel.tags(), "admin_level");
    if (admin_level_str.empty()) {
        return;
    }

    int admin_level = 0;
    auto result = std::from_chars(admin_level_str.data(), admin_level_str.data() + admin_level_str.size(), admin_level);

    if (result.ec != std::errc{}) {
        return;
    }

    if (admin_level < 2 || admin_level > 4) {
        return;
    }

    /* Заполняем объект для сохранения */
    RelationBoundary rb;
    rb.id = rel.id();
    rb.relation_members.reserve(rel.members().size());

    for (const auto &rel_member : rel.members()) {
        // Фильтруем по type=way
        if (rel_member.type() != osmium::item_type::way) {
            continue;
        }

        auto role = get_relation_member_role(rel_member);

        // Фильтруем по role = "outer" | "inner" | ""
        if (!role.empty() && role != "outer" && role != "inner") {
            continue;
        }

        RelationBoundaryMember rbm;
        rbm.id = rel_member.ref();

        rb.relation_members.push_back(rbm);
    }

    /* Добавляем в коллекцию */
    boundaries.push_back(std::move(rb));
}
