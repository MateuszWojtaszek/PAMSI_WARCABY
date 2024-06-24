#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QComboBox>
#include "checkersgame.h"
#include "evalbar.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    /**
     * Constructor for the MainWindow class.
     * Sets up the UI and initializes the game and bot vs bot mode.
     *
     * @param parent The parent QWidget, default is nullptr.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * Destructor for the MainWindow class.
     * Cleans up allocated resources.
     */
    ~MainWindow();

private slots:
    /**
     * Slot to handle button clicks on the board.
     * This is connected to each button to manage player moves.
     *
     * @param row The row index of the clicked button.
     * @param col The column index of the clicked button.
     */
    void handleButtonClick(int row, int col);

    /**
     * Slot to handle moves during bot vs bot mode.
     * This is called periodically by the timer to make moves for the bots.
     */
    void botVsBotMove();

    /**
     * Slot to handle game mode selection change.
     * Updates the game mode based on the selection.
     */
    void handleModeChange();

private:
    QWidget *centralWidget;          ///< The central widget of the main window.
    QGridLayout *gridLayout;         ///< Layout to arrange buttons in a grid representing the board.
    CheckersGame *game;              ///< Instance of the CheckersGame class to manage the game logic.
    QPushButton *buttons[8][8];      ///< Array of buttons representing the cells on the board.
    QLabel *statusLabel;             ///< Label to display the current game status or turn information.
    QTimer *botTimer;                ///< Timer to handle moves in bot vs bot mode.
    bool botVsBotActive;             ///< Flag to indicate if bot vs bot mode is active.
    EvalBar *evalBar;                ///< Widget to display the evaluation of the board state.
    QComboBox *modeComboBox;         ///< Combo box to select the game mode.
    QPushButton *startButton;        ///< Button to start the game based on the selected mode.

    /**
     * Updates the visual representation of the board.
     * Refreshes the button texts to reflect the current state of the game board.
     */
    void updateBoard();

    /**
     * Starts the game based on the selected mode.
     * Initializes the game according to player vs bot or bot vs bot mode.
     */
    void startGame();

    /**
     * Updates the evaluation bar with the current board evaluation.
     */
    void updateEvaluationBar();
};

#endif // MAINWINDOW_H
