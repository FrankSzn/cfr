#ifndef CFR_CFR_H
#define CFR_CFR_H

class CFR{


 protected:
    std::unordered_map<NodeIndex, std::vector<int>> action_value;
    std::unordered_map<PlayerIndex, std::unordered_map<InfoIndex, std::vector<double>>> regret;
};

#endif //CFR_CFR_H
