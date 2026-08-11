/**
 * @file S3Uploader.cpp
 * @brief Minimal HTTP PUT uploader for the backup pipeline.
 */

#include "S3Uploader.h"

#include <drogon/HttpClient.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include <spdlog/spdlog.h>

#include <future>
#include <string>

namespace nextra::backup
{

bool uploadToS3(const BackupConfig& cfg,
                const std::string& key,
                const std::vector<unsigned char>& payload)
{
    using namespace drogon;
    auto client = HttpClient::newHttpClient(cfg.s3Endpoint);
    auto req    = HttpRequest::newHttpRequest();
    req->setMethod(Put);
    req->setPath("/" + cfg.bucket + "/" + key);
    req->addHeader("Content-Type", "application/octet-stream");
    req->setBody(std::string(
        reinterpret_cast<const char*>(payload.data()),
        payload.size()));

    std::promise<ReqResult> rp;
    std::promise<HttpResponsePtr> hp;
    client->sendRequest(
        req,
        [&](ReqResult r, const HttpResponsePtr& resp) {
            rp.set_value(r);
            hp.set_value(resp);
        });

    const auto result = rp.get_future().get();
    const auto resp   = hp.get_future().get();
    if (result != ReqResult::Ok || !resp)
    {
        spdlog::error("backup: s3 upload transport failure");
        return false;
    }
    const int code = static_cast<int>(resp->statusCode());
    if (code < 200 || code >= 300)
    {
        spdlog::error("backup: s3 PUT {} -> {}", key, code);
        return false;
    }
    spdlog::info("backup: uploaded {} ({} bytes)", key,
                 payload.size());
    return true;
}

}  // namespace nextra::backup
