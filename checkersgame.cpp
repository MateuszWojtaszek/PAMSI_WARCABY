#include "checkersgame.h"
#include <iostream>

CheckersGame::CheckersGame(QObject *parent)
        : QObject(parent), playerTurn(true), selectedRow(-1), selectedCol(-1) {
    initializeBoard();
}

const Board& CheckersGame::getBoard() const {
    return board;
}

void CheckersGame::initializeBoard() {
    board.resize(BOARD_SIZE, vector<Piece>(BOARD_SIZE, EMPTY));
    // Initialize player's pieces
    for (int row = 0; row < 3; ++row) {
        for (int col = (row % 2); col < BOARD_SIZE; col += 2) {
            board[row][col] = PLAYER;
        }
    }
    // Initialize computer's pieces
    for (int row = BOARD_SIZE - 1; row >= BOARD_SIZE - 3; --row) {
        for (int col = (row % 2); col < BOARD_SIZE; col += 2) {
            board[row][col] = COMPUTER;
        }
    }
}

bool CheckersGame::isValidMove(int startRow, int startCol, int endRow, int endCol) const {
    if (endRow < 0 || endRow >= BOARD_SIZE || endCol < 0 || endCol >= BOARD_SIZE) return false;
    if (board[endRow][endCol] != EMPTY) return false;

    int rowDiff = endRow - startRow;
    int colDiff = abs(endCol - startCol);

    Piece player = board[startRow][startCol];

    // Normal move
    if (player == PLAYER && rowDiff == 1 && colDiff == 1) return true;
    if (player == COMPUTER && rowDiff == -1 && colDiff == 1) return true;

    // King movement (both directions)
    if ((player == PLAYER_KING || player == COMPUTER_KING) && abs(rowDiff) == 1 && colDiff == 1) return true;

    // Capture move
    if (colDiff == 2) {
        int middleRow = (startRow + endRow) / 2;
        int middleCol = (startCol + endCol) / 2;
        if (board[middleRow][middleCol] != EMPTY &&
            board[middleRow][middleCol] != player &&
            board[middleRow][middleCol] != (player == PLAYER || player == PLAYER_KING ? PLAYER : COMPUTER) &&
            board[middleRow][middleCol] != (player == PLAYER || player == PLAYER_KING ? PLAYER_KING : COMPUTER_KING)) {
            return true;
        }
    }

    return false;
}

vector<Move> CheckersGame::getValidMoves(Piece player) const {
    vector<Move> moves;
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            if (board[row][col] == player || board[row][col] == (player == PLAYER ? PLAYER_KING : COMPUTER_KING)) {
                // Regular piece movement (only forward)
                if (board[row][col] == PLAYER || board[row][col] == COMPUTER) {
                    int direction = (board[row][col] == PLAYER) ? 1 : -1;
                    for (int dcol = -1; dcol <= 1; dcol += 2) {
                        int newRow = row + direction;
                        int newCol = col + dcol;
                        if (isValidMove(row, col, newRow, newCol)) {
                            moves.push_back({row, col, newRow, newCol, false});
                        }
                        // Check for capturing move
                        newRow = row + 2 * direction;
                        newCol = col + 2 * dcol;
                        if (isValidMove(row, col, newRow, newCol)) {
                            moves.push_back({row, col, newRow, newCol, false});
                        }
                    }
                }
                // King movement (both directions)
                if (board[row][col] == PLAYER_KING || board[row][col] == COMPUTER_KING) {
                    for (int drow = -1; drow <= 1; drow += 2) {
                        for (int dcol = -1; dcol <= 1; dcol += 2) {
                            int newRow = row + drow;
                            int newCol = col + dcol;
                            if (isValidMove(row, col, newRow, newCol)) {
                                moves.push_back({row, col, newRow, newCol, false});
                            }
                            // Check for capturing move
                            newRow = row + 2 * drow;
                            newCol = col + 2 * dcol;
                            if (isValidMove(row, col, newRow, newCol)) {
                                moves.push_back({row, col, newRow, newCol, false});
                            }
                        }
                    }
                }
            }
        }
    }
    return moves;
}

