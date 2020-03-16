#ifndef CFR_GAME_H
#define CFR_GAME_H

#include <vector>
#include <unordered_map>
#include <string>

typedef std::unordered_map<int, std::vector<double>> EquilibriumProfile[2];

typedef int PlayerIndex;
typedef int NodeIndex;
typedef int InfoIndex;

class Game{
 public:
    // get the index of the root
    NodeIndex get_root() {
        return root;
    }

    // returns the number of actions in a nonterminal node
    int get_num_actions_node(NodeIndex node_id);

    // Returns the number of actions in a player's information set
    int get_num_actions_infoset(PlayerIndex  player, InfoIndex infoset);

    // Returns the node following this node/action pair
    NodeIndex get_child_id(NodeIndex node_id, int action){
        return node_children[node_id][action];
    }

    //
    bool is_leaf(NodeIndex node_id){
        if(node_utility.count(node_id))
            return true;
        return false;
    }

    //

    // Read the game file
    void read_game_file(std::string filename);

    //
    void node_set_general_information(const std::vector<std::string>& line, NodeIndex node_id);

    //
    void create_leaf_node(const std::vector<std::string>& line);

    //
    void create_natural_node(const std::vector<std::string>& line);

    //
    void create_player_node(const std::vector<std::string>& line);

    //
    void read_game_info(const std::vector<std::string>& line);

    //
    void print_game_info();

    //
    void print_game_tree();

    //
    void print_node_info(NodeIndex node_id);



    // Returns the utility of a terminal node (for the first player!)
    int get_leaf_utility(NodeIndex node_id);

    // Returns the probability of a chance node's action
    double get_nature_probability(NodeIndex node_id, int action);

    //
    int get_num_infosets(PlayerIndex player){
        return num_infosets[player];
    }



    // Return the player who acts at this node, or -1 for chance/nature
    PlayerIndex get_current_player(NodeIndex node_id){
        return node_player[node_id];
    }

    // Returns this node's information set
    InfoIndex get_node_infoset(NodeIndex node_id){
        return node_infoset[node_id];
    }

    int get_num_nodes(){
        return num_nodes;
    }

    // End of helper methods.

    void create_nature_node(const std::vector<std::string>& line);
    //
    double compute_strategy_profile_ev(EquilibriumProfile& strategy_profile);
    //
    double compute_strategy_profile_ev_rec(EquilibriumProfile& strategy_profile,
            NodeIndex current);
    //
    double compute_strategy_profile_exp(EquilibriumProfile& strategy_profile);

    //
    void exploitability_initialize();

    //
    int compute_height(NodeIndex nid);

    //
    double compute_strategy_profile_exp_rec(PlayerIndex p, NodeIndex nid, double reach,
            int t, EquilibriumProfile& strategy_profile);

 private:
    NodeIndex root;
    int num_nodes;
    int total_num_actions;
    int num_chance_histories;
    int num_combined_player_histories;
    int num_terminal_histories;

    std::unordered_map<PlayerIndex ,std::unordered_map<InfoIndex, int>> infoset_num_actions;
    std::unordered_map<PlayerIndex ,std::unordered_map<InfoIndex, std::vector<NodeIndex> >> infoset_nodes;
    std::unordered_map<NodeIndex ,std::vector<int>> node_children;
    std::unordered_map<NodeIndex , std::string> node_names;
    std::unordered_map<NodeIndex, int> node_utility;
    std::unordered_map<NodeIndex, int> node_player;
    std::unordered_map<NodeIndex, std::vector<std::string>> node_action_names;
    std::unordered_map<NodeIndex, int> node_num_actions;
    std::unordered_map<NodeIndex, std::vector<double>> node_nature_probabilities;
    std::unordered_map<PlayerIndex, std::unordered_map<InfoIndex , int>> num_actions;
    std::unordered_map<NodeIndex, InfoIndex> node_infoset;
    std::unordered_map<PlayerIndex, int> num_infosets;

    std::unordered_map<NodeIndex, int> node_height;
    std::unordered_map<PlayerIndex, std::unordered_map<InfoIndex, int>> height;
    std::unordered_map<PlayerIndex, std::unordered_map<InfoIndex ,std::vector<double>>> action_value;
    std::unordered_map<PlayerIndex, std::unordered_map<InfoIndex, int>> last_touched;


};

#endif //CFR_GAME_H
