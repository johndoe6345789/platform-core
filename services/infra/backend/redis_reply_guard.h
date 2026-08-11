#pragma once
/**
 * @file redis_reply_guard.h
 * @brief RAII guard that frees a hiredis redisReply.
 */

#include <hiredis/hiredis.h>

namespace nextra::infra
{

/**
 * @struct ReplyGuard
 * @brief Owns a redisReply pointer and frees it on scope exit.
 */
struct ReplyGuard
{
    redisReply* r;

    /** @brief Wrap a void* returned by redisCommand*. */
    explicit ReplyGuard(void* p)
        : r(static_cast<redisReply*>(p))
    {
    }

    ~ReplyGuard()
    {
        if (r != nullptr) {
            freeReplyObject(r);
        }
    }

    ReplyGuard(const ReplyGuard&) = delete;
    ReplyGuard& operator=(const ReplyGuard&) = delete;
};

} // namespace nextra::infra