void CheckersGame::applyMove(Move &move) {
    Piece piece = board[move.startRow][move.startCol];
    board[move.startRow][move.startCol] = EMPTY;
    board[move.endRow][move.endCol] = piece;

    // Handle capture (if any)
    if (abs(move.startRow - move.endRow) == 2) {
        int middleRow = (move.startRow + move.endRow) / 2;
        int middleCol = (move.startCol + move.endCol) / 2;
        board[middleRow][middleCol] = EMPTY;
    }

    // Handle promotion to king
    move.wasPromotion = false;
    if (piece == PLAYER && move.endRow == 7) {
        board[move.endRow][move.endCol] = PLAYER_KING;
        move.wasPromotion = true;
    } else if (piece == COMPUTER && move.endRow == 0) {
        board[move.endRow][move.endCol] = COMPUTER_KING;
        move.wasPromotion = true;
    }

    switchTurn();
}

void CheckersGame::undoMove(const Move &move, Piece capturedPiece) {
    Piece piece = board[move.endRow][move.endCol];
    board[move.endRow][move.endCol] = EMPTY;
    board[move.startRow][move.startCol] = piece;

    // Restore captured piece if there was a capture
    if (abs(move.startRow - move.endRow) == 2) {
        int middleRow = (move.startRow + move.endRow) / 2;
        int middleCol = (move.startCol + move.endCol) / 2;
        board[middleRow][middleCol] = capturedPiece;
    }

    // Undo promotion
    if (move.wasPromotion) {
        if (piece == PLAYER_KING && move.endRow == 7) {
            board[move.startRow][move.startCol] = PLAYER;
        } else if (piece == COMPUTER_KING && move.endRow == 0) {
            board[move.startRow][move.startCol] = COMPUTER;
        }
    }

    switchTurn();
}

int CheckersGame::evaluateBoard(const Board &board) const {
    int score = 0;
    int playerPieceCount = 0, computerPieceCount = 0;
    int playerKingCount = 0, computerKingCount = 0;
    int playerMobility = 0, computerMobility = 0;
    int playerSafety = 0, computerSafety = 0;
    int playerAdvancement = 0, computerAdvancement = 0;

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            Piece piece = board[row][col];

            switch (piece) {
                case PLAYER:
                    playerPieceCount++;
                    playerAdvancement += row; // More advanced pieces are more valuable
                    playerMobility += getValidMoves(PLAYER).size(); // Mobility of player's pieces
                    // Check for safety: not on the edge and not easily capturable
                    if (col > 0 && col < BOARD_SIZE - 1 && row > 0 && row < BOARD_SIZE - 1) {
                        playerSafety++;
                    }
                    break;

                case PLAYER_KING:
                    playerKingCount++;
                    playerMobility += getValidMoves(PLAYER_KING).size(); // Mobility of player's kings
                    break;

                case COMPUTER:
                    computerPieceCount++;
                    computerAdvancement += (BOARD_SIZE - 1 - row); // More advanced pieces are more valuable
                    computerMobility += getValidMoves(COMPUTER).size(); // Mobility of computer's pieces
                    // Check for safety: not on the edge and not easily capturable
                    if (col > 0 && col < BOARD_SIZE - 1 && row > 0 && row < BOARD_SIZE - 1) {
                        computerSafety++;
                    }
                    break;

                case COMPUTER_KING:
                    computerKingCount++;
                    computerMobility += getValidMoves(COMPUTER_KING).size(); // Mobility of computer's kings
                    break;

                default:
                    break;
            }
        }
    }

    // Piece value (weight kings more)
    score += 5 * (computerPieceCount - playerPieceCount);
    score += 10 * (computerKingCount - playerKingCount);

    // Mobility (encourage pieces with more moves)
    score += 1 * (computerMobility - playerMobility);

    // Safety (prefer pieces in safer positions)
    score += 2 * (computerSafety - playerSafety);

    // Advancement (reward pieces closer to promotion)
    score += 3 * (computerAdvancement - playerAdvancement);

    return score;
}

