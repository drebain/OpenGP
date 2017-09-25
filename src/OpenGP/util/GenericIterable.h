// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <functional>
#include <type_traits>


//=============================================================================
namespace OpenGP {
//=============================================================================

template <typename T>
class GenericIterable {
public:

    using Advancer = std::function<T*()>;

    class Iterator {
    private:

        T *ptr;

        Advancer advancer;

    public:

        Iterator(Advancer advancer) {
            this->advancer = advancer;
            ptr = this->advancer();
        }

        T &operator*() { return *ptr; }

        bool operator==(const Iterator &rhs) const {
            return ptr == rhs.ptr;
        }

        bool operator!=(const Iterator &rhs) const {
            return ptr != rhs.ptr;
        }

        Iterator &operator++() {
            ptr = advancer();
            return *this;
        }

    };

private:

    Advancer base_advancer;

public:

    GenericIterable(Advancer advancer) {
        base_advancer = advancer;
    }

    Iterator begin() {
        return Iterator(base_advancer);
    }

    Iterator end() {
        return Iterator([](){ return nullptr; });
    }

    template <typename I>
    static GenericIterable adaptor(I &iterable) {

        auto it = iterable.begin();
        auto end = iterable.end();

        Advancer advancer = [it, end]() mutable {
            if (it != end) {
                T *ptr = &(*it);
                ++it;
                return ptr;
            }
            return (T*)nullptr;
        };

        return GenericIterable(advancer);

    }

    GenericIterable filter(std::function<bool(T&)> predicate) {
        Advancer advancer = base_advancer;
        GenericIterable new_iterable([predicate, advancer](){

            T *ptr = nullptr;

            do {
                ptr = advancer();
            } while(ptr != nullptr && !predicate(*ptr));

            return ptr;

        });
        return new_iterable;
    }

    template <typename Predicate>
    GenericIterable<typename std::remove_pointer<typename std::result_of<Predicate(T&)>::type>::type> map(Predicate predicate) {

        using ResultPointerType = typename std::result_of<Predicate(T&)>::type;
        using ResultValueType = typename std::remove_pointer<ResultPointerType>::type;
        
        Advancer advancer = base_advancer;
        GenericIterable<ResultValueType> new_iterable([predicate, advancer](){
            T *ptr = advancer();
            if (ptr == nullptr) {
                return (ResultPointerType)nullptr;
            } else {
                return predicate(*ptr);
            }
        });
        return new_iterable;
    }

    template <typename U>
    GenericIterable<U> cast() {
        Advancer advancer = base_advancer;
        GenericIterable new_iterable([advancer](){
            return dynamic_cast<U*>(advancer());
        });
        return new_iterable;
    }

};

//=============================================================================
} // OpenGP::
//=============================================================================
