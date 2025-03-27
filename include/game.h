#pragma once
#include <fstream>
#include <iostream>
#include <random>
#include <vector>
#include <string>
#include <sstream>

class Ship {
public:

    uint8_t size;
    bool horizontal;
    uint64_t x;
    uint64_t y;

    Ship(uint8_t size, bool horizontal, uint64_t x, uint64_t y);
};

class Game {
public:

    bool role = false;
    bool master = false;

    bool not_bot_shot = false;
    bool awaiting_result = false;
    uint64_t killCount = 0;

    bool ordered = false;
    bool status = false;

    uint64_t height = 1;
    uint64_t width = 0;
    std::vector<std::vector<char>> field;

    uint64_t currentX = 0;
    uint64_t currentY = 0;

    uint64_t ships[4] = {0};
    std::vector<Ship> shipsList;
    uint64_t remainingShips = 0;

    std::string in_path;
    std::string out_path;
    bool loaded = false;

    void display();

    bool is_game_finished();

    bool is_player_winner();

    bool is_player_loser();

    void calculate_remaining_ships();

    bool dump();

    bool load();

    void create_matrix();

    bool can_generate();

    bool can_generate(uint64_t width, uint64_t height, uint64_t ships[4]);

    void generate();

    bool can_place(Ship &ship);

    void place(Ship &ship);

    void random_place();

    bool is_ship_destroyed(Ship& ship);

    std::string process_shot(uint64_t x, uint64_t y);

    std::pair<uint64_t, uint64_t> get_bot_shot_custom();

    std::pair<uint64_t, uint64_t> get_bot_shot_ordered();

};