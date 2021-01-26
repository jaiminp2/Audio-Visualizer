//
// Created by jaimi on 11/17/2020.
//

#include <visualizer/audio_visualizer_app.h>

namespace audiovisualizer {

namespace visualizer {

        AudioVisualizerApp::AudioVisualizerApp() {
            ci::app::setWindowSize(static_cast<int>(kWindowSize), static_cast<int>(kWindowSize));
        }

        void AudioVisualizerApp::setup() {
            sourceFile = ci::audio::load(ci::app::loadAsset(getCommandLineArgs()[1]));
            auto ctx = Context::master();
            mBufferPlayer = ctx->makeNode(new BufferPlayerNode());
            mBufferPlayer->loadBuffer(sourceFile);

            auto monitorFormat = MonitorSpectralNode::Format().fftSize(2048).windowSize(1024);
            mMonitorSpectralNode = ctx->makeNode(new MonitorSpectralNode(monitorFormat));

            mBufferPlayer >> mMonitorSpectralNode;

            mBufferPlayer->enable();
            ctx->enable();

            mSpectrumPlot.setBounds(ci::Rectf(kMargin, kMargin, kWindowSize - kMargin, kWindowSize / 2));
            mWaveformPlot.load(mBufferPlayer->getBuffer(),
                               ci::Rectf(kMargin, kWindowSize / 2, kWindowSize - kMargin, kWindowSize - kMargin));
            getWindow()->setTitle("Audio Visualizer App - " + getCommandLineArgs()[1]);

            mVoice = Voice::create(sourceFile);
            mVoice->start();
            is_playing_ = true;
        }

        void AudioVisualizerApp::draw() {
            ci::gl::clear(ci::Color("black"));
            ci::gl::enableAlphaBlending();

            DrawSpectrograph();
            DrawWaveformGraph();
        }

        void AudioVisualizerApp::update() {
            mMagSpectrum = mMonitorSpectralNode->getMagSpectrum();

        }

        // Unused Methods
        void AudioVisualizerApp::DrawPlayPauseButton(bool playing, float left, float top) {
            ci::gl::color(ci::Color("white"));
            ci::gl::drawStrokedRect(ci::Rectf(ci::vec2{left, top}, ci::vec2{left + kButtonSize, top + kButtonSize}));
            if (playing) {
                // draw pause button
                ci::gl::drawSolidRect(ci::Rectf(
                        ci::vec2{left + kButtonSize / 8, top + kButtonSize / 4},
                        ci::vec2{left + kButtonSize * 3 / 8, top + kButtonSize * 3 / 4}));
                ci::gl::drawSolidRect(ci::Rectf(
                        ci::vec2{left + kButtonSize * 5 / 8, top + kButtonSize / 4},
                        ci::vec2{left + kButtonSize * 7 / 8, top + kButtonSize * 3 / 4}));
            } else {
                // draw play button
                ci::gl::drawSolidTriangle(
                        ci::vec2{left + kButtonSize / 4, top + kButtonSize / 4},
                        ci::vec2{left + kButtonSize / 4, top + kButtonSize * 3 / 4},
                        ci::vec2{left + kButtonSize * 3 / 4, top + kButtonSize / 2});
            }
        }

        void AudioVisualizerApp::DrawStopButton(bool visible, float left, float top) {
            ci::gl::color(ci::Color("white"));
            ci::gl::drawStrokedRect(ci::Rectf(ci::vec2{left, top}, ci::vec2{left + kButtonSize, top + kButtonSize}));
            if (!visible) {
                ci::gl::color(ci::Color("gray"));
            }
            ci::gl::drawSolidRect(ci::Rectf(
                    ci::vec2{left + kButtonSize / 4, top + kButtonSize / 4},
                    ci::vec2{left + kButtonSize * 3 / 4, top + kButtonSize * 3 / 4}));
        }

        void AudioVisualizerApp::DrawRecordButton(bool visible, float left, float top) {
            ci::gl::color(ci::Color("white"));
            ci::gl::drawStrokedRect(ci::Rectf(ci::vec2{left, top}, ci::vec2{left + kButtonSize, top + kButtonSize}));
            if (visible) {
                ci::gl::color(ci::Color("red"));
            } else {
                ci::gl::color(ci::Color("gray"));
            }
            ci::gl::drawSolidCircle(ci::vec2{left + kButtonSize / 2, top + kButtonSize / 2}, kButtonSize / 4);
        }

        void AudioVisualizerApp::DrawPlaybackBar(float left, float top) {
            // progress_ratio should be out of 3 minutes if recording, at which point recording automatically stops
            double progress_ratio = 0.5;
            ci::gl::color(ci::Color("white"));
            // Draw shell
            ci::gl::drawStrokedRoundedRect(
                    ci::Rectf(ci::vec2{left, top}, ci::vec2{left + kWindowSize * 5 / 6, top + kButtonSize / 2}),
                    kButtonSize);
            // Fill in with how much time has passed
            ci::gl::drawSolidRoundedRect(
                    ci::Rectf(ci::vec2{left, top},
                              ci::vec2{left + progress_ratio * kWindowSize * 5 / 6, top + kButtonSize / 2}),
                    kButtonSize);
        }

        void AudioVisualizerApp::DrawLabel(const std::string &label, float left, float top) {
            ci::gl::color(ci::Color("white"));
            ci::gl::drawStrokedRect(
                    ci::Rectf(ci::vec2{left, top}, ci::vec2{left + kButtonSize * 3, top + kButtonSize}));
            ci::gl::drawStringCentered(label, ci::vec2{left + kButtonSize * 3 / 2, top + kButtonSize / 2});
        }

        void AudioVisualizerApp::DrawSpectralCentroid() {
            // Code used from Cinder InputAnalyzerApp sample

            float spectralCentroid = mMonitorSpectralNode->getSpectralCentroid();
            float nyquist = static_cast<float>(master()->getSampleRate()) / 2.0f;
            ci::Rectf bounds = mSpectrumPlot.getBounds();

            float freqNorm = spectralCentroid / nyquist;
            float barCenter = bounds.x1 + freqNorm * bounds.getWidth();
            ci::Rectf verticalBar = {barCenter - 2, bounds.y1, barCenter + 2, bounds.y2};

            ci::gl::ScopedColor colorScope(0.05f, 0.5f, 0, 0.5f);
            if (is_playing_) {
                ci::gl::drawSolidRect(verticalBar);
            }
        }

        void AudioVisualizerApp::DrawSpectrograph() {
            mSpectrumPlot.draw(mMagSpectrum);
            DrawSpectralCentroid();
        }

        void AudioVisualizerApp::DrawWaveformGraph() {
            ci::gl::drawStrokedRect(ci::Rectf(kMargin, kWindowSize / 2, kWindowSize - kMargin, kWindowSize - kMargin));
            ci::gl::translate(kMargin, kWindowSize / 2);
            mWaveformPlot.draw();
            DrawCurrentPlayPosition();
            ci::gl::translate(-kMargin, -kWindowSize / 2);
        }

        void AudioVisualizerApp::DrawCurrentPlayPosition() {
            if (is_playing_) {
                float progress =
                        (kWindowSize - 2 * kMargin) * mBufferPlayer->getReadPosition() / mBufferPlayer->getNumFrames();
                ci::gl::color(ci::Color("Green"));
                ci::gl::drawSolidRect(ci::Rectf(progress - 2, 0, progress + 2, kWindowSize / 2 - kMargin));
            }
        }

        void AudioVisualizerApp::keyDown(ci::app::KeyEvent event) {
            if (event.getCode() == ci::app::KeyEvent::KEY_SPACE) {
                TogglePlayPause();
            }
        }

        void AudioVisualizerApp::mouseDown(ci::app::MouseEvent event) {
            TogglePlayPause();
        }

        void AudioVisualizerApp::TogglePlayPause() {
            if (is_playing_) {
                mBufferPlayer->stop();
                mVoice->stop();
                is_playing_ = false;
            } else {
                mBufferPlayer->start();
                mVoice->start();
                is_playing_ = true;
            }
        }


}  // namespace visualizer

}  // namespace audiovisualizer
