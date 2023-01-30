#ifndef A1_LRU_C
#define A1_LRU_C
#include "LRU.h"
/*this is LRU implementaition file, used the double linked node for the page update */
class Node;

LRU::LRU(int fullSize) {
    this->curSizeNum = 0;
    this->fullSizeCapcity = fullSize;
}

void LRU::insert(Node *node) {
    curSizeNum++;
    // before u insert the new page node, you need to judge at first to avoid the overflow
    if (curSizeNum<=fullSizeCapcity){
        if(curSizeNum == 1) {
            head = node;
            tail = node;
        } else {
            tail->setNext(node);
            node->setPre(tail);
            tail = node;
        }
    }
    else{
        std::cout << "This is already full, do not add any node\n";
        std::cout << "You need to use the LRU algorithm\n";
        return;
    }
}

void LRU::update(Node *node) {
    Node* tmpHead = new Node(nullptr);
    Node* tempTail = new Node(nullptr);
    head->setPre(tmpHead);
    tail->setNext(tempTail);
    tmpHead->setNext(head);
    tempTail->setPre(tail);

    Node* preNode = node->getPre();
    Node* nextNode = node->getNext();

    nextNode->setPre(preNode);
    preNode->setNext(nextNode);

    node->setNext(tempTail);
    node->setPre(tempTail->getPre());

    tempTail->getPre()->setNext(node);
    tempTail->setPre(node);

    head = tmpHead->getNext();
    tail = tempTail->getPre();

    delete tmpHead;
    delete tempTail;
    tail->setNext(nullptr);
    head->setPre(nullptr);
}

Node* LRU::evict() {
    Node* dummyHead = new Node(nullptr);
    Node* dummyTail = new Node(nullptr);
    head->setPre(dummyHead);
    tail->setNext(dummyTail);
    dummyHead->setNext(head);
    dummyTail->setPre(tail);

    Node* node = head;
    while(node->getPage()->isIsPin()) {
        node = node->getNext();
    }

    Node* preNode = node->getPre();
    Node* nextNode = node->getNext();

    nextNode->setPre(preNode);
    preNode->setNext(nextNode);

    node->setNext(nullptr);
    node->setPre(nullptr);

    if (dummyHead->getNext() == dummyTail){
        head = nullptr;
        tail = nullptr;
    }else{
        head = dummyHead->getNext();
        tail = dummyTail->getPre();
        tail->setNext(nullptr);
        head->setPre(nullptr);
    }

    delete dummyHead;
    delete dummyTail;

    curSizeNum--;
    return node;
}

bool LRU::isEmpty() {
    return head == nullptr;
}

Node* LRU::finalEvict() {
    Node* dummyHead = new Node(nullptr);
    Node* dummyTail = new Node(nullptr);
    head->setPre(dummyHead);
    tail->setNext(dummyTail);
    dummyHead->setNext(head);
    dummyTail->setPre(tail);

    Node* node = head;

    Node* preNode = node->getPre();
    Node* nextNode = node->getNext();

    nextNode->setPre(preNode);
    preNode->setNext(nextNode);

    node->setNext(nullptr);
    node->setPre(nullptr);

    if (dummyHead->getNext() == dummyTail){
        head = nullptr;
        tail = nullptr;
    }else{
        head = dummyHead->getNext();
        tail = dummyTail->getPre();
        tail->setNext(nullptr);
        head->setPre(nullptr);
    }

    delete dummyHead;
    delete dummyTail;

    curSizeNum--;
    return node;
}


Page *Node::getPage() const {
    return page;
}

Node *Node::getPre() const {
    return pre;
}

Node *Node::getNext() const {
    return next;
}


void Node::setPre(Node *pre) {
    this->pre = pre;
}

void Node::setNext(Node *next) {
    this->next = next;
}

Node::Node(Page* page){
    this->page = page;
    this->pre = nullptr;
    this->next = nullptr;
};



#endif