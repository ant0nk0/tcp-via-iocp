#pragma once

#include "noncopyable.h"
#include <functional>

namespace Utils
{
    //! Simple binary tree. Can add unique elements and walk around the elements
    //! Smart pointers not used for performance purposes
    template<typename T>
    class BinaryTree : NonCopyable
    {
    public:

        //! Tree node
        class Node
        {
        public:
            Node(const T& value)
                : left_(nullptr)
                , right_(nullptr)
                , value_(value)
            {}

            ~Node()
            {
                delete left_;
                delete right_;
            }

            Node* left_;
            Node* right_;
            T value_;
        };

        //! Tree value type
        typedef typename T ValueType;

        //! Walk func
        typedef std::function<void(typename const ValueType&)> WalkFunc;

        //! Tree root
        typename Node* root_;

    public:
        BinaryTree() 
            : root_(nullptr)
        {}
        
        ~BinaryTree()
        {
            delete root_;
        }

        //! Add new value to the tree. Returns false if value already exist, true otherwise
        bool Add(const T& element)
        {
            return AddInternal(&root_, element);
        }

        //! Walk around the tree and call 'func' for every node's value
        void Walk(WalkFunc&& func) const
        {
            WalkInternal(root_, func);
        }

    private:
        // non-copyable
        BinaryTree(const BinaryTree&);
        void operator = (const BinaryTree&);

        //! Recursive find tree node to add new element to. Search starts from 'node'. Returns false if value already exist, true otherwise
        bool AddInternal(Node** node, const T& element)
        {
            if (*node == nullptr)
            {
                *node = new Node(element);
                return true;
            }

            Node* const node_ptr = *node;

            if (element == node_ptr->value_)
                return false;

            return AddInternal(element < node_ptr->value_ ? &node_ptr->left_ : &node_ptr->right_, element);
        }

        //! Recursive tree walker. Starts from 'node'
        void WalkInternal(typename const Node* const node, WalkFunc& func) const
        {
            if (node == nullptr)
                return;

            WalkInternal(node->left_, func);
            func(node->value_);
            WalkInternal(node->right_, func);
        }
    };

} // namespace Utils