# Configuration

## Firebase mobile app

1. Create a Firebase project and Realtime Database.
2. Add the Android application and place the downloaded `google-services.json` in the native Android project when building locally. Add `GoogleService-Info.plist` for iOS.
3. Configure database rules to allow only the authenticated access your deployment needs. Do not use public read/write rules in production.
4. Create the `controller/` schema shown in the README.

These platform files are ignored by Git. CI should receive them through the build system's encrypted secrets, never as committed files.

## Firmware

Firmware credentials must be supplied locally. Prefer a `secrets.h` file excluded by `.gitignore`, or your board platform's secret manager. The committed sketches must contain no Wi-Fi passwords or Firebase tokens. Rotate credentials immediately if they were ever exposed.

## Assets

Replace the placeholder images under `assets/images/` with valid PNG/JPEG files before shipping. Keep large generated assets out of Git unless they are required by the app.
