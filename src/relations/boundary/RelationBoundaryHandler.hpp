#pragma once

#include <osmium/handler.hpp>
#include <osmium/osm/relation.hpp>
#include <string>
#include <vector>

#include "../../model/RelaltionBoundary.hpp"

class RelationBoundaryHandler : public osmium::handler::Handler {
  public:
    RelationBoundaryHandler() = default;

    void relation(const osmium::Relation &rel) noexcept;

    const std::vector<RelationBoundary> &get_boundaries() const noexcept { return boundaries; }

  private:
    std::vector<RelationBoundary> boundaries;
    std::string_view get_tag(const osmium::TagList &tags, const char *key) const noexcept;
    std::string_view get_relation_member_role(const osmium::RelationMember &rel_member) const noexcept;
};
