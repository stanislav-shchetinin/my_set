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
        Color color;
        value_type data_;
    };

public:
    class element {

    public:
        using value_type = T;
        using const_reference = const value_type&;
        using const_pointer = const value_type*;

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

        element& operator++() {
            if (!data_->right) {
                while (data_->parent && data_->parent->right == data_) {
                    data_ = data_->parent;
                }
                data_ = data_->parent;
            } else {
                data_ = data_->right;
                while (data_->left) {
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
    #define NIL &sentinel
    #define cmp_eq(l, r) (!cmp(l, r) && !cmp(r, l))
    Node sentinel = { NIL, NIL, 0, BLACK, 0};
    Node *root = NIL;
    size_t size_ = 0;
    Comparator cmp = Comparator{};

    void rotateLeft(Node *x) {
        Node *y = x->right;
        x->right = y->left;
        if (y->left != NIL) y->left->parent = x;
        if (y != NIL) y->parent = x->parent;
        if (x->parent) {
            if (x == x->parent->left)
                x->parent->left = y;
            else
                x->parent->right = y;
        } else {
            root = y;
        }
        y->left = x;
        if (x != NIL) x->parent = y;
    }

    void rotateRight(Node *x) {
        Node *y = x->left;
        x->left = y->right;
        if (y->right != NIL) y->right->parent = x;
        if (y != NIL) y->parent = x->parent;
        if (x->parent) {
            if (x == x->parent->right)
                x->parent->right = y;
            else
                x->parent->left = y;
        } else {
            root = y;
        }
        y->right = x;
        if (x != NIL) x->parent = y;
    }

    void insertFixup(Node *x) {
        while (x != root && x->parent->color == RED) {
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
                        rotateLeft(x);
                    }
                    x->parent->color = BLACK;
                    x->parent->parent->color = RED;
                    rotateRight(x->parent->parent);
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
                        rotateRight(x);
                    }
                    x->parent->color = BLACK;
                    x->parent->parent->color = RED;
                    rotateLeft(x->parent->parent);
                }
            }
        }
        root->color = BLACK;
    }

    void deleteFixup(Node *x) {
        while (x != root && x->color == BLACK) {
            if (x == x->parent->left) {
                Node *w = x->parent->right;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rotateLeft (x->parent);
                    w = x->parent->right;
                }
                if (w->left->color == BLACK && w->right->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->right->color == BLACK) {
                        w->left->color = BLACK;
                        w->color = RED;
                        rotateRight (w);
                        w = x->parent->right;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->right->color = BLACK;
                    rotateLeft (x->parent);
                    x = root;
                }
            } else {
                Node *w = x->parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    x->parent->color = RED;
                    rotateRight (x->parent);
                    w = x->parent->left;
                }
                if (w->right->color == BLACK && w->left->color == BLACK) {
                    w->color = RED;
                    x = x->parent;
                } else {
                    if (w->left->color == BLACK) {
                        w->right->color = BLACK;
                        w->color = RED;
                        rotateLeft (w);
                        w = x->parent->left;
                    }
                    w->color = x->parent->color;
                    x->parent->color = BLACK;
                    w->left->color = BLACK;
                    rotateRight (x->parent);
                    x = root;
                }
            }
        }
        x->color = BLACK;
    }

public:
    set() = default;

    set(Comparator cmp) : cmp(cmp) {}

    set(const set<T, Comparator>& other) :
            root(copy(other.root)),
            size_(other.size_),
            cmp(other.cmp) {}

    set<T, Comparator>& operator=(const set<T, Comparator>& other) {
        if (this != &other) {
            clear();
            root = copy(other.root);
            size_ = other.size_;
            cmp = other.cmp;
        }
        return *this;
    }

    ~set() {
        clear();
    }

    bool empty() const {
        return size_ == 0;
    }

    size_type size() const {
        return size_;
    }

    void clear() {
        clear(root);
        root = nullptr;
        size_ = 0;
    }

    element begin() const {
        return begin(root);
    }

    element end() const {
        return element(nullptr);
    }

    element find(const key_type& value) const {
        return element(find(root, value));
    }

    std::pair<element, bool> insert(const value_type& value) {
        Node *current, *parent, *x;
        current = this->root;
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

        if(parent) {
            if(cmp(value, parent->data_))
                parent->left = x;
            else
                parent->right = x;
        } else {
            root = x;
        }

        insertFixup(x);
        return {element(x), true};
    }

    void erase(const value_type& value) {
        Node* z = find(root, value);
        Node *x, *y;
        if (!z || z == NIL) return;

        if (z->left == NIL || z->right == NIL) {
            y = z;
        } else {
            y = z->right;
            while (y->left != NIL) y = y->left;
        }

        if (y->left != NIL)
            x = y->left;
        else
            x = y->right;

        x->parent = y->parent;
        if (y->parent)
            if (y == y->parent->left)
                y->parent->left = x;
            else
                y->parent->right = x;
        else
            root = x;

        if (y != z) z->data_ = y->data_;


        if (y->color == BLACK)
            deleteFixup (x);

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
        if (!root) {
            return end();
        }
        while (root->left) {
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
        while(current != NIL)
            if(cmp_eq(value, current->data_))
                return (current);
            else
                current = cmp(value, current->data_) ?
                          current->left : current->right;
        return(0);
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
        if (node->left) {
            node->left->parent = node;
        }
        if (node->right) {
            node->right->parent = node;
        }
        return node;
    }

};

}
