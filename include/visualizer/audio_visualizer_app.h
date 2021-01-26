//
// Created by jaimi on 11/17/2020.
//

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/audio/audio.h"
#include "AudioDrawUtils.h"

namespace audiovisualizer {

namespace visualizer {

    using namespace ci::audio;

    class AudioVisualizerApp : public ci::app::App {
    public:
        AudioVisualizerApp();

        // constructs everything necessary to run the visualizer
        void setup() override;

        // renders the visuals
        void draw() override;

        // increments the time
        void update() override;

        // Use Spacebar for play/pause toggle
        void keyDown(ci::app::KeyEvent event) override;

        // Use Mouse click for play/pause toggle
        void mouseDown(ci::app::MouseEvent event) override;

    private:

        // Square window side length dimension
        const float kWindowSize = 800;
        const float kMargin = kWindowSize / 20;
        const float kButtonSize = kWindowSize / 24;

        // Audio File
        VoiceRef mVoice;

        SourceFileRef sourceFile;
        BufferPlayerNodeRef mBufferPlayer;
        MonitorSpectralNodeRef mMonitorSpectralNode;
        std::vector<float> mMagSpectrum;
        SpectrumPlot mSpectrumPlot;
        WaveformPlot mWaveformPlot;
        ci::gl::TextureFontRef mTextureFont;

        bool is_playing_ = false;

        // Draw methods
        // Draws the spectrograph of the frequency values
        void DrawSpectrograph();

        // Draws the waveform of the audio track
        void DrawWaveformGraph();

        // Helper methods
        // Draws the spectral centroid of the frequency values
        void DrawSpectralCentroid();

        // Draws the current playback position in the waveform graph
        void DrawCurrentPlayPosition();

        // Switches between playing and pausing the audio track
        void TogglePlayPause();

        // Transport controls: decided not to use
        // Draws a play button if false, pause button if true
        void DrawPlayPauseButton(bool playing, float left, float top);

        // Draws a stop button, grayed out if false;
        void DrawStopButton(bool visible, float left, float top);

        // Draws a record button, grayed out if false
        void DrawRecordButton(bool visible, float left, float top);

        // Draws the playback time bar
        void DrawPlaybackBar(float left, float top);

        // Draws a button with text
        void DrawLabel(const std::string &label, float left, float top);

    };

}  // namespace visualizer

}  // namespace audiovisualizer
