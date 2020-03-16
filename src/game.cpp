#include "game.h"
#include "utility.h"
#include <string>
#include <sstream>
#include <fstream>
#include <ctype.h>
#include <algorithm>

// Returns the number of actions in non-terminal node
int Game::get_num_actions_node(NodeIndex node_id) {
    if(!this->node_num_actions.count(node_id)){
        std::cout << "Error: Attempt to get number of actions for a leaf node[" << node_id << "]\n";
    }
    return this->node_num_actions[node_id];
}

int Game::get_num_actions_infoset(PlayerIndex player, InfoIndex infoset) {
    if((player != 0 && player != 1) || !this->infoset_num_actions[player].count(infoset)){
        std::cout << "Error: Attempt to get number of actions for a leaf node\n";
    }
    return this->infoset_num_actions[player][infoset];
}


int Game::get_leaf_utility(NodeIndex node_id){
    if(!is_leaf(node_id)){
        std::cout << "Error: Attempted to get utility of a non-leaf node";
    }
    return this->node_utility[node_id];
}

double Game::get_nature_probability(NodeIndex node_id, int action) {
    if(!this->node_nature_probabilities.count(node_id))
        std::cout << "Error: Attempted to get nature probabilities for a non-nature node";

    return this->node_nature_probabilities[node_id][action];
}

void Game::read_game_file(std::string filename) {
    std::string line;
    std::string dir = getCwd();

    dir += "/../";
    //std::cout << dir;
    std::string inputpath = dir + filename;
    std::cout << inputpath;
    std::ifstream myfile;
    myfile.open(inputpath);

    if (myfile.is_open())
    {

        while ( getline (myfile,line) )
        {

            std::vector<std::string> results = split(line, ' ');
            if(results[0] == "#")
                continue;
            else if(isdigit(results[0].c_str()[0])) {
                NodeIndex node_id = (NodeIndex) atoi(results[0].c_str());
                node_set_general_information(results, node_id);
                if (results.size() == 3 || results.size() == 5) {
                    create_leaf_node(results);

                } else if (node_id < num_chance_histories) {
                    create_natural_node(results);
                } else // player node
                {
                    this->create_player_node(results);

                }
            }
            else{
                this->read_game_info(results);
            }


        }
        print_game_info();
        print_game_tree();
        myfile.close();
    }

}

void Game::node_set_general_information(const std::vector<std::string> &line, NodeIndex node_id) {
    std::string name = line[1];
    if(name == "/")
        this->root = node_id;
}

void Game::create_leaf_node(const std::vector<std::string>& line){
    NodeIndex node_id = (NodeIndex) str_to_int(line[0]);
    this->node_names[node_id] = line[1];
    int utility = str_to_int(line[2]);
    this->node_utility[node_id] = utility;
}


void Game::create_natural_node(const std::vector<std::string>& line) {
    NodeIndex node_id = (NodeIndex) str_to_int(line[0]);
    this->node_names[node_id] = line[1];
    this->node_player[node_id] = -1;
    int num_actions = str_to_int(line[2]);
    int sum = 0;
    this->node_num_actions[node_id] = num_actions;
    this->node_action_names[node_id] = {};
    this->node_children[node_id] = {};
    for (int i = 0; i < num_actions; ++i) {
        int child_id = str_to_int(line[4+3*i]);
        this->node_children[node_id].push_back(child_id);
        std::string action_name = line[3 + 3 * i];
        this->node_action_names[node_id].push_back(action_name);
        sum += str_to_int(line[5+3*i]);
    }

    this->node_nature_probabilities[node_id] = {};
    for (int j = 0; j < num_actions; ++j) {
        double prob = (double)1.0 * str_to_int(line[5+3*j]) / sum;
        this->node_nature_probabilities[node_id].push_back(prob);
    }
}

void Game::create_player_node(const std::vector<std::string>& line) {
    NodeIndex node_id = (NodeIndex)str_to_int(line[0]);
    this->node_names[node_id] = line[1];
    PlayerIndex player = (PlayerIndex)str_to_int(line[2]);
    InfoIndex infoset = (InfoIndex)str_to_int(line[3]);
    int num_actions = str_to_int(line[4]);
    this->infoset_num_actions[player][infoset] = num_actions;
    this->node_num_actions[node_id] = num_actions;
    this->node_infoset[node_id] = infoset;
    this->node_player[node_id] = player;
    this->node_action_names[node_id] = {};
    auto vec = this->infoset_nodes[player][infoset];
    vec.push_back(node_id);

    for (int i = 0; i < num_actions; ++i) {
        int child_id = str_to_int(line[6+2*i]);
        std::string action_name = line[5+2*i];
        this->node_action_names[node_id].push_back(action_name);
        this->node_children[node_id].push_back(child_id);
    }
}


