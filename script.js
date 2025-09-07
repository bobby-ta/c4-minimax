const ROWS = 6;
const COLS = 7;
let currentPlayer = 1;
const playerNames = new Map();
playerNames.set(1, "red"); //player
playerNames.set(-1, "yellow"); //bot
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
    message.textContent = "Red's turn";
};
//end WASM stuff

function createBoard() {
    gameBoard.innerHTML = '';
    /*for (let r = ROWS-1; r > -1; r--) {
        for (let c = 0; c < COLS; c++) {
            const cell = document.createElement('div');
            cell.classList.add('cell');
            cell.dataset.row = r;
            cell.dataset.col = c;
            cell.addEventListener('click', handleCellClick);
            gameBoard.appendChild(cell);
        }
    }*/

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
    const result = JSON.parse(makeMove(col));
    console.log(result);
    if (!result.moveValid) {
        console.log("invalid")
        return;
    } else {
        const cell = document.querySelector(`.cell[data-row="${result.row}"][data-col="${col}"]`);
        let pName = playerNames.get(currentPlayer);
        cell.classList.add(pName);
        if (result.win) {
            showTerminalMsg(`${pName.charAt(0).toUpperCase() + pName.slice(1)} wins!`);
            gameOver = true;
        } else if (result.boardFull) {
            showTerminalMsg("It's a draw!");
            gameOver = true;
        } else {
            currentPlayer *= -1;
            pName = playerNames.get(currentPlayer);
            message.textContent = `${pName.charAt(0).toUpperCase() + pName.slice(1)}'s turn`;
        }
    }
    
    //Call makeMove from the WASM
    //Parse the JSON return
    //If JSON(moveValid) then set cell[col][JSON(row)] to current player's colour
    //If JSON(win) then put "[current player] wins!" in DOM and freeze game
        //Else if JSON(boardFull) then put "Draw!" in DOM and freeze game
        //Else switch player and continue

}


messageColors = new Map();
messageColors.set(1, "#e74c3c");
messageColors.set(-1, "#f1c40f");
function showTerminalMsg(text) {
    message.textContent = text;
    message.style = `color: ${messageColors.get(currentPlayer)}`;
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
    message.textContent = "Red's turn";
}

restartBtn.addEventListener('click', restartGame);
