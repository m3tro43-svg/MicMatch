# MIC//MATCH — VST3 microphone voicing

A JUCE 8 / C++17 VST3 and standalone plugin prototype. It applies a selectable broad-band EQ profile to a WA-47jr recording and blends the processed signal with the dry input using one Mix control.

## Profiles
- A — U87-inspired: restrained low-mid reduction and modest open top end.
- B — TLM 102-inspired: more forward upper presence and air.
- C — C800G-inspired: brighter upper presence and airy shelf.
- D — C414-inspired: controlled low mids and moderate top end.

These are starting voicings, not measured microphone-response matching or exact hardware models. Accurate matching requires measured WA-47jr and target-microphone response data captured under comparable conditions.

## Build
This repository includes a Windows GitHub Actions workflow. Push to main or run it manually from the Actions tab. The workflow builds the VST3 on a Windows runner and uploads a ZIP artifact.

Manual Windows build:
```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
```
