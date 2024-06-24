#include "evalbar.h"
#include <QPainter>
#include <QBrush>

EvalBar::EvalBar(QWidget *parent)
        : QWidget(parent), evaluationValue(0) {
    setFixedWidth(50); // Set the fixed width for the evaluation bar
}

void EvalBar::setEvaluationValue(int value) {
    evaluationValue = value;
    update(); // Request a repaint
}

void EvalBar::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);

    // Calculate the bar's height based on the evaluation value
    int height = (evaluationValue + 100) * this->height() / 200; // Scale between -100 and +100

    // Draw the background
    painter.fillRect(this->rect(), Qt::white);

    // Draw the evaluation bar
    if (evaluationValue > 0) {
        // Positive evaluation (favoring computer)
        painter.fillRect(0, this->height() - height, this->width(), height, Qt::red);
    } else {
        // Negative evaluation (favoring player)
        painter.fillRect(0, 0, this->width(), height, Qt::blue);
    }
}
