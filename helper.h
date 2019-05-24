//
// Created by jiahua on 2019/4/26.
//

#ifndef HASHCOMP_HELPER_H
#define HASHCOMP_HELPER_H
#include <random>
#include <vector>
#include <cstdint>
#include <chrono>
#include <cassert>
#include <thread>
#include "neatlib/concurrent_hash_table.h"
#include "atomic_shared_ptr.h"
#include "neatlib/lock_free_hash_table.h"

using mhash = neatlib::ConcurrentHashTable<uint64_t, uint64_t, std::hash<uint64_t>, 4, 16>;
using unsafe = neatlib::ConcurrentHashTable<uint64_t, uint64_t,
                                              std::hash<uint64_t>, 4, 16,
                                              neatlib::unsafe::atomic_shared_ptr,
                                              neatlib::unsafe::shared_ptr>;
using ebr = neatlib::LockFreeHashTable<uint64_t , uint64_t , std::hash<uint64_t>, 4, 16>;

std::vector<size_t> time_vec;

std::chrono::time_point<std::chrono::steady_clock> get_time() {
    return std::chrono::steady_clock::now();
}

size_t average_time() {
    assert(!time_vec.empty());
    size_t sum = 0;
    sum = std::accumulate(time_vec.begin(), time_vec.end(), 0ull);
    return sum / time_vec.size();
}

template <typename DIS = std::uniform_int_distribution<uint64_t> >
void generate_data(std::vector<uint64_t> &data, size_t num, DIS dis) {
    data.resize(num, 0);
    std::mt19937 en(std::chrono::system_clock::now().time_since_epoch().count());
    for (size_t i = 0; i < num; i++) {
        data[i] = dis(en);
    }
}

template <typename TP, typename M = std::chrono::microseconds>
size_t time_duration(TP t1, TP t2) {
    return std::chrono::duration_cast<M>(t2 - t1).count();
}

template<typename HT>
void insert_func(HT *ht, const std::vector<uint64_t> &data, size_t thd_cnt, size_t thd_idx) {
    auto t1 = get_time();
    for (size_t i = thd_idx; i < data.size(); i += thd_cnt) {
        ht->Insert(data[i], 123);
    }
    auto t2 = get_time();
    time_vec[thd_idx] = time_duration(t1, t2);
}


template <typename HT>
void get_func(HT *ht, const std::vector<uint64_t> &data, size_t thd_cnt, size_t thd_idx, size_t task) {
    auto t1 = get_time();
    size_t data_cnt = data.size();
    for (size_t i = thd_idx; i < task; i += thd_cnt) {
        auto p = ht->Get(data[i%data_cnt]);
        if (p.second != 123 && p.second != 321) exit(1);
    }
    auto t2 = get_time();
    time_vec[thd_idx] = time_duration(t1, t2);
}


template <typename HT>
void update_func(HT *ht, const std::vector<uint64_t> &data, size_t thd_cnt, size_t thd_idx, size_t task) {
    auto t1 = get_time();
    size_t data_cnt = data.size();
    for (size_t i = thd_idx; i < task; i += thd_cnt) {
        auto p = ht->Update(data[i%data_cnt], 321);
    }
    auto t2 = get_time();
    time_vec[thd_idx] = time_duration(t1, t2);
}


template <typename HT>
void remove_func(HT *ht, const std::vector<uint64_t> &data, size_t thd_cnt, size_t thd_idx) {
    auto t1 = get_time();
    for (size_t i = thd_idx; i < data.size(); i += thd_cnt) {
        ht->Remove(data[i]);
    }
    auto t2 = get_time();
    time_vec[thd_idx] = time_duration(t1, t2);
}









#endif //HASHCOMP_HELPER_H
