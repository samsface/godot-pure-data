# 🎵 Godot-Pure-Data

> **Godot libpd without rebuilding Godot.** Supports 🪟Windows(x86_x64), 🐧Linux(x86_x64), & 🍎macOS(x86_x64, aarch64).

This library works and is ready to use. But repo in progress. Hit me up with an issue.

## Downloading

For now, the best way to download this is to download the `project` artifact of the latest CI build. Take a look at [this guide](https://docs.github.com/en/actions/managing-workflow-runs-and-deployments/managing-workflow-runs/downloading-workflow-artifacts) if you don't know how to do this. You will want the latest successful build from [this page](https://github.com/samsface/godot-pure-data/actions?query=event%3Apush+branch%3Amaster). A demo project is included (open the `project.godot` file in the root of the archive you downloaded), and you can include this in your own project by copying the `addons` directory into your project.

## Building from source

Most of the dependencies are bundled with the library as submodules. As long as you ran git clone with `--recurse-submodules` and you have CMake installed, you should just be able to run `dev.sh` to build.
