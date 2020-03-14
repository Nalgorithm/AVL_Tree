#pragma once
#include<functional>
#include<iterator>
#include <algorithm>

template<class T, class Compare = std::less<T>>
class BST {
private:

	struct node {
		T value, height;
		node *left, *right, *parent;

		explicit node(T v = 0, T h = 0);
	};

	Compare pr;
	node * root;
	size_t size_;

public:
	class Iterator;
	typedef Iterator iterator;
	typedef Iterator const_iterator;
	BST();

	BST(const BST& ot);

	BST& operator=(const BST& ot);

	template<class InputIt>
	void assign(InputIt first, InputIt last);

	iterator begin();

	const_iterator cbegin();

	iterator end();

	const_iterator cend();

	iterator insert(const T& value);

	iterator remove(const T& value);

	iterator find(const T& value);

	const_iterator find(const T& value) const;

	bool empty() const;

	size_t size() const;

	class Iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
		node* p, *r;
	public:
		friend class BST;
		explicit Iterator(node* p, node* root = nullptr);

		bool operator ==(const iterator& other);

		bool operator !=(const iterator& other);

		T operator *();

		T* operator->();

		Iterator& operator++();

		Iterator operator++(int);

		Iterator& operator--();

		Iterator operator--(int);
	private:
		node* next(node* x);

		node* prev(node* x);

		node* minimum(node* x);

		node* maximum(node* x);
	};

private:

	void clean(node* r);

	node* copy_tree(node* r);

	node* next(node* x);

	node* prev(node* x);

	node* minimum(node* x);

	node* maximum(node* x);

	int bfactor(node* r);

	void fixheight(node* r);

	node* smallLeft(node* q);

	node* smallRight(node* p);

	node* balance(node* p);

	void balanceTree(node* k);
};



template <class T, class Compare>
BST<T, Compare>::node::node(T v, T h): value(v), height(h), left(nullptr),
                                       right(nullptr), parent(nullptr)
{
}

template <class T, class Compare>
BST<T, Compare>::BST(): root(nullptr), size_()
{
}

template <class T, class Compare>
BST<T, Compare>::BST(const BST& ot)
{
	root = copy_tree(ot.root);
	size_ = ot.size_;
}

template <class T, class Compare>
BST<T, Compare>& BST<T, Compare>::operator=(const BST& ot)
{
	if (this != &ot)
	{
		clean(root);
		root = copy_tree(ot.root);
		size_ = ot.size();
	}
	return *this;
}

template <class T, class Compare>
template <class InputIt>
void BST<T, Compare>::assign(InputIt first, InputIt last)
{
	clean(root);
	for (InputIt it = first; it != last; ++it)
		insert(*it);
}

template <class T, class Compare>
typename BST<T, Compare>::iterator BST<T, Compare>::begin()
{
	if (root == nullptr)
		return Iterator(nullptr);
	node* t = minimum(root);
	return Iterator(t, root);
}

template <class T, class Compare>
typename BST<T, Compare>::const_iterator BST<T, Compare>::cbegin()
{
	return begin();
}

template <class T, class Compare>
typename BST<T, Compare>::iterator BST<T, Compare>::end()
{
	if (root == nullptr)
		return Iterator(nullptr);
	return Iterator(nullptr, root);
}

template <class T, class Compare>
typename BST<T, Compare>::const_iterator BST<T, Compare>::cend()
{
	return end();
}

template <class T, class Compare>
typename BST<T, Compare>::iterator BST<T, Compare>::insert(const T& value)
{
	node *ins = root, *prev = nullptr;
	while (ins != nullptr)
	{
		if (ins->value == value)
			return Iterator(ins, root);
		if (!pr(value, ins->value))
		{
			prev = ins;
			ins = ins->right;
		}
		else
		{
			prev = ins;
			ins = ins->left;
		}
	}
	ins = new node;
	ins->value = value;
	ins->left = nullptr;
	ins->right = nullptr;
	ins->height = 1;
	if (prev == nullptr)
	{
		ins->parent = nullptr;
		root = ins;
	}
	else
	{
		ins->parent = prev;
		!pr(value, prev->value) ? prev->right = ins : prev->left = ins;
	}
	balanceTree(ins);
	++size_;
	return Iterator(ins, root);
}

