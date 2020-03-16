#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include "game.h"
#include "cfr_plus.h"
#include "mccfr.h"
#include <time.h>

int main(int argc, char* argv[]){


    if(argc <= 2){
        std::cout << " format is  ./cfr <path/to/gamefile> <iteration>";
        return 0;
    }

    std::string filename = std::string(argv[1]);
    int iterations = atoi(argv[2]);
    Game game = Game();
    game.read_game_file(filename);
    std::unordered_map<int, std::vector<double>> equilibrium_profile[2];

    CFR_Plus cfr_plus;
    cfr_plus.solve(game, iterations, equilibrium_profile);
    std::cout << "\nExpected Value: " << game.compute_strategy_profile_ev(equilibrium_profile);

    std::cout << "\nExploitability: " << game.compute_strategy_profile_exp(equilibrium_profile);
    /*
    MCCFR mccfr;
    mccfr.mccfr_solve(game, iterations, equilibrium_profile);
    std::cout << "\nExpected Value: " << game.compute_strategy_profile_ev(equilibrium_profile);

    std::cout << "\nExploitability: " << game.compute_strategy_profile_exp(equilibrium_profile);

    /*
    if(filename.find("coin") != std::string::npos){
        equilibrium_profile[0][0] = {0.375, 0.625};
        equilibrium_profile[0][1] = {1.0, 0.0};
        equilibrium_profile[0][2] = {0.333333, 0.666667};
        equilibrium_profile[0][3] = {0.2, 0.8};
        equilibrium_profile[0][4] = {1.0, 0.0};

        equilibrium_profile[1][0] = {0.508929, 0.491071};
        equilibrium_profile[1][1] = {0.666667, 0.333333};
        equilibrium_profile[1][2] = {0.8, 0.2};
        std::cout << game.compute_strategy_profile_ev(equilibrium_profile) << "\n";

    }else if(filename.find("kuhn") != std::string::npos){
        equilibrium_profile[0][0] = {0.666667, 0.333333};
        equilibrium_profile[0][1] = {1.0, 0.0};
        equilibrium_profile[0][2] = {0.0, 1.0};
        equilibrium_profile[0][3] = {0.0, 1.0};
        equilibrium_profile[0][4] = {1.0, 0.0};
        equilibrium_profile[0][5] = {1.0, 0.0};

        equilibrium_profile[1][0] = {0.666667, 0.333333};
        equilibrium_profile[1][1] = {1.0, 0.0};
        equilibrium_profile[1][2] = {0.0, 1.0};
        equilibrium_profile[1][3] = {0.0, 1.0};
        equilibrium_profile[1][4] = {0.333333, 0.666667};
        equilibrium_profile[1][5] = {1.0, 0.0};
        std::cout << game.compute_strategy_profile_ev(equilibrium_profile) << "\n";
    }
    */


    return 0;

}