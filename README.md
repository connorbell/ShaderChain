# ofxShaderChain

This tool is a development interface for chaining glsl fragment shaders.

✔️  Load a chain of shaders from json
✔️  Define their uniforms and map them to midi keys
✔️  Render out to pngs with a duration and fps (good for making loops!)
✔️  Custom texture sizes
✔️  Feedback buffer for each pass available as json param
✔️  Fly around 3d wasd/mouse camera controls that map to uniforms (for exploring raymarched worlds).

🔜 Audio input
🔜 Load presets at runtime
🔜 Add/remove passes at runtime
🔜 Lotsss

Disclaimer: Very early stages. Great tools I've used like this in the past include [Hedron](https://github.com/nudibranchrecords/hedron) and [Veda](https://atom.io/packages/veda).

## Controls

F - Fullscreen
space - Pause / play

## Dependencies

ofxFft
ofxGui
ofxJSON
ofxMidi
ofxAutoReloadedShader

## Install

Make sure you have all the addons.
Checkout this repo into your OpenFrameworks "apps" folder.
Run "make" then "make run"
