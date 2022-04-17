#include <algorithm>
#include <cmath>
#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include <influence_map.hpp>

InfluenceMap::InfluenceMap(std::string name, std::shared_ptr<CollisionMap> collision_map, float strength, float decay, float momentum, bool collision_enabled) :
    name(name),
    collision_map(collision_map),
    strength(strength),
    decay(decay),
    momentum(momentum),
    collision_enabled(collision_enabled),
    influence_map(collision_map->get_width() * collision_map->get_height(), 0.0f),
    influence_buffer(collision_map->get_width() * collision_map->get_height(), 0.0f) {}

void InfluenceMap::add_influence(int tile_x, int tile_y) {
    if (tile_x >= 0 && tile_y >= 0 && tile_x < collision_map->get_width() && tile_y < collision_map->get_height()) {
        auto itr = std::find_if(influence_sources.begin(), influence_sources.end(), [&](const InfluenceSource& s) { return s.x == tile_x && s.y == tile_y; });
        if (itr == influence_sources.end()) {
            InfluenceSource src = {tile_x, tile_y};
            influence_sources.push_back(src);
        }
    }
}

void InfluenceMap::remove_influence(int tile_x, int tile_y) {
    if (tile_x >= 0 && tile_y >= 0 && tile_x < collision_map->get_width() && tile_y < collision_map->get_height()) {
        std::erase_if(influence_sources, [&](const InfluenceSource& s) { return s.x == tile_x && s.y == tile_y; });
    }
}

void InfluenceMap::recalculate() {
    int width = collision_map->get_width();
    int height = collision_map->get_height();
    const auto& blocked_map = collision_map->get_collision_map();

    for (const auto& src : influence_sources) {
        // first setup the sources of influence
        influence_buffer[width * src.y + src.x] = strength;
    }

    // not dealing with diagonal influence, so the distance is always 1.0
    float coefficient = expf(-1.0 * decay);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (collision_enabled && blocked_map[width * y + x]) {
                // skip blocked tiles and reset influence to zero
                influence_buffer[width * y + x] = 0.0f;
                continue;
            }

            float max_influence = 0.0f;
            float min_influence = 0.0f;

            // discover neighboring influences
            if (y > 0) {
                float tmp_influence = influence_buffer[width * (y - 1) + x] * coefficient;
                max_influence = std::max(tmp_influence, max_influence);
                min_influence = std::min(tmp_influence, min_influence);
            }
            if (y < height - 1) {
                float tmp_influence = influence_buffer[width * (y + 1) + x] * coefficient;
                max_influence = std::max(tmp_influence, max_influence);
                min_influence = std::min(tmp_influence, min_influence);
            }
            if (x > 0) {
                float tmp_influence = influence_buffer[width * y + (x - 1)] * coefficient;
                max_influence = std::max(tmp_influence, max_influence);
                min_influence = std::min(tmp_influence, min_influence);
            }
            if (x < width - 1) {
                float tmp_influence = influence_buffer[width * y + (x + 1)] * coefficient;
                max_influence = std::max(tmp_influence, max_influence);
                min_influence = std::min(tmp_influence, min_influence);
            }

            // linearly interpolate the influence based on the strongest neighbor and momentum (positive or negative)
            if (std::abs(min_influence) > max_influence) {
                influence_map[width * y + x] = std::lerp(influence_buffer[width * y + x], min_influence, momentum);
            }
            else {
                influence_map[width * y + x] = std::lerp(influence_buffer[width * y + x], max_influence, momentum);
            }
        }
    }

    // now apply changes to the buffer for the next calculation
    influence_buffer = influence_map;
}