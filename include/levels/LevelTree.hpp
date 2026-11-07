#pragma once

#include <mtreenode.hpp>

enum class LevelEra
{
    None,
    Past,
    Present,
    Future
};

struct LevelInfo
{
    enum class Type
    {
        FinalBoss,
        MiniBoss,
        Level
    };

    Type type;
    LevelEra era;
    int number;
    bool completed = false;
};

class LevelTree
{
public:
    LevelTree(int levelsPerEra = 2);
    ~LevelTree();

    LevelTree(const LevelTree&) = delete;
    LevelTree& operator=(const LevelTree&) = delete;

    const Designar::MTreeNode<LevelInfo>* get_root() const;
    const Designar::MTreeNode<LevelInfo>* get_mini_boss(LevelEra era) const;
    const Designar::MTreeNode<LevelInfo>* get_next_level(LevelEra era) const;
    bool complete_next_level(LevelEra era);
    bool can_fight_mini_boss(LevelEra era) const;
    bool complete_mini_boss(LevelEra era);
    bool is_era_completed(LevelEra era) const;
    bool can_fight_final_boss() const;
    bool complete_final_boss();

private:
    Designar::MTreeNode<LevelInfo>* root = nullptr;

    int levelsPerEra;

    Designar::MTreeNode<LevelInfo>* find_mini_boss(LevelEra era) const;
    Designar::MTreeNode<LevelInfo>* find_next_level(LevelEra era) const;
};