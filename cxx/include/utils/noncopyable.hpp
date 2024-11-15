/*
 * @Author: NiceBlueChai
 * @Date: 2023-02-03 
 * @LastEditTime: 2023-02-03 
 * @Description: 单例基类
 */

#pragma once

namespace nbc {
namespace utils {
    class noncopyable {
    protected:
        noncopyable() noexcept = default;
        virtual ~noncopyable() = default;

        noncopyable(const noncopyable&) = delete;
        noncopyable& operator=(const noncopyable&) = delete;
    };
} // namespace utils
} // namespace nbc