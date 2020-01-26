#pragma once

#include <vector>
#include <string>
#include <memory>
#include <map>

class Object;

class Component
{
public:
    enum class Type
    {
        Renderable,
        Physics,
        Sound,
        Network,
        Script
    };

    Component(Type type) : m_type(type) {}
    virtual ~Component() {}

    Type getType() const { return m_type; }

    void setOwner(std::shared_ptr<Object> owner) { m_owner = owner; }
    std::shared_ptr<Object> getOwner() { return m_owner; }

protected:
    Type m_type;
    std::shared_ptr<Object> m_owner;
};

class Object
{
public:
    Object(const std::string& name = "root")
        : m_name(name)
    {
    }

    void setName(const std::string& name) { m_name = name; }
    const std::string& getName() { return m_name; }

    void setRoot(std::shared_ptr<Object> root) { m_root = root; }
    std::shared_ptr<Object> getRoot() { return m_root; }

    void updateChildPositions()
    {
        for (auto c : m_childs)
        {
            c->updateChildPositions();
        }
    }

    void addChild(std::shared_ptr<Object> object) { m_childs.push_back(object); }
    bool hasComponent(Component::Type type)
    {
        for (auto c : m_components)
        {
            if (c.first == type)
            {
                return true;
            }
        }
        return false;
    }

    std::vector<std::shared_ptr<Component>> getComponents(Component::Type type)
    {
        for (auto c : m_components)
        {
            if (c.first == type)
            {
                return c.second;
            }
        }
        static std::vector<std::shared_ptr<Component>> empty;
        return empty;
    }

    void addComponent(std::shared_ptr<Component> c)
    {
        Component::Type type = c->getType();
        m_components[type].push_back(c);
    }

private:
    std::string m_name;
    std::vector<std::shared_ptr<Object>> m_childs;
    std::map<Component::Type,  std::vector<std::shared_ptr<Component>>> m_components;
    std::shared_ptr<Object> m_root;
};