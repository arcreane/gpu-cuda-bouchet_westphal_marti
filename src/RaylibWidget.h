#pragma once
#include <QWidget>
#include <raylib.h>

class RaylibWidget : public QWidget {
    Q_OBJECT
public:
    explicit RaylibWidget(QWidget* parent = nullptr);
    ~RaylibWidget();

protected:
	// Avoir rendu graphique Raylib au lieu de Qt
    void paintEvent(QPaintEvent* event) override;

    // Pour ne pas que Qt efface le fond (Raylib le fera)
    QPaintEngine* paintEngine() const override { return nullptr; }

private:
    void initRaylib();
    bool m_isInitialized = false;
};