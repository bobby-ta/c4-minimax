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

        if (row != -1) {
            this->result.moveValid = true;
            this->result.row = row;
            this->board[row][col] = this->player; //Put move down
            this->result.win = this->checkWin(row, col);
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

    bool checkWin(int row, int col) {
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