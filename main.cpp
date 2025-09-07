#include <stdio.h>
#include <array>
#include "json.hpp"
using json = nlohmann::json;

constexpr int ROWS = 6;
constexpr int COLS = 7;


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

    std::string makeMove(int col) {
        this->result.col = col;
        int row = this->whereTopRow(col);
        this->lastMove = {row, col};

        if (row != -1) {
            this->result.moveValid = true;
            this->result.row = row;
            this->board[row][col] = this->player; //Put move down
            this->result.win = this->checkWin();
            this->result.boardFull = this->boardFull();

            this->player *= -1; //Switch player only once board state has been altered;
        } else {
            this->result.moveValid = false;
        }

        std::string jsonStr = this->result.to_json().dump();
        return jsonStr;
    }

    int whereTopRow(int col) {
        for (int i=0;i<ROWS;i++) {
            if (board[i][col] == 0) {
                return i;
            }
        }
        return -1;
    }

    bool checkWin() {
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
                if (r < 0 || r >= ROWS || c < 0 || c >= COLS || this->board[r][c] != this->player) break;
                count++;
            }

            //Check backwards
            for (int i=1;i<4;i++) {
                int r = row - i*v[0]; int c = col - i*v[1];
                if (r < 0 || r >= ROWS || c < 0 || c >= COLS || this->board[r][c] != this->player) break;
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

        //Remember the bot is marked -1: this means bot advantage yields low score
        //But we always want to maximise for the bot and minimise for the player
        //Therefore we switch signs to make make eval more intuitive - high scores for bot advantage
        //So that we don't get muddled up with min and max and minuses in the minimax function itself
        return -score;
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

}