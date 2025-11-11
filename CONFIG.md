# Configuration

1. Firebase
- Ensure `google-services.json` (Android) is present in the project root if using native builds.
- The React Native app uses `@react-native-firebase/database`. Configure your Firebase project and ensure the Realtime Database has a `controller/` node with sensor & device state data.

2. Images & assets
- Replace `assets/images/bg.png` with your desired background image.

3. Secrets
- If you want to remove secrets from the repository, remove the firebase auth tokens and WiFi credentials from the Arduino/ESP sketches and keep them locally.
