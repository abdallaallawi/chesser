
let socket = null;

var Module = {
    onRuntimeInitialized: function() {
        socket = new WebSocket("ws://127.0.0.1:8080");
        socket.onopen = function() {
            let message = {
                type: "auth",
                body: {
                    ticket: localStorage.getItem('ticket') || localStorage.getItem('Ticket') 
                }
            };
            socket.send(JSON.stringify(message));
        }
        socket.onclose = function() {
            window.location.href = "http://localhost:3999/Main%20Page/index.html"
        }
        socket.onmessage = function(event) {
            let message = JSON.parse(event.data);
            if (message.type === "startgame") {
                let position = message.body.lastpos;
                let color = message.body.color;
                let turn = null;
                
                if (message.body.turn === "true") turn = true;
                if (message.body.turn === "false") turn = false;

                if (color === "white") white = true;
                if (color === "black") white = false;

                gameBoard = Module.Board.createWithParams(position, turn);
                createBoard(white);
                setupPieces(position);
            }
            if (message.type === "move") {
                let piece = message.body.piece;
                let from = message.body.from;
                let to = message.body.to;
                removeAffects();


                selectedPiece = gameBoard.selectPiece(Number(piece), Number(from));

                availableMoves = gameBoard.availableMoves(selectedPiece, selectedPiece.getSquare());
                

                if (availableMoves != null) {
                    let square = document.getElementById(selectedPiece.getSquare().toString());
                    const arr = [];
                    for (let i = 0; i < availableMoves.size(); i++) {
                        arr.push(availableMoves.get(i));
                    }
                    square.dataset.movement = JSON.stringify(arr.join(' ')).replaceAll('"', '');
                }
                movePiece(to, true);
                removeAffects();
            }
            if (message.type === "checkmate") {
                let winner = message.body.winner;
                console.log(message);
                if (winner === "true") {
                    alert("Checkmate! White wins!");
                }
                if (winner ==="false") {
                    alert("Checkmate! Black wins!");
                }

                let msg = {
                    type: "CLOSE"
                };
                socket.send(JSON.stringify(msg));
                setInterval(() => {
                    window.location.href = "http://localhost:3999/Main%20Page/index.html";
                }, 3000);
                
            }
            if (message.type === "draw") {
                alert("Draw!");

                let msg = {
                    type: "CLOSE"
                };
                socket.send(JSON.stringify(msg));

                window.location.href = "http://localhost:3999/Main%20Page/index.html";
            }
            
            if (message.type === "resign") {

            }

            if (message.type === "offer_draw") {

            }

        }
    }
};

document.getElementsByClassName('table')[0].addEventListener('contextmenu', function(e) {
    e.preventDefault();
});

let token;

let white = null;


let selectedPiece = null;

let availableMoves;


let dragging = false;
let startDragPosX = null;    //for drag and drop
let startDragPosY = null;



function searchClass (class_, element) {
    for (let i = 0; i < element.classList.length; i++) {
        if (element.classList[i] == class_) {
            return true;
        }
    }
    return false;
}


