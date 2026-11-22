#pragma once
#include <array.hpp>
#include <functional>
#include <memory>

class Enemy;
class Player;

enum class NodeStatus {
    Success,
    Failure,
    Running
};

struct BTContext {
    Enemy& enemy;
    Player& player;
    float dt;
    float distanceToPlayer;
};

class BTNode {
public:
    virtual ~BTNode() = default;
    virtual NodeStatus tick(BTContext& ctx) = 0;
};

class CompositeNode : public BTNode {
protected:
    Designar::DynArray<BTNode*> children;

public:
    CompositeNode() = default;
    CompositeNode(const CompositeNode&) = delete;
    CompositeNode& operator=(const CompositeNode&) = delete;
    ~CompositeNode() override;

    void addChild(std::unique_ptr<BTNode> child);
};

// OR lógico: ejecuta hijos en orden hasta que uno devuelva Success o Running
class Selector : public CompositeNode {
public:
    NodeStatus tick(BTContext& ctx) override;
};

// AND lógico: ejecuta hijos en orden mientras devuelvan Success
class Sequence : public CompositeNode {
public:
    NodeStatus tick(BTContext& ctx) override;
};

// Decorador: invierte Success <-> Failure
class Inverter : public BTNode {
private:
    std::unique_ptr<BTNode> child;

public:
    explicit Inverter(std::unique_ptr<BTNode> childNode);
    NodeStatus tick(BTContext& ctx) override;
};

// Nodo hoja de condición
class ConditionNode : public BTNode {
public:
    using Predicate = std::function<bool(BTContext&)>;

private:
    Predicate predicate;

public:
    explicit ConditionNode(Predicate pred);
    NodeStatus tick(BTContext& ctx) override;
};

// Nodo hoja de acción
class ActionNode : public BTNode {
public:
    using Action = std::function<NodeStatus(BTContext&)>;

private:
    Action action;

public:
    explicit ActionNode(Action act);
    NodeStatus tick(BTContext& ctx) override;
};