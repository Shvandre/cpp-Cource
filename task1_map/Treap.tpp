#ifndef TREAP_H
#define TREAP_H

#include <memory>
#include <random>
#include <stack>

template <typename T, class C>
struct Node;

template <typename T, class C>
using PNode = std::shared_ptr<Node<T, C>>;

template <typename T, class C>
using PNodePair = std::pair<PNode<T, C>, PNode<T, C>>;


template <typename T, class C>
struct Node {
private:
    inline static auto rnd = std::mt19937(std::random_device()());

public:
    unsigned prior;
    T key;
    C value;
    PNode<T, C> l = nullptr, r = nullptr;

    Node(const T& _key, const C& _value) {
        key = _key;
        value = _value;
        prior = rnd();
    }
};

template <typename T, class C>
class SearchingTree {
    PNode<T, C> root = nullptr;
    PNode<T, C> merge(const PNode<T, C>& l, const PNode<T, C>& r) {
        if (!l) return r;
        if (!r) return l;
        if (l->prior > r->prior) {
            l->r = merge(l->r, r);
            return l;
        }
        else {
            r->l = merge(l, r->l);
            return r;
        }
    }

    PNodePair<T, C> split(const PNode<T, C>& v, const T& key) {
        if (!v) return {nullptr, nullptr};
        if (v->key < key) {
            auto p = split(v->r, key);
            v->r = p.first;
            return {v, p.second};
        }
        else {
            auto p = split(v->l, key);
            v->l = p.second;
            return {p.first, v};
        }
    }
public:
    void insert(const T& key, const C& value) {
        if (is_present(key)) return;
        PNodePair<T, C> q = split(root, key);
        auto t = std::make_shared<Node<T, C>>(key, value);
        root = merge(q.first, merge(t, q.second));
    }

    void erase(const T& key) {
        if(!is_present(key)) return;
        PNodePair<T, C> q = split(root, key);
        PNodePair<T, C> q1 = split(q.second, key + 1);
        delete q1.first;
        root = merge(q.first, q1.second);
    }
    bool is_present(const T& key) {
        PNode<T, C> v = root;
        while (v) {
            if (v->key == key) return true;
            if (v->key < key) v = v->r;
            else v = v->l;
        }
        return false;
    }
    class Iterator {
        friend class SearchingTree;
        PNode<T, C> cur;
        std::stack<PNode<T, C>> st;
        bool is_end = false;
        void move_left() {
            while (cur) {
                st.push(cur);
                cur = cur->l;
            }
            cur = st.top();
            st.pop();
        }
    public:
        Iterator(std::stack<PNode<T, C>> _st, PNode<T, C> _cur, bool _is_end = false) {
            st = _st;
            cur = _cur;
            is_end = _is_end;
        }
        bool operator==(const Iterator& other) const {
            return (is_end && other.is_end) || (!is_end && !other.is_end && cur->key == other.cur->key);
        }
        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
        std::pair<T, C> operator*() {
            if (is_end) {
                throw std::runtime_error("Cannot dereference end() iterator");
            }
            return {cur->key, cur->value};
        }

        Iterator& operator++() {
            if (cur == nullptr) {
                throw std::runtime_error("Cannot increment end() iterator");
            }

            if (cur->r != nullptr) {
                // Переходим к правому поддереву и затем максимально влево
                cur = cur->r;
                while (cur->l != nullptr) {
                    st.push(cur);
                    cur = cur->l;
                }
            } else {
                if (st.empty()) {
                    cur = nullptr;
                    is_end = true;
                    return *this;
                }
                PNode<T, C> prev = cur;
                do {
                    cur = st.top();
                    st.pop();
                } while(cur->r == prev);
            }
            return *this;
        }
        Iterator operator->() {
            return **this;
        }
    };
    Iterator begin() {
        Iterator it({}, root);
        it.move_left();
        return it;
    }
    Iterator end() {
        return Iterator({}, nullptr, true);
    }
    Iterator find(const T& key) {
        PNode<T, C> v = root;
        std::stack<PNode<T, C>> st;
        while (v) {
            if (v->key == key) {
                return Iterator(st, v, v->l);
            }
            if (v->key < key) {
                st.push(v);
                v = v->r;
            }
            else {
                st.push(v);
                v = v->l;
            }
        }
        return end();
    }
    Iterator lower_bound(const T& key) {
        PNode<T, C> v = root;
        std::stack<PNode<T, C>> st;
        while (v) {
            if (v->key == key) {
                return Iterator(st, v);
            }
            if (v->key < key) {
                st.push(v);
                v = v->r;
            }
            else {
                st.push(v);
                v = v->l;
            }
        }
        return Iterator(st, v);
    }
    Iterator upper_bound(const T& key) {
        PNode<T, C> v = root;
        std::stack<PNode<T, C>> st;
        PNode<T, C> candidate = nullptr;

        while (v) {
            if (v->key > key) {
                candidate = v;
                st.push(v);
                v = v->l;
            } else {
                v = v->r;
            }
        }

        if (candidate) {
            return Iterator(st, candidate);
        } else {
            return end();
        }
    }
    struct Range {
        Iterator first, second;
        Iterator begin() {
            return first;
        }
        Iterator end() {
            return second;
        }
        std::pair<T, C> operator*() {
            return *first;
        }
        Range& operator++() {
            ++first;
            return *this;
        }
        Range operator->() {
            return *this;
        }
    };
    Range range(T left, T right) {
        return {lower_bound(left), lower_bound(right)};
    }
};

#endif //TREAP_H
