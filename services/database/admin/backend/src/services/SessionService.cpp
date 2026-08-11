/**
 * @file SessionService.cpp
 * @brief JWT session token create and verify.
 */

#include "SessionService.h"
#include "JwtCodec.h"

#include <chrono>
#include <json/json.h>
#include <openssl/evp.h>
#include <sstream>

namespace pgadmin
{

std::string SessionService::createToken(
    const SessionPayload& p,
    const std::string& secret)
{
    auto now = std::chrono::system_clock::now();
    auto exp = now + std::chrono::hours(24);
    auto iat = std::chrono::duration_cast<
        std::chrono::seconds>(
        now.time_since_epoch()).count();
    auto eat = std::chrono::duration_cast<
        std::chrono::seconds>(
        exp.time_since_epoch()).count();

    std::string header =
        R"({"alg":"HS256","typ":"JWT"})";

    Json::Value pay;
    pay["userId"] = p.userId;
    pay["username"] = p.username;
    pay["iat"] = Json::Int64(iat);
    pay["exp"] = Json::Int64(eat);
    Json::StreamWriterBuilder wb;
    wb["indentation"] = "";
    auto body = Json::writeString(wb, pay);

    auto hp = b64url(header) + "." + b64url(body);
    return hp + "." + hmacSign(hp, secret);
}

std::optional<SessionPayload>
SessionService::verify(
    const std::string& token,
    const std::string& secret)
{
    auto d1 = token.find('.');
    auto d2 = token.find('.', d1 + 1);
    if (d1 == std::string::npos ||
        d2 == std::string::npos)
        return std::nullopt;

    auto hp = token.substr(0, d2);
    auto sig = token.substr(d2 + 1);
    if (hmacSign(hp, secret) != sig)
        return std::nullopt;

    auto payB64 =
        token.substr(d1 + 1, d2 - d1 - 1);
    for (auto& c : payB64) {
        if (c == '-') c = '+';
        else if (c == '_') c = '/';
    }
    while (payB64.size() % 4) payB64 += '=';

    std::string decoded;
    decoded.resize(payB64.size());
    int outLen = EVP_DecodeBlock(
        reinterpret_cast<unsigned char*>(
            decoded.data()),
        reinterpret_cast<const unsigned char*>(
            payB64.data()),
        static_cast<int>(payB64.size()));
    if (outLen < 0) return std::nullopt;
    decoded.resize(static_cast<size_t>(outLen));

    Json::CharReaderBuilder rb;
    Json::Value pay;
    std::istringstream ss(decoded);
    if (!Json::parseFromStream(rb, ss, &pay, nullptr))
        return std::nullopt;

    auto now = std::chrono::duration_cast<
        std::chrono::seconds>(
        std::chrono::system_clock::now()
            .time_since_epoch()).count();
    if (pay["exp"].asInt64() < now)
        return std::nullopt;

    return SessionPayload{
        pay["userId"].asInt(),
        pay["username"].asString(),
    };
}

} // namespace pgadmin
