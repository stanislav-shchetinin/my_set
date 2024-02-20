#pragma once

namespace ads {

enum Color {
    BLACK,
    RED
};

template<typename T, typename Comparator = std::less<T>>
class set {

public:
    using key_type = T;
    using value_type = T;
    using size_type = std::size_t;

private:
    struct Node {
        Node* left = nullptr;
        Node* right = nullptr;
        Node* parent = nullptr;
        Color color = BLACK;
        value_type data_;
    };
    #define NIL &(element::sentinel)
public:
    class element {

    public:
        using value_type = T;
        using const_reference = const value_type&;
        using const_pointer = const value_type*;
        static inline Node sentinel = {0, 0, 0, BLACK, 0};
    private:
        Node* data_ = nullptr;
    public:
        element() = default;
        element(Node* data) : data_(data) {}

        const_reference operator*() const {
            if (!data_) return (const_reference)0;
            return data_->data_;
        }

        const_pointer operator->() const {
            if (!data_) return nullptr;
            return &data_->data_;
        }

        bool operator==(const element& other) const {
            return data_ == other.data_;
        }

        bool operator!=(const element& other) const {
            return data_ != other.data_;
        }

        element& operator++() {
            if (data_->right == NIL) {
                while (data_->parent && data_->parent != NIL && data_->parent->right == data_) {
                    data_ = data_->parent;
                }
                data_ = data_->parent;
            } else {
                data_ = data_->right;
                while (data_->left != NIL) {
                    data_ = data_->left;
                }
            }
            return *this;
        }

        element operator++(int) {
            element iter = *this;
            ++(*this);
            return iter;
        }

    };

private:

    #define cmp_eq(l, r) (!cmp(l, r) && !cmp(r, l))
    //Node sentinel = {NIL, NIL, 0, BLACK, 0};
    Node *root_ = NIL;
    size_t size_ = 0;
    Comparator cmp = Comparator{};

    void rotate_left(Node *x) {
        Node *y = x->right;
        x->right = y->left;
        if (y->left != NIL) y->left->parent = x;
        if (y != NIL) y->parent = x->parent;
        if (x->parent) {
            if (x == x->parent->left) {
                x->parent->left = y;
            } else {
                x->parent->right = y;
            }
        } else {
            root_ = y;
        }
        y->left = x;
        if (x != NIL) x->parent = y;
    }

    void rotate_right(Node *x) {
        Node *y = x->left;
        x->left = y->right;
        if (y->right != NIL) y->right->parent = x;
        if (y != NIL) y->parent = x->parent;
        if (x->parent) {
            if (x == x->parent->right) {
                x->parent->right = y;
            } else {
                x->parent->left = y;
            }
        } else {
            root_ = y;
        }
        y->right = x;
        if (x != NIL) x->parent = y;
    }

    void insert_fixup(Node *x) {
        while (x != root_ && x->parent->color == RED) {
            if (x->parent == x->parent->parent->left) {
                Node *y = x->parent->parent->right;
                if (y->color == RED) {
                    x->parent->color = BLACK;
                    y->color = BLACK;
                    x->parent->parent->color = RED;
                    x = x->parent->parent;
                } else {
                    if (x == x->parent->right) {
                        x = x->parent;
                        rotate_left(x);
                    }
                    x->parent->color = BLACK;
                    x->parent->parent->color = RED;
                    rotate_right(x->parent->parent);
                }
            } else {
                Node *y = x->parent->parent->left;
                if (y->color == RED) {
                    x->parent->color = BLACK;
                    y->color = BLACK;
                    x->parent->parent->color = RED;
                    x = x->parent->parent;
                } else {
                    if (x == x->parent->left) {
                        x = x->parent;
                        rotate_right(x);
                    }
                    x->parent->color = BLACK;
                    x->parent->parent->color = RED;
                    rotate_left(x->parent->parent);
                }
            }
        }
        root_->color = BLACK;
    }

