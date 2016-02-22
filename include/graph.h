#ifndef GRAPH_H
#define GRAPH_H

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/unordered_map.hpp>     //boost::unordered_map
#include <boost/functional/hash.hpp>   //boost::hash

#include <functional>                  //std::equal_to

using VectorShmemAllocator = boost::interprocess::allocator<int, boost::interprocess::managed_shared_memory::segment_manager>;

//Alias a vector that uses the previous STL-like allocator
using MyVector = boost::interprocess::vector<int, VectorShmemAllocator>;

//Note that unordered_map<Key, MappedType>'s value_type is std::pair<const Key, MappedType>,
//so the allocator must allocate that pair.
using KeyType = int;
using MappedType = MyVector;
using ValueType = std::pair<const KeyType, MappedType>;

//Typedef the allocator
using UMapShmemAllocator = boost::interprocess::allocator<ValueType, boost::interprocess::managed_shared_memory::segment_manager>;

//Alias an unordered_map of ints that uses the previous STL-like allocator.
using Graph =  boost::unordered_map
< KeyType               , MappedType
, boost::hash<KeyType>  ,std::equal_to<KeyType>
, UMapShmemAllocator>;
#endif /* GRAPH_H */
