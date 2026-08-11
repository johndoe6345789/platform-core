/**
 * @file JobTypes.cpp
 * @brief String conversions for the job scheduler enums.
 *
 * Kept in their own TU so the header stays header-only and cheap
 * to include from hot paths (workers, cron tick, REST controller).
 */

#include "JobTypes.h"

#include <stdexcept>

namespace nextra::jobs
{

std::string toString(JobStatus s)
{
    switch (s)
    {
        case JobStatus::Queued:    return "queued";
        case JobStatus::Running:   return "running";
        case JobStatus::Retrying:  return "retrying";
        case JobStatus::Succeeded: return "succeeded";
        case JobStatus::Failed:    return "failed";
    }
    return "queued";
}

std::string toString(BackoffStrategy s)
{
    switch (s)
    {
        case BackoffStrategy::Exponential: return "exponential";
        case BackoffStrategy::Linear:      return "linear";
        case BackoffStrategy::Fixed:       return "fixed";
    }
    return "exponential";
}

JobStatus parseJobStatus(const std::string& s)
{
    if (s == "queued")    return JobStatus::Queued;
    if (s == "running")   return JobStatus::Running;
    if (s == "retrying")  return JobStatus::Retrying;
    if (s == "succeeded") return JobStatus::Succeeded;
    if (s == "failed")    return JobStatus::Failed;
    throw std::invalid_argument("Unknown JobStatus: " + s);
}

BackoffStrategy parseBackoff(const std::string& s)
{
    if (s == "exponential") return BackoffStrategy::Exponential;
    if (s == "linear")      return BackoffStrategy::Linear;
    if (s == "fixed")       return BackoffStrategy::Fixed;
    throw std::invalid_argument("Unknown BackoffStrategy: " + s);
}

}  // namespace nextra::jobs
