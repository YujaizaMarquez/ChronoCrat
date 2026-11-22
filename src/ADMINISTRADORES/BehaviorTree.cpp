#include "BehaviorTree.hpp"

CompositeNode::~CompositeNode()
{
    for (size_t i = 0; i < children.size(); ++i) {
        delete children[i];
    }
    children.clear();
}

void CompositeNode::addChild(std::unique_ptr<BTNode> child)
{
    if (child) {
        children.append(child.release());
    }
}

NodeStatus Selector::tick(BTContext& ctx)
{
    for (size_t i = 0; i < children.size(); ++i) {
        NodeStatus status = children[i]->tick(ctx);
        if (status != NodeStatus::Failure) {
            return status;
        }
    }
    return NodeStatus::Failure;
}

NodeStatus Sequence::tick(BTContext& ctx)
{
    for (size_t i = 0; i < children.size(); ++i) {
        NodeStatus status = children[i]->tick(ctx);
        if (status != NodeStatus::Success) {
            return status;
        }
    }
    return NodeStatus::Success;
}

Inverter::Inverter(std::unique_ptr<BTNode> childNode)
    : child(std::move(childNode))
{
}

NodeStatus Inverter::tick(BTContext& ctx)
{
    if (!child) {
        return NodeStatus::Failure;
    }

    NodeStatus status = child->tick(ctx);
    if (status == NodeStatus::Success) {
        return NodeStatus::Failure;
    }
    if (status == NodeStatus::Failure) {
        return NodeStatus::Success;
    }
    return NodeStatus::Running;
}

ConditionNode::ConditionNode(Predicate pred)
    : predicate(std::move(pred))
{
}

NodeStatus ConditionNode::tick(BTContext& ctx)
{
    return (predicate && predicate(ctx)) ? NodeStatus::Success : NodeStatus::Failure;
}

ActionNode::ActionNode(Action act)
    : action(std::move(act))
{
}

NodeStatus ActionNode::tick(BTContext& ctx)
{
    return action ? action(ctx) : NodeStatus::Failure;
}