    void delete_fixup(Node *x) {
        while (x != root_ && x->color == BLACK) {
            if (x == x->parent->left) {
                Node *w = x->parent->right;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rotate_left(x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->right->color == BLACK) {
                        w->left->color = BLACK;
                        w->color = RED;
                        rotate_right(w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    rotate_left(x->parent);
                    x = root_;
                }
            } else {
                Node *w = x->parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rotate_right(x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        rotate_left(w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    rotate_right(x->parent);
                    x = root_;
                }
            }
        }
        x->color = BLACK;
    }

public:
    set() = default;

    set(Comparator cmp) : cmp(cmp) {}

    set(const set<T, Comparator>& other) :
            root_(copy(other.root_)),
            size_(other.size_),
            cmp(other.cmp) {}

    set<T, Comparator>& operator=(const set<T, Comparator>& other) {
        if (this != &other) {
            clear();
            root_ = copy(other.root_);
            size_ = other.size_;
            cmp = other.cmp;
        }
        return *this;
    }

    ~set() {
        clear();
    }

    [[nodiscard]] bool empty() const {
        return size_ == 0;
    }

    [[nodiscard]] size_type size() const {
        return size_;
    }

    void clear() {
        clear(root_);
        root_ = nullptr;
        size_ = 0;
    }

    element begin() const {
        return begin(root_);
    }

    element end() const {
        return element(nullptr);
    }

    element find(const key_type& value) const {
        return element(find(root_, value));
    }

    std::pair<element, bool> insert(const value_type& value) {
        Node *current, *parent, *x;
        current = this->root_;
        parent = 0;
        while (current != NIL) {
            if (cmp_eq(value, current->data_)) return {element(current), false};
            parent = current;
            current = cmp(value, current->data_) ?
                      current->left : current->right;
        }

        if ((x = (Node*)malloc (sizeof(*x))) == 0) {
            printf ("insufficient memory (insertNode)\n");
            exit(1);
        }
        x->data_ = value;
        x->parent = parent;
        x->left = NIL;
        x->right = NIL;
        x->color = RED;

        if (parent) {
            if(cmp(value, parent->data_)) {
                parent->left = x;
            }
            else {
                parent->right = x;
            }
        } else {
            root_ = x;
        }

        insert_fixup(x);
        ++size_;
        return {element(x), true};
    }

    void erase(const value_type& value) {
        Node* z = find(root_, value);
        Node *x, *y;
        if (!z || z == NIL) return;

        if (z->left == NIL || z->right == NIL) {
            y = z;
        } else {
            y = z->right;
            while (y->left != NIL) y = y->left;
        }

        if (y->left != NIL) {
            x = y->left;
        } else {
            x = y->right;
        }

        x->parent = y->parent;
        if (y->parent) {
            if (y == y->parent->left) {
                y->parent->left = x;
            } else {
                y->parent->right = x;
            }
        } else {
            root_ = x;
        }

        if (y != z) z->data_ = y->data_;


        if (y->color == BLACK) {
            delete_fixup(x);
        }

        free (y);
    }

    friend void swap(set<T, Comparator>& lhs, set<T, Comparator>& rhs) {
        using std::swap;
        swap(lhs.data_, rhs.data_);
        swap(lhs.size_, rhs.size_);
        swap(lhs.cmp, rhs.cmp);
    }

private:
    element begin(Node* root) const {
        if (!root || root == NIL) {
            return end();
        }
        while (root->left != NIL) {
            root = root->left;
        }
        return element(root);
    }

    void clear(Node* root) {
        if (root != NIL) {
            if (root != NIL)
                clear(root->left);
            if (root != NIL)
                clear(root->right);
            delete root;
        }
    }

    Node* find(Node* root, const key_type& value) const {
        Node *current = root;
        while (current != NIL) {
            if (cmp_eq(value, current->data_)) {
                return current;
            } else {
                current = cmp(value, current->data_) ?
                          current->left : current->right;
            }
        }
        return nullptr;
    }

    Node* copy(const Node* other) {
        if (!other) {
            return nullptr;
        }
        Node* node = new Node {
                other->data_,
                nullptr,
                copy(other->left),
                copy(other->right)
        };
        if (node->left != NIL) {
            node->left->parent = node;
        }
        if (node->right != NIL) {
            node->right->parent = node;
        }
        return node;
    }

};

}
