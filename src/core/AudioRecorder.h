#pragma once

#include <QObject>
#include <QString>
#include <functional>

#ifdef MCW_HAS_MULTIMEDIA
class QAudioInput;
class QMediaCaptureSession;
class QMediaRecorder;
#endif

namespace mcw {

// Manages microphone audio recording for voice input.
// Records to a temporary WAV/M4A file and notifies when recording completes.
class AudioRecorder : public QObject {
    Q_OBJECT
public:
    using FinishCallback = std::function<void(bool success, const QString &filePath, const QString &error)>;

    explicit AudioRecorder(QObject *parent = nullptr);
    ~AudioRecorder();

    bool isRecording() const { return m_recording; }
    void startRecording();
    void stopRecording(FinishCallback callback);

private:
    bool m_recording = false;
    QString m_outputFile;
    FinishCallback m_pendingCallback;

#ifdef MCW_HAS_MULTIMEDIA
    QAudioInput *m_audioInput = nullptr;
    QMediaCaptureSession *m_session = nullptr;
    QMediaRecorder *m_recorder = nullptr;
#endif
};

}  // namespace mcw
