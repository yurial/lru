#pragma once

#include <list>
#include <unordered_map>
#include <functional>

template<
    class Key,
    class T,
    class Hash = std::hash<Key>,
    class KeyEqual = std::equal_to<Key>,
    class Allocator = std::allocator<void>
>
class LRU
{
public:
using key_type = Key;
using mapped_type = T;
using value_type = std::pair<const Key, T>;
using size_type = size_t;
using difference_type = std::ptrdiff_t;
using hasher = Hash;
using key_equal = KeyEqual;
using allocator_type = Allocator;

using data_allocator_type = typename std::allocator_traits<allocator_type>::template rebind_alloc<value_type>;
using data_container = std::list<value_type, data_allocator_type>;
using reference = value_type&;
using const_reference = const value_type&;
using pointer = typename std::allocator_traits<allocator_type>::template rebind_alloc<mapped_type>::pointer;
using const_pointer = typename std::allocator_traits<allocator_type>::template rebind_alloc<mapped_type>::const_pointer;
using iterator = typename data_container::iterator;
using const_iterator = typename data_container::const_iterator;

using key_wrapper_type = const std::reference_wrapper<const key_type>;
using index_allocator_type = typename std::allocator_traits<allocator_type>::template rebind_alloc<std::pair<key_wrapper_type, const iterator>>;
using index_container = std::unordered_map<key_wrapper_type, const iterator, hasher, key_equal, index_allocator_type>;

