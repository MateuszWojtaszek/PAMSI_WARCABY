#ifndef EVALBAR_H
#define EVALBAR_H

#include <QWidget>

class EvalBar : public QWidget {
Q_OBJECT

public:
    /**
     * Constructor for the EvalBar class.
     *
     * @param parent The parent QWidget, default is nullptr.
     */
    explicit EvalBar(QWidget *parent = nullptr);

    /**
     * Sets the evaluation value to be displayed by the bar.
     *
     * @param value The evaluation value.
     */
    void setEvaluationValue(int value);

protected:
    /**
     * Paints the evaluation bar based on the evaluation value.
     *
     * @param event The paint event.
     */
    void paintEvent(QPaintEvent *event) override;

private:
    int evaluationValue; ///< The evaluation value for the board state.
};

#endif // EVALBAR_H
