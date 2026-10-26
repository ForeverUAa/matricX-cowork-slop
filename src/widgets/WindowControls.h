#pragma once

#include <QEnterEvent>
#include <QWidget>

#include "widgets/CircleButton.h"

class QHBoxLayout;

namespace mcw {

// The three window buttons (minimize / maximize / close); hovering anywhere over the
// group reveals all their symbols. Shape and size follow the user's appearance prefs.
class WindowControls : public QWidget {
public:
    CircleButton *minBtn = nullptr;
    CircleButton *maxBtn = nullptr;
    CircleButton *closeBtn = nullptr;

    explicit WindowControls(QWidget *parent = nullptr);

    // Re-reads shape/size from SettingsManager and repaints the buttons.
    void applyPreferences();

protected:
    void enterEvent(QEnterEvent *) override;
    void leaveEvent(QEvent *) override;

private:
    void setHover(bool on);

    QHBoxLayout *m_layout = nullptr;
};

}  // namespace mcw