                        LRU() = default;
inline  explicit        LRU(const allocator_type& alloc);
inline  allocator_type  get_allocator() const noexcept;

inline  bool        empty() const noexcept;
inline  size_type   size() const noexcept;
inline  size_type   max_size() const noexcept;
inline  void        clear() noexcept;
inline  void        reserve(const size_type count);

template<class... Args>
std::pair<iterator,bool> emplace(iterator pos, Args&&... args);
template<class... Args>
inline  std::pair<iterator,bool> emplace_front(Args&&... args);
template<class... Args>
inline  std::pair<iterator,bool> emplace_back(Args&&... args);
template <class... Args>
std::pair<iterator, bool> try_emplace(iterator pos, const key_type& k, Args&&... args);
template <class... Args>
inline  std::pair<iterator, bool> try_emplace_front(const key_type& k, Args&&... args);
template <class... Args>
inline  std::pair<iterator, bool> try_emplace_back(const key_type& k, Args&&... args);
template <class... Args>
std::pair<iterator, bool> try_emplace(iterator pos, key_type&& k, Args&&... args);
template <class... Args>
inline  std::pair<iterator, bool> try_emplace_front(key_type&& k, Args&&... args);
template <class... Args>
inline  std::pair<iterator, bool> try_emplace_back(key_type&& k, Args&&... args);
inline  size_type   erase(const key_type& key) noexcept;
inline  iterator    erase(const const_iterator pos) noexcept;
inline  iterator    erase(const_iterator first, const const_iterator last) noexcept;
inline  void        swap(LRU<Key,T,Hash,KeyEqual,Allocator>& other) noexcept(std::allocator_traits<index_allocator_type>::is_always_equal::value && std::is_nothrow_swappable<hasher>::value && std::is_nothrow_swappable<key_equal>::value);
inline  void        splice(const iterator pos, const iterator it) noexcept;
inline  void        bump_front(const iterator it) noexcept;
inline  void        bump_back(const iterator it) noexcept;

template<class K>
inline  mapped_type&        operator[](const K& key);
template<class K>
inline  const mapped_type&  operator[](K&& key);
template<class K>
inline  size_type           count(const K& key) const noexcept;
template<class K>
inline  iterator            find(const K& key) noexcept;
template<class K>
inline  const_iterator      find(const K& key) const noexcept;
template<class K>
inline  bool                contains(const K& key) const noexcept;
template<class K>
inline  mapped_type&        at(const K& key);
template<class K>
inline  const mapped_type&  at(const K& key) const;

inline  reference       front() noexcept;
inline  const_reference front() const noexcept;
inline  reference       back() noexcept;
inline  const_reference back() const noexcept;

inline  iterator        begin() noexcept;
inline  iterator        end() noexcept;
inline  const_iterator  begin() const noexcept;
inline  const_iterator  end() const noexcept;
inline  const_iterator  cbegin() const noexcept;
inline  const_iterator  cend() const noexcept;

inline  void            max_load_factor(const float count);

protected:
data_container  m_data;
index_container m_index;
}; //class LRU

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
LRU<Key,T,Hash,KeyEqual,Allocator>::LRU(const allocator_type& alloc):
        m_data(alloc),
        m_index(alloc)
    {
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::allocator_type LRU<Key,T,Hash,KeyEqual,Allocator>::get_allocator() const noexcept
    {
    return m_data.get_allocator();
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
bool LRU<Key,T,Hash,KeyEqual,Allocator>::empty() const noexcept
    {
    return m_index.empty();
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::size_type LRU<Key,T,Hash,KeyEqual,Allocator>::size() const noexcept
    {
    return m_index.size();
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::size_type LRU<Key,T,Hash,KeyEqual,Allocator>::max_size() const noexcept
    {
    return std::min(m_data.max_size(), m_index.max_size());
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
void LRU<Key,T,Hash,KeyEqual,Allocator>::clear() noexcept
    {
    m_index.clear();
    m_data.clear();
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
void LRU<Key,T,Hash,KeyEqual,Allocator>::reserve(const size_type count)
    {
    m_index.reserve(count);
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template<class... Args>
std::pair<typename LRU<Key,T,Hash,KeyEqual,Allocator>::iterator, bool> LRU<Key,T,Hash,KeyEqual,Allocator>::emplace(iterator pos, Args&&... args)
    {
    const iterator data_it = m_data.emplace(pos, std::forward<Args>(args)...);
    const auto index_it = m_index.find(data_it->first);
    if (index_it != m_index.end())
        {
        m_data.erase(data_it);
        return std::make_pair(index_it->second, false);
        }
    try
        {
        m_index.emplace(data_it->first, data_it);
        return std::make_pair(data_it, true);
        }
    catch (...)
        {
        m_data.erase(data_it);
        throw;
        }
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template<class... Args>
std::pair<typename LRU<Key,T,Hash,KeyEqual,Allocator>::iterator, bool> LRU<Key,T,Hash,KeyEqual,Allocator>::emplace_front(Args&&... args)
    {
    return emplace(m_data.begin(), std::forward<Args>(args)...);
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template<class... Args>
std::pair<typename LRU<Key,T,Hash,KeyEqual,Allocator>::iterator, bool> LRU<Key,T,Hash,KeyEqual,Allocator>::emplace_back(Args&&... args)
    {
    return emplace(m_data.end(), std::forward<Args>(args)...);
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template <class... Args>
std::pair<typename LRU<Key,T,Hash,KeyEqual,Allocator>::iterator, bool> LRU<Key,T,Hash,KeyEqual,Allocator>::try_emplace(iterator pos, const key_type& key, Args&&... args)
    {
    const auto index_it = m_index.find(key);
    if (index_it != m_index.end())
        return std::make_pair(index_it->second, false);
    const iterator data_it = m_data.emplace(pos, std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(std::forward<Args>(args)...));
    try
        {
        m_index.emplace(data_it->first, data_it);
        return std::make_pair(data_it, true);
        }
    catch (...)
        {
        m_data.erase(data_it);
        throw;
        }
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template <class... Args>
std::pair<typename LRU<Key,T,Hash,KeyEqual,Allocator>::iterator, bool> LRU<Key,T,Hash,KeyEqual,Allocator>::try_emplace_front(const key_type& key, Args&&... args)
    {
    return try_emplace(m_data.begin(), key, std::forward<Args>(args)...);
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template <class... Args>
std::pair<typename LRU<Key,T,Hash,KeyEqual,Allocator>::iterator, bool> LRU<Key,T,Hash,KeyEqual,Allocator>::try_emplace_back(const key_type& key, Args&&... args)
    {
    return try_emplace(m_data.end(), key, std::forward<Args>(args)...);
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template <class... Args>
std::pair<typename LRU<Key,T,Hash,KeyEqual,Allocator>::iterator, bool> LRU<Key,T,Hash,KeyEqual,Allocator>::try_emplace(iterator pos, key_type&& key, Args&&... args)
    {
    const auto index_it = m_index.find(key);
    if (index_it != m_index.end())
        return std::make_pair(index_it->second, false);
    const iterator data_it = m_data.emplace(pos, std::piecewise_construct, std::forward_as_tuple(std::move(key)), std::forward_as_tuple(std::forward<Args>(args)...));
    try
        {
        m_index.emplace(data_it->first, data_it);
        return std::make_pair(data_it, true);
        }
    catch (...)
        {
        m_data.erase(data_it);
        throw;
        }
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template <class... Args>
std::pair<typename LRU<Key,T,Hash,KeyEqual,Allocator>::iterator, bool> LRU<Key,T,Hash,KeyEqual,Allocator>::try_emplace_front(key_type&& key, Args&&... args)
    {
    return try_emplace(m_data.begin(), std::forward<key_type>(key), std::forward<Args>(args)...);
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template <class... Args>
std::pair<typename LRU<Key,T,Hash,KeyEqual,Allocator>::iterator, bool> LRU<Key,T,Hash,KeyEqual,Allocator>::try_emplace_back(key_type&& key, Args&&... args)
    {
    return try_emplace(m_data.end(), std::forward<key_type>(key), std::forward<Args>(args)...);
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::size_type LRU<Key,T,Hash,KeyEqual,Allocator>::erase(const key_type& key) noexcept
    {
    const auto index_it = m_index.find(key);
    if (index_it == m_index.end())
        return 0;
    const auto data_it = index_it->second;
    m_index.erase(index_it);
    m_data.erase(data_it);
    return 1;
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::iterator LRU<Key,T,Hash,KeyEqual,Allocator>::erase(const const_iterator pos) noexcept
    {
    m_index.erase(pos->first);
    return m_data.erase(pos);
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::iterator LRU<Key,T,Hash,KeyEqual,Allocator>::erase(const_iterator first, const const_iterator last) noexcept
    {
    while (first != last)
        first = erase(first);
    return first;
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
void LRU<Key,T,Hash,KeyEqual,Allocator>::swap(LRU<Key,T,Hash,KeyEqual,Allocator>& other) noexcept(std::allocator_traits<index_allocator_type>::is_always_equal::value && std::is_nothrow_swappable<hasher>::value && std::is_nothrow_swappable<key_equal>::value)
    {
    m_index.swap(other.m_index);
    m_data.swap(other.m_data);
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
void LRU<Key,T,Hash,KeyEqual,Allocator>::splice(const iterator pos, const iterator it) noexcept
    {
    m_data.splice(pos, m_data, it);
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
void LRU<Key,T,Hash,KeyEqual,Allocator>::bump_front(const iterator it) noexcept
    {
    splice(m_data.begin(), it);
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
void LRU<Key,T,Hash,KeyEqual,Allocator>::bump_back(const iterator it) noexcept
    {
    splice(m_data.end(), it);
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template<class K>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::mapped_type& LRU<Key,T,Hash,KeyEqual,Allocator>::at(const K& key)
    {
    return m_index.at(key)->second;
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template<class K>
const typename LRU<Key,T,Hash,KeyEqual,Allocator>::mapped_type& LRU<Key,T,Hash,KeyEqual,Allocator>::at(const K& key) const
    {
    return m_index.at(key)->second;
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template<class K>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::mapped_type& LRU<Key,T,Hash,KeyEqual,Allocator>::operator[](const K& key)
    {
    const auto index_it = m_index.find(key);
    if (index_it != m_index.end())
        return index_it->second;
    return emplace_back(key, mapped_type()).first->second;
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template<class K>
const typename LRU<Key,T,Hash,KeyEqual,Allocator>::mapped_type& LRU<Key,T,Hash,KeyEqual,Allocator>::operator[](K&& key)
    {
    const auto index_it = m_index.find(key);
    if (index_it != m_index.end())
        return index_it->second->second;
    return emplace_back(std::forward<K>(key), mapped_type()).first->second;
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template<class K>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::size_type LRU<Key,T,Hash,KeyEqual,Allocator>::count(const K& key) const noexcept
    {
    return m_index.count(key);
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template<class K>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::iterator LRU<Key,T,Hash,KeyEqual,Allocator>::find(const K& key) noexcept
    {
    const auto index_it = m_index.find(key);
    return (index_it == m_index.end())? m_data.end() : index_it->second;
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template<class K>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::const_iterator LRU<Key,T,Hash,KeyEqual,Allocator>::find(const K& key) const noexcept
    {
    const auto index_it = m_index.find(key);
    return (index_it == m_index.cend())? m_data.cend() : index_it->second;
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
template<class K>
bool LRU<Key,T,Hash,KeyEqual,Allocator>::contains(const K& key) const noexcept
    {
    return m_index.count(key);
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::reference LRU<Key,T,Hash,KeyEqual,Allocator>::front() noexcept
    {
    return m_data.front();
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::const_reference LRU<Key,T,Hash,KeyEqual,Allocator>::front() const noexcept
    {
    return m_data.front();
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::reference LRU<Key,T,Hash,KeyEqual,Allocator>::back() noexcept
    {
    return m_data.back();
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::const_reference LRU<Key,T,Hash,KeyEqual,Allocator>::back() const noexcept
    {
    return m_data.back();
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::iterator LRU<Key,T,Hash,KeyEqual,Allocator>::begin() noexcept
    {
    return m_data.begin();
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::iterator LRU<Key,T,Hash,KeyEqual,Allocator>::end() noexcept
    {
    return m_data.end();
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::const_iterator LRU<Key,T,Hash,KeyEqual,Allocator>::begin() const noexcept
    {
    return cbegin();
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::const_iterator LRU<Key,T,Hash,KeyEqual,Allocator>::end() const noexcept
    {
    return cend();
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::const_iterator LRU<Key,T,Hash,KeyEqual,Allocator>::cbegin() const noexcept
    {
    return m_data.cbegin();
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
typename LRU<Key,T,Hash,KeyEqual,Allocator>::const_iterator LRU<Key,T,Hash,KeyEqual,Allocator>::cend() const noexcept
    {
    return m_data.cend();
    }

template<class Key, class T, class Hash, class KeyEqual, class Allocator>
void LRU<Key,T,Hash,KeyEqual,Allocator>::max_load_factor(const float count)
    {
    m_index.max_load_factor(count);
    }
