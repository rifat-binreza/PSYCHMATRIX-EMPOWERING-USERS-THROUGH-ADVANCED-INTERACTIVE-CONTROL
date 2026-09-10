# Contributing

Thanks for helping improve PsychMatrix.

## Before you start

1. Open an issue for substantial behavior or architecture changes.
2. Do not include credentials or private user data.
3. Keep the Firebase data contract and hardware assumptions documented.

## Local checks

```sh
npm ci
npm run typecheck
npm run lint
npm run format:check
```

## Pull requests

- Use a descriptive title and explain the user impact.
- Keep each pull request focused.
- Include screenshots or a short recording for UI changes.
- Describe hardware, Firebase schema, or configuration changes explicitly.
- Confirm that no secrets are included.
