const ROWS = 6;
const COLS = 7;
let currentPlayer = 0;
const playerNames = new Map();
playerNames.set(0, "red"); //player
playerNames.set(1, "yellow"); //bot
let gameOver = false;

const gameBoard = document.getElementById('game-board');
const message = document.getElementById('message');
const restartBtn = document.getElementById('restart');

//WASM stuff
let makeMove, newGame;

Module.onRuntimeInitialized = () => {
    // Wrap the C++ functions for JS
    makeMove = Module.cwrap('make_move', 'string', ['number']);
    newGame = Module.cwrap('new_game', null, []);
    newGame(); //Initialise game
    createBoard(); //Initialise display
};
//end WASM stuff

function createBoard() {
    gameBoard.innerHTML = '';

    for (let c=0;c<COLS;c++) {
        const column = document.createElement("div");
        column.classList.add("column");
        column.dataset.col = c;
        column.addEventListener('click', handleCellClick);
        gameBoard.appendChild(column);
        for (let r = ROWS-1; r > -1; r--) {
            const cell = document.createElement("div");
            cell.dataset.row = r;
            cell.dataset.col = c;
            column.appendChild(cell);
            cell.classList.add("cell");
        } 
    }
}

function handleCellClick(e) {
    if (gameOver) return;
    const col = parseInt(e.target.dataset.col);
    processMove(col);
}

function processMove(col) {
    const res = JSON.parse(makeMove(col));
    if (!res.moveValid) {
        return;
    } else {
        //Mark player cell
        const playerCell = document.querySelector(`.cell[data-row="${res.playerRow}"][data-col="${col}"]`);
        playerCell.classList.add("red");

        if (res.botCol != -1) { //Bot made move
            const botCell = document.querySelector(`.cell[data-row="${res.botRow}"][data-col="${res.botCol}"]`);
            botCell.classList.add("yellow");
        }

        if (res.winner == 0) {
            showTerminalMsg(0);
            gameOver = true;
        } else if (res.winner == 1) {
            showTerminalMsg(1);
            gameOver = true;
        } else if (res.draw) {
            showTerminalMsg(-1);
            gameOver = true;
        }
    }
}


function showTerminalMsg(scenario) {
    if (scenario == -1) {
        message.textContent = "It's a draw!";
    } else if (scenario == 0) {
        message.textContent = "Red wins!";
        message.style = "color: #e74c3c";
    } else {
        message.textContent = "Yellow wins!";
        message.style = "color: #f1c40f";
    }
    message.classList.add("terminal");
}

function clearBoard() {
    const cells = document.querySelectorAll('.cell');
    for (let r = 0; r < ROWS; r++) {
        for (let c = 0; c < COLS; c++) {
            const idx = r * COLS + c;
            cells[idx].classList.remove('red', 'yellow');
        }
    }
}

function restartGame() {
    newGame();
    currentPlayer = 1;
    gameOver = false;
    clearBoard();
    message.classList.remove("terminal");
    let defaultColour = getComputedStyle(document.body).getPropertyValue("--main");
    message.style = `color: ${defaultColour}`;
}

restartBtn.addEventListener('click', restartGame);
