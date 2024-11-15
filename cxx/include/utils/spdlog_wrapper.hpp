/*
 * @Author: songshuda
 * @Date: 2020-02-03 
 * @LastEditTime: 2021-03-25 
 * @LastEditors: Please set LastEditors
 * @Description: 基于spdlog，封装而成的单例写日志类，做成了同步和异步两种模式。
 */

#ifndef _ZHF_SPDLOG_WRAPPER_H_
#define _ZHF_SPDLOG_WRAPPER_H_

#ifndef _WIN32
    #include <sys/stat.h>
#else
    #include <direct.h>
#endif

#include <spdlog/spdlog.h>
#include <spdlog/async.h>
#include <spdlog/sinks/rotating_file_sink.h>

#include "noncopyable.hpp"

namespace zhf {
namespace utils {
namespace log {

    
    template<typename Derived, typename Logger>
    class spdlog_wrapper_base : noncopyable {
		friend Derived;
    public:
        static Derived& getInstance() {
            static Derived ins;
            return ins;
        }

        std::shared_ptr<Logger> get_logger() const { return spdlogger_ptr_; }

        void init(const std::string& log_dir, const std::string& log_name, std::size_t max_size = 1024*2048, int max_count = 10, int log_level = 0) {
            if (is_init_)
                return;

        #ifndef _WIN32
            int ret = access(log_dir.c_str(), F_OK);
            if (ret != 0)
                ret = mkdir(log_dir.c_str(), 0777);
        #else
            int ret = _access(log_dir.c_str(), 0);
            if (ret != 0)
                ret = _mkdir(log_dir.c_str());
        #endif
            if (ret != 0)
                throw std::string("mkdir failed");

            std::vector<spdlog::sink_ptr> sinks;
			auto sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(log_dir + "/" + log_name + ".log", max_size, max_count);
			sinks.push_back(sink);

			static_cast<Derived*>(this)->init_impl(log_name, sinks);
            
            spdlogger_ptr_->set_level(spdlog::level::level_enum(log_level));
            spdlogger_ptr_->set_pattern("[%Y-%m-%d %H:%M:%S.%e][Pid:%P][Tid:%t][%l]%v");

            spdlog::register_logger(spdlogger_ptr_);
            spdlog::flush_on(spdlog::level::info);

            is_init_ = true;
        }

    private:
        bool is_init_{ false };
        std::shared_ptr<Logger> spdlogger_ptr_;
    };

	class async_spdlog_wrapper : public spdlog_wrapper_base<async_spdlog_wrapper, spdlog::async_logger> {
		public:
			~async_spdlog_wrapper() {
#ifdef _WIN32
					//spdlog::shutdown();
#endif
			}

			void init_impl(const std::string& log_name, std::vector<spdlog::sink_ptr>& sinks) {
				spdlog::init_thread_pool(1024, 1);
				spdlogger_ptr_ = std::make_shared<spdlog::async_logger>(log_name, sinks.begin(), sinks.end(), spdlog::thread_pool(), spdlog::async_overflow_policy::block);
			}
	};

	class sync_spdlog_wrapper : public spdlog_wrapper_base<sync_spdlog_wrapper, spdlog::logger> {
	public:
		void init_impl(const std::string& log_name, std::vector<spdlog::sink_ptr>& sinks) {
			spdlogger_ptr_ = std::make_shared<spdlog::logger>(log_name, sinks.begin(), sinks.end());
		}
	};

} // namespace log
} // namespace utils
} // namespace zhf

#ifndef _WIN32
    #define ZHF_LOG_FILENAME (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)
#else
    #define ZHF_LOG_FILENAME (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1) : __FILE__)
#endif

#define ZHF_LOG_ADDINFO(msg) (std::string("[") + \
        std::string(ZHF_LOG_FILENAME) + \
        std::string(":") + \
        std::to_string(__LINE__) + \
        std::string("][function:") +  \
        std::string(__FUNCTION__) + \
        std::string("] ") + \
        std::string(msg))

#ifdef _MSC_VER
#define ZHF_LOG_INIT(...) zhf::utils::log::sync_spdlog_wrapper::getInstance().init(__VA_ARGS__)
#define ZHF_LOG_TRACE(msg, ...) zhf::utils::log::sync_spdlog_wrapper::getInstance().get_logger()->trace(ZHF_LOG_ADDINFO(msg), __VA_ARGS__)
#define ZHF_LOG_DEBUG(msg, ...) zhf::utils::log::sync_spdlog_wrapper::getInstance().get_logger()->debug(ZHF_LOG_ADDINFO(msg), __VA_ARGS__)
#define ZHF_LOG_INFO(msg, ...) zhf::utils::log::sync_spdlog_wrapper::getInstance().get_logger()->info(ZHF_LOG_ADDINFO(msg), __VA_ARGS__)
#define ZHF_LOG_WARN(msg, ...) zhf::utils::log::sync_spdlog_wrapper::getInstance().get_logger()->warn(ZHF_LOG_ADDINFO(msg), __VA_ARGS__)
#define ZHF_LOG_ERROR(msg, ...) zhf::utils::log::sync_spdlog_wrapper::getInstance().get_logger()->error(ZHF_LOG_ADDINFO(msg), __VA_ARGS__)
#define ZHF_LOG_CRITICAL(msg, ...) zhf::utils::log::sync_spdlog_wrapper::getInstance().get_logger()->critical(ZHF_LOG_ADDINFO(msg), __VA_ARGS__)

