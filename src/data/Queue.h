#ifndef QUEUE_H
#define QUEUE_H

#include <stdexcept>

template <typename T>
class Queue
{
private:
    struct Node
    {
        T data;
        Node *next;
    };

    Node *frontNode;
    Node *rearNode;
    int nodeCount;

public:
    Queue()
        : frontNode(nullptr), rearNode(nullptr), nodeCount(0) {}

    ~Queue()
    {
        clear();
    }

    void enqueue(T item)
    {
        Node *newNode = new Node;
        newNode->data = item;
        newNode->next = nullptr;

        if (rearNode == nullptr)
        {
            frontNode = newNode;
            rearNode = newNode;
        }
        else
        {
            rearNode->next = newNode;
            rearNode = newNode;
        }

        nodeCount++;
    }

    void dequeue()
    {
        if (frontNode == nullptr)
        {
            throw std::runtime_error("Cannot dequeue from an empty queue");
        }

        Node *temp = frontNode;
        frontNode = frontNode->next;

        if (frontNode == nullptr)
        {
            rearNode = nullptr;
        }

        delete temp;
        nodeCount--;
    }

    T peek() const
    {
        if (frontNode == nullptr)
        {
            throw std::runtime_error("Cannot peek into an empty queue");
        }

        return frontNode->data;
    }

    bool isEmpty() const
    {
        return frontNode == nullptr;
    }

    int size() const
    {
        return nodeCount;
    }

    void clear()
    {
        while (!isEmpty())
        {
            dequeue();
        }
    }
};

#endif
