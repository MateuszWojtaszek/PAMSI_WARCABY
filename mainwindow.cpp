#include "mainwindow.h"
#include <QMessageBox>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), game(new CheckersGame(this)), botVsBotActive(false) {
    // Initialize the central widget and grid layout
    centralWidget = new QWidget(this);
    gridLayout = new QGridLayout(centralWidget);

    // Create and add a status label to the layout
    statusLabel = new QLabel("Select game mode and click Start", this);
    gridLayout->addWidget(statusLabel, 8, 0, 1, 8);

    // Create buttons for each cell on the board and add them to the layout
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            buttons[row][col] = new QPushButton(this);
            buttons[row][col]->setFixedSize(50, 50);
            gridLayout->addWidget(buttons[row][col], row, col);
            // Connect button clicks to handleButtonClick slot
            connect(buttons[row][col], &QPushButton::clicked, [=]() {
                handleButtonClick(row, col);
            });
        }
    }

    // Create the evaluation bar and add it to the layout
    evalBar = new EvalBar(this);
    gridLayout->addWidget(evalBar, 0, 9, 8, 1); // Adjust position as needed

    // Create a combo box for selecting the game mode
    modeComboBox = new QComboBox(this);
    modeComboBox->addItem("Player vs Bot");
    modeComboBox->addItem("Bot vs Bot");
    gridLayout->addWidget(modeComboBox, 9, 0, 1, 4);

    // Create a button to start the game
    startButton = new QPushButton("Start Game", this);
    gridLayout->addWidget(startButton, 9, 4, 1, 4);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::handleModeChange);

    // Set the layout as the central widget of the main window
    setCentralWidget(centralWidget);
    updateBoard(); // Initial board update
    updateEvaluationBar(); // Initial evaluation update

    // Initialize the timer for bot vs bot moves
    botTimer = new QTimer(this);
    connect(botTimer, &QTimer::timeout, this, &MainWindow::botVsBotMove);
}

MainWindow::~MainWindow() {
    delete game;     // Clean up game instance
    delete botTimer; // Clean up timer instance
    delete evalBar;  // Clean up evaluation bar instance
}

void MainWindow::handleButtonClick(int row, int col) {
    // Ignore clicks during bot vs bot mode
    if (botVsBotActive) return;

    // Handle player move
    game->handleClick(row, col);
    updateBoard();
    updateEvaluationBar(); // Update evaluation after move

    // If the game mode is Player vs Bot, let the bot make a move
    if (!game->isPlayerTurn() && !game->isGameOver()) {
        Move bestMove = game->getBestMove(COMPUTER);
        if (bestMove.startRow != -1) {
            game->applyMove(bestMove);
            updateBoard();
            updateEvaluationBar();
        }
    }
}

void MainWindow::botVsBotMove() {
    // If the game is not over, make a move for the current bot
    if (!game->isGameOver()) {
        Move bestMove = game->getBestMove(game->isPlayerTurn() ? PLAYER : COMPUTER);
        if (bestMove.startRow != -1) {
            game->applyMove(bestMove);
            updateBoard();
            updateEvaluationBar(); // Update evaluation after move
        } else {
            botTimer->stop();
        }
    } else {
        // Stop the timer and show a message when the game is over
        botTimer->stop();
        QMessageBox::information(this, "Game Over", "The game has ended.");
    }
}

void MainWindow::updateBoard() {
    // Get the current state of the board from the game instance
    auto board = game->getBoard();

    // Update each button to reflect the state of the board
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            QString text;
            switch (board[row][col]) {
                case EMPTY: text = ""; break;
                case PLAYER: text = "P"; break;
                case COMPUTER: text = "C"; break;
                case PLAYER_KING: text = "K"; break;
                case COMPUTER_KING: text = "Q"; break;
            }
            buttons[row][col]->setText(text);
        }
    }

    // Update the status label to show the current turn or game over status
    statusLabel->setText(game->isGameOver() ? "Game Over" : (game->isPlayerTurn() ? "Player's Turn" : "Computer's Turn"));
}

void MainWindow::updateEvaluationBar() {
    // Evaluate the current board state and update the evaluation bar
    int evaluationValue = game->evaluateBoard(game->getBoard());
    evalBar->setEvaluationValue(evaluationValue);
}

void MainWindow::handleModeChange() {
    // Stop any ongoing bot vs bot game
    botTimer->stop();

    // Reset the game board
    delete game;
    game = new CheckersGame(this);
    updateBoard();
    updateEvaluationBar();

    // Start the game based on the selected mode
    startGame();
}

void MainWindow::startGame() {
    // Get the selected mode from the combo box
    QString selectedMode = modeComboBox->currentText();

    if (selectedMode == "Bot vs Bot") {
        botVsBotActive = true;
        botTimer->start(1000); // Set the interval to 1 second
    } else {
        botVsBotActive = false;
        statusLabel->setText("Player's Turn");
    }
}
