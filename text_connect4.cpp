#include <iostream>
using namespace std;

const int ROWS = 6;
const int COLS = 7;

void printBoard(int board[ROWS][COLS]) {
    cout << "\n";
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            char ch = '.';
            if (board[r][c] == 1) ch = 'X';
            else if (board[r][c] == 2) ch = 'O';
            cout << ch << " ";
        }
        cout << endl;
    }
    for (int c = 0; c < COLS; ++c) cout << c+1 << " ";
    cout << endl;
}

bool makeMove(int board[ROWS][COLS], int col, int player) {
    if (col < 0 || col >= COLS) return false;
    for (int r = ROWS-1; r >= 0; --r) {
        if (board[r][col] == 0) {
            board[r][col] = player;
            return true;
        }
    }
    return false;
}

bool checkWin(int board[ROWS][COLS], int player) {
    // Horizontal
    for (int r = 0; r < ROWS; ++r)
        for (int c = 0; c <= COLS-4; ++c)
            if (board[r][c]==player && board[r][c+1]==player && board[r][c+2]==player && board[r][c+3]==player)
                return true;
    // Vertical
    for (int c = 0; c < COLS; ++c) {
        for (int r = 0; r <= ROWS-4; ++r) {
            if (board[r][c]==player && board[r+1][c]==player && board[r+2][c]==player && board[r+3][c]==player)
                return true;
        }
    }
    // Diagonal
    for (int r = 0; r <= ROWS-4; ++r) {
        for (int c = 0; c <= COLS-4; ++c) {
            if (board[r][c]==player && board[r+1][c+1]==player && board[r+2][c+2]==player && board[r+3][c+3]==player)
                return true;
        }
    
    // Diagonal
    for (int r = 3; r < ROWS; ++r) {
        for (int c = 0; c <= COLS-4; ++c) {
            if (board[r][c]==player && board[r-1][c+1]==player && board[r-2][c+2]==player && board[r-3][c+3]==player)
                return true;
        }
    }
    return false;
}

bool isFull(int board[ROWS][COLS]) {
    for (int c = 0; c < COLS; ++c)
        if (board[0][c] == 0) return false;
    return true;
}

int main() {
    int board[ROWS][COLS] = {};
    int player = 1;
    while (true) {
        printBoard(board);
        cout << "Player " << (player==1 ? 'X' : 'O') << ", enter column (1-" << COLS << "): ";
        int col;
        cin >> col;
        if (!cin) break;
        if (!makeMove(board, col-1, player)) {
            cout << "Invalid move. Try again.\n";
            continue;
        }
        if (checkWin(board, player)) {
            printBoard(board);
            cout << "Player " << (player==1 ? 'X' : 'O') << " wins!\n";
            break;
        }
        if (isFull(board)) {
            printBoard(board);
            cout << "It's a draw!\n";
            break;
        }
        player = 3 - player;
    }
    return 0;
}
