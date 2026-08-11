import { test, expect } from '@playwright/test';

/**
 * Smoke tests for the pgAdmin dashboard tool.
 * Validates app load and database connection UI.
 */
test.describe('pgAdmin Smoke Tests', () => {
  test('app loads successfully', async ({ page }) => {
    const response = await page.goto('/', {
      waitUntil: 'domcontentloaded',
      timeout: 30_000,
    });
    expect(response?.status()).toBeLessThan(500);
    await expect(
      page.locator('body'),
    ).not.toBeEmpty();
  });

  test('login page renders', async ({ page }) => {
    await page.goto('/login', {
      waitUntil: 'domcontentloaded',
    });
    const form = page.getByTestId('login-form');
    await expect(form).toBeVisible();
  });

  test('login has connection fields', async ({
    page,
  }) => {
    await page.goto('/login', {
      waitUntil: 'domcontentloaded',
    });
    const host = page.getByTestId('db-host-input');
    const port = page.getByTestId('db-port-input');
    const user = page.getByTestId('db-user-input');
    await expect(host).toBeVisible();
    await expect(port).toBeVisible();
    await expect(user).toBeVisible();
  });

  test('connect button exists', async ({ page }) => {
    await page.goto('/login', {
      waitUntil: 'domcontentloaded',
    });
    const btn = page.getByTestId('db-connect-button');
    await expect(btn).toBeVisible();
    await expect(btn).toBeEnabled();
  });

  test('dashboard page loads', async ({ page }) => {
    const response = await page.goto('/dashboard', {
      waitUntil: 'domcontentloaded',
      timeout: 30_000,
    });
    expect(response?.status()).toBeLessThan(500);
  });

  test('dashboard has database tree', async ({
    page,
  }) => {
    await page.goto('/dashboard', {
      waitUntil: 'domcontentloaded',
    });
    const tree = page.getByTestId('database-tree');
    await expect(tree).toBeVisible();
  });

  test('dashboard has query panel', async ({
    page,
  }) => {
    await page.goto('/dashboard', {
      waitUntil: 'domcontentloaded',
    });
    const panel = page.getByTestId('query-panel');
    await expect(panel).toBeVisible();
  });

  test('page has valid title', async ({ page }) => {
    await page.goto('/', {
      waitUntil: 'domcontentloaded',
    });
    const title = await page.title();
    expect(title).toBeTruthy();
  });
});
