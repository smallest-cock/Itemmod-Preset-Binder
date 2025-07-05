# Itemmod Preset Binder (BakkesMod plugin)
Bind BakkesMod item mod codes to garage presets in Rocket League

## 🚀 Description
Create bindings to automatically apply an item mod code when equipping a garage preset. Ideal for players who frequently switch presets but want to avoid opening BakkesMod every time just to change items.

<img src="https://i.imgur.com/ySo6qoV.png" width="80%">

## ⚙️ Settings
- Enable/disable plugin functionality with a simple checkbox
- Change menu text colors for:
    - currently equipped garage preset
    - presets with a binding

## 🛠️ Building
After cloning the repo, make sure to initialize the submodules before attempting to build the project:

Run `./scripts/init-submodules.bat` to initialize the submodules in an optimal way

<details> <summary>🔍 Why this instead of <code>git submodule update --init --recursive</code> ?</summary>
<li>Avoids downloading 200MB of history for the <strong>nlohmann/json</strong> library</li>
<li>Allows Git to detect updates for the other submodules</li>
</details>

## 👀 Credits
Idea by @atomictyler