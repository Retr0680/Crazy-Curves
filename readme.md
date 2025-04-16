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

## 📝 Automated Changelog

Your GitHub Releases will include a clean, formatted changelog section automatically.

Use clear commit messages following [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/):
- `feat:` for new features
- `fix:` for bug fixes
- `docs:` for documentation updates
- `chore:` for maintenance

### Example Release Notes:
```markdown
### 🚀 Features
- feat: add elastic decay smoothing

### 🐛 Fixes
- fix: handle empty keyframe selections

### 🧹 Maintenance
- chore: migrate to Node 18
```

This makes your release notes clean, readable, and professional.

Powered by [softprops/action-gh-release](https://github.com/softprops/action-gh-release), which appends commits since last tag.

📘 In addition, every release will auto-update `CHANGELOG.md` with latest changes.

To manually generate a changelog:
```bash
npx auto-changelog --template keepachangelog --output CHANGELOG.md
```

### 🧩 CI Integration

In your GitHub Actions workflow, add:
```yaml
- name: Generate CHANGELOG.md
  run: npx auto-changelog --template keepachangelog --output CHANGELOG.md

- name: Commit and push changelog
  run: |
    git config user.name github-actions
    git config user.email github-actions@github.com
    git add CHANGELOG.md
    git commit -m "docs: update changelog"
    git push
```

This keeps `CHANGELOG.md` version-controlled with every release.

---

## 🔐 Signing (optional)

To enable signed `.zxp` in CI:
- Add repo secrets:
  - `CERT_P12_BASE64`: Base64-encoded .p12 certificate
  - `CERT_PASSWORD`: Password for the certificate

---

## 📄 License
MIT License