function createBoard(white) {
    let board = document.getElementsByClassName("chessboard")[0];


    board.addEventListener('mousemove', function(e) {
        if (!dragging) return;

        let currentPosX = e.pageX;
        let currentPosY = e.pageY;

        let offsetX = currentPosX - startDragPosX;
        let offsetY = currentPosY - startDragPosY;
        if (!selectedPiece) return;
        let piece = document.getElementById(selectedPiece.getSquare().toString()).querySelector('.piece');
        piece.style.zIndex = '9999';
        piece.style.transform = `translate(${offsetX}px, ${offsetY}px)`;
    });

    board.addEventListener('mouseup', function(e) {
        if (!dragging) return;
        if (availableMoves != null && selectedPiece != null) {
            const piece = document.getElementById(selectedPiece.getSquare().toString()).querySelector('.piece');

            piece.style.visibility = 'hidden'; 
            let dropSquare = document.elementFromPoint(e.clientX, e.clientY);
            piece.style.visibility = 'visible';
            dropSquare = dropSquare ? dropSquare.closest('.square') : null;
            if (dropSquare) dropSquare = dropSquare.closest('.square');
            else {
                piece.style.transform = "";
                dragging = false;
                return;
            }
            let found = false;
            for (let i = 0; i < availableMoves.size(); i++) {
                if (Number(dropSquare.id) == availableMoves.get(i)) {
                    piece.style.transform = "";
                    movePiece(dropSquare.id, false);
                    found = true;
                    break;
                }
            }
            if (!found) {
                piece.style.transform = "";
            }
            piece.style.zIndex = '1';
            startDragPosX = null;
            startDragPosY = null;
        }
        dragging = false;
    });

    let row = 0;
    let column = 0;

    if (white) {
        for (let i = 0; i < 64; i++) {

            let square = document.createElement('div');
            square.classList.add('square');

            row = 7 - Math.floor(i / 8);
            column = 7 - i / 8 - row;
            column = Math.abs(column / 0.125);

            square.id = row * 8 + column;

            // Color the squares
            if (row % 2 == 0) {
                if (Number(square.id) % 2 == 0) {
                    square.classList.add('dark');
                }
                else {
                    square.classList.add('light');
                }
            }
            else {
                if (Number(square.id) % 2 == 0) {
                    square.classList.add('light');
                }
                else {
                    square.classList.add('dark');
                }
            }
            square.addEventListener('click', () => movePiece(square.id, true), true);

            board.appendChild(square);
        }
    }
    else {
        for (let i = 0; i < 64; i++) {
            let square = document.createElement('div');
            square.classList.add('square');

            row = Math.floor(i / 8);
            column = i / 8 - row;
            column = Math.abs(column / 0.125);

            square.id = row * 8 + 7 - column;

            // Color the squares
            if (row % 2 == 0) {
                if (Number(square.id) % 2 == 0) {
                    square.classList.add('dark');
                }
                else {
                    square.classList.add('light');
                }
            }
            else {
                if (Number(square.id) % 2 == 0) {
                    square.classList.add('light');
                }
                else {
                    square.classList.add('dark');
                }
            }
            
            square.addEventListener('click', () => movePiece(square.id, true), true);


            board.appendChild(square);
        }
    }
}


function dragAndDrop (img, name, pos) {
    img.addEventListener('mousedown', function(e) {
        highlightPiece(name, pos);
        dragging = true;
        startDragPosX = e.pageX;
        startDragPosY = e.pageY;
    });
    
}

function setupPieces (position) {
    for (let i = 0; i < 64; i++) {
        let square = document.getElementById(i.toString());
        let img = document.createElement('img');
        img.classList.add('piece');
        img.ondragstart = function() { return false; }; 

        if (position[i] != '0') {

                if (position[i] == 'R') {
                    img.src = 'images/wRook.png';
                    img.alt = 'white rook';
                    if (white) { 
                        img.addEventListener('click', () => highlightPiece(82, i));
                        dragAndDrop(img, 82, i);
                    }
                }


                if (position[i] == 'N') {
                    img.src = 'images/wKnight.png';
                    img.alt = 'white knight';
                    if (white) { 
                        img.addEventListener('click', () => highlightPiece(78, i));
                        dragAndDrop(img, 78, i);
                    }
                }


                if (position[i] == 'B') {
                    img.src = 'images/wBishop.png';
                    img.alt = 'white bishop';
                    if (white) { 
                        img.addEventListener('click', () => highlightPiece(66, i));
                        dragAndDrop(img, 66, i);
                    }
                }


                if (position[i] == 'Q') {
                    img.src = 'images/wQueen.png';
                    img.alt = 'white queen';
                    if (white) {
                        img.addEventListener('click', () => highlightPiece(81, i));
                        dragAndDrop(img, 81, i);
                    }
                }


                if (position[i] == 'K') {
                    img.src = 'images/wKing.png';
                    img.alt = 'white king';
                    if (white) {
                        img.addEventListener('click', () => highlightPiece(75, i));
                        dragAndDrop(img, 75, i);
                    }
                }


                if (position[i] == 'P') {
                    img.src = 'images/wPawn.png';
                    img.alt = 'white pawn';
                    if (white) {
                        img.addEventListener('click', () => highlightPiece(80, i));
                        dragAndDrop(img, 80, i);
                    }
                }


                if (position[i] == 'r') {
                    img.src = 'images/bRook.png';
                    img.alt = 'black rook';
                    if (!white) {
                        img.addEventListener('click', () => highlightPiece(82, i));
                        dragAndDrop(img, 82, i);
                    }
                }


                if (position[i] == 'n') {
                    img.src = 'images/bKnight.png';
                    img.alt = 'black knight';
                    if (!white) {
                        img.addEventListener('click', () => highlightPiece(78, i));
                        dragAndDrop(img, 78, i);
                    }
                }
                    
                if (position[i] == 'b') {
                    img.src = 'images/bBishop.png';
                    img.alt = 'black bishop';
                    if (!white) {
                        img.addEventListener('click', () => highlightPiece(66, i));
                        dragAndDrop(img, 66, i);
                    }
                }


                if (position[i] == 'q') {
                    img.src = 'images/bQueen.png';
                    img.alt = 'black queen';
                    if (!white) {
                        img.addEventListener('click', () => highlightPiece(81, i));
                        dragAndDrop(img, 81, i);
                    }
                }


                if (position[i] == 'k') {
                    img.src = 'images/bKing.png';
                    img.alt = 'black king';
                    if (!white) {
                        img.addEventListener('click', () => highlightPiece(75, i));
                        dragAndDrop(img, 75, i);
                    }
                }


                if (position[i] == 'p') {
                    img.src = 'images/bPawn.png';
                    img.alt = 'black pawn';
                    if (!white) {
                        img.addEventListener('click', () => highlightPiece(80, i));
                        dragAndDrop(img, 80, i);
                    }
                }
                
                square.appendChild(img);
            }
    }
}


