'use client';

import { useLoginForm } from '@/hooks/useLoginForm';
import styles from '@scss/atoms/auth.module.scss';

/** @brief Props for LoginForm. */
interface LoginFormProps {
  onLogin: (
    token: string, username: string,
  ) => void;
}

/** @brief Admin login form component. */
export default function LoginForm(
  { onLogin }: LoginFormProps,
) {
  const {
    username, setUsername,
    password, setPassword,
    error, loading, handleSubmit,
  } = useLoginForm(onLogin);

  return (
    <div
      className={styles.authPage}
      data-testid="login-form"
    >
      <div className={styles.authCard}>
        <div className={styles.authHeader}>
          <h1 className={styles.authTitle}>
            PgAdmin
          </h1>
          <p className={styles.authSubtitle}>
            Sign in to manage your database
          </p>
        </div>
        {error && (
          <div className="error" role="alert"
            aria-live="assertive">
            {error}
          </div>
        )}
        <form
          className={styles.authForm}
          onSubmit={handleSubmit}
          aria-label="Sign in"
        >
          <label htmlFor="username">
            Username
          </label>
          <input
            id="username"
            type="text"
            value={username}
            onChange={
              (e) => setUsername(e.target.value)
            }
            disabled={loading}
            aria-label="Username"
            data-testid="username-input"
            required
          />
          <label htmlFor="password">
            Password
          </label>
          <input
            id="password"
            type="password"
            value={password}
            onChange={
              (e) => setPassword(e.target.value)
            }
            disabled={loading}
            aria-label="Password"
            data-testid="password-input"
            required
          />
          <button
            type="submit"
            disabled={loading}
            data-testid="login-button"
          >
            {loading
              ? 'Signing in...'
              : 'Sign In'}
          </button>
        </form>
      </div>
    </div>
  );
}
