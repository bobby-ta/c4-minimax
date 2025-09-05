const ROWS = 6;
const COLS = 7;
let currentPlayer = 'red';
let gameOver = false;

const gameBoard = document.getElementById('game-board');
const message = document.getElementById('message');
const restartBtn = document.getElementById('restart');

function createBoard() {
    gameBoard.innerHTML = '';
    for (let r = ROWS-1; r > -1; r--) {
        for (let c = 0; c < COLS; c++) {
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
    const col = parseInt(e.target.dataset.col);
    
    //Call makeMove from the WASM
    //Parse the JSON return
    //If JSON(moveValid) then set cell[col][JSON(row)] to current player's colour
    //If JSON(win) then put "[current player] wins!" in DOM and freeze game
        //Else if JSON(boardFull) then put "Draw!" in DOM and freeze game
        //Else switch player and continue

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
