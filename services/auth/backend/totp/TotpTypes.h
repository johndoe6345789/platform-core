#pragma once
/**
 * @file TotpTypes.h
 * @brief Shared TOTP enrolment POD types.
 */

#include <string>
#include <vector>

namespace services::auth::totp
{

/** @brief New enrolment payload returned by enroll(). */
struct EnrolmentResult
{
    std::string secretB32;
    std::string otpauthUri;
    std::vector<std::string> recoveryPlain;
};

/** @brief Persisted enrolment row. */
struct TotpRow
{
    std::string userId;
    std::string secretB32;
    bool enabled{false};
    std::vector<std::string> recoveryHashes;
};

} // namespace services::auth::totp
