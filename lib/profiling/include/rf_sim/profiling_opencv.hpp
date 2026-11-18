/**
 * @file profiling_opencv.hpp
 * @brief Optional tracking of cv::Mat buffers as a separate Tracy memory pool.
 */
#pragma once

#ifdef RF_SIM_PROFILING

namespace RfSim::Profiling
{
/// Makes OpenCV report Mat buffers to Tracy (pool "OpenCV").
/// Call once at the very start of main(), before any cv::Mat is created.
void installOpenCvMemoryTracking();
} // namespace RfSim::Profiling

#define RF_SIM_PROFILE_TRACK_OPENCV() ::RfSim::Profiling::installOpenCvMemoryTracking()

#else

#define RF_SIM_PROFILE_TRACK_OPENCV() static_cast<void>(0)

#endif