function highlightPiece (name, pos) {
    let square = document.getElementById(pos.toString());

    if (!searchClass('highlight', square)) {
        removeAffects();

        selectedPiece = gameBoard.selectPiece(name, pos);


        if (selectedPiece == null) return;
        
        if (selectedPiece.getColor() == white && gameBoard.getTurn() == white) {
            square.classList.add('highlight');
            if (!gameBoard.getCheck()) {
                availableMoves = gameBoard.availableMoves(selectedPiece, selectedPiece.getSquare());
            }
            else {
                availableMoves = gameBoard.availableMovesCheck(selectedPiece, selectedPiece.getSquare());
            }

            for (let i = 0; i < availableMoves.size(); i++) {
                let square1 = document.getElementById(availableMoves.get(i).toString());
                                    
                let circle = document.createElement('img');
                circle.src = 'images/circle.png';
                circle.alt = "";
                circle.classList.add('circle');
                circle.ondragstart = function() { return false; };
                square1.appendChild(circle);
                if (availableMoves != null) {
                    const arr = [];
                    for (let j = 0; j < availableMoves.size(); j++) {
                        arr.push(availableMoves.get(j));
                    }
                    square.dataset.movement = JSON.stringify(arr.join(' ')).replaceAll('"', '');
                }
            }
        }
    }   
}

function removeAffects () {         // Nullifies circles, highlights, selectedPiece and availableMoves
    for (let i = 0; i < 64; i++) {
        let square = document.getElementById(i.toString());
        if (searchClass('highlight', square)) square.classList.remove('highlight');
        if (square.querySelector('.circle')) square.querySelector('.circle').remove();
        delete square.dataset.movement;
    }
    selectedPiece = null;
    availableMoves = null;
}


