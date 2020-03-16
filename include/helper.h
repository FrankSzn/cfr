#ifndef CFR_MATH_UTIL_H
#define CFR_MATH_UTIL_H

#include <unordered_map>
#include <iostream>
#include <cstdlib>
#include <vector>
#include "game.h"
#include <utility>

double eps = 1e-9;

int rand_select(std::unordered_map<int, double> dist){
    double r = (double)rand()/(RAND_MAX);
    double total = 0.0;
    for(auto it = dist.begin(); it != dist.end(); it++){
        total += it->second;
        if(total >= r)
            return it->first;
    }
    return 0;
}

std::unordered_map<int, double> zipWith_probabilities(Game& game, NodeIndex node)
{
    int num_actions = game.get_num_actions_node(node);
    std::unordered_map<int, double> act_to_prob;
    std::vector<double> probabilities;
    if(game.get_current_player(node) == -1){
        for (int action = 0; action < num_actions; ++action) {
            probabilities.push_back(game.get_nature_probability(node, action));
        }
    }
    else{

        double p = 1.0 / (double)num_actions;
        for (int i = 0; i < num_actions; ++i) {
            probabilities.push_back(p);
        }
    }
    for (int action = 0; action < num_actions; ++action) {
        act_to_prob.insert(std::make_pair(action, probabilities[action]));
    }
    return act_to_prob;
}

void init_action_values(Game& game, std::unordered_map<NodeIndex, std::vector<double>>& d){
    for (int node = 0; node < game.get_num_nodes(); ++node) {
        if (!game.is_leaf(node) && game.get_current_player(node) != -1){
            d[node] = std::vector<double>(game.get_num_actions_node(node), 0.0);
        }
    }

}

void init_regret_values(Game& game, std::unordered_map<PlayerIndex, std::unordered_map<InfoIndex, std::vector<double>>>& d)
{
    for (int node = 0; node < game.get_num_nodes(); ++node) {
        if(!game.is_leaf(node) && game.get_current_player(node) != -1){
            PlayerIndex p = game.get_current_player(node);
            InfoIndex infoset_id = game.get_node_infoset(node);
            d[p][infoset_id] = std::vector<double>(game.get_num_actions_infoset(p, infoset_id), 0.0);
        }
    }
}

void init_prob(Game& game, std::unordered_map<PlayerIndex,std::unordered_map<InfoIndex ,std::vector<double>>>& d){
    for (NodeIndex node = 0; node < game.get_num_nodes(); ++node) {
        if(!game.is_leaf(node) && game.get_current_player(node) != -1){
            PlayerIndex p = game.get_current_player(node);
            InfoIndex infoset_id = game.get_node_infoset(node);
            d[p][infoset_id] = std::vector<double>(game.get_num_actions_infoset(p, infoset_id), 0.0);
        }
    }
}

void init_seen(Game& game, std::unordered_map<PlayerIndex , std::unordered_map<InfoIndex, int>>& d)
{
    for (NodeIndex node = 0; node < game.get_num_nodes(); ++node) {
        if(!game.is_leaf(node) && game.get_current_player(node) != -1){
            PlayerIndex p = game.get_current_player(node);
            InfoIndex infoset_id = game.get_node_infoset(node);
            d[p][infoset_id] = 0;
        }
    }
}

std::vector<double> equal_probs(Game& game, NodeIndex node){
    int num_action = game.get_num_actions_node(node);
    std::vector<double> probabilities;
    double p = 1.0 / (double) num_action;
    for (int i = 0; i < num_action; ++i) {
        probabilities.push_back(p);
    }
    return probabilities;
}

void set_zero_regrets(Game& game,
                      std::unordered_map<PlayerIndex, std::unordered_map<InfoIndex, std::vector<double>>>& regret){
    for (NodeIndex node = 0; node < game.get_num_nodes(); ++node) {
        if (!game.is_leaf(node) && game.get_current_player(node) != -1){
            PlayerIndex p = game.get_current_player(node);
            InfoIndex infoset_id = game.get_node_infoset(node);
            for (int action = 0; action < regret[p][infoset_id].size(); ++action) {
                if (regret[p][infoset_id][action] < 0)
                    regret[p][infoset_id][action] = 0;
            }
        }
    }
}



std::vector<double> normalize_regret(const std::vector<double>& regret){
    std::vector<double> temp = regret;
    double total = 0.0;
    for (int i = 0; i < temp.size(); ++i) {
        if (temp[i] < 0)
            temp[i] = 0;
        total += temp[i];
    }

    std::vector<double> normalized(temp.size(), 0.0);
    for (int i = 0; i < temp.size(); ++i) {
        normalized[i] = ((double) (temp[i])) / ((double) (total+eps));

    }
    return normalized;
}

std::vector<double> normalize(const std::vector<double>& L)
{
    std::vector<double> temp = L;
    double total = 0.0;
    for (int i = 0; i < L.size(); ++i) {
        total += L[i];
    }

    for (int i = 0; i < L.size(); ++i) {
        temp[i] = ((double) temp[i]) / ((double) (total+eps));
    }
    return temp;
}

bool no_regrets(const std::vector<double>& regret){
    int no_regrets = 0;
    for (int i = 0; i < regret.size(); ++i) {
        if(regret[i] <= 0)
            no_regrets += 1;
    }
    return (no_regrets == regret.size());
}

#endif //CFR_MATH_UTIL_H
