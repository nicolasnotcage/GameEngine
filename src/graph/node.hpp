/*
Developed for JHU 605.668: Computer Gaming Engines

This is free and unencumbered software released into the public domain.
For more information, please refer to <https://unlicense.org>
*/

#ifndef GRAPH_NODE_HPP
#define GRAPH_NODE_HPP

#include "graph/scene_state.hpp"

#include <memory>
#include <string>
#include <vector>

namespace cge
{
class Node
{
  public:
    Node() = default;

    ~Node() = default;

    virtual void init(SceneState &scene_state) = 0;

    virtual void init_children(SceneState &scene_state);

    virtual void destroy() = 0;

    virtual void destroy_children();

    virtual void draw(SceneState &scene_state) = 0;

    virtual void draw_children(SceneState &scene_state);

    virtual void update(SceneState &scene_state) = 0;

    virtual void update_children(SceneState &scene_state);

    void set_name(const std::string &n);

    void add_child(std::shared_ptr<Node> child);

    void clear_children();

  protected:
    std::string                        name_;
    std::vector<std::shared_ptr<Node>> children_;
};

} // namespace cge

#endif // GRAPH_NODE_HPP
