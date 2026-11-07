# Install the Linux preview

The prebuilt preview is prepared locally for **Arch Linux x86_64**. A public
download is not available yet; distribution review remains open. These steps
apply when you have the preview archive and its matching checksum file.

You need your own *Conquest: Frontier Wars* installation, a working Vulkan driver,
and the original font families. No compiler or Wine is required to run the game.

## 1. Install system dependencies

On an up-to-date Arch system:

```sh
sudo pacman -Syu --needed python sdl3 freetype2
```

Use the Vulkan driver appropriate for your GPU. The preview was built with SDL
3.4.16 and FreeType 2.14.3. Its ELF symbol requirements are recorded in
`manifest.json`; this build requires at least GLIBC 2.44 and GLIBCXX 3.4.32.
Older distributions and Steam Deck are not validated.

Package references: [Arch SDL3](https://archlinux.org/packages/extra/x86_64/sdl3/)
and [Arch FreeType](https://archlinux.org/packages/extra/x86_64/freetype2/).

## 2. Extract the preview

Put the archive and checksum in the same directory, then run:

```sh
sha256sum -c frontierx-linux-x86_64-preview.tar.gz.sha256
tar -xzf frontierx-linux-x86_64-preview.tar.gz
cd frontierx-linux-x86_64-preview
```

Keep this folder wherever you want to run FrontierX. Do not extract it over your
original game installation. No system-wide installation or root access is needed
for the preview itself.

## 3. Point it at your game and fonts

The default Steam location is shown below. Change it if your Steam library is
elsewhere. The selected directory must contain `DATA`, `gametypes.db`,
`gendata.db`, and `stringpack.db`.

```sh
./frontierx configure \
  --game-dir "$HOME/.local/share/Steam/steamapps/common/Conquest Frontier Wars" \
  --font-dir /path/to/your/fonts
```

Supply lawfully obtained **Arial Regular/Bold** and **Arial Narrow Regular/Bold**
as `arial.ttf`, `arialbd.ttf`, `arialn.ttf`, and `arialnb.ttf`. Filename case does
not matter. The game installation supplies `FONTS/bauhmib.ttf` for Bauhaus.
Repeat `--font-dir` if your font files are in separate directories. FrontierX
references the files in place; it does not download, copy, or redistribute them.

If you already have a complete FrontierX font map, use this instead:

```sh
./frontierx configure --game-dir /path/to/game --font-map /path/to/font-map.txt
```

Configuration validates paths. The game's font loader checks actual families and
weights when it starts. Substituting unrelated fonts is not a supported setup.

## 4. Check and play

```sh
./frontierx check
./frontierx play
```

`check` verifies game/font paths and dynamic-library loading without opening a
window. It does not test Vulkan or gameplay. `play` opens the original menus for
Campaign, Load, Options, and offline Skirmish. Use **Alt+Enter** for fullscreen.

## Saves, updates, and troubleshooting

Saves and `player.log` live in `~/.local/share/frontierx-preview/`. Launcher and
game settings live in `~/.config/frontierx-preview/`. The launcher honors
`XDG_DATA_HOME` and `XDG_CONFIG_HOME` if you use different locations.

Updates can be extracted into a new folder and use the same settings and saves.
Deleting an extracted package leaves that player data intact. Existing private
playtest saves are not imported automatically; keep backups before migrating
any saves.

If a path changes, repeat `configure`. If `check` reports a missing shared library,
verify system dependencies and extract the complete archive again. For a failed
launch, inspect `player.log` and report your GPU, driver, revision from
`manifest.json`, and steps to reproduce. Remove private paths before sharing logs.

The preview includes replaceable LGPL media libraries and their source archive.
It contains no retail installation, proprietary font files, or saved games.
It remains under the complete [Conquest source license](../Conquest%20Source%20License.txt).
