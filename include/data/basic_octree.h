#ifndef BASIC_OCTREE_H
#define	BASIC_OCTREE_H

#include <array>
#include <memory>
#include <type_traits>
#include <stdexcept>
#include <cassert>
#include "math/vector.h"
#include "make_unique.h"

namespace trillek {

typedef vector3d<std::size_t> octree_index_t;

/**
 * @brief Basic octree for storing any type
 * Requirements on type T:
 * Must be assignable and copy-constructable
 * Must support comparison - if two instances compare
 * equal, they are considered redundant and only one
 * will be stored.
 */
template <typename T>
class basic_octree {
public:
    typedef basic_octree<T> basic_octree_;
    typedef typename std::decay<T>::type value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    basic_octree();
    basic_octree(const basic_octree& other);
    basic_octree(basic_octree&& other);
    octree_index_t size() const;
    void resize(octree_index_t target_size);
    //unset points not valid
    const_reference operator [](octree_index_t ind) const;
    //unset points not valid
    const_reference get_data(octree_index_t ind) const;
    //unset points return def
    const_reference get_data_default(octree_index_t ind,
            const_reference def) const;
    template <typename U>   //convertable to T
    void set_data(octree_index_t ind, U&& data);
    void unset_data(octree_index_t ind);
private:
    std::pair<std::size_t, octree_index_t> compute_child_access(
            octree_index_t ind) const;
    void split_children();
    void combine_children();
    typedef std::unique_ptr<basic_octree_> child_ptr;
    typedef std::array<child_ptr, 8> children_type;
    typedef std::unique_ptr<value_type> value_type_ptr;
    std::size_t m_size_exp;
    bool m_has_children;
    children_type m_children;
    bool m_has_data;
    value_type_ptr m_data;
};

template <typename T>
basic_octree<T>::basic_octree() : m_size_exp(0), m_has_children(false),
        m_has_data(false) {}
template <typename T>
basic_octree<T>::basic_octree(const basic_octree& other)
        : m_size_exp(other.m_size_exp), m_has_children(other.m_has_children),
        m_has_data(other.m_has_data) {
    if(m_has_children) {
        for(std::size_t i = 0; i != m_children.size(); ++i) {
            m_children[i] = make_unique(*other.m_children[i].get());
        }
    }
    if(m_has_data) {
        m_data = make_unique(*other.m_data.get());
    }
}
template <typename T>
basic_octree<T>::basic_octree(basic_octree&& other)
        : m_size_exp(other.m_size_exp), m_has_children(other.m_has_children),
        m_has_data(other.m_has_data) {
    if(m_has_children) {
        for(std::size_t i = 0; i != m_children.size(); ++i) {
            m_children[i] = std::move(other.m_children[i]);
        }
    }
    if(m_has_data) {
        m_data = std::move(other.m_data);
    }
}
template <typename T>
octree_index_t basic_octree<T>::size() const {
    const std::size_t actual_size = (1 << m_size_exp);
    return octree_index_t(actual_size, actual_size, actual_size);
}
template <typename T>
void basic_octree<T>::resize(octree_index_t target_size) {
    std::size_t max_dimention = std::max(
            std::max(target_size.x, target_size.y), target_size.z);
    assert(!(m_has_children || m_has_data));
    for(m_size_exp = 0; (1 << m_size_exp) < max_dimention; ++m_size_exp);
}
template <typename T>
typename basic_octree<T>::const_reference basic_octree<T>::operator [](
        octree_index_t ind) const {
    return get_data(ind);
}
template <typename T>
typename basic_octree<T>::const_reference basic_octree<T>::get_data(
        octree_index_t ind) const {
    std::size_t child_index;
    octree_index_t child_access;
    std::tie(child_index, child_access) = compute_child_access(ind);
    assert(m_has_children || m_has_data);
    if(m_has_data) {
        return *m_data.get();
    } else if(m_has_children) {
        return m_children[child_index].get()->get_data(child_access);
    }
}
template <typename T>
typename basic_octree<T>::const_reference basic_octree<T>::get_data_default(
        octree_index_t ind, const_reference def) const {
    std::size_t child_index;
    octree_index_t child_access;
    std::tie(child_index, child_access) = compute_child_access(ind);
    if(m_has_data) {
        return *m_data.get();
    } else if(m_has_children) {
        return m_children[child_index].get()->get_data(child_access);
    } else {
        return def;
    }
}
template <typename T> template <typename U>
void basic_octree<T>::set_data(octree_index_t ind, U&& data) {
    std::size_t child_index;
    octree_index_t child_access;
    std::tie(child_index, child_access) = compute_child_access(ind);
    if(m_size_exp == 0) {
        //I am leaf
        m_data = make_unique<value_type>(std::forward<U>(data));
        m_has_data = true;
    } else {
        if(!m_has_children) {
            split_children();
        }
        m_children[child_index].get()->set_data(child_access,
                std::forward<U>(data));
        combine_children();
    }
}
template <typename T>
void basic_octree<T>::unset_data(octree_index_t ind) {
    std::size_t child_index;
    octree_index_t child_access;
    std::tie(child_index, child_access) = compute_child_access(ind);
    if(m_size_exp == 0) {
        //I am leaf
        m_data.reset(nullptr);
        m_has_data = false;
    } else if(m_has_children) {
        m_children[child_index].get()->unset_data(child_access);
        combine_children();
    }
}
template <typename T>
std::pair<std::size_t, octree_index_t>
        basic_octree<T>::compute_child_access(octree_index_t ind) const {
    const octree_index_t s = size();
    assert(ind.x < s.x);
    assert(ind.y < s.y);
    assert(ind.z < s.z);
    const std::size_t mask = (1 << (m_size_exp - 1));
    const std::size_t z_bit = (ind.z & mask) != 0;
    const std::size_t y_bit = (ind.y & mask) != 0;
    const std::size_t x_bit = (ind.x & mask) != 0;
    const std::size_t zyx_ind = static_cast<std::size_t>(
            (z_bit << 2) | (y_bit << 1) | (x_bit << 0));
    const octree_index_t zyx_access(ind.x & ~mask,
            ind.y & ~mask, ind.z & ~mask);
    return std::make_pair(zyx_ind, zyx_access);
}
template <typename T>
void basic_octree<T>::split_children() {
    assert(!m_has_children);
    for(child_ptr& child : m_children) {
        child = make_unique<basic_octree_>();
        child.get()->m_size_exp = m_size_exp - 1;
    }
}
template <typename T>
void basic_octree<T>::combine_children() {
    auto same_data = [](const value_type_ptr& lhs,
            const value_type& rhs)->bool {
        if(lhs.get() == rhs.get()) {
            return true;
        } else if(lhs.get() == nullptr || rhs.get() == nullptr) {
            return false;
        } else {
            return *lhs.get() == *rhs.get();
        }
    };
    assert(m_has_children);
    for(std::size_t i = 0; i != m_children.size(); ++i) {
        assert(!m_children[i].get()->m_has_children);
        if(i == 0) continue;
        if(!same_data(m_children[i-1].get()->m_data,
                m_children[i].get()->m_data)) {
            return;
        }
    }
    m_has_data = std::move(m_children[0].get()->m_has_data);
    m_data = std::move(m_children[0].get()->m_data);
    for(child_ptr& child : m_children) {
        child.reset(nullptr);
    }
    m_has_children = false;
}

}


#endif	/* BASIC_OCTREE_H */

