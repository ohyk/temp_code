#pragma once

#include "abs_linear.h"

template <class T>
struct ChainNode
{
    // dates
    T             element_;
    ChainNode<T>* next_;

    // funcs
    ChainNode() {}
    ChainNode(T const& elem) : element_{elem} {}
    ChainNode(T const& elem, ChainNode<T>* next) : element_{elem}, next_{next} {}
};

template <class T>
class Chain : public AbsLinear<T>
{
public:
    using size_type = typename AbsLinear<T>::size_type;
    using pos_type  = typename AbsLinear<T>::pos_type;

public:
    Chain(size_type init = 10);
    Chain(Chain&&)                 = default;
    Chain(const Chain&)            = default;
    Chain& operator=(Chain&&)      = default;
    Chain& operator=(const Chain&) = default;
    virtual ~Chain()               = default;

    size_type size() const override;
    bool      empty() const override;
    T&        get(pos_type index) override;
    void      erase(pos_type index) override;
    pos_type  index_of(T const& elem) override;
    void      insert(pos_type index, T const& elem) override;

private:
    ChainNode<T>* first_node_{nullptr};
    size_type     size_;

    void check_index(size_type i) const;
};
