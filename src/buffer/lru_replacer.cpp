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

auto LRUReplacer::Victim(frame_id_t *frame_id) -> bool {
    latch.lock();
    // 判断是否为空，为空返回false
    if (data.empty()) {
        latch.unlock();
        return false;
    }

    // 不为空移除链表最后一个，同时删除map里面的
    frame_id_t victim = lruList.back();
    data.erase(victim);
    lruList.pop_back();
    *frame_id = victim;
    latch.unlock();
    return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
    latch.lock();
    // 找到就删除
    if (data.count(frame_id) != 0) {
        lruList.erase(data[frame_id]);
        data.erase(frame_id);
    } 
    latch.unlock();
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
    latch.lock();
    // 确定不在才操作
    if (data.count(frame_id) == 0) {
        // 不断异常链表末尾的，直到容量足够
        while (static_cast<int>(Size()) >= capacity) {
            data.erase(lruList.back());
            lruList.pop_back();
        }

        lruList.push_front(frame_id);
        data[frame_id] = lruList.begin();
    }
    latch.unlock();
}

auto LRUReplacer::Size() -> size_t {
    return data.size();
}

}  // namespace bustub