function movePiece (to, animation) { //animation = takes boolean, move with animation or not
    if (selectedPiece != null && availableMoves != null) {
        let srcSquare = document.getElementById(selectedPiece.getSquare().toString());
        if (srcSquare.dataset.movement) {
            const arr = JSON.stringify(srcSquare.dataset.movement).split(' ');
            let found = false;
            for (let i = 0; i < arr.length; i++) {
                if (arr[i].replaceAll('"', '') === to) {
                    found = true;
                    break;
                }
            }
            if (found) {
                let fromSquareId = srcSquare.id; 
                let toSquareId = to;
                let pieceName = selectedPiece.getName(); 

                //castling
                if (gameBoard.getCastle() != -1) {
                    if (gameBoard.getCastle() == 6 && Number(to) == 6) {
                        let ogRookSquare = document.getElementById("7");
                        let clonedRook = ogRookSquare.querySelector('.piece').cloneNode(true);

                        if (gameBoard.getTurn() == white) {
                            clonedRook.addEventListener('click', () => highlightPiece(82, 5), true);
                            clonedRook.ondragstart = function() { return false; }; 
                            dragAndDrop(clonedRook, 82, 5);
                        } 

                        let newRookSquare = document.getElementById("5");
                
                
                        newRookSquare.appendChild(clonedRook);
                        animatePiece(ogRookSquare, newRookSquare, clonedRook);
                        ogRookSquare.innerHTML = "";
                    }

                    if (gameBoard.getCastle() == 2 && Number(to) == 2) {

                        let ogRookSquare = document.getElementById("0");
                        let clonedRook = ogRookSquare.querySelector('.piece').cloneNode(true);

                        if (gameBoard.getTurn() == white) {
                            clonedRook.addEventListener('click', () => highlightPiece(82, 3), true);
                            clonedRook.ondragstart = function() { return false; }; 
                            dragAndDrop(clonedRook, 82, 3);
                        } 

                        let newRookSquare = document.getElementById("3");
                
                
                        newRookSquare.appendChild(clonedRook);
                        animatePiece(ogRookSquare, newRookSquare, clonedRook);
                        ogRookSquare.innerHTML = "";
                    }

                    if (gameBoard.getCastle() == 58 && Number(to) == 58) {

                        let ogRookSquare = document.getElementById("56");
                        let clonedRook = ogRookSquare.querySelector('.piece').cloneNode(true);

                        if (gameBoard.getTurn() == white) {
                            clonedRook.addEventListener('click', () => highlightPiece(82, 59), true);
                            clonedRook.ondragstart = function() { return false; }; 
                            dragAndDrop(clonedRook, 82, 59);
                        } 

                        let newRookSquare = document.getElementById("59");
                
                
                        newRookSquare.appendChild(clonedRook);
                        animatePiece(ogRookSquare, newRookSquare, clonedRook);
                        ogRookSquare.innerHTML = "";
                    }

                    if (gameBoard.getCastle() == 62 && Number(to) == 62) {

                        let ogRookSquare = document.getElementById("63");
                        let clonedRook = ogRookSquare.querySelector('.piece').cloneNode(true);

                        if (gameBoard.getTurn() == white) {
                            clonedRook.addEventListener('click', () => highlightPiece(82, 61), true);
                            clonedRook.ondragstart = function() { return false; }; 
                            dragAndDrop(clonedRook, 82, 61);
                        } 

                        let newRookSquare = document.getElementById("61");
                
                
                        newRookSquare.appendChild(clonedRook);
                        animatePiece(ogRookSquare, newRookSquare, clonedRook);
                        ogRookSquare.innerHTML = "";
                    }
                }

                //enPassant
                if (gameBoard.getEnPassant() != -1 && Number(to) == gameBoard.getEnPassant()) {
                    if (!gameBoard.getTurn()) {
                        let enPassantSquare = gameBoard.getEnPassant();
                        document.getElementById((enPassantSquare + 8).toString()).querySelector('.piece').remove();
                    }
                    else {
                        let enPassantSquare = gameBoard.getEnPassant();
                        document.getElementById((enPassantSquare - 8).toString()).querySelector('.piece').remove();
                    }
                }

                
                //Normal piece movement
                if (!gameBoard.pushMove(Number(toSquareId), selectedPiece)) {
                    console.log('move was not pushed');
                    return;
                }

                let clonedImg = srcSquare.querySelector('.piece').cloneNode(true);

                if (gameBoard.getTurn() != white) {
                    clonedImg.addEventListener('click', () => highlightPiece(pieceName, Number(toSquareId)), true);
                    clonedImg.ondragstart = function() { return false; }; 
                    dragAndDrop(clonedImg, pieceName, Number(toSquareId));
                } 

                let toSquare = document.getElementById(toSquareId);
                if (toSquare.querySelector('.piece')) {
                    toSquare.querySelector('.piece').remove();
                }
                
                toSquare.appendChild(clonedImg);
                if (animation) animatePiece(srcSquare, toSquare, clonedImg);
                
                if (gameBoard.getTurn() != white) {
                    sendMove(pieceName.toString(), fromSquareId, toSquareId);
                }

                srcSquare.innerHTML = "";
                removeAffects();
            }
            else {
                if (Number(to) != selectedPiece.getSquare()) removeAffects();
            }
        }
        else {
            if (Number(to) != selectedPiece.getSquare()) removeAffects();
        }
    }
    else {
        if (selectedPiece == null || Number(to) != selectedPiece.getSquare()) removeAffects();
    }
}


function sendMove (piece, from, to) {
    if (socket.readyState == WebSocket.OPEN) {
        const packet = {
            type: "move",
            body: {
                piece: piece,
                from: from,
                to: to
            }
        }
        socket.send(JSON.stringify(packet));
    }
}

function animatePiece(srcSquare, toSquare, clonedImg) {

    const startCords = srcSquare.getBoundingClientRect();
    

    const endCords = toSquare.getBoundingClientRect();

    const deltaX = startCords.left - endCords.left;
    const deltaY = startCords.top - endCords.top;

    clonedImg.style.zIndex = '1000';


    const animation = clonedImg.animate([

        { transform: `translate(${deltaX}px, ${deltaY}px)` },

        { transform: `translate(0px, 0px)` }
    ], {
        duration: 200,
        easing: "ease-in-out"
    });

    animation.onfinish = () => {
        clonedImg.style.zIndex = '1';
    };
}


