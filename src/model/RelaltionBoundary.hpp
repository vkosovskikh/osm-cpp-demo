#pragma once

#include <osmium/osm/relation.hpp>
#include <osmium/osm/types.hpp>
#include <vector>

#include "./RelationBoundaryMember.hpp"

struct RelationBoundary {
    osmium::object_id_type id;
    std::vector<RelationBoundaryMember> relation_members;
};
