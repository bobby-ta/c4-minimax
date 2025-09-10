#include <stdio.h>
#include <array>
#include "json.hpp"
using json = nlohmann::json;

constexpr int ROWS = 6;
constexpr int COLS = 7;
constexpr int DEPTH = 6;


/*
IMPORTANT!!!

REMEMBER ROW 5 IS THE TOP AND ROW 0 IS THE BOTTOM!!!
SO IT LOOKS UPSIDE-DOWN IN TEXT REPRESENTATION!!!! 

IMPORTANT!!!
*/
class Game {
    private:
    std::array<std::array<int, COLS>, ROWS> board;
    int player = 1; //player = 1, bot = -1
    std::array<int, 2> lastMove;
    struct moveResult {
        bool moveValid;
        int row; //0-ROWS if successful, -1 if unsuccessful
        int col; //Redundant but makes sense to have
        bool win; //Did move win?
        bool boardFull; //Obvious

        json to_json() const {
            return json{
                {"moveValid", moveValid},
                {"row", row},
                {"col", col},
                {"win", win},
                {"boardFull", boardFull}
            };
        }
    };

    moveResult result;

    public:
    Game(const std::array<std::array<int, COLS>, ROWS>& board) : board(board) {}
    
    Game() {
        for (int i=0;i<ROWS;i++) {
            for (int j=0;j<COLS;j++) {
                board[i][j] = 0;
            }
        }
    }

    std::string makeMove(int col) { //For actual moves
        this->result.col = col;

        if (this->moveValid(col)) {
            this->result.moveValid = true;
            this->result.row = this->lowestEmpty(col);
            this->lastMove = {this->result.row, col}; //Will use for backtracking
            this->board[this->result.row][col] = this->player; //Put move down
            this->result.win = this->checkWin(this->player);
            this->result.boardFull = this->boardFull();

            this->player *= -1; //Switch player only once board state has been altered;
            
            /*if (player == -1) {
                this->makeMove(this->getBestMove(6));
            }*/
        } else {
            this->result.moveValid = false;
        }

        std::string jsonStr = this->result.to_json().dump();
        return jsonStr;
    }

    void placeMove(int col) { //For bot minimax eval only
        int row = this->lowestEmpty(col);
        if (row != -1) {
            this->board[row][col] = this->player;
            this->lastMove = {row, col};
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

    bool moveValid(int col) {
        return this->board[5][col] == 0  && col < COLS && col >= 0;
    }

    int lowestEmpty(int col) {
        for (int i=0;i<ROWS;i++) {
            if (board[i][col] == 0) {
                return i;
            }
        }
        return -1;
    }

    bool checkWin(int player) {
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
                if (r < 0 || r >= ROWS || c < 0 || c >= COLS || this->board[r][c] != player) break;
                count++;
            }

            //Check backwards
            for (int i=1;i<4;i++) {
                int r = row - i*v[0]; int c = col - i*v[1];
                if (r < 0 || r >= ROWS || c < 0 || c >= COLS || this->board[r][c] != player) break;
                count++;
            }

            if (count >= 4) return true;
        }
        return false;
    }

    bool boardFull() {
        for (int j=0;j<COLS;j++) {
            if (this->board[5][j] == 0) {
                return false;
            }
        }
        return true;
    }

    int evalBoardState() {
        //H2 from https://www.researchgate.net/publication/331552609_Research_on_Different_Heuristics_for_Minimax_Algorithm_Insight_from_Connect-4_Game
        int squareScores[6][7] = {
            {3, 4, 5, 6, 5, 4, 3},
            {4, 6, 8, 10, 8, 6, 4},
            {5, 8, 11, 13, 11, 8, 5},
            {5, 8, 11, 13, 11, 8, 5},
            {4, 6, 8, 10, 8, 6, 4},
            {3, 4, 5, 6, 5, 4, 3}
        };
        int score = 0;
        for (int c=0;c<COLS;c++) {
            for (int r=0;r<ROWS;r++) {
                if (this->board[r][c] == 0) {
                    break; //Don't waste time multiplying the 0s
                } else {
                    score += (this->board[r][c]) * (squareScores[r][c]);
                }
            }
        }

        //The bot is marked -1: bot advantage yields low score
        //But we want to maximise bot and minimise player
        //Therefore we switch signs to give high scores for bot advantage
        //So that we don't get muddled up with signs in the minimax function itself
        return -score;
    }

    int minimax(int depth, bool isMaximisingPlayer) { //Calculates minimax only; helper for getBestMove
        if (this->checkWin(this->player * -1)) {
            return -
        }
        
        if (depth == 0) {
            return this->evalBoardState();
        }
        if (depth == 0 || this->checkWin(this->player * -1) || this->boardFull()) { //Already cycled to next player in placeMove; undo
            return this->evalBoardState();
        }

        if (isMaximisingPlayer) { //bot's turn
            int maxEval = -INFINITY;
            for (int c=0;c<COLS;c++) {
                if (moveValid(c)) {
                    this->placeMove(c);
                    int eval = minimax(depth-1, false);
                    maxEval = std::max(eval, maxEval);
                    this->undoMove(c);
                }
            }
            return maxEval;
        } else {
            int minEval = INFINITY;
            for (int c=0;c<COLS;c++) {
                if (moveValid(c)) {
                    this->placeMove(c);
                    int eval = minimax(depth-1, true);
                    minEval = std::min(eval, minEval);
                    this->undoMove(c);
                }
            }
            return minEval;
        }
    }

    int getBotMove(int depth) { //ONLY run for the bot, so no need to check player turn
        int max_score = -INFINITY;
        int best = -1;
        for (int c=0;c<COLS;c++) {
            if (this->moveValid(c)) {
                this->placeMove(c);
                int score = this->minimax(depth, true);
                if (score > max_score) {
                    max_score = score;
                    best = c;
                }
                this->undoMove(c);
        }
        return best;
    }

};

Game* game = nullptr;

extern "C" {

    // Create a new game instance
    void new_game() {
        if (game) delete game;
        game = new Game();
    }

    // Make a move and return JSON result
    const char* make_move(int col) {
        static std::string result;
        result = game->makeMove(col); // makeMove should return a JSON string
        return result.c_str();
    }

    int get_bot_move(int depth) {
        return game->getBotMove(depth);
    }

}