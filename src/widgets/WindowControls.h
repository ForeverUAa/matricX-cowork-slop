#pragma once

#include <QEnterEvent>
#include <QWidget>

#include "widgets/CircleButton.h"

namespace mcw {

// The three window buttons (minimize / maximize / close); hovering anywhere over the
// group reveals all their symbols.
class WindowControls : public QWidget {
public:
    CircleButton *minBtn = nullptr;
    CircleButton *maxBtn = nullptr;
    CircleButton *closeBtn = nullptr;

    explicit WindowControls(QWidget *parent = nullptr);

protected:
    void enterEvent(QEnterEvent *) override;
    void leaveEvent(QEvent *) override;

private:
    void setHover(bool on);
};

}  // namespace mcw
