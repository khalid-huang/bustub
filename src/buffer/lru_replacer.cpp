//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.cpp
//
// Identification: src/buffer/lru_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_replacer.h"

namespace bustub {

LRUReplacer::LRUReplacer(size_t num_pages) {
    capacity = num_pages;
}

LRUReplacer::~LRUReplacer() = default;

bool LRUReplacer::Victim(frame_id_t *frame_id) { 
    latch.lock();
    // 如果为空就返回false
    if (Size() == 0) {
        latch.unlock();
        return false;
    }
    // 从尾部去掉一个
    frame_id_t victim = lruList.back();
    data.erase(victim);
    lruList.pop_back();
    *frame_id = victim;
    latch.unlock();
    return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
    latch.lock();
    // 如果存在lru里面，就去除掉，避免改页被淘汰了
    if (data.count(frame_id) == 0) {
        latch.unlock();
        return;
    }
    lruList.erase(data[frame_id]);
    data.erase(frame_id);
    latch.unlock();
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
    latch.lock(); 
    // 如果已经存在，就跳过
    if (data.count(frame_id) != 0) {
        latch.unlock();
        return;
    }
    // 确保有空间
    while (Size() >= capacity) {
        // 不断从lru尾部移除
        frame_id_t victim = lruList.back();
        data.erase(victim);
        lruList.pop_back();
    }
    lruList.push_front(frame_id);
    data[frame_id] = lruList.begin();
    latch.unlock();
}

size_t LRUReplacer::Size() {
    return data.size();
}

}  // namespace bustub
