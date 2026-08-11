#pragma once
/**
 * @file UserProvision.h
 * @brief Just-in-time provisioning of in-house `users`
 *        rows for Keycloak-authenticated identities.
 *
 * Keycloak owns identity post-migration but the
 * in-house backend still has its own `users` table for
 * gamification, preferences, avatar, etc. New users
 * registered through Keycloak (or federated from an
 * external IdP) won't have a row in `users`, which
 * makes downstream endpoints 404. This helper inserts
 * one if missing, idempotent and cached per-process.
 */

#include "KeycloakVerifier.h"

namespace services::auth::keycloak
{

/**
 * @brief Ensure a `users` row exists for the given
 *        Keycloak claims. Idempotent; in-memory cache
 *        means at most one INSERT per process per user.
 *        Errors are swallowed (filter must keep going).
 */
void ensureUserRow(const KeycloakClaims& claims);

}  // namespace services::auth::keycloak
