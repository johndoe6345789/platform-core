/** @brief Authentication-related types. */

/** @brief Login API request body. */
export interface LoginRequest {
  username: string;
  password: string;
}

/** @brief Login API response. */
export interface LoginResponse {
  token: string;
  username: string;
}

/** @brief Session state for the client. */
export interface SessionState {
  token: string | null;
  username: string | null;
  isAuthenticated: boolean;
}
