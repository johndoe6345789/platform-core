#pragma once
/**
 * @file RateLimitDemoController.h
 * @brief Tiny /api/ratelimit/ping route that demonstrates the
 *        new nextra::filters::RateLimitFilter in isolation.
 *
 * The existing auth / admin controllers are owned by other
 * agents and are not edited here. This controller exists so
 * the template has at least one wired consumer of the Redis
 * token-bucket filter out of the box — E2E tests can hit it
 * to verify 429 semantics.
 */

#include <drogon/HttpController.h>

namespace nextra::controllers
{

/**
 * @class RateLimitDemoController
 * @brief Minimal ping endpoint gated by the bucket filter.
 */
class RateLimitDemoController
    : public drogon::HttpController<RateLimitDemoController>
{
  public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(
        RateLimitDemoController::ping,
        "/api/ratelimit/ping", drogon::Get,
        "nextra::filters::RateLimitFilter");
    METHOD_LIST_END

    /**
     * @brief Reply with {ok:true}; filter may return 429.
     * @param req Drogon request.
     * @param cb  Response callback.
     */
    void ping(
        const drogon::HttpRequestPtr& req,
        std::function<void(
            const drogon::HttpResponsePtr&)>&& cb);
};

} // namespace nextra::controllers