template <class T, class Compare>
typename BST<T, Compare>::iterator BST<T, Compare>::remove(const T& value)
{
	node *t = find(value).p, *res = nullptr;
	if (t == nullptr)
		return Iterator(root, root);

	if (t->left == nullptr && t->right == nullptr)
	{
		if (t->parent == nullptr)
		{
			root = nullptr;
			delete t;
			--size_;
			return Iterator(root, root);
		}
		if (t->parent->left == t)
		{
			res = t->parent;
			t->parent->left = nullptr;
		}
		if (t->parent->right == t)
		{
			res = t->parent;
			t->parent->right = nullptr;
		}
		delete t;
		balanceTree(res);
		--size_;
		return Iterator(root, root);
	}

	if (t->left == nullptr || t->right == nullptr)
	{
		if (t->left == nullptr)
		{
			res = t->right;
			if (t->parent == nullptr)
			{
				t->right->parent = nullptr;
				root = t->right;
				delete t;
				balanceTree(res);
				--size_;
				return Iterator(root, root);
			}
			if (t->parent->left == t)
				t->parent->left = t->right;
			else
				t->parent->right = t->right;
			t->right->parent = t->parent;
			delete t;
			balanceTree(res);
			--size_;
			return Iterator(root, root);
		}
		if (t->right == nullptr)
		{
			res = t->left;
			if (t->parent == nullptr)
			{
				t->left->parent = nullptr;
				root = t->left;
				delete t;
				balanceTree(res);
				--size_;
				return Iterator(root, root);
			}
			if (t->parent->left == t)
				t->parent->left = t->left;
			else
				t->parent->right = t->left;
			t->left->parent = t->parent;
			delete t;
			balanceTree(res);
			--size_;
			return Iterator(root, root);
		}
	}

	node* prevElem = prev(t);
	prevElem->left ? res = prevElem->left : res = prevElem->parent;
	t->value = prevElem->value;
	if (prevElem->parent->right == prevElem)
	{
		prevElem->parent->right = prevElem->left;
		if (prevElem->left != nullptr)
			prevElem->left->parent = prevElem->parent;
	}
	else
	{
		prevElem->parent->left = prevElem->left;
		if (prevElem->left != nullptr)
			prevElem->left->parent = prevElem->parent;
	}
	delete prevElem;
	balanceTree(res);
	--size_;
	return Iterator(root, root);
}

template <class T, class Compare>
typename BST<T, Compare>::iterator BST<T, Compare>::find(const T& value)
{
	node* t = root;
	while (t != nullptr)
	{
		if (value == t->value)
		{
			return Iterator(t);
		}
		if (!pr(value, t->value))
			t = t->right;
		else
			t = t->left;
	}
	return Iterator(t, root);
}

template <class T, class Compare>
typename BST<T, Compare>::const_iterator BST<T, Compare>::find(const T& value) const
{
	return find(value);
}

template <class T, class Compare>
bool BST<T, Compare>::empty() const
{
	return size_ == 0;
}

template <class T, class Compare>
size_t BST<T, Compare>::size() const
{
	return size_;
}

template <class T, class Compare>
BST<T, Compare>::Iterator::Iterator(node* p, node* root): p(p), r(root)
{
}

template <class T, class Compare>
bool BST<T, Compare>::Iterator::operator==(const iterator& other)
{
	return p = other.p;
}

template <class T, class Compare>
bool BST<T, Compare>::Iterator::operator!=(const iterator& other)
{
	return p != other.p;
}

template <class T, class Compare>
T BST<T, Compare>::Iterator::operator*()
{
	return p->value;
}

template <class T, class Compare>
T* BST<T, Compare>::Iterator::operator->()
{
	return &(p->value);
}

template <class T, class Compare>
typename BST<T, Compare>::Iterator& BST<T, Compare>::Iterator::operator++()
{
	if (p == nullptr)
		p = minimum(r);
	else
		p = next(p);
	return *this;
}

template <class T, class Compare>
typename BST<T, Compare>::Iterator BST<T, Compare>::Iterator::operator++(int)
{
	Iterator tmp = *this;
	++(*this);
	return tmp;
}

template <class T, class Compare>
typename BST<T, Compare>::Iterator& BST<T, Compare>::Iterator::operator--()
{
	if (p == nullptr)
		p = maximum(r);
	else
		p = prev(p);
	return *this;
}

template <class T, class Compare>
typename BST<T, Compare>::Iterator BST<T, Compare>::Iterator::operator--(int)
{
	Iterator tmp = *this;
	--(*this);
	return tmp;
}

template <class T, class Compare>
typename BST<T, Compare>::node* BST<T, Compare>::Iterator::next(node* x)
{
	if (x->right != nullptr)
	{
		return minimum((x->right));
	}
	node* y = x->parent;
	while (y != nullptr && x != y->left)
	{
		x = y;
		y = y->parent;
	}
	return y;
}

