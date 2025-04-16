# 🎨 Crazy Curves for After Effects

**Crazy Curves** is a custom After Effects extension built with CEP that allows motion designers to easily apply advanced easing, bounce, elastic, and distribution curves to keyframes.

![Crazy Curves Demo](https://raw.githubusercontent.com/Retr0680/Crazy-Curves/main/demo/crazy-curves-demo.gif)

---

## 🚀 Features

- 🔁 **Ease Mode** – Quickly smooth keyframe timing
- 🧘 **Elastic Mode** – Add natural spring animations
- 🏀 **Bounce Mode** – Add dynamic bouncy movement
- 🧩 **Distribute Mode** – Offset layers and keyframes evenly
- 🎞️ Animated previews for each mode

---

## 📦 Installation

Download the latest `.zxp` from the release:

[![Download ZXP](https://img.shields.io/badge/Download-CrazyCurves.zxp-blue?logo=adobe)](https://github.com/Retr0680/Crazy-Curves/releases/latest)

Install it using [ZXPInstaller](https://zxpinstaller.com) or [ExManCmd](https://helpx.adobe.com/extension-manager/using/command-line.html).

---

## 🛠 Usage

1. Open **After Effects**
2. Go to `Window > Extensions > Crazy Curves`
3. Select a layer or keyframes
4. Choose a mode (Ease, Elastic, Bounce, Distribute)
5. Adjust parameters → Click **Apply**

---

## 🧪 Developer Setup

```bash
git clone https://github.com/Retr0680/Crazy-Curves.git
cd Crazy-Curves
```

To build the plugin manually:
```bash
zip -r CrazyCurves.zip crazy-curves/
```

To trigger GitHub CI workflow:
[![Build](https://github.com/Retr0680/Crazy-Curves/actions/workflows/build.yml/badge.svg)](https://github.com/Retr0680/Crazy-Curves/actions/workflows/build.yml)

You can trigger the workflow manually and choose version bump type:
- `patch` (default)
- `minor`
- `major`

---

## 📄 License
MIT License