int CheckersGame::minimax(Board &board, int depth, int alpha, int beta, bool maximizingPlayer) {
    if (depth == 0 || getValidMoves(maximizingPlayer ? COMPUTER : PLAYER).empty()) {
        return evaluateBoard(board);
    }

    if (maximizingPlayer) {
        int maxEval = numeric_limits<int>::min();
        vector<Move> moves = getValidMoves(COMPUTER);
        for (auto &move : moves) {
            Piece capturedPiece = EMPTY;
            if (abs(move.startRow - move.endRow) == 2) {
                int middleRow = (move.startRow + move.endRow) / 2;
                int middleCol = (move.startCol + move.endCol) / 2;
                capturedPiece = board[middleRow][middleCol];
            }
            applyMove(move);
            int eval = minimax(board, depth - 1, alpha, beta, false);
            undoMove(move, capturedPiece);
            maxEval = max(maxEval, eval);
            alpha = max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    } else {
        int minEval = numeric_limits<int>::max();
        vector<Move> moves = getValidMoves(PLAYER);
        for (auto &move : moves) {
            Piece capturedPiece = EMPTY;
            if (abs(move.startRow - move.endRow) == 2) {
                int middleRow = (move.startRow + move.endRow) / 2;
                int middleCol = (move.startCol + move.endCol) / 2;
                capturedPiece = board[middleRow][middleCol];
            }
            applyMove(move);
            int eval = minimax(board, depth - 1, alpha, beta, true);
            undoMove(move, capturedPiece);
            minEval = min(minEval, eval);
            beta = min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

Move CheckersGame::getBestMove(Piece player) {
    int bestValue = player == COMPUTER ? numeric_limits<int>::min() : numeric_limits<int>::max();
    Move bestMove;
    vector<Move> moves = getValidMoves(player);
    for (auto &move : moves) {
        Piece capturedPiece = EMPTY;
        if (abs(move.startRow - move.endRow) == 2) {
            int middleRow = (move.startRow + move.endRow) / 2;
            int middleCol = (move.startCol + move.endCol) / 2;
            capturedPiece = board[middleRow][middleCol];
        }
        applyMove(move);
        int boardValue = minimax(board, 5, numeric_limits<int>::min(), numeric_limits<int>::max(), player == COMPUTER);
        undoMove(move, capturedPiece);
        if ((player == COMPUTER && boardValue > bestValue) ||
            (player == PLAYER && boardValue < bestValue)) {
            bestValue = boardValue;
            bestMove = move;
        }
    }
    return bestMove;
}

void CheckersGame::switchTurn() {
    playerTurn = !playerTurn;
}

bool CheckersGame::isPlayerTurn() const {
    return playerTurn;
}

bool CheckersGame::hasNoValidMoves(Piece player) const {
    return getValidMoves(player).empty();
}

bool CheckersGame::isGameOver() const {
    return hasNoValidMoves(PLAYER) && hasNoValidMoves(PLAYER_KING) ||
           hasNoValidMoves(COMPUTER) && hasNoValidMoves(COMPUTER_KING);
}

void CheckersGame::handleClick(int row, int col) {
    if (selectedRow == -1 && selectedCol == -1) {
        selectedRow = row;
        selectedCol = col;
    } else {
        if (isValidMove(selectedRow, selectedCol, row, col)) {
            Move move = {selectedRow, selectedCol, row, col, false};
            applyMove(move);
        }
        selectedRow = selectedCol = -1;
    }

    if (!playerTurn) {
        Move bestMove = getBestMove(COMPUTER);
        if (bestMove.startRow != -1) {
            applyMove(bestMove);
        }
    }
}
