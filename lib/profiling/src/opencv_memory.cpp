#include "rf_sim/profiling_opencv.hpp"

#include <opencv2/core/mat.hpp>

#include <tracy/Tracy.hpp>

namespace {

    constexpr const char *kPoolName = "OpenCV";

    class TracyMatAllocator final : public cv::MatAllocator {
    public:
        explicit TracyMatAllocator(cv::MatAllocator *inner) : m_inner(inner) {
        }

        cv::UMatData *allocate(int dims,
                               const int *sizes,
                               int type,
                               void *data,
                               size_t *step,
                               cv::AccessFlag flags,
                               cv::UMatUsageFlags usageFlags) const override {
            cv::UMatData *u = m_inner->allocate(dims, sizes, type, data, step, flags, usageFlags);
            if (u == nullptr) {
                return nullptr;
            }

            u->currAllocator = this;
            u->prevAllocator = this;
            if (ownsBuffer(u)) {
                TracyAllocN(u->origdata, u->size, kPoolName);
            }
            return u;
        }

        bool allocate(cv::UMatData *data,
                      cv::AccessFlag accessFlags,
                      cv::UMatUsageFlags usageFlags) const override {
            return m_inner->allocate(data, accessFlags, usageFlags);
        }

        void deallocate(cv::UMatData *u) const override {
            if (u != nullptr && ownsBuffer(u)) {
                TracyFreeN(u->origdata, kPoolName);
            }
            m_inner->deallocate(u);
        }

    private:
        static bool ownsBuffer(const cv::UMatData *u) noexcept {
            return u->origdata != nullptr && !(u->flags & cv::UMatData::USER_ALLOCATED);
        }

        cv::MatAllocator *m_inner;
    };

}

namespace RfSim::Profiling {

    void installOpenCvMemoryTracking() {
        static TracyMatAllocator allocator{cv::Mat::getStdAllocator()};
        cv::Mat::setDefaultAllocator(&allocator);
    }

}
