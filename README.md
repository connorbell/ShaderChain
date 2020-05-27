# ofxShaderChain

This tool is a development interface for chaining glsl fragment shaders.

[Trello board](https://trello.com/b/EPuGtf6P/ofxshaderchain)

✔️  Load a chain of shaders from json and add passes at runtime.

✔️  Define their uniforms and map them to midi keys.

✔️  Render out to pngs with a duration and fps (good for making loops!)

✔️  Custom texture sizes

✔️  Feedback buffer for each pass available as json param

✔️  Fly around 3d wasd/mouse camera controls that map to uniforms (for exploring raymarched worlds).

✔️  Texture input

🔜 Audio input

🔜 Lotsss

Disclaimer: Very early stages. Great tools I've used like this in the past include [Hedron](https://github.com/nudibranchrecords/hedron) and [Veda](https://atom.io/packages/veda).

## Controls

F - Fullscreen

space - Pause / play

## Dependencies

```
git clone https://github.com/kylemcdonald/ofxFft
git clone https://github.com/jeffcrouse/ofxJSON
git clone https://github.com/danomatika/ofxMidi
git clone https://github.com/andreasmuller/ofxAutoReloadedShader
git clone https://github.com/moebiussurfing/ofxGuiExtended2
git clone https://github.com/connorbell/ofxSortableList2
git clone https://github.com/liquidzym/ofxGStreamer.git
```

## Install

Make sure you have all the addons.

Checkout this repo into your OpenFrameworks "apps" folder.

Run "make" then "make run"
