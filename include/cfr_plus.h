#ifndef CFR_PLUS_H
#define CFR_PLUS_H

#include <unordered_map>
#include <vector>
#include "helper.h"

// Implementation of CFR_Plus

class CFR_Plus{
 public:
  double cfrplus(Game& game, NodeIndex node, double reach, PlayerIndex p,
          int iteration, EquilibriumProfile& strategy_profile){
      if(game.is_leaf(node)){
          if (p == 0)
              return game.get_leaf_utility(node) * reach;
          if (p == 1)
              return -1 * game.get_leaf_utility(node) * reach;
      }
      else{

          double ev = 0.0;
          if (game.get_current_player(node) == -1){
              for (int action = 0; action < game.get_num_actions_node(node); ++action) {

                  ev += cfrplus(game, game.get_child_id(node, action),
                          reach * game.get_nature_probability(node, action), p, iteration,strategy_profile);
              }

          }
          else{
              if (no_regrets(regret[game.get_current_player(node)][game.get_node_infoset(node)])){

                  prob[game.get_current_player(node)][game.get_node_infoset(node)] = equal_probs(game, node);
              }
              else{

                  if(seen[game.get_current_player(node)][game.get_node_infoset(node)] == 0)
                      prob[game.get_current_player(node)][game.get_node_infoset(node)] =
                              normalize_regret(regret[game.get_current_player(node)][game.get_node_infoset(node)]);
              }

              if(game.get_current_player(node) == p){
                  seen[p][game.get_node_infoset(node)] = 1;
                  for (int action = 0; action < game.get_num_actions_node(node); ++action) {
                      action_ev[node][action] = cfrplus(game, game.get_child_id(node, action), reach, p, iteration,
                              strategy_profile);
                      ev += prob[p][game.get_node_infoset(node)][action] * action_ev[node][action];
                  }
                  for (int action = 0; action < game.get_num_actions_node(node); ++action) {
                      regret[p][game.get_node_infoset(node)][action] += (action_ev[node][action] - ev);
                  }
              }
              else{
                  seen[game.get_current_player(node)][game.get_node_infoset(node)] = 1;
                  for (int action = 0; action < game.get_num_actions_node(node); ++action) {
                      strategy_profile[game.get_current_player(node)][game.get_node_infoset(node)][action]
                              += iteration * reach * prob[game.get_current_player(node)][game.get_node_infoset(node)][action];
                      ev += cfrplus(game, game.get_child_id(node, action),
                              reach * prob[game.get_current_player(node)][game.get_node_infoset(node)][action], p, iteration, strategy_profile);
                  }
              }

          }
          return ev;
      }

  }

  void solve(Game& game, int num_iterations,
          EquilibriumProfile& strategy_profile){
      for (PlayerIndex player = 0; player < 2; ++player) {
          for (InfoIndex info_id = 0; info_id < game.get_num_infosets(player); ++info_id) {
              strategy_profile[player][info_id] = std::vector<double>(game.get_num_actions_infoset(player, info_id),0.0);
          }
      }

      init_action_values(game, action_ev);
      init_regret_values(game, regret);
      init_prob(game, prob);

      for (int i = 0; i < num_iterations; ++i) {
          for (PlayerIndex player = 0; player < 2; ++player) {
              init_seen(game, seen);

              cfrplus(game, game.get_root(), 1, player, i+1, strategy_profile);

              init_seen(game, seen);
          }
          // set regrets below 0 to 0
          set_zero_regrets(game, regret);
      }

      for (PlayerIndex player = 0; player < 2; ++player) {
          for (InfoIndex info_id = 0; info_id < game.get_num_infosets(player); ++info_id) {
              strategy_profile[player][info_id] = normalize(strategy_profile[player][info_id]);
          }
      }
  }
 private:
  std::unordered_map<PlayerIndex , std::unordered_map<InfoIndex, int>> seen;
  std::unordered_map<PlayerIndex,std::unordered_map<InfoIndex ,std::vector<double>>> prob;
  std::unordered_map<NodeIndex, std::vector<double>> action_ev;
  std::unordered_map<PlayerIndex, std::unordered_map<InfoIndex, std::vector<double>>> regret;
};

#endif //CFR_PLUS_H
