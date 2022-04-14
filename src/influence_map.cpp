#include <algorithm>
#include <cmath>
#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include <influence_map.hpp>

InfluenceMap::InfluenceMap(std::string name, int width, int height, int tile_size, float strength, float decay, float momentum) :
    name(name),
    width(width),
    height(height),
    tile_size(tile_size),
    strength(strength),
    decay(decay),
    momentum(momentum),
    influence_map(width * height, 0.0f),
    influence_buffer(width * height, 0.0f) {}

void InfluenceMap::draw(int start_x, int start_y, int end_x, int end_y) {
    for (int y = std::max(start_y / tile_size, 0); y < height && y * tile_size < end_y; y++) {
        for (int x = std::max(start_x / tile_size, 0); x < width && x * tile_size < end_x; x++) {
            if (influence_map[width * y + x] > 0.0f) {
                al_draw_filled_rectangle(x * tile_size + 1 - start_x, y * tile_size + 1 - start_y,
                        (x + 1) * tile_size - 1 - start_x, (y + 1) * tile_size - 1 - start_y,
                        al_map_rgba(0, 0, 225, influence_map[width * y + x] * 255 / strength));
            }
        }
    }
}

void InfluenceMap::add_influence(int tile_x, int tile_y) {
    if (tile_x >= 0 && tile_y >= 0 && tile_x < width && tile_y < height) {
        auto itr = std::find_if(influence_sources.begin(), influence_sources.end(), [&](const InfluenceSource& s) { return s.x == tile_x && s.y == tile_y; });
        if (itr == influence_sources.end()) {
            InfluenceSource src = {tile_x, tile_y};
            influence_sources.push_back(src);
        }
    }
}

void InfluenceMap::remove_influence(int tile_x, int tile_y) {
    if (tile_x >= 0 && tile_y >= 0 && tile_x < width && tile_y < height) {
        std::erase_if(influence_sources, [&](const InfluenceSource& s) { return s.x == tile_x && s.y == tile_y; });
    }
}

void InfluenceMap::recalculate() {
    for (const auto& src : influence_sources) {
        // first setup the sources of influence
        influence_buffer[width * src.y + src.x] = strength;
    }

    // not dealing with diagonal influence, so the distance is always 1.0
    float coefficient = expf(-1.0 * decay);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
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

void InfluenceMap::print_values() {
    std::cout << "Printing influence map values..." << std::endl;
    std::cout << "{" << std::endl;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            std::cout << "(" << x << ", " << y << "), " << influence_map[width * y + x];
        }
        std::cout << std::endl;
    }
    std::cout << "}" << std::endl;
}