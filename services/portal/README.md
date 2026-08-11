# services/portal

Developer portal homepage — a grid of cards linking
to every tool in the Nextra stack. Served by the
main Drogon `nextra-api` binary at `/portal/`.

## Data source

Reads `shared/constants/tool-links.json`, the
single source of truth also consumed by the main
app's drawer (`DrawerToolLinks`) and the webmail
sidebar. Adding a new tool there makes it appear
in all three places on the next backend rebuild.

## Layout

```
services/portal/
  backend/
    portal_renderer.h         # pure HTML renderer
    portal_renderer.cpp
    portal_html_helpers.h     # esc + renderCard
    portal_embedded.h.in      # configured by cmake
  controllers/
    PortalController.{h,cpp}  # Drogon routes
  static/
    styles.css                # baked into binary
  tests/
    test_portal_renderer.cpp
  constants.json
```

## Build-time embed

`cmake/portal.cmake` reads the JSON and CSS and
runs `configure_file` to generate
`portal/backend/portal_embedded.h` under
`build/.../generated/`. That header is added to
the include path so the renderer can reference
the baked content as constexpr strings.

## Routes

| Method | Path                   | Response           |
| ------ | ---------------------- | ------------------ |
| GET    | `/`                    | 302 `/portal/`     |
| GET    | `/portal`              | 302 `/portal/`     |
| GET    | `/portal/`             | HTML document      |
| GET    | `/portal/styles.css`   | CSS stylesheet     |

nginx proxies `/portal/` (and `/`) to the backend
on port 8080; the old static mount is gone.
