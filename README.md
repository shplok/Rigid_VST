# Gesture-Controlled Audio Plugin (VST3)

### By: Sawyer Bowerman and Alicia Cao

Rigid is a JUCE-based VST3 audio plugin that lets you control audio effects using hand gestures sent over OSC (Open Sound Control). Inspired by Imogen Heap's glove tech — no wearable hardware needed, just a webcam and gesture classifier.

OSC (Open Sound Control) is a protocol used to send real-time messages between devices or software over a network. In the context of this project, OSC is used to send hand gesture data from a Python script (which processes webcam input) to the JUCE-based audio plugin. For example, when the script detects a “fist” gesture, it sends an OSC message to the plugin, which then responds by changing its gain or applying granular audio effects — all in sync with the music software.

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
## 3. To control the plugin via hand gestures:

1. Clone or download the repository.
2. Run the Python gesture classifier:

```bash
python send_osc_gesture.py
```
## 4. Open the VST in your DAW and voila!
