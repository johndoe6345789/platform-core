/**
 * @file backups_controller_trigger.cpp
 * @brief POST /api/backups/trigger — run a backup now.
 */

#include "BackupsController.h"

#include "backup/backend/BackupRegistry.h"
#include "backup/backend/BackupTypes.h"

#include <drogon/drogon.h>
#include <spdlog/spdlog.h>

namespace nextra::backup
{

using namespace drogon;

namespace
{
DumpKind parseKind(const std::string& s)
{
    if (s == "schema") return DumpKind::Schema;
    if (s == "data")   return DumpKind::Data;
    return DumpKind::Full;
}
}  // namespace

void BackupsController::trigger(
    const HttpRequestPtr& req,
    std::function<void(const HttpResponsePtr&)>&& cb)
{
    auto runner = BackupRegistry::instance().runner();
    if (!runner)
    {
        Json::Value err;
        err["error"] = "backup-manager daemon not attached";
        auto resp = HttpResponse::newHttpJsonResponse(err);
        resp->setStatusCode(k503ServiceUnavailable);
        cb(resp);
        return;
    }

    std::string kindStr{"full"};
    if (const auto j = req->getJsonObject(); j && j->isMember("kind"))
        kindStr = (*j)["kind"].asString();

    const auto result = runner->run(parseKind(kindStr));

    Json::Value body;
    body["id"]     = static_cast<Json::Int64>(result.id);
    body["status"] = result.status;
    body["bytes"]  = static_cast<Json::Int64>(result.bytes);
    body["s3_key"] = result.s3Key;
    body["error"]  = result.error;
    body["kind"]   = kindToString(result.kind);
    cb(HttpResponse::newHttpJsonResponse(body));
}

}  // namespace nextra::backup