#define ZHF_ALOG_INIT(...) zhf::utils::log::async_spdlog_wrapper::getInstance().init(__VA_ARGS__)
#define ZHF_ALOG_TRACE(msg, ...) zhf::utils::log::async_spdlog_wrapper::getInstance().get_logger()->trace(ZHF_LOG_ADDINFO(msg), __VA_ARGS__)
#define ZHF_ALOG_DEBUG(msg, ...) zhf::utils::log::async_spdlog_wrapper::getInstance().get_logger()->debug(ZHF_LOG_ADDINFO(msg), __VA_ARGS__)
#define ZHF_ALOG_INFO(msg, ...) zhf::utils::log::async_spdlog_wrapper::getInstance().get_logger()->info(ZHF_LOG_ADDINFO(msg), __VA_ARGS__)
#define ZHF_ALOG_WARN(msg, ...) zhf::utils::log::async_spdlog_wrapper::getInstance().get_logger()->warn(ZHF_LOG_ADDINFO(msg), __VA_ARGS__)
#define ZHF_ALOG_ERROR(msg, ...) zhf::utils::log::async_spdlog_wrapper::getInstance().get_logger()->error(ZHF_LOG_ADDINFO(msg), __VA_ARGS__)
#define ZHF_ALOG_CRITICAL(msg, ...) zhf::utils::log::async_spdlog_wrapper::getInstance().get_logger()->critical(ZHF_LOG_ADDINFO(msg), __VA_ARGS__)
#else
#define ZHF_LOG_INIT(...) zhf::utils::log::sync_spdlog_wrapper::getInstance().init(__VA_ARGS__)
#define ZHF_LOG_TRACE(msg, ...) zhf::utils::log::sync_spdlog_wrapper::getInstance().get_logger()->trace(ZHF_LOG_ADDINFO(msg), ##__VA_ARGS__)
#define ZHF_LOG_DEBUG(msg, ...) zhf::utils::log::sync_spdlog_wrapper::getInstance().get_logger()->debug(ZHF_LOG_ADDINFO(msg), ##__VA_ARGS__)
#define ZHF_LOG_INFO(msg, ...) zhf::utils::log::sync_spdlog_wrapper::getInstance().get_logger()->info(ZHF_LOG_ADDINFO(msg), ##__VA_ARGS__)
#define ZHF_LOG_WARN(msg, ...) zhf::utils::log::sync_spdlog_wrapper::getInstance().get_logger()->warn(ZHF_LOG_ADDINFO(msg), ##__VA_ARGS__)
#define ZHF_LOG_ERROR(msg, ...) zhf::utils::log::sync_spdlog_wrapper::getInstance().get_logger()->error(ZHF_LOG_ADDINFO(msg), ##__VA_ARGS__)
#define ZHF_LOG_CRITICAL(msg, ...) zhf::utils::log::sync_spdlog_wrapper::getInstance().get_logger()->critical(ZHF_LOG_ADDINFO(msg), ##__VA_ARGS__)

#define ZHF_ALOG_INIT(...) zhf::utils::log::async_spdlog_wrapper::getInstance().init(__VA_ARGS__)
#define ZHF_ALOG_TRACE(msg, ...) zhf::utils::log::async_spdlog_wrapper::getInstance().get_logger()->trace(ZHF_LOG_ADDINFO(msg), ##__VA_ARGS__)
#define ZHF_ALOG_DEBUG(msg, ...) zhf::utils::log::async_spdlog_wrapper::getInstance().get_logger()->debug(ZHF_LOG_ADDINFO(msg), ##__VA_ARGS__)
#define ZHF_ALOG_INFO(msg, ...) zhf::utils::log::async_spdlog_wrapper::getInstance().get_logger()->info(ZHF_LOG_ADDINFO(msg), ##__VA_ARGS__)
#define ZHF_ALOG_WARN(msg, ...) zhf::utils::log::async_spdlog_wrapper::getInstance().get_logger()->warn(ZHF_LOG_ADDINFO(msg), ##__VA_ARGS__)
#define ZHF_ALOG_ERROR(msg, ...) zhf::utils::log::async_spdlog_wrapper::getInstance().get_logger()->error(ZHF_LOG_ADDINFO(msg), ##__VA_ARGS__)
#define ZHF_ALOG_CRITICAL(msg, ...) zhf::utils::log::async_spdlog_wrapper::getInstance().get_logger()->critical(ZHF_LOG_ADDINFO(msg), ##__VA_ARGS__)
#endif

#endif