//
// Created by jaimi on 11/17/2020.
//

#include <visualizer/audio_visualizer_app.h>

using audiovisualizer::visualizer::AudioVisualizerApp;

void prepareSettings(AudioVisualizerApp::Settings* settings) {
    settings->setResizable(false);
}

// This line is a macro that expands into an "int main()" function.
CINDER_APP(AudioVisualizerApp, ci::app::RendererGl, prepareSettings);