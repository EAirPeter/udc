#ifndef UDC_REF_CONTAINER_HPP_
#define UDC_REF_CONTAINER_HPP_

#include <functional>
#include <iterator>
#include <type_traits>
#include <vector>

namespace udc {

template<class tIt>
class RefIt {
private:
    using Iterator = tIt;
    using Traits = std::iterator_traits<Iterator>;
    using Ref = typename Traits::value_type;
public:
    using iterator_category = typename Traits::iterator_category;
    using value_type = typename Ref::type;
    using difference_type = typename Traits::difference_type;
    using pointer = value_type *;
    using reference = value_type &;
public:
    inline RefIt() noexcept(std::is_nothrow_default_constructible_v<Iterator>) = default;
    inline RefIt(const RefIt &) noexcept(std::is_nothrow_copy_constructible_v<Iterator>) = default;
    inline RefIt(const tIt &it) noexcept(std::is_nothrow_copy_constructible_v<Iterator>) : x_it(it) {}
    inline RefIt &operator =(const RefIt &) noexcept(std::is_nothrow_copy_assignable_v<Iterator>) = default;

    inline reference operator *() const noexcept {
        return x_it->get();
    }

    inline pointer operator ->() const noexcept {
        return &x_it->get();
    }

    inline reference operator [](difference_type diff) const noexcept {
        return x_it[diff].get();
    }

    inline bool operator ==(const RefIt &it) const noexcept {
        return x_it == it.x_it;
    }

    inline bool operator !=(const RefIt &it) const noexcept {
        return x_it != it.x_it;
    }

    inline bool operator <(const RefIt &it) const noexcept {
        return x_it < it.x_it;
    }

    inline bool operator >(const RefIt &it) const noexcept {
        return x_it > it.x_it;
    }

    inline bool operator <=(const RefIt &it) const noexcept {
        return x_it <= it.x_it;
    }

    inline bool operator >=(const RefIt &it) const noexcept {
        return x_it >= it.x_it;
    }

    inline RefIt &operator ++() noexcept {
        ++x_it;
        return *this;
    }

    inline RefIt operator ++(int) noexcept {
        return x_it++;
    }

    inline RefIt &operator --() noexcept {
        --x_it;
        return *this;
    }

    inline RefIt operator --(int) noexcept {
        return x_it--;
    }

    inline RefIt operator +(difference_type diff) const noexcept {
        return x_it + diff;
    }

    inline RefIt operator -(difference_type diff) const noexcept {
        return x_it - diff;
    }

    inline RefIt &operator +=(difference_type diff) noexcept {
        x_it += diff;
        return *this;
    }

    inline RefIt &operator -=(difference_type diff) noexcept {
        x_it -= diff;
        return *this;
    }

private:
    Iterator x_it;
};

template<class tCont>
class RefContainer : public tCont {
private:
    using Base = tCont;
    using Ref = typename Base::value_type;
public:
    using value_type = typename Ref::type;
    using reference = value_type &;
    using const_reference = const value_type &;
    using iterator = RefIt<typename Base::iterator>;
    using const_iterator = RefIt<typename Base::const_iterator>;
    using reverse_iterator = RefIt<typename Base::reverse_iterator>;
    using const_reverse_iterator = RefIt<typename Base::const_reverse_iterator>;
public:
    using Base::Base;
public:
    inline iterator begin() noexcept {
        return Base::begin();
    }

    inline iterator end() noexcept {
        return Base::end();
    }

    inline const_iterator begin() const noexcept {
        return Base::begin();
    }

    inline const_iterator end() const noexcept {
        return Base::end();
    }

    inline const_iterator cbegin() const noexcept {
        return Base::cbegin();
    }

    inline const_iterator cend() const noexcept {
        return Base::cend();
    }

    inline reverse_iterator rbegin() noexcept {
        return Base::rbegin();
    }

    inline reverse_iterator rend() noexcept {
        return Base::rend();
    }

    inline const_reverse_iterator rbegin() const noexcept {
        return Base::rbegin();
    }

    inline const_reverse_iterator rend() const noexcept {
        return Base::rend();
    }

    inline const_reverse_iterator crbegin() const noexcept {
        return Base::crbegin();
    }

    inline const_reverse_iterator crend() const noexcept {
        return Base::crend();
    }

    inline void swap(RefContainer &rc) noexcept {
        Base::swap(rc);
    }

    friend inline void swap(RefContainer &lhs, RefContainer &rhs) noexcept {
        lhs.swap(rhs);
    }
};

template<class tObj>
using RefVec = RefContainer<std::vector<std::reference_wrapper<tObj>>>;

}

#endif
