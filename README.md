# AVL_Tree
Реализация АВЛ-дерева в виде контейнера, обеспеченного двунаправленным итератором.
Базовый шаблон класса:
```C++
template<class T, class Compare = std::less<T>>
class BST {
public:
  BST();
  BST(const BST &);
  BST& operator= (const BST &);
  
  template<class InputIt>
  void assign(InputIt, InputIt);
  
  iterator begin();
  const_iterator cbegin();
  iterator end();
  const_iterator cend();
    
  iterator insert(const T &);
  iterator remove(const T &);
  iterator find(const T &);
  const_iterator find(const T &) const;
    
  bool empty() const;
  size_t size() const;
};
```
