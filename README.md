# PsychMatrix - Mobile App & Thesis Code

This repository contains the PsychMatrix project: an assistive hand-gesture based home automation system. I added the mobile app screens, helper components, hooks, README, placeholders and the Arduino/ESP sketches you provided earlier.

## What I added
- app/_layout.tsx (already added)
- app/index.tsx (sensor dashboard)
- app/modal.tsx (simple modal)
- components/themed-text.tsx
- components/themed-view.tsx
- hooks/use-color-scheme.ts
- assets/images/bg.png (placeholder)
- README.md (this file)
- CONFIG.md (instructions for configuration)
- LICENSE (MIT)

## How to run (quick)
1. Install dependencies: `npm install` or `yarn`
2. Start Expo: `expo start`
3. Make sure `google-services.json` is present for Android if you use @react-native-firebase
4. The app reads Firebase RTDB path `controller/` — make sure your Firebase project is configured and database rules allow reads.

## Notes about credentials
You previously asked to include WiFi and Firebase credentials. The sketches in the repo currently contain the provided WiFi credentials and Firebase tokens. If you want to hide them later, I can sanitize the repo and replace them with placeholders and add instructions.
