'use client';

import {
  Drawer, List, ListItemButton, ListItemIcon,
  ListItemText, Typography, Divider, IconButton,
  Box, Close, Home, Email, Storage, Archive,
  CloudQueue, OpenInNew,
} from '@shared/m3';
import siteLinks from '@/constants/site-links.json';

const iconMap: Record<string, React.ReactElement> = {
  Home: <Home />, Email: <Email />,
  Storage: <Storage />, Inventory: <Archive />,
  CloudQueue: <CloudQueue />,
};

/** @brief Props for the SiteDrawer component. */
interface SiteDrawerProps {
  open: boolean; onClose: () => void;
}

const paperSx = {
  width: 280, borderRadius: '0 16px 16px 0',
};
const headerSx = {
  display: 'flex', alignItems: 'center',
  justifyContent: 'space-between', px: 2, py: 1,
};
const iconStyle = {
  minWidth: 40,
  color: 'var(--md-sys-color-primary)',
};
const extStyle = {
  fontSize: 14,
  color: 'var(--md-sys-color-on-surface-variant)',
};

/** @brief Drawer showing cross-site navigation. */
export default function SiteDrawer(
  { open, onClose }: SiteDrawerProps,
) {
  return (
    <Drawer
      anchor="left"
      open={open}
      onClose={onClose}
      data-testid="site-drawer"
      PaperProps={{ sx: paperSx }}
    >
      <Box sx={headerSx}>
        <Typography variant="h6">NextExtra</Typography>
        <IconButton
          onClick={onClose}
          aria-label="Close drawer"
          data-testid="close-drawer"
        >
          <Close />
        </IconButton>
      </Box>
      <Divider />
      <Typography
        variant="overline"
        sx={{ px: 2, pt: 1 }}
      >
        Sites
      </Typography>
      <List>
        {siteLinks.map((link) => {
          const cur = !!link.current;
          return (
            <ListItemButton
              key={link.label}
              component="a"
              href={link.url}
              selected={cur}
              target={cur ? undefined : '_blank'}
              rel="noopener noreferrer"
              data-testid={`site-${link.icon}`}
            >
              <ListItemIcon style={iconStyle}>
                {iconMap[link.icon]}
              </ListItemIcon>
              <ListItemText primary={link.label} />
              {cur ? null : (
                <OpenInNew style={extStyle} />
              )}
            </ListItemButton>
          );
        })}
      </List>
    </Drawer>
  );
}
