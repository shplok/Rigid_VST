# Gesture-Controlled Audio Plugin (VST3)

A JUCE-based VST3 audio plugin that lets you control audio effects using hand gestures sent over OSC (Open Sound Control). Inspired by Imogen Heap's glove tech — no wearable hardware needed, just a webcam and gesture classifier.

---

## Features

- Real-time gesture control via OSC messages
- Gain automation triggered by recognized gestures
- Granular audio effect when "palm" is detected
- Built with JUCE and compatible with major DAWs

---

## Installation

### 1. Download VST
Download the `Rigid_test.vst3` file from the [Releases](https://github.com/shplok/RigidVST/releases) section.

### 2. Place the Plugin

| DAW         | Plugin Folder Location                                  |
|-------------|----------------------------------------------------------|
| **FL Studio** | `C:\Program Files\Common Files\VST3`                    |
| **Ableton Live** | `C:\Program Files\Common Files\VST3` (Windows) or `~/Library/Audio/Plug-Ins/VST3` (macOS) |
| **Logic Pro** | `~/Library/Audio/Plug-Ins/Components` (.component format only)* |

>  _This plugin is currently VST3 format only. Logic Pro support requires conversion to AU format._

---
## To control the plugin via hand gestures:

1. Clone or download the repository.
2. Run the Python gesture classifier:

```bash
python send_osc_gesture.py