template <class T, class Compare>
typename BST<T, Compare>::node* BST<T, Compare>::Iterator::prev(node* x)
{
	if (x->left != nullptr)
		return maximum((x->left));
	node* y = x->parent;
	while (y != nullptr && x != y->right)
	{
		x = y;
		y = y->parent;
	}
	return y;
}

template <class T, class Compare>
typename BST<T, Compare>::node* BST<T, Compare>::Iterator::minimum(node* x)
{
	if (x->left == nullptr)
		return x;
	else
		return minimum(x->left);
}

template <class T, class Compare>
typename BST<T, Compare>::node* BST<T, Compare>::Iterator::maximum(node* x)
{
	if (x->right == nullptr)
		return x;
	else
		return maximum(x->right);
}

template <class T, class Compare>
void BST<T, Compare>::clean(node* r)
{
	if (r != nullptr)
	{
		clean(r->left);
		clean(r->right);
		delete r;
		r = nullptr;
	}
	size_ = 0;
}

template <class T, class Compare>
typename BST<T, Compare>::node* BST<T, Compare>::copy_tree(node* r)
{
	if (r == nullptr)
		return nullptr;
	node* new_root = new node(r->value, r->height);
	new_root->left = copy_tree(r->left);
	new_root->right = copy_tree(r->right);
	if (new_root->left)
		new_root->left->parent = new_root;
	if (new_root->right)
		new_root->right->parent = new_root;
	return new_root;
}

template <class T, class Compare>
typename BST<T, Compare>::node* BST<T, Compare>::next(node* x)
{
	if (x->right != nullptr)
	{
		return minimum((x->right));
	}
	node* y = x->parent;
	while (y != nullptr && x != y->left)
	{
		x = y;
		y = y->parent;
	}
	return y;
}

template <class T, class Compare>
typename BST<T, Compare>::node* BST<T, Compare>::prev(node* x)
{
	if (x->left != nullptr)
		return maximum((x->left));
	node* y = x->parent;
	while (y != nullptr && x != y->right)
	{
		x = y;
		y = y->parent;
	}
	return y;
}

template <class T, class Compare>
typename BST<T, Compare>::node* BST<T, Compare>::minimum(node* x)
{
	if (x->left == nullptr)
		return x;
	else
		return minimum(x->left);
}

template <class T, class Compare>
typename BST<T, Compare>::node* BST<T, Compare>::maximum(node* x)
{
	if (x->right == nullptr)
		return x;
	else
		return maximum(x->right);
}

template <class T, class Compare>
int BST<T, Compare>::bfactor(node* r)
{
	if (r == nullptr)
		return 0;
	int hl, hr;
	r->left != nullptr ? hl = r->left->height : hl = 0;
	r->right != nullptr ? hr = r->right->height : hr = 0;
	return hr - hl;
}

template <class T, class Compare>
void BST<T, Compare>::fixheight(node* r)
{
	int hl, hr;
	r->left != nullptr ? hl = r->left->height : hl = 0;
	r->right != nullptr ? hr = r->right->height : hr = 0;
	r->height = std::max(hl, hr) + 1;
}

template <class T, class Compare>
typename BST<T, Compare>::node* BST<T, Compare>::smallLeft(node* q)
{
	node* p = q->right;
	q->right = p->left;
	if (p->left)
		p->left->parent = q;
	p->left = q;
	p->parent = q->parent;
	q->parent = p;
	if (p->parent)
		p->parent->right == q ? p->parent->right = p : p->parent->left = p;
	fixheight(q);
	fixheight(p);
	return p;
}

template <class T, class Compare>
typename BST<T, Compare>::node* BST<T, Compare>::smallRight(node* p)
{
	node* q = p->left;
	p->left = q->right;
	if (q->right)
		q->right->parent = p;
	q->right = p;
	q->parent = p->parent;
	p->parent = q;
	if (q->parent)
		q->parent->right == p ? q->parent->right = q : q->parent->left = q;
	fixheight(p);
	fixheight(q);
	return q;
}

template <class T, class Compare>
typename BST<T, Compare>::node* BST<T, Compare>::balance(node* p)
{
	fixheight(p);
	if (bfactor(p) == 2)
	{
		if (bfactor(p->right) < 0)
			p->right = smallRight(p->right);
		return smallLeft(p);
	}
	if (bfactor(p) == -2)
	{
		if (bfactor(p->left) > 0)
			p->left = smallLeft(p->left);
		return smallRight(p);
	}
	return p;
}

template <class T, class Compare>
void BST<T, Compare>::balanceTree(node* k)
{
	while (k != nullptr)
	{
		k = balance(k);
		root = k;
		k = k->parent;
	}
}
