const ROWS = 6;
const COLS = 7;
const board = [];
let currentPlayer = 'red';
let gameOver = false;

const gameBoard = document.getElementById('game-board');
const message = document.getElementById('message');
const restartBtn = document.getElementById('restart');

function createBoard() {
    gameBoard.innerHTML = '';
    for (let r = ROWS-1; r > -1; r--) {
        board[r] = [];
        for (let c = 0; c < COLS; c++) {
            board[r][c] = null;
            const cell = document.createElement('div');
            cell.classList.add('cell');
            cell.dataset.row = r;
            cell.dataset.col = c;
            cell.addEventListener('click', handleCellClick);
            gameBoard.appendChild(cell);
        }
    }
}

function handleCellClick(e) {
    if (gameOver) return;
    const col = parseInt(e.target.dataset.col);
    for (let r = ROWS-1; r > -1; r--) {
        if (!board[r][col]) {
            board[r][col] = currentPlayer;
            updateBoard();
            if (checkWin(r, col)) {
                message.textContent = `${currentPlayer.charAt(0).toUpperCase() + currentPlayer.slice(1)} wins!`;
                gameOver = true;
            } else if (isBoardFull()) {
                message.textContent = "It's a draw!";
                gameOver = true;
            } else {
                currentPlayer = currentPlayer === 'red' ? 'yellow' : 'red';
                message.textContent = `${currentPlayer.charAt(0).toUpperCase() + currentPlayer.slice(1)}'s turn`;
            }
            break;
        }
    }
}

function updateBoard() {
    const cells = document.querySelectorAll('.cell');
    for (let r = 0; r < ROWS; r++) {
        for (let c = 0; c < COLS; c++) {
            const idx = r * COLS + c;
            cells[idx].classList.remove('red', 'yellow');
            if (board[r][c]) {
                cells[idx].classList.add(board[r][c]);
            }
        }
    }
}

function checkWin(row, col) {
    return (
        checkDirection(row, col, 0, 1) || // horizontal
        checkDirection(row, col, 1, 0) || // vertical
        checkDirection(row, col, 1, 1) || // diagonal \
        checkDirection(row, col, 1, -1)   // diagonal /
    );
}

function checkDirection(row, col, dr, dc) {
    let count = 1;
    let r = row + dr, c = col + dc;
    while (inBounds(r, c) && board[r][c] === currentPlayer) {
        count++;
        r += dr;
        c += dc;
    }
    r = row - dr; c = col - dc;
    while (inBounds(r, c) && board[r][c] === currentPlayer) {
        count++;
        r -= dr;
        c -= dc;
    }
    return count >= 4;
}

function inBounds(r, c) {
    return r >= 0 && r < ROWS && c >= 0 && c < COLS;
}

function isBoardFull() {
    for (let c = 0; c < COLS; c++) {
        if (!board[0][c]) return false;
    }
    return true;
}

function restartGame() {
    for (let r = 0; r < ROWS; r++) {
        for (let c = 0; c < COLS; c++) {
            board[r][c] = null;
        }
    }
    currentPlayer = 'red';
    gameOver = false;
    updateBoard();
    message.textContent = "Red's turn";
}

restartBtn.addEventListener('click', restartGame);

// Initialize
document.addEventListener('DOMContentLoaded', () => {
    createBoard();
    message.textContent = "Red's turn";
});
