#pragma once

#include <QWidget>

namespace mcw {

class IconButton;

// Left panel: "Chats" heading, chat list and the settings button.
class Sidebar : public QWidget {
public:
    explicit Sidebar(QWidget *parent = nullptr);

    IconButton *settingsButton() const { return m_settings; }
    void setCornerRadius(int radius);

private:
    IconButton *m_settings = nullptr;
};

}  // namespace mcw
