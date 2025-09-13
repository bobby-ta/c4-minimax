# c4-minimax
Connect Four AI using minimax algorithm, with performance optimisation via alpha-beta pruning and bitboards. 

Instead of keeping the game logic in JS, I elected to write it in C++ and compile to WASM - which allows for deeper decision trees while maintaining barely-noticeable latency.
