# Security Policy

## Supported versions

This project is an active prototype. Security fixes are applied to the default branch.

## Reporting a vulnerability

Please do not open a public issue for a suspected vulnerability. Use GitHub's private vulnerability reporting for this repository, or contact the maintainers privately through the contact details on the GitHub profile.

Include the affected file or component, reproduction steps, potential impact, and a suggested mitigation when possible. Do not include passwords, Firebase tokens, or other live credentials in a report.

## Credential handling

- Never commit `.env` files, Firebase platform configuration, Wi-Fi passwords, or private keys.
- Rotate credentials immediately if they are exposed.
- Use authenticated Firebase rules for anything beyond local development.
