//
// Created by Lin Sinan on 2020-12-30.
//

#ifndef COMPILER_ALLOCATOR_H
#define COMPILER_ALLOCATOR_H

#include <memory>
#include <cassert>

static const size_t VAR_BLOCK_SIZE_POOL = 50;
static const size_t FUN_BLOCK_SIZE_POOL = 20;

template <typename T>
struct Pool {
    union pool_item {
    private:
#if defined(__clang__)
        using StorageType = __attribute__((alignof(T))) char[sizeof(T)];
#else
        using StorageType = alignas(alignof(T)) char[sizeof(T)];
#endif
        pool_item *next;
        StorageType datum;
    public:
        pool_item* get_next_item() const { return next; }
        void       set_next_item(pool_item *n) { next = n; }
        T*         get_data() { return reinterpret_cast<T*>(datum); }
        static pool_item*  cast_to_item(T *t) { return reinterpret_cast<pool_item*>(t);}
    }; // pool item

    // a vector of pool items
    struct pool_block {
        std::unique_ptr<pool_item[]> items;
        std::unique_ptr<pool_block>  next;

        pool_item* get_items() const { return items.get(); /* get ptr of item array */ }
        void set_next_block(std::unique_ptr<pool_block> &&n) { assert(!next); next.reset(n.release()); }
        explicit pool_block(size_t block_size) :
            items(new pool_item[block_size]) {
            for (size_t i = 1; i < block_size; i++) {
                items[i - 1].set_next_item(&items[i]);
            }
            items[block_size - 1].set_next_item(nullptr);
        }
    }; // pool_block

    // Allocates an object in the current arena.
    template <typename... Args>
    T *alloc(Args &&... args) {
        // re-new a block
        if (free_list == nullptr) {
            std::unique_ptr<pool_block> new_block(new pool_block(block_size));
            new_block->set_next_block(std::move(cur_block));
            cur_block.reset(new_block.release());
            free_list = cur_block->get_items();
        }

        // Get the first free item.
        pool_item *cur_item = free_list;
        // Update the free list to the next free item.
        free_list = cur_item->get_next_item();

        // Get the storage for T.
        T *result = cur_item->get_data();
        // Construct the object in the obtained storage.
        new (result) T(std::forward<Args>(args)...);
        return result;
    }

    explicit Pool(size_t l_block_size) :
            block_size(l_block_size),
            cur_block(new pool_block(l_block_size)),
            free_list(cur_block->get_items())
    {}

    void free(T *t) {
        t->T::~T();
        pool_item *cur_item = pool_item::cast_to_item(t);

        // Add the item at the beginning of the free list.
        cur_item->set_next_item(free_list);
        free_list = cur_item;
    }

    size_t block_size;
    std::unique_ptr<pool_block> cur_block;
    pool_item *free_list;
};

struct Memory_manager {
    template <typename ...Args>
    RT_Value* alloc_var(Args &&... args)  { return pool_var.alloc(std::forward<Args>(args)...); }
    void dealloc_var(RT_Value* var)       { pool_var.free(var); }
    template <typename ...Args>
    RT_Function* alloc_func(Args &&... args) { return pool_func.alloc(std::forward<Args>(args)...); }
    void dealloc_func(RT_Function*)  {  }

    Pool<RT_Value>    pool_var;
    Pool<RT_Function> pool_func;

    Memory_manager() :
        pool_var(VAR_BLOCK_SIZE_POOL),
        pool_func(FUN_BLOCK_SIZE_POOL)
    {}
};
#endif //COMPILER_ALLOCATOR_H
