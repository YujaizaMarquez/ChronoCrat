#include "levels/LevelTree.hpp"
#include <memory>
#include <stdexcept>

LevelTree::LevelTree(int levelsPerEra) : levelsPerEra(levelsPerEra)
{
    if (levelsPerEra < 2)
    {
        throw std::invalid_argument("Cada epoca necesita al menos dos niveles");
    }

    auto destroyTree = [](Designar::MTreeNode<LevelInfo>* node)
    {
        Designar::MTreeNode<LevelInfo>::destroy_tree(node);
    };
    std::unique_ptr<Designar::MTreeNode<LevelInfo>, decltype(destroyTree)> tree(
        new Designar::MTreeNode<LevelInfo>(LevelInfo{
            LevelInfo::Type::FinalBoss,
            LevelEra::None,
            0
        }),
        destroyTree);

    const LevelEra eras[] = {LevelEra::Past, LevelEra::Present, LevelEra::Future};
    for (LevelEra era : eras)
    {
        Designar::MTreeNode<LevelInfo>* miniBoss =
            new Designar::MTreeNode<LevelInfo>(LevelInfo{
                LevelInfo::Type::MiniBoss,
                era,
                0
            });
        tree->append_child(miniBoss);

        for (int number = 1; number <= levelsPerEra; ++number)
        {
            miniBoss->append_child(new Designar::MTreeNode<LevelInfo>(LevelInfo{
                LevelInfo::Type::Level,
                era,
                number
            }));
        }
    }

    root = tree.release();
}

LevelTree::~LevelTree()
{
    Designar::MTreeNode<LevelInfo>::destroy_tree(root);
}

const Designar::MTreeNode<LevelInfo>* LevelTree::get_root() const
{
    return root;
}

Designar::MTreeNode<LevelInfo>* LevelTree::find_mini_boss(LevelEra era) const
{
    for (Designar::MTreeNode<LevelInfo>* node = root->get_first_child();
         node != nullptr; node = node->get_right_sibling())
    {
        if (node->get_key().era == era)
        {
            return node;
        }
    }

    return nullptr;
}

const Designar::MTreeNode<LevelInfo>* LevelTree::get_mini_boss(LevelEra era) const
{
    return find_mini_boss(era);
}

Designar::MTreeNode<LevelInfo>* LevelTree::find_next_level(LevelEra era) const
{
    Designar::MTreeNode<LevelInfo>* miniBoss = find_mini_boss(era);
    if (miniBoss == nullptr || miniBoss->get_key().completed)
    {
        return nullptr;
    }

    for (Designar::MTreeNode<LevelInfo>* node = miniBoss->get_first_child();
         node != nullptr; node = node->get_right_sibling())
    {
        if (!node->get_key().completed)
        {
            return node;
        }
    }

    return nullptr;
}

const Designar::MTreeNode<LevelInfo>* LevelTree::get_next_level(LevelEra era) const
{
    return find_next_level(era);
}

bool LevelTree::complete_next_level(LevelEra era)
{
    Designar::MTreeNode<LevelInfo>* level = find_next_level(era);
    if (level == nullptr)
    {
        return false;
    }

    level->get_key().completed = true;
    return true;
}

bool LevelTree::can_fight_mini_boss(LevelEra era) const
{
    Designar::MTreeNode<LevelInfo>* miniBoss = find_mini_boss(era);
    if (miniBoss == nullptr || miniBoss->get_key().completed)
    {
        return false;
    }

    for (Designar::MTreeNode<LevelInfo>* level = miniBoss->get_first_child();
         level != nullptr; level = level->get_right_sibling())
    {
        if (!level->get_key().completed)
        {
            return false;
        }
    }

    return true;
}

bool LevelTree::complete_mini_boss(LevelEra era)
{
    if (!can_fight_mini_boss(era))
    {
        return false;
    }

    find_mini_boss(era)->get_key().completed = true;
    return true;
}

bool LevelTree::is_era_completed(LevelEra era) const
{
    Designar::MTreeNode<LevelInfo>* miniBoss = find_mini_boss(era);
    return miniBoss != nullptr && miniBoss->get_key().completed;
}

bool LevelTree::can_fight_final_boss() const
{
    if (root->get_key().completed)
    {
        return false;
    }

    for (Designar::MTreeNode<LevelInfo>* miniBoss = root->get_first_child();
         miniBoss != nullptr; miniBoss = miniBoss->get_right_sibling())
    {
        if (!miniBoss->get_key().completed)
        {
            return false;
        }
    }

    return true;
}

bool LevelTree::complete_final_boss()
{
    if (!can_fight_final_boss() || root->get_key().completed)
    {
        return false;
    }

    root->get_key().completed = true;
    return true;
}
