#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <new>

#ifdef _WIN32
#include <malloc.h>
#endif

#include <tracy/Tracy.hpp>

#ifndef RF_SIM_PROFILING_ALLOC_CALLSTACK_DEPTH
#define RF_SIM_PROFILING_ALLOC_CALLSTACK_DEPTH 0
#endif

namespace {

    inline void reportAlloc(const void *ptr, std::size_t size) noexcept {
#if RF_SIM_PROFILING_ALLOC_CALLSTACK_DEPTH > 0
        TracyAllocS(ptr, size, RF_SIM_PROFILING_ALLOC_CALLSTACK_DEPTH);
#else
        TracyAlloc(ptr, size);
#endif
    }

    inline void reportFree(const void *ptr) noexcept {
        TracyFree(ptr);
    }

    template <typename RawAlloc>
    void *allocateOrThrow(std::size_t size, RawAlloc rawAlloc) {
        if (size == 0) {
            size = 1;
        }
        for (;;) {
            if (void *ptr = rawAlloc(size)) {
                reportAlloc(ptr, size);
                return ptr;
            }
            const std::new_handler handler = std::get_new_handler();
            if (handler == nullptr) {
                throw std::bad_alloc{};
            }
            handler();
        }
    }

    void *allocate(std::size_t size) {
        return allocateOrThrow(size,
                               [](std::size_t n) {
                                   return std::malloc(n);
                               });
    }

    void *allocateAligned(std::size_t size, std::align_val_t alignment) {
        const auto align = static_cast<std::size_t>(alignment);
        const auto rawAlloc = [align](std::size_t n) -> void * {
#ifdef _WIN32
            return _aligned_malloc(n, align);
#else
            if (n > SIZE_MAX - (align - 1)) {
                return nullptr;
            }
            return std::aligned_alloc(align, (n + align - 1) / align * align);
#endif
        };
        return allocateOrThrow(size, rawAlloc);
    }

    void deallocate(void *ptr) noexcept {
        if (ptr == nullptr) {
            return;
        }
        reportFree(ptr);
        std::free(ptr);
    }

    void deallocateAligned(void *ptr) noexcept {
        if (ptr == nullptr) {
            return;
        }
        reportFree(ptr);
#ifdef _WIN32
        _aligned_free(ptr);
#else
        std::free(ptr);
#endif
    }

    template <typename Alloc>
    void *allocateNoThrow(Alloc alloc) noexcept {
        try {
            return alloc();
        } catch (const std::bad_alloc &) {
            return nullptr;
        }
    }

}

void *operator new(std::size_t size) {
    return allocate(size);
}

void *operator new[](std::size_t size) {
    return allocate(size);
}

void *operator new(std::size_t size, const std::nothrow_t &) noexcept {
    return allocateNoThrow([size] {
        return allocate(size);
    });
}

void *operator new[](std::size_t size, const std::nothrow_t &) noexcept {
    return allocateNoThrow([size] {
        return allocate(size);
    });
}

void *operator new(std::size_t size, std::align_val_t alignment) {
    return allocateAligned(size, alignment);
}

void *operator new[](std::size_t size, std::align_val_t alignment) {
    return allocateAligned(size, alignment);
}

void *operator new(std::size_t size, std::align_val_t alignment, const std::nothrow_t &) noexcept {
    return allocateNoThrow([size, alignment] {
        return allocateAligned(size, alignment);
    });
}

void *operator new[](std::size_t size, std::align_val_t alignment, const std::nothrow_t &) noexcept {
    return allocateNoThrow([size, alignment] {
        return allocateAligned(size, alignment);
    });
}

void operator delete(void *ptr) noexcept {
    deallocate(ptr);
}

void operator delete[](void *ptr) noexcept {
    deallocate(ptr);
}

void operator delete(void *ptr, std::size_t) noexcept {
    deallocate(ptr);
}

void operator delete[](void *ptr, std::size_t) noexcept {
    deallocate(ptr);
}

void operator delete(void *ptr, const std::nothrow_t &) noexcept {
    deallocate(ptr);
}

void operator delete[](void *ptr, const std::nothrow_t &) noexcept {
    deallocate(ptr);
}

void operator delete(void *ptr, std::align_val_t) noexcept {
    deallocateAligned(ptr);
}

void operator delete[](void *ptr, std::align_val_t) noexcept {
    deallocateAligned(ptr);
}

void operator delete(void *ptr, std::size_t, std::align_val_t) noexcept {
    deallocateAligned(ptr);
}

void operator delete[](void *ptr, std::size_t, std::align_val_t) noexcept {
    deallocateAligned(ptr);
}

void operator delete(void *ptr, std::align_val_t, const std::nothrow_t &) noexcept {
    deallocateAligned(ptr);
}

void operator delete[](void *ptr, std::align_val_t, const std::nothrow_t &) noexcept {
    deallocateAligned(ptr);
}
