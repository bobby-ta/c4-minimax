#include <iostream>
#include <array>
#include <limits>
#include <algorithm>

constexpr int ROWS = 6;
constexpr int COLS = 7;
constexpr int DEPTH = 3;

/*
IMPORTANT!!!

REMEMBER ROW 5 IS THE TOP AND ROW 0 IS THE BOTTOM!!!
SO IT LOOKS UPSIDE-DOWN IN TEXT REPRESENTATION!!!! 

IMPORTANT!!!
*/
class Game {
private:
    std::array<std::array<int, COLS>, ROWS> board{};
    int player = 1; // player = -1 (X), bot = -1 (O)
    std::array<int, 2> lastMove{-1, -1};

public:
    Game() {
        for (int i=0;i<ROWS;i++) {
            for (int j=0;j<COLS;j++) {
                board[i][j] = 0;
            }
        }
    }

    void printBoard() const {
        std::cout << "\n";
        for (int r = ROWS - 1; r >= 0; r--) {
            std::cout << "|";
            for (int c = 0; c < COLS; c++) {
                char ch = (board[r][c] == 1) ? 'X' : (board[r][c] == -1) ? 'O' : '.';
                std::cout << ch << " ";
            }
            std::cout << "|\n";
        }
        std::cout << " 0 1 2 3 4 5 6\n";
    }

    bool moveValid(int col) const {
        return col >= 0 && col < COLS && board[ROWS-1][col] == 0;
    }

    int lowestEmptyRow(int col) const {
        for (int i = 0; i < ROWS; i++)
            if (board[i][col] == 0)
                return i;
        return -1;
    }

    void placeMove(int col) {
        int row = lowestEmptyRow(col);
        if (row != -1) {
            board[row][col] = player;
            lastMove = {row, col};
            player *= -1;
        }
    }

    void undoMove(int col) {
        for (int r = ROWS - 1; r >= 0; r--) {
            if (board[r][col] != 0) {
                board[r][col] = 0;
                player *= -1;
                break;
            }
        }
    }

    bool checkWin(int lastPlayer) {
        int row = this->lastMove[0];
        int col = this->lastMove[1];
        const int vectors[4][2] {
            {0, 1}, //hor
            {1, 0}, //vert
            {1, 1}, //diag up
            {1, -1} //diag down
        };
        
        for (auto& v:vectors) {
            int count = 1;

            //Check forwards
            for (int i=1;i<4;i++) {
                int r = row + i*v[0]; int c = col + i*v[1];
                if (r < 0 || r >= ROWS || c < 0 || c >= COLS || this->board[r][c] != lastPlayer) break;
                count++;
            }

            //Check backwards
            for (int i=1;i<4;i++) {
                int r = row - i*v[0]; int c = col - i*v[1];
                if (r < 0 || r >= ROWS || c < 0 || c >= COLS || this->board[r][c] != lastPlayer) break;
                count++;
            }

            if (count >= 4) return true;
        }
        return false;
    }

    bool boardFull() const {
        for (int j = 0; j < COLS; j++)
            if (board[ROWS-1][j] == 0) {
                return false;
            }
        return true;
    }

    int evalBoardState() const {
        //H2 from https://www.researchgate.net/publication/331552609_Research_on_Different_Heuristics_for_Minimax_Algorithm_Insight_from_Connect-4_Game
        int squareScores[6][7] = {
            {3, 4, 5, 6, 5, 4, 3},
            {4, 6, 8,10, 8, 6, 4},
            {5, 8,11,13,11, 8, 5},
            {5, 8,11,13,11, 8, 5},
            {4, 6, 8,10, 8, 6, 4},
            {3, 4, 5, 6, 5, 4, 3}
        };

        int score = 0;
        for (int r = 0; r < ROWS; r++)
            for (int c = 0; c < COLS; c++)
                score += board[r][c] * squareScores[r][c];
        return -score; //We want to maximise for bot (-1)
    }

    int minimax(int depth, int alpha, int beta, bool isMaximisingPlayer, int& bestCol) {
        if (lastMove[0] != -1 && checkWin(player * -1)) { //Don't checkWin for 1st move (dummy value will crash)
            return isMaximisingPlayer ? -1000000 + depth : 1000000 - depth; //Reward win speed/punish loss speed
        }
        if (boardFull()) { //Draw
            return 0;
        }
        if (depth == 0)
            return evalBoardState();

        if (isMaximisingPlayer) {
            int maxEval = -10000000;
            for (int c = 0; c < COLS; c++) {
                if (!moveValid(c)) continue;
                placeMove(c);
                int dummy;
                int eval = minimax(depth-1, alpha, beta, false, dummy);
                undoMove(c);
                if (eval > maxEval) {
                    maxEval = eval;
                    if (depth == DEPTH) bestCol = c;
                }
                alpha = std::max(alpha, eval);
                if (beta <= alpha) break;
            }
            return maxEval;
        } else {
            int minEval = 10000000;
            for (int c = 0; c < COLS; c++) {
                if (!moveValid(c)) continue;
                placeMove(c);
                int dummy;
                int eval = minimax(depth-1, alpha, beta, true, dummy);
                undoMove(c);
                if (eval < minEval) minEval = eval;
                beta = std::min(beta, eval);
                if (beta <= alpha) break;
            }
            return minEval;
        }
    }

    int getBotMove(int depth) {
        int bestCol = 0;
        minimax(depth, -10000000, 10000000, true, bestCol);
        return bestCol;
    }

    int getCurrentPlayer() const { return player; }
    std::array<int, 2> getLastMove() const { return lastMove; }
};

int main() {
    Game g;
    std::cout << "Player is X, bot is O \n";
    g.printBoard();

    bool gameOver = false;
    while (!gameOver) {
        if (g.getCurrentPlayer() == 1) {
            int col;

            while (true) { //Validate move
                std::cout << "Enter move 0-6: ";
                std::cin >> col;
                if (std::cin.fail() || !(g.moveValid(col))) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "Invalid move\n";
                } else {
                    break;
                }
            }

            g.placeMove(col);
            g.printBoard();
            auto last = g.getLastMove();

            if (g.checkWin(-g.getCurrentPlayer())) { //Negate because placing move switched player already
                std::cout << "You win!\n";
                gameOver = true;
            } else if (g.boardFull()) {
                std::cout << "Draw!\n";
                gameOver = true;
            }
        } else {
            std::cout << "Processing...\n";
            int botCol = g.getBotMove(DEPTH);
            g.placeMove(botCol);
            std::cout << "Bot chose column " << botCol << "\n";
            g.printBoard();
            if (g.checkWin(-g.getCurrentPlayer())) {
                std::cout << "Bot wins!\n";
                gameOver = true;
            } else if (g.boardFull()) {
                std::cout << "Draw!\n";
                gameOver = true;
            }
        }
    }
    return 0;
}