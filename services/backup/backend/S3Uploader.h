#pragma once

/**
 * @file S3Uploader.h
 * @brief Uploads a finished encrypted backup blob to the
 *        in-repo s3server (tools/s3server).
 *
 * The uploader speaks raw HTTP PUT against the s3server
 * endpoint defined in constants/backup-manager.json — no
 * multipart, no AWS SigV4.  The s3server accepts unsigned
 * PUTs from the internal docker network.
 */

#include "BackupTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace nextra::backup
{

/**
 * @brief Upload a complete ciphertext blob to s3server.
 *
 * @param cfg       Backup configuration (endpoint + bucket).
 * @param key       Object key — usually backups/<ts>.pgd.enc.
 * @param payload   Fully assembled ciphertext buffer.
 * @return True on HTTP 2xx from s3server, false otherwise.
 */
bool uploadToS3(const BackupConfig& cfg,
                const std::string& key,
                const std::vector<unsigned char>& payload);

}  // namespace nextra::backup
