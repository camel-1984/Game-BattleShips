#include "include/game.h"

    Ship::Ship(uint8_t size, bool horizontal, uint64_t x, uint64_t y) :
    size(size), horizontal(horizontal), x(x), y(y){}

    void Game::display() {
        for (std::vector<char> row : field) {
            for (char cell : row)
                std::cout << cell << " ";
            std::cout << "\n";
        }
    }

    bool Game::is_game_finished() {
        return is_player_loser() || is_player_winner();
    }

    bool Game::is_player_winner() {
        return remainingShips == 0;
    }

    bool Game::is_player_loser() {
        return killCount == (ships[0] + ships[1] + ships[2]
            + ships[3]) && remainingShips > 0;
    }

    void Game::calculate_remaining_ships() {
        remainingShips = 0;
        for (std::vector<char> &row : field) {
            for (char cell : row) {
                if (cell == 'S') remainingShips++;
            }
        }
    }

    bool Game::dump() {
        std::ofstream outFile(in_path);
        if (!outFile.is_open()) {
            return false;
        }

        outFile << width << " " << height << "\n";

        for (Ship &ship : shipsList) {
            outFile << static_cast<int>(ship.size) << " "
                    << (ship.horizontal ? "h" : "v") << " "
                    << ship.x << " " << ship.y << "\n";
        }

        outFile.close();
        return true;
    }

    bool Game::load() {
        std::ifstream inFile(out_path);
        if (!inFile.is_open()) {
            return false;
        }

        shipsList.clear();

        inFile >> width >> height;
        create_matrix();

        std::string line;
        std::getline(inFile, line);
        while (std::getline(inFile, line)) {
            std::istringstream iss(line);
            int size;
            char orientation;
            uint64_t x, y;

            if (!(iss >> size >> orientation >> x >> y)) {
                return false;
            }

            bool horizontal = (orientation == 'h');
            Ship ship(size, horizontal, x, y);
            if (can_place(ship)) {
                shipsList.push_back(ship);
                place(ship);
            } else {
                return false;
            }
        }

        inFile.close();
        loaded = true;
        return true;
    }

    void Game::create_matrix() {

        field.resize(height, std::vector<char>(width, '.'));
    }

    bool Game::can_generate() {
        uint64_t totalCells = width * height;

        uint64_t requiredCells = 0;
        requiredCells += ships[0] * 1;
        requiredCells += ships[1] * 2;
        requiredCells += ships[2] * 3;
        requiredCells += ships[3] * 4;

        uint64_t paddingCells = requiredCells / 2;

        return (requiredCells + paddingCells) <= totalCells;
    }

    bool Game::can_generate(uint64_t width, uint64_t height, uint64_t ships[4]) {
        uint64_t totalCells = width * height;

        uint64_t requiredCells = 0;
        requiredCells += ships[0] * 1;
        requiredCells += ships[1] * 2;
        requiredCells += ships[2] * 3;
        requiredCells += ships[3] * 4;

        uint64_t paddingCells = requiredCells / 2;

        return (requiredCells + paddingCells) <= totalCells;
    }

    void Game::generate() {

        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<uint64_t> distWidthHeight(1, 100);
        std::uniform_int_distribution<uint64_t> distOneDeckShips(1, 100);
        std::uniform_int_distribution<uint64_t> distTwoDeckShips(0, 100);
        std::uniform_int_distribution<uint64_t> distThreeDeckShips(0, 100);
        std::uniform_int_distribution<uint64_t> distFourDeckShips(0, 100);

        while (true) {

            width = distWidthHeight(gen);
            height = distWidthHeight(gen);
            ships[0] = distOneDeckShips(gen);
            ships[1] = distTwoDeckShips(gen);
            ships[2] = distThreeDeckShips(gen);
            ships[3] = distFourDeckShips(gen);

            if (can_generate()) {
                create_matrix();
                break;
            }
        }
    }

    bool Game::can_place(Ship &ship) {

        if (ship.horizontal) {
            if (ship.x + ship.size > width) return false;


            for (int i = -1; i <= ship.size; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    uint64_t checkX = ship.x + i;
                    uint64_t checkY = ship.y + j;
                    if (checkX >= 0 && checkX < width && checkY >= 0 && checkY < height) {
                        if (i >= 0 && i < ship.size && j == 0) {

                            if (field[ship.y][ship.x + i] != '.') return false;
                        } else {
                            if (field[checkY][checkX] != '.') return false;
                        }
                    }
                }
            }
        } else {

            if (ship.y + ship.size > height) return false;

            for (int i = -1; i <= ship.size; ++i) {
                for (int j = -1; j <= 1; ++j) {
                    uint64_t checkX = ship.x + j;
                    uint64_t checkY = ship.y + i;
                    if (checkX >= 0 && checkX < width && checkY >= 0 && checkY < height) {
                        if (i >= 0 && i < ship.size && j == 0) {
                            if (field[ship.y + i][ship.x] != '.') return false;
                        } else {
                            if (field[checkY][checkX] != '.') return false;
                        }
                    }
                }
            }
        }
        return true;
    }

    void Game::place(Ship &ship) {
        if (ship.horizontal) {
            for (uint8_t i = 0; i < ship.size; ++i) {
                field[ship.y][ship.x + i] = 'S';
            }
        } else {
            for (uint8_t i = 0; i < ship.size; ++i) {
                field[ship.y + i][ship.x] = 'S';
            }
        }
        shipsList.push_back(ship);
    }

    void Game::random_place() {

        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<uint64_t> distX(0, width - 1);
        std::uniform_int_distribution<uint64_t> distY(0, height - 1);
        std::uniform_int_distribution distOrientation(0, 1);

        for (int type = 0; type < 4; ++type) {
            for (uint64_t count = 0; count < ships[type]; ++count) {
                bool placed = false;
                while (!placed) {
                    Ship ship(type+1, distOrientation(gen), distX(gen), distY(gen));
                    if (can_place(ship)) {
                        place(ship);
                        placed = true;
                    }
                }
            }
        }
    }

    std::string Game::process_shot(uint64_t x, uint64_t y) {
        if (x >= width || y >= height) {
            return "fail: out of bounds";
        }

        for (Ship &ship: shipsList) {
            if (ship.horizontal) {
                if (y == ship.y && x >= ship.x && x < ship.x + ship.size) {
                    field[y][x] = 'X';
                    if (is_ship_destroyed(ship)) {
                        return "kill";
                    }
                    return "hit";
                }
            } else {
                if (x == ship.x && y >= ship.y && y < ship.y + ship.size) {
                    field[y][x] = 'X';
                    if (is_ship_destroyed(ship)) {
                        return "kill";
                    }
                    return "hit";
                }
            }
        }

        if (field[y][x] == '.') {
            field[y][x] = 'M';
            return "miss";
        }
        return "was already shot";
    }

    bool Game::is_ship_destroyed(Ship &ship) {
        if (ship.horizontal) {
            for (uint64_t i = 0; i < ship.size; ++i) {
                if (field[ship.y][ship.x + i] != 'X') {
                    return false;
                }
            }
        } else {
            for (uint64_t i = 0; i < ship.size; ++i) {
                if (field[ship.y + i][ship.x] != 'X') {
                    return false;
                }
            }
        }
        return true;
    }

    std::pair<uint64_t, uint64_t> Game::get_bot_shot_custom() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint64_t> distX(0, width - 1);
        std::uniform_int_distribution<uint64_t> distY(0, height - 1);

        while (true) {
            uint64_t x = distX(gen);
            uint64_t y = distY(gen);
            if (field[y][x] == '.' || field[y][x] == 'S') {
                return {x, y};
            }
        }
    }

    std::pair<uint64_t, uint64_t> Game::get_bot_shot_ordered() {

        if (currentX >= width) {
            currentX = 0;
            currentY++;
        }

        uint64_t shotX = currentX;
        uint64_t shotY = currentY;

        currentX++;

        return {shotX, shotY};
    }