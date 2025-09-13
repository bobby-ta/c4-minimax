//To my future self: i hope this finds you well. Debugging ts without copilot is hell how did i live for 17 years
//Bug: c5r0 (35) renders as c4r3 (31)???? c6r0 also. maybe 31 is the limit???? 
//Like it registers in lowestFree perfectly
//Also win detection is fucked 
//Why didn't I do this w internet fml
//Also minimax is deterministic how's my bot gained free will bruh
//Cba rn

#include <iostream>
#include <algorithm>
#include <map>
#include <string>
#include <cstdint>
#include "json.hpp"
using json = nlohmann::json;

constexpr int ROWS = 6;
constexpr int COLS = 7;
constexpr int DEPTH = 8;
constexpr uint64_t TOP = 0b1000000100000010000001000000100000010000001000000ULL; //Top guards for bitboard - 1 above each col of 0s

class Game {
private:
    //Bitboards and makeMove(), undoMove(), checkWin() and moveValid() adapted from
    //https://github.com/denkspuren/BitboardC4/blob/master/BitboardDesign.md
    /*
      6 13 20 27 34 41 48   55 62     Additional row
    +---------------------+ 
    | 5 12 19 26 33 40 47 | 54 61     top row
    | 4 11 18 25 32 39 46 | 53 60
    | 3 10 17 24 31 38 45 | 52 59
    | 2  9 16 23 30 37 44 | 51 58
    | 1  8 15 22 29 36 43 | 50 57
    | 0  7 14 21 28 35 42 | 49 56 63  bottom row
    +---------------------+
*/
    uint64_t bitboards[2] = {0, 0}; //Player first, bot second
    int lowestFree[COLS] = {0, 7, 14, 21, 28, 35, 42}; //Next cell to be filled for every column in order
    int counter = 0; //Player even, bot odd; counter & 1 is identical to counter % 2
    int moves[ROWS*COLS]; //Stack of moves (for undo)

public:
    int highestFilled(int col) {
        int highestRow = lowestFree[col] % 7;
        return (highestRow != 0) ? highestRow - 1 : -1; 
        //Should never be -1; always called on filled cols
    }
    void placeMove(int col) {
        //Create number where only the bit at the lowestFree position equals 1 (i.e. identify which cell to flip)
        //Will be XOR'd to insert into bitboard
        uint64_t move = 1ULL << lowestFree[col];

        lowestFree[col]++; //Now the cell above becomes lowestFree
        bitboards[counter & 1] ^= move;  //XOR in, i.e. turn the 0 at lowestFree to a 1
        moves[counter] = col;
        counter++;         
    }

    void undoMove() {
        //Get the column
        counter--;
        int col = moves[counter];

        lowestFree[col]--; //Set the cell (currently filled) to unfilled status

        uint64_t move = 1ULL << lowestFree[col]; //Create number/index like in placeMove
        bitboards[counter & 1] ^= move; //Flip cell at this index (XOR-ing turns 1 to 0)
    }

    bool checkWin(int player) {
        uint64_t bitboard = bitboards[player];
        int directions[4] = {1, 7, 8, 6}; //vert/hor/diag up/diag down
        for(int direction : directions) {
            //If 4 in a row then last cell in sequence will always equal 1
                //Because all the others will shift to it one by one
            //Guard cells very important here; will always equal 0 in original bitboard
                //So avoid false wins from overflow into next col
            if ((bitboard & (bitboard >> direction) &
            (bitboard >> (2 * direction)) & (bitboard >> (3 * direction))) != 0) {
                return true;
            }
        }
        return false;
    }

    bool moveValid(int col) {
        //"If I insert a bit is it gonna go to a row other than top?"
        return ((TOP & (1ULL << lowestFree[col])) == 0);
    }

    bool boardFull() {
        for (int c=0;c<COLS;c++) {
            if (moveValid(c)) return false;
        }
        return true;
    }