void Game::read_game_info(const std::vector<std::string> &line) {
    this->num_chance_histories = str_to_int(line[1]);
    this->num_combined_player_histories = str_to_int(line[2]);
    this->num_terminal_histories = str_to_int(line[3]);
    this->num_nodes = str_to_int(line[5]) + 1;
    this->num_infosets[0] = str_to_int(line[7]);
    this->num_infosets[1] = str_to_int(line[8]);

}

void Game::print_game_info() {
    std::cout << " \nNum nodes: " + std::to_string(this->num_nodes);
    std::cout << " \nNum infosets of player 0: " << get_num_infosets(0);
    std::cout << " \nNum infosets of player 1: " << get_num_infosets(1);
    std::cout << " \nNum chance histories: " << this->num_chance_histories;
    std::cout << " \nNum terminal histories: " << this->num_terminal_histories;
}

void Game::print_game_tree() {
    std::cout << "\nRoot: " << this->root;
    for (int i = 0; i < this->num_nodes; ++i) {
        std::cout << "\n";
        print_node_info(i);

    }
}

void Game::print_node_info(NodeIndex node_id) {
    std::cout << node_id;
    std::cout << " " << this->node_names[node_id];
    if (is_leaf(node_id)) {
        std::cout << "leaf, utility: " << this->node_utility[node_id];
    }
    else if (node_nature_probabilities.count(node_id) >= 1){
        std::cout << ", nature";
        std::cout << ", actions: ";
        int len = this->node_action_names[node_id].size();
        for (int i = 0; i < len-1 ;++i) {
            std::cout  << this->node_action_names[node_id][i] << ", ";
        }
        std::cout << this->node_action_names[node_id][len-1];

        len = this->node_children[node_id].size();
        std::cout << ", children: ";
        for (int i = 0; i < len-1; ++i) {
            std::cout << this->node_children[node_id][i] << ", ";
        }
        std::cout << this->node_children[node_id][len-1];

        len = this->node_nature_probabilities[node_id].size();
        std::cout << ", probabilities :";
        for (int i = 0; i < len-1; ++i) {
            std::cout << this->node_nature_probabilities[node_id][i] << ", ";
        }
        std::cout << this->node_nature_probabilities[node_id][len-1];
    }
    else{
        InfoIndex infoset = this->node_infoset[node_id];
        std::cout << ", player " << get_current_player(node_id);
        std::cout << ", infoset: " << infoset;

        std::cout << ", actions: ";
        int len = this->node_action_names[node_id].size();
        for (int i = 0; i < len-1 ;++i) {
            std::cout  << this->node_action_names[node_id][i] << ", ";
        }
        std::cout << this->node_action_names[node_id][len-1];

        len = this->node_children[node_id].size();
        std::cout << ", children: ";
        for (int i = 0; i < len-1; ++i) {
            std::cout << this->node_children[node_id][i] << ", ";
        }
        std::cout << this->node_children[node_id][len-1];
    }
}

// End of helper methods


//
double Game::compute_strategy_profile_ev(EquilibriumProfile& strategy_profile) {
    return compute_strategy_profile_ev_rec(strategy_profile, get_root());
}

double Game::compute_strategy_profile_ev_rec(EquilibriumProfile& strategy_profile,

                                            NodeIndex current) {

    if(is_leaf(current))
        return get_leaf_utility(current);

    double val = 0.0;
    PlayerIndex player = get_current_player(current);

    for (int i = 0; i < get_num_actions_node(current); ++i) {
        PlayerIndex child_id = get_child_id(current, i);
        double prob = 0.0;
        if (player == -1){
            prob = get_nature_probability(current, i);
        }
        else{
            InfoIndex infoset = get_node_infoset(current);
            prob = strategy_profile[player][infoset][i];
        }
        val += (prob * compute_strategy_profile_ev_rec(strategy_profile, child_id) );
    }

    return val;
}

double Game::compute_strategy_profile_exp(EquilibriumProfile& strategy_profile) {
    double exp0 = 0.0;
    double exp1 = 0.0;

    exploitability_initialize();
    compute_height(get_root());

    for (int p = 0; p < 2; ++p) {
        //int infosets = get_num_infosets(p);
        int h = this->node_height[get_root()];
        for (int t = 0; t < h+2; ++t) {
            if(p == 0)
                exp0 = compute_strategy_profile_exp_rec(p, get_root(), 1.0, t, strategy_profile);
            else
                exp1 = compute_strategy_profile_exp_rec(p, get_root(), 1.0, t, strategy_profile);
        }
    }

    return exp1 + exp0;
}

void Game::exploitability_initialize() {
    for (int p = 0; p < 2; ++p) {
        int infosets = get_num_infosets(p);
        for (int infoset = 0; infoset < infosets; ++infoset) {

            action_value[p][infoset] = std::vector<double>(get_num_actions_infoset(p, infoset), 0.0);

            height[p][infoset] = -1;

            last_touched[p][infoset] = -1;

        }
    }
    int nodes = get_num_nodes();
    for (int node = 0; node < nodes; ++node) {
        this->node_height[node] = -1;
    }
}

int Game::compute_height(NodeIndex nid) {
    if(is_leaf(nid))
        return -1;
    else{
        int num_actions = get_num_actions_node(nid);
        int max_height = -1;
        for (int action = 0; action < num_actions; ++action) {
            int action_height = compute_height(get_child_id(nid, action));
            if (max_height == -1 || action_height > max_height)
                max_height = action_height;
        }
        node_height[nid] = max_height + 1;
        if(get_current_player(nid) != -1) {
            InfoIndex infoset = get_node_infoset(nid);
            PlayerIndex player = get_current_player(nid);
            if (this->height[player][infoset] == -1 || this->height[player][infoset] > node_height[nid]){
                this->height[player][infoset] = node_height[nid];
            }
        }
        return node_height[nid];
    }
}

double Game::compute_strategy_profile_exp_rec(PlayerIndex p,
                                             NodeIndex nid,
                                              double reach,
                                             int t,
                                             EquilibriumProfile& strategy_profile) {
    double val = 0.0;

    if(is_leaf(nid)){
        if(p == 0){
            val = reach * (double) get_leaf_utility(nid);
            return val;
        }
        else{
            val = - reach * (double) get_leaf_utility(nid);
            return val;
        }
    }
    else if(get_current_player(nid) == -1){
        int num_actions = get_num_actions_node(nid);
        double ev = 0.0;
        for (int action = 0; action < num_actions; ++action) {
            double prob = get_nature_probability(nid, action);
            ev += compute_strategy_profile_exp_rec(p, get_child_id(nid, action), reach * prob,
                    t, strategy_profile);
        }
        return ev;
    }
    else{
        InfoIndex infoset = get_node_infoset(nid);
        PlayerIndex player = get_current_player(nid);
        int num_actions = get_num_actions_infoset(player, infoset);

        if(player == p) {
            if (t > height[player][infoset]) {

                int max_action = -1;
                double max_value = 0.0;
                for (int action = 0; action < num_actions; ++action) {
                    if (max_action == -1 || max_value < this->action_value[player][infoset][action]) {
                        max_action = action;
                        max_value = action_value[player][infoset][action];
                    }
                }

                return compute_strategy_profile_exp_rec(p, get_child_id(nid, max_action), reach, t, strategy_profile);
            }
            else{
                if(this->last_touched[player][infoset] < t){
                    this->last_touched[player][infoset] = t;
                    for (int action = 0; action < num_actions; ++action) {
                        this->action_value[player][infoset][action] = 0.0;
                    }
                }

                for (int action = 0; action < num_actions; ++action) {
                    this->action_value[player][infoset][action] += compute_strategy_profile_exp_rec(p, get_child_id(nid,
                            action), reach, t, strategy_profile);
                }
                return 0;
            }
        }
        else{
            double ev = 0.0;

            double prob_sum = 0.0;
            for (int action = 0; action < num_actions; ++action) {
                prob_sum += strategy_profile[player][infoset][action];
            }

            double prob = 1.0 / num_actions;
            for (int action = 0; action < num_actions; ++action) {
                if(prob_sum > 0.0){
                    prob = strategy_profile[player][infoset][action] / prob_sum;
                }
                ev += compute_strategy_profile_exp_rec(p, get_child_id(nid, action),  reach * prob,
                        t, strategy_profile);
            }
            return ev;
        }
    }
}