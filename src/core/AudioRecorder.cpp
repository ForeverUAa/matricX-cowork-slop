#include "core/AudioRecorder.h"

#include <QDir>
#include <QStandardPaths>
#include <QUrl>

#ifdef MCW_HAS_MULTIMEDIA
#include <QAudioInput>
#include <QMediaCaptureSession>
#include <QMediaFormat>
#include <QMediaRecorder>
#endif

namespace mcw {

AudioRecorder::AudioRecorder(QObject *parent) : QObject(parent) {
#ifdef MCW_HAS_MULTIMEDIA
    m_session = new QMediaCaptureSession(this);
    m_audioInput = new QAudioInput(this);
    m_recorder = new QMediaRecorder(this);

    m_session->setAudioInput(m_audioInput);
    m_session->setRecorder(m_recorder);

    connect(m_recorder, &QMediaRecorder::recorderStateChanged, this, [this](QMediaRecorder::RecorderState state) {
        if (state == QMediaRecorder::StoppedState && m_pendingCallback) {
            auto cb = m_pendingCallback;
            m_pendingCallback = nullptr;
            cb(true, m_outputFile, QString());
        }
    });

    connect(m_recorder, &QMediaRecorder::errorOccurred, this, [this](QMediaRecorder::Error, const QString &errorString) {
        if (m_pendingCallback) {
            auto cb = m_pendingCallback;
            m_pendingCallback = nullptr;
            cb(false, QString(), errorString);
        }
    });
#endif
}

AudioRecorder::~AudioRecorder() {
#ifdef MCW_HAS_MULTIMEDIA
    if (m_recording && m_recorder) {
        m_recorder->stop();
    }
#endif
}

void AudioRecorder::startRecording() {
    if (m_recording) return;

    const QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    m_outputFile = QDir(tempDir).filePath("mcowork_voice_input.m4a");

#ifdef MCW_HAS_MULTIMEDIA
    if (m_recorder) {
        QMediaFormat format;
        format.setFileFormat(QMediaFormat::MPEG4);
        format.setAudioCodec(QMediaFormat::AudioCodec::AAC);
        m_recorder->setMediaFormat(format);
        m_recorder->setOutputLocation(QUrl::fromLocalFile(m_outputFile));
        m_recorder->record();
        m_recording = true;
    }
#else
    m_recording = true;
#endif
}

void AudioRecorder::stopRecording(FinishCallback callback) {
    if (!m_recording) {
        if (callback) callback(false, QString(), "Not recording");
        return;
    }

    m_recording = false;
    m_pendingCallback = callback;

#ifdef MCW_HAS_MULTIMEDIA
    if (m_recorder) {
        m_recorder->stop();
    } else if (m_pendingCallback) {
        m_pendingCallback(false, QString(), "Media recorder not available");
    }
#else
    if (m_pendingCallback) {
        m_pendingCallback(false, QString(), "Qt6 Multimedia support not compiled in this build");
    }
#endif
}

}  // namespace mcw