    void printBoard() const {
        std::cout << "\n";
        for (int r = ROWS - 1; r >= 0; r--) {
            std::cout << "|";
            //r + 7*c
            for (int c = 0; c < COLS; c++) {
                int cellIndex = r + 7*c;
                uint64_t mask = 1ULL << cellIndex;
                if ((bitboards[0] & mask) != 0) { //player
                    std::cout << " X";
                } else if ((bitboards[1] & mask) != 0) { //Bot placed it
                    std::cout << " O";
                } else {
                    std::cout << " .";
                }
            }
            std::cout << "|\n";
        }
        std::cout << "  0 1 2 3 4 5 6\n";
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
            for (int c = 0; c < COLS; c++) {
                int cellIndex = r + 7*c;
                uint64_t mask = 1ULL << cellIndex; //Put 1 only at cellIndex, all 0s otherwise
                if ((bitboards[0] & mask) != 0) { //Player placed it
                    score -= squareScores[r][c];
                } else if ((bitboards[1] & mask) != 0) { //Bot placed it
                    score += squareScores[r][c];
                }
            }
        return score;
    }

    int minimax(int depth, int alpha, int beta, bool isMaximisingPlayer, int& bestCol) {
        int player = counter&1;
        if (checkWin(player)) {
            return isMaximisingPlayer ? 1e6 + depth : -1e6 - depth;
        }
        if (checkWin(1 - player)) { //Check if previous player won
            return isMaximisingPlayer ? -1e6 - depth : 1e6 + depth; //Reward win speed/punish loss speed
        }
        if (boardFull()) { //Draw
            return 0;
        }
        if (depth == 0)
            return evalBoardState();

        if (isMaximisingPlayer) {
            int maxEval = -1e6;
            for (int c = 0; c < COLS; c++) {
                if (!moveValid(c)) continue;
                placeMove(c);
                int dummy;
                int eval = minimax(depth-1, alpha, beta, false, dummy);
                undoMove();
                if (eval > maxEval) {
                    maxEval = eval;
                    if (depth == DEPTH) bestCol = c;
                }
                alpha = std::max(alpha, eval);
                if (beta <= alpha) break;
            }
            return maxEval;
        } else {
            int minEval = 1e6;
            for (int c = 0; c < COLS; c++) {
                if (!moveValid(c)) continue;
                placeMove(c);
                int dummy;
                int eval = minimax(depth-1, alpha, beta, true, dummy);
                undoMove();
                if (eval < minEval) minEval = eval;
                beta = std::min(beta, eval);
                if (beta <= alpha) break;
            }
            return minEval;
        }
    }

    int getBotMove(int depth) {
        int bestCol = 0;
        minimax(depth, -1e6, 1e6, true, bestCol);
        return bestCol;
    }

    int getCurrentPlayer() const {
        return counter%2; 
    }
};

Game* game = nullptr;

const char* jsonify(std::map<std::string,int> outp) {
    json j = outp;
    static std::string result_str;
    result_str = j.dump();
    return result_str.c_str();
}

extern "C" {

    // Create a new game instance
    void new_game() {
        if (game) delete game;
        game = new Game();
    }

    const char* make_move(int col) {
        /*{
            [playerCol is already provided as arg]
            moveValid: 0/1,
            playerRow: 0-5,
            botCol: 0-6 (-1 if player move finishes game),
            botRow: 0-5 (-1 if player move finishes game), 
            winner: -1 none/0 player/1 bot,
            draw = 0/1

        }*/
        //Setup
        int winner = -1; bool draw = 0; int botCol = -1; int botRow = -1;

        //PLAYER TURN

        //If move invalid skip everything
        if (!game->moveValid(col)) {
            return jsonify({{"moveValid", false}});
        }

        game->placeMove(col);
        int playerRow = game->highestFilled(col);

        if (game->checkWin(0)) {
            winner = 0;
        } else if (game->boardFull()) {
            draw = 1;
        } else { //BOT TURN
            botCol = game->getBotMove(DEPTH);
            game->placeMove(botCol);
            botRow = game->highestFilled(botCol);
            if (game->checkWin(1)) {
                winner = 1;
            } else if (game->boardFull()) {
                draw = 1;
            } 
        }
        
        std::map<std::string, int> res = {
            {"moveValid", 1},
            {"playerRow", playerRow},
            {"botCol", botCol},
            {"botRow", botRow},
            {"winner", winner},
            {"draw", draw}
        };

        return jsonify(res);
        
    }

}