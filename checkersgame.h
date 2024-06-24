#ifndef CHECKERSGAME_H
#define CHECKERSGAME_H

#include <QObject>
#include <vector>

using namespace std;

// Enum to represent different pieces on the board
enum Piece {
    EMPTY, PLAYER, COMPUTER, PLAYER_KING, COMPUTER_KING
};

const int BOARD_SIZE = 8; // size of the board

// Struct to represent a move in the game
struct Move {
    int startRow, startCol, endRow, endCol; // Coordinates for the start and end of the move
    bool wasPromotion; // Flag to track if the move resulted in a promotion
};

typedef vector<vector<Piece>> Board; // Typedef for a 2D board of pieces

class CheckersGame : public QObject {
Q_OBJECT

public:
    /**
     * Constructor for the CheckersGame class.
     * Initializes the game board and sets the initial player turn.
     *
     * @param parent The parent QObject, default is nullptr.
     */
    explicit CheckersGame(QObject *parent = nullptr);

    /**
     * Gets the current state of the board.
     *
     * @return A const reference to the board.
     */
    const Board &getBoard() const;

    /**
     * Handles the click on the board at the specified row and column.
     * This function is used for processing player moves.
     *
     * @param row The row index of the clicked cell.
     * @param col The column index of the clicked cell.
     */
    void handleClick(int row, int col);

    /**
     * Checks if it's the player's turn.
     *
     * @return True if it's the player's turn, false otherwise.
     */
    bool isPlayerTurn() const;

    /**
     * Checks if the game is over.
     *
     * @return True if the game is over, false otherwise.
     */
    bool isGameOver() const;

    /**
     * Applies a move to the board. This includes updating the board state
     * and handling promotions if necessary.
     *
     * @param move The move to be applied.
     */
    void applyMove(Move &move);

    /**
     * Undoes a move on the board. This includes restoring any captured pieces
     * and handling demotions if necessary.
     *
     * @param move The move to be undone.
     * @param capturedPiece The piece that was captured during the move, if any.
     */
    void undoMove(const Move &move, Piece capturedPiece);

    /**
     * Gets the best move for the specified player using the minimax algorithm
     * with alpha-beta pruning.
     *
     * @param player The player for whom to get the best move.
     * @return The best move for the player.
     */
    Move getBestMove(Piece player);

    /**
    * Evaluates the board and returns a score indicating the board's
    * favorability for the AI.
    *
    * @param board The board to be evaluated.
    * @return An integer score representing the board's favorability.
    */
    int evaluateBoard(const Board &board) const;

private:
    Board board; // The game board
    bool playerTurn; // Flag to indicate if it's the player's turn
    int selectedRow, selectedCol; // Coordinates of the currently selected piece

    /**
     * Initializes the game board with the starting positions of the pieces.
     */
    void initializeBoard();

    /**
     * Checks if a move is valid according to the game rules.
     *
     * @param startRow The row index of the starting position.
     * @param startCol The column index of the starting position.
     * @param endRow The row index of the ending position.
     * @param endCol The column index of the ending position.
     * @return True if the move is valid, false otherwise.
     */
    bool isValidMove(int startRow, int startCol, int endRow, int endCol) const;

    /**
     * Gets all valid moves for the specified player.
     *
     * @param player The player whose valid moves are to be returned.
     * @return A vector of valid moves for the player.
     */
    vector<Move> getValidMoves(Piece player) const;


    /**
     * Switches the turn between the player and the computer.
     */
    void switchTurn();

    /**
     * Minimax algorithm with alpha-beta pruning to determine the best move.
     *
     * @param board The current board state.
     * @param depth The depth of the search.
     * @param alpha The alpha value for alpha-beta pruning.
     * @param beta The beta value for alpha-beta pruning.
     * @param maximizingPlayer True if maximizing player, false otherwise.
     * @return The best score for the current board state.
     */
    int minimax(Board &board, int depth, int alpha, int beta, bool maximizingPlayer);

    /**
     * Checks if the player has no valid moves left.
     *
     * @param player The player to check.
     * @return True if the player has no valid moves, false otherwise.
     */
    bool hasNoValidMoves(Piece player) const;
};

#endif // CHECKERSGAME_H
