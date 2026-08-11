/**
 * @file HealthCtrl.cpp
 * @brief Health check endpoint for pgadmin.
 */

#include "HealthCtrl.h"

using namespace drogon;

namespace pgadmin
{

void HealthCtrl::check(
    const HttpRequestPtr&,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto r = HttpResponse::newHttpJsonResponse(
        Json::Value("ok"));
    cb(r);
}

} // namespace pgadmin
