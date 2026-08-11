/**
 * @file ObjectMutCtrl.cpp
 * @brief S3 object PUT + DELETE handlers.
 */

#include "../services/BucketStore.h"
#include "../services/Globals.h"
#include "../services/ObjectStore.h"
#include "ObjectCtrl.h"

using namespace drogon;

namespace s3
{

void ObjectCtrl::putObject(const HttpRequestPtr& req,
                           std::function<void(const HttpResponsePtr&)>&& cb,
                           const std::string& bucket, const std::string& key)
{
    int bid = BucketStore::getId(bucket);
    if (bid == 0) {
        auto r = HttpResponse::newHttpResponse();
        r->setStatusCode(k404NotFound);
        r->setBody("NoSuchBucket");
        cb(r);
        return;
    }

    auto body = std::string(req->body());
    auto ct = req->getHeader("Content-Type");
    if (ct.empty())
        ct = "application/octet-stream";

    auto res = Globals::blobs->store(bucket, key, body);
    ObjectStore::put(bid, key, res.etag, (int64_t)res.size, ct, res.path);

    auto r = HttpResponse::newHttpResponse();
    r->addHeader("ETag", "\"" + res.etag + "\"");
    r->setStatusCode(k200OK);
    cb(r);
}

void ObjectCtrl::deleteObject(const HttpRequestPtr& req,
                              std::function<void(const HttpResponsePtr&)>&& cb,
                              const std::string& bucket, const std::string& key)
{
    int bid = BucketStore::getId(bucket);
    if (bid == 0) {
        auto r = HttpResponse::newHttpResponse();
        r->setStatusCode(k404NotFound);
        cb(r);
        return;
    }

    auto path = ObjectStore::remove(bid, key);
    if (!path.empty())
        Globals::blobs->remove(path);

    auto r = HttpResponse::newHttpResponse();
    r->setStatusCode(k204NoContent);
    cb(r);
}

} // namespace s3
