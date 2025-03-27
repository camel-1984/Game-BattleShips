#include "include/parse.h"

int play(Game &game) {
    while (true) {

        std::string cmd;
        std::getline(std::cin, cmd);

        std::istringstream iss(cmd);
        std::string mainCommand;
        iss >> mainCommand;

        if (mainCommand == "exit") {
            return 0;

        } if (mainCommand == "ping") {
            std::cout << "pong\n";

        } else if (mainCommand == "win") {
            if (!game.role) {
                std::cout << "fail: game was not created\n";
                continue;
            }
            game.calculate_remaining_ships();
            if (game.is_player_winner()) {
                std::cout << "yes\n";
                game.status = false;
            } else std::cout << "no\n";

        } else if (mainCommand == "lose") {
            if (!game.role) {
                std::cout << "fail: game was not created\n";
                continue;
            }
            game.calculate_remaining_ships();
            if (game.is_player_loser()) {
                std::cout << "yes\n";
                game.status = false;
            } else std::cout << "no\n";

        } else if (mainCommand == "finished") {
            if (!game.role) {
                std::cout << "fail: game was not created\n";
                continue;
            }
            if (game.is_game_finished()) {
                std::cout << "yes\n";
                game.status = false;
            } else std::cout << "no\n";

        } else if (mainCommand == "create") {
            if (game.role) {
                std::cout << "fail: game was already created\n";
                continue;
            }

            std::string parameter;
            iss >> parameter;

            if (parameter == "master") {
                game.not_bot_shot = true;
                game.master = false;
                game.role = true;
                game.generate();
                std::cout << "ok\n";
            } else if (parameter == "slave") {
                game.master = true;
                game.role = true;
                std::cout << "ok\n";
            } else {
                std::cout << "fail: invalid parameter\n";
            }

        } else if (mainCommand == "dump") {
            if (!game.role) {
                std::cout << "fail: game was not created\n";
                continue;
            }

            std::string path;
            if (iss >> path) {
                game.in_path = path;
                if (game.dump()) {
                    std::cout << "ok\n";
                } else {
                    std::cout << "fail: unable to save game\n";
                }
            } else {
                std::cout << "fail: invalid path\n";
            }


        } else if (mainCommand == "load") {

            std::string path;
            if (iss >> path) {
                game.out_path = path;
                if (game.load()) {
                    std::cout << "ok\n";
                    game.role = true;
                } else {
                    std::cout << "fail: unable to load game\n";
                }
            } else {
                std::cout << "fail: invalid path\n";
            }

        } else if (mainCommand == "start") {
            if (!game.role) {
                std::cout << "fail: game was not created\n";
                continue;
            }

            if (game.status) {
                std::cout << "fail: game was already started\n";
                continue;
            }

            if (game.loaded) {
                game.status = true;
                std::cout << "ok\n";
            } else {
                if (game.height > 0 && game.width > 0 &&
                    (game.ships[0] > 0 || game.ships[1] > 0 ||
                     game.ships[2] > 0 || game.ships[3] > 0)) {
                    game.random_place();
                    game.status = true;
                    std::cout << "ok\n";
                     } else {
                         std::cout << "fail: invalid game configuration\n";
                     }
            }

        } else if (mainCommand == "stop") {

            if (!game.role) {
                std::cout << "fail: game was not created\n";
                continue;
            }

            game.status = !game.status;
            std::cout << "ok\n";

        } else if (mainCommand == "set") {
            if (!game.role) {
                std::cout << "fail: game was not created\n";
                continue;
            }

            std::string parameter;
            iss >> parameter;


            if (game.status && parameter != "strategy" && parameter != "result") {
                std::cout << "fail: game was already started\n";
                continue;
            }

            if (parameter == "strategy") {
                std::string strategy;
                if (iss >> strategy) {
                    if (strategy == "ordered") {
                        game.ordered = true;
                        std::cout << "ok\n";
                    } else if (strategy == "custom") {
                        game.ordered = false;
                        std::cout << "ok\n";
                    } else {
                        std::cout << "fail: invalid strategy\n";
                    }
                } else {
                    std::cout << "fail: invalid strategy\n";
                }
            } else if (parameter == "width") {
                if (!game.master) {
                    std::cout << "fail: only master can set parameters\n";
                    continue;
                }

                uint64_t width;
                if (iss >> width && width > 0) {
                    game.width = width;
                    game.create_matrix();
                    std::cout << "ok\n";
                } else {
                    std::cout << "fail: invalid width\n";
                }
            } else if (parameter == "height") {
                if (!game.master) {
                    std::cout << "fail: only master can set parameters\n";
                    continue;
                }

                uint64_t height;
                if (iss >> height && height > 0) {
                    game.height = height;
                    game.create_matrix();
                    std::cout << "ok\n";
                } else {
                    std::cout << "fail: invalid height\n";
                }
            } else if (parameter == "count") {
                if (!game.master) {
                    std::cout << "fail: only master can set parameters\n";
                    continue;
                }

                int ship;
                uint64_t amount;
                if (iss >> ship >> amount) {
                    if (ship < 1 || ship > 4) {
                        std::cout << "fail: invalid ship type\n";
                        continue;
                    }

                    uint64_t tempShips[4] = {game.ships[0], game.ships[1], game.ships[2], game.ships[3]};
                    tempShips[ship - 1] = amount;

                    if (!game.can_generate(game.width, game.height, tempShips)) {
                        std::cout << "fail: invalid field size\n";
                    } else {
                        game.ships[ship - 1] = amount;
                        std::cout << "ok\n";
                    }
                } else {
                    std::cout << "fail: invalid input\n";
                }
            } else if (parameter == "result") {

                if (!game.status) {
                    std::cout << "fail: game was not started\n";
                    continue;
                }


                if (!game.awaiting_result) {
                    std::cout << "fail: shot was not made\n";
                    continue;
                }

                std::string result;
                iss >> result;

                if (result == "miss" || result == "hit" || result == "kill") {
                    if (result == "kill") {
                        game.killCount++;
                    }
                    std::cout << "ok\n";
                    game.awaiting_result = false;
                    if (result == "miss") {
                        game.not_bot_shot = true;
                    } else {
                        game.not_bot_shot = false;
                    }
                } else {
                    std::cout << "fail: invalid result\n";
                }
            } else {
                std::cout << "fail: unknown parameter\n";
            }

        } else if (mainCommand == "get") {
            if (!game.role) {
                std::cout << "fail: game was not created\n";
                continue;
            }

            std::string parameter;
            iss >> parameter;

            if (parameter == "width") {
                std::cout << game.width << "\n";
            } else if (parameter == "height") {
                std::cout << game.height << "\n";
            } else if (parameter == "count") {
                int ship;
                if (iss >> ship) {
                    switch (ship) {
                        case 1: std::cout << game.ships[0] << "\n"; break;
                        case 2: std::cout << game.ships[1] << "\n"; break;
                        case 3: std::cout << game.ships[2] << "\n"; break;
                        case 4: std::cout << game.ships[3] << "\n"; break;
                        default: std::cout << "fail: invalid ship type\n";
                    }
                } else {
                    std::cout << "fail: invalid ship type\n";
                }
            } else {
                std::cout << "fail: unknown parameter\n";
            }

        } else if (mainCommand == "shot") {

            if (!game.role || !game.status) {
                std::cout << "fail: game was not started\n";
                continue;
            }

            if (game.awaiting_result) {
                std::cout << "fail: awaiting result from shot\n";
                continue;
            }

            uint64_t x, y;
            if (iss >> x >> y) {
                if (game.not_bot_shot == false) {
                    std::cout << "fail: not your turn\n";
                } else {
                    std::string result = game.process_shot(x, y);
                    std::cout << result << "\n";
                    if (result == "hit" || result == "kill") {
                        game.not_bot_shot = true;
                    } else {
                        game.not_bot_shot = false;
                    }
                }

            } else if (game.not_bot_shot == false) {
                if (game.ordered == true) {
                    std::pair<uint64_t, uint64_t> bot_shot = game.get_bot_shot_ordered();
                    game.awaiting_result = true;
                    std::cout << bot_shot.first << " " << bot_shot.second << "\n";
                } else {
                    std::pair<uint64_t, uint64_t> bot_shot = game.get_bot_shot_custom();
                    game.awaiting_result = true;
                    std::cout << bot_shot.first << " " << bot_shot.second << "\n";
                }

            } else {
                std::cout << "fail: not your turn\n";
            }

        } else if (mainCommand == "display") {
            game.display();

        } else {
            std::cout << "fail: unknown command\n";
        }
    }
}