#ifndef CFR_MCCFR_H
#define CFR_MCCFR_H

#include <unordered_map>
#include <vector>
#include "helper.h"




// Implementation of PureCFR



class MCCFR{
 public:


  void  mccfr_solve(Game& game, int num_iterations,
                    EquilibriumProfile& strategy_profile)
  {
      PlayerIndex player;
      for (player = 0; player < 2; ++player) {
          for (int info_id = 0; info_id < game.get_num_infosets(player); ++info_id) {
              strategy_profile[player][info_id] =
                      std::vector<double>(game.get_num_actions_infoset(player, info_id), 0.0);
          }
      }


      init_action_values(game, action_value);
      init_regret_values(game, regret);


      int i;
      for (i = 0; i < num_iterations; ++i) {
          for (player = 0; player < 2; ++player) {

              PureCRF(game, game.get_root(), player, strategy_profile);
          }
      }

      for (player = 0; player < 2; ++player) {
          for (InfoIndex info_id = 0; info_id < game.get_num_infosets(player); ++info_id) {
              strategy_profile[player][info_id] = normalize(strategy_profile[player][info_id]);
          }
      }

  }

  double PureCRF(Game& game, NodeIndex node, PlayerIndex player,
          EquilibriumProfile& strategy_profile){
      if (game.is_leaf(node)){

          if(player == 0)
              return game.get_leaf_utility(node);
          if(player == 1)
              return -1 * game.get_leaf_utility(node);
      }
      else{

          if (game.get_current_player(node) == -1){
              // sample action a from chance distribution
              std::unordered_map<int, double> dist = zipWith_probabilities(game, node);
              int action = rand_select(dist);
              return PureCRF(game, game.get_child_id(node, action), player, strategy_profile);
          }
          else if(game.get_current_player(node) != player){

              // set probabilities for actions in proportion to positive regret
              // sample action a
              // if all regrets are 0 or all negatives
              int action = -1;

              std::vector<double> curr_regrets = regret[game.get_current_player(node)][game.get_node_infoset(node)];


              if (no_regrets(curr_regrets)){
                  std::unordered_map<int, double> dist = zipWith_probabilities(game, node);
                  action = rand_select(dist);
              }
              else{
                  std::vector<double> action_probs = normalize_regret(curr_regrets);
                  std::unordered_map<int, double> dist;
                  for (int action = 0; action < game.get_num_actions_node(node); ++action) {
                      dist.insert(std::make_pair(action, action_probs[action]));
                  }

                  action = rand_select(dist);
              }
              strategy_profile[game.get_current_player(node)][game.get_node_infoset(node)][action] += 1;
              return PureCRF(game, game.get_child_id(node, action), player, strategy_profile);
          }
          else{

              // set probabilities for actions in proportion to positive regret
              // sample action a
              int a = -1;
              std::vector<double> curr_regrets = regret[player][game.get_node_infoset(node)];
              std::unordered_map<int, double> dist;

              if(no_regrets(curr_regrets)){
                  dist = zipWith_probabilities(game, node);
                  a = rand_select(dist);
              }
              else{
                  std::vector<double> action_probs = normalize_regret(curr_regrets);
                  for (int i = 0; i < game.get_num_actions_node(node); ++i) {
                      dist.insert(std::make_pair(i, action_probs[i]));
                  }
                  a = rand_select(dist);
              }

              for (int action = 0; action < game.get_num_actions_node(node); ++action) {
                  action_value[node][action] = PureCRF(game, game.get_child_id(node, action), player, strategy_profile);
              }

              for (int action = 0; action < game.get_num_actions_node(node); ++action) {
                  regret[player][game.get_node_infoset(node)][action] += (action_value[node][action] -
                          action_value[node][a]);
              }
              return action_value[node][a];

          }
      }

  }
 private:

  std::unordered_map<NodeIndex, std::vector<double>> action_value;
  std::unordered_map<PlayerIndex, std::unordered_map<InfoIndex, std::vector<double>>> regret;

};



#endif //CFR_MCCFR_H
