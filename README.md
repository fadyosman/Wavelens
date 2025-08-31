## WaveLens (pronounced similar to Wave Length)
<p align="center">
<img src="images/wavelens.png" alt="drawing" width="200"/><br />
WaveLength Tool
</p>

WaveLength is a tool for taking screenshots, the goal is to make it easy to use and fast, unlike other tools that use a delay to make sure the page is loaded, this tool takes a different approach, it takes advantage of the fact that when the page is still not rendered, the screenshot is an image filled with white pixels only.

## Installation
Simply download the stand alone app image from the [releases section](https://github.com/fadyosman/Wavelens/releases)

## Features
- GUI for taking screenshots for a list of URLs loaded from a file.
- The project can be saved in a single file containing all the screenshots.
- The file is a simple sqlite database with the images stored as blobs.

<p align="center">
<img src="images/wavelens_sample.png" alt="drawing"/><br />
WaveLength Tool
</p>

You can also view the file using sqlitebrowser tool as seen below.<br />
<p align="center">
<img src="images/sqlitebrowser.png" alt="drawing"/><br />
Viewing results in sqlite browser
</p>


## Building From Source
Simple open `WaveLens.pro` in qtcreator and click the build button.

