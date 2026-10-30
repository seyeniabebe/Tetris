#include <iostream>
#include <raylib.h>

using namespace std;

enum Block { BarBlock, BoxBlock, TBlock, LBlock, JBlock, ZBlock, SBlock };
enum Orientation { Up, Right, Down, Left };

const int BOARD_WIDTH = 300;
const int BOARD_HEIGHT = 600;
const int ROWS = 20;
const int COLS = 10;
const int CELL_WIDTH = BOARD_WIDTH / COLS;
const int CELL_HEIGHT = BOARD_HEIGHT / ROWS;

const int INFO_AREA_WIDTH = 250;

const Color WINDOW_BG_COLOR = WHITE;
const Color GRID_LINE_COLOR = LIGHTGRAY;
const Color LOCKED_CELL_COLOR = DARKGRAY;

int score = 0;
bool gameEnded = false;

struct ActiveBlock { // Holds the state of the currently falling piece.
  Block block;
  Orientation orientation;
  int x;
  int y;
  Color color;
};

int cellInfo[ROWS][COLS];

void init();
void initCells();

void spawnBlock(ActiveBlock &activeBlock);
int findMiddle(Block block, Orientation Orientation);

Block chooseRandomBlock();
Orientation chooseRandomOrientation();

void drawGrid();

void playGame(ActiveBlock &activeBlock);

void drawActiveBlock(ActiveBlock activeBlock);
void drawBarBlock(ActiveBlock activeBlock);
void drawBoxBlock(ActiveBlock activeBlock);
void drawTBlock(ActiveBlock activeBlock);
void drawLBlock(ActiveBlock activeBlock);
void drawJBlock(ActiveBlock activeBlock);
void drawZBlock(ActiveBlock activeBlock);
void drawSBlock(ActiveBlock activeBlock);

bool canBlockGoDown(ActiveBlock activeBlock);
bool canBarGoDown(ActiveBlock activeBlock);
bool canBoxGoDown(ActiveBlock activeBlock);
bool canTGoDown(ActiveBlock activeBlock);
bool canLGoDown(ActiveBlock activeBlock);
bool canJGoDown(ActiveBlock activeBlock);
bool canZGoDown(ActiveBlock activeBlock);
bool canSGoDown(ActiveBlock activeBlock);

void lockActiveBlock(ActiveBlock activeBlock);
void lockBarBlock(ActiveBlock activeBlock);
void lockBoxBlock(ActiveBlock activeBlock);
void lockTBlock(ActiveBlock activeBlock);
void lockLBlock(ActiveBlock activeBlock);
void lockJBlock(ActiveBlock activeBlock);
void lockZBlock(ActiveBlock activeBlock);
void lockSBlock(ActiveBlock activeBlock);

void drawLockedCells(); // draws grey squares when the cells lock.

bool canBlockGoLeft(ActiveBlock activeBlock);
bool canBarGoLeft(ActiveBlock activeBlock);
bool canBoxGoLeft(ActiveBlock activeBlock);
bool canTGoLeft(ActiveBlock activeBlock);
bool canLGoLeft(ActiveBlock activeBlock);
bool canJGoLeft(ActiveBlock activeBlock);
bool canZGoLeft(ActiveBlock activeBlock);
bool canSGoLeft(ActiveBlock activeBlock);

bool canBlockGoRight(ActiveBlock activeBlock);
bool canBarGoRight(ActiveBlock activeBlock);
bool canBoxGoRight(ActiveBlock activeBlock);
bool canTGoRight(ActiveBlock activeBlock);
bool canLGoRight(ActiveBlock activeBlock);
bool canJGoRight(ActiveBlock activeBlock);
bool canZGoRight(ActiveBlock activeBlock);
bool canSGoRight(ActiveBlock activeBlock);

bool canBlockRotate(ActiveBlock activeBlock);
bool canBarRotate(ActiveBlock activeBlock);
bool canBoxRotate(ActiveBlock activeBlock);
bool canTRotate(ActiveBlock activeBlock);
bool canLRotate(ActiveBlock activeBlock);
bool canJRotate(ActiveBlock activeBlock);
bool canZRotate(ActiveBlock activeBlock);
bool canSRotate(ActiveBlock activeBlock);

void rotateBlock(ActiveBlock &activeBlock);
int clearFullLines();
bool isGameOver();
void drawHud();


int main() { // The game's loop.
  init();
  initCells();
  ActiveBlock activeBlock;
  spawnBlock(activeBlock); // Creates the first falling piece.
  while (!WindowShouldClose()) {
    BeginDrawing();
      ClearBackground(WINDOW_BG_COLOR);
      drawGrid();
      drawHud();
      playGame(activeBlock);
    EndDrawing();
  }
  CloseWindow();
  return 0;
}

void init() {
  InitWindow(BOARD_WIDTH + INFO_AREA_WIDTH, BOARD_HEIGHT, "TETRIS");
}

void initCells() { // Saves spaces for each cell.
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      cellInfo[row][col] =
          0; // Empty at the start. 1 means the cell is occupied, 0 is empty.
    }
  }
}

void spawnBlock(ActiveBlock &activeBlock) {
  activeBlock.block = chooseRandomBlock();
  activeBlock.orientation = chooseRandomOrientation();
  activeBlock.x = findMiddle(activeBlock.block, activeBlock.orientation);
  activeBlock.y = 0;
  activeBlock.color = GetRandomValue(0, 1) == 0 ? RED : MAGENTA;
}

int findMiddle(Block block, Orientation orientation) {
  switch (block) {
  case BarBlock:
    if (orientation == Up || orientation == Down) {
      return COLS / 2;
    } else {
      return (COLS / 2) - 2;
    }
    break;
  case BoxBlock:
    return (COLS / 2) - 1;
    break;
  case TBlock:
    if (orientation == Left || orientation == Right) {
      return (COLS / 2);
    } else {
      return (COLS / 2) - 1;
    }
    break;
  case LBlock:
    if (orientation == Up || orientation == Left) {
      return (COLS / 2);
    } else {
      return (COLS / 2) - 1;
    }
    break;
  case JBlock:
    if (orientation == Up || orientation == Right) {
      return (COLS / 2);
    } else {
      return (COLS / 2) - 1;
    }
    break;
  case ZBlock:
    if (orientation == Up || orientation == Down) {
      return (COLS / 2) - 1;
    } else {
      return (COLS / 2);
    }
    break;
  case SBlock:
    if (orientation == Up || orientation == Down) {
      return (COLS / 2) - 1;
    } else {
      return (COLS / 2);
    }
    break;
  }
  return false;// i guess
}

Block chooseRandomBlock() {
  return Block(GetRandomValue(
      BarBlock, SBlock)); // Randomly picks blocks. min= bar, max= s.
}

Orientation chooseRandomOrientation() {
  return Orientation(GetRandomValue(
      Up, Left)); // From 0 to 3. endemalet new. syntax is (min,max)
}

void drawGrid() {
  int verticalLineStart = 0;
  int horizontalLineStart = 0;

  for (int row = 0; row <= ROWS; row++) {
    DrawLine(horizontalLineStart, row * CELL_HEIGHT, BOARD_WIDTH,
             row * CELL_HEIGHT, GRID_LINE_COLOR);
  }

  for (int col = 0; col <= COLS; col++) {
    DrawLine(col * CELL_WIDTH, verticalLineStart, col * CELL_WIDTH,
             BOARD_HEIGHT, GRID_LINE_COLOR);
  }
}

void playGame(ActiveBlock &activeBlock) {
  if (gameEnded) {
    drawLockedCells();
    DrawText("GAME OVER", BOARD_WIDTH + 20, 120, 28, RED);
    return;
  }

  drawActiveBlock(activeBlock);
  drawLockedCells();
  float gameSpeed = 0.2;
  static float fallDelay = 0;
  fallDelay += GetFrameTime();

  if (fallDelay > gameSpeed) {
    if (canBlockGoDown(activeBlock)) {
      activeBlock.y++;
      fallDelay = 0;
    } else {
      lockActiveBlock(activeBlock);
      score += clearFullLines();
      if (isGameOver()) {
        gameEnded = true;
        return;
      }
      spawnBlock(activeBlock);
    }
  }

  if (IsKeyPressed(KEY_LEFT)) {
    if (canBlockGoLeft(activeBlock))
      activeBlock.x--;
  }

  if (IsKeyPressed(KEY_RIGHT)) {
    if (canBlockGoRight(activeBlock))
      activeBlock.x++;
  }

  if (IsKeyPressed(KEY_UP)) {
    if (canBlockRotate(activeBlock)) {
      rotateBlock(activeBlock);
    }
  }

  if (IsKeyPressed(KEY_SPACE)) { // Hard drop.
    while (canBlockGoDown(activeBlock)) {
      activeBlock.y++; // keep moving down as long as there's space
    }
    lockActiveBlock(activeBlock); // lock it once it can't go further
    score += clearFullLines();
    if (isGameOver()) {
      gameEnded = true;
      return;
    }
    spawnBlock(activeBlock); // spawn the next piece
    fallDelay = 0;                // reset the gravity timer
  }
}

void drawActiveBlock(ActiveBlock activeBlock) {
  switch (activeBlock.block) {
  case BarBlock:
    drawBarBlock(activeBlock);
    break;
  case BoxBlock:
    drawBoxBlock(activeBlock);
    break;
  case TBlock:
    drawTBlock(activeBlock);
    break;
  case LBlock:
    drawLBlock(activeBlock);
    break;
  case JBlock:
    drawJBlock(activeBlock);
    break;
  case ZBlock:
    drawZBlock(activeBlock);
    break;
  case SBlock:
    drawSBlock(activeBlock);
    break;
  }
}

void drawTBlock(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    DrawRectangle(activeBlock.x * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 2) * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    break;
  case Right:
    DrawRectangle(activeBlock.x * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle(activeBlock.x * CELL_WIDTH, (activeBlock.y + 1) * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    DrawRectangle(activeBlock.x * CELL_WIDTH, (activeBlock.y + 2) * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    break;
  case Down:

    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle(activeBlock.x * CELL_WIDTH, (activeBlock.y + 1) * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    DrawRectangle((activeBlock.x + 2) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    break;
  case Left:
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle(activeBlock.x * CELL_WIDTH, (activeBlock.y + 1) * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 2) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    break;
  }
}

void drawLBlock(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    DrawRectangle(activeBlock.x * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle(activeBlock.x * CELL_WIDTH, (activeBlock.y + 1) * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle(activeBlock.x * CELL_WIDTH, (activeBlock.y + 2) * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 2) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    break;
  case Right: 
    DrawRectangle(activeBlock.x * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 2) * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle(activeBlock.x * CELL_WIDTH, (activeBlock.y + 1) * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    break;
  case Down: 
    DrawRectangle(activeBlock.x * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 2) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    break;
  case Left: 
    DrawRectangle((activeBlock.x + 2) * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle(activeBlock.x * CELL_WIDTH, (activeBlock.y + 1) * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    DrawRectangle((activeBlock.x + 2) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    break;
  }
}
void drawJBlock(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    DrawRectangle(activeBlock.x * CELL_WIDTH, (activeBlock.y + 2) * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 2) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    break;
  case Right: 
    DrawRectangle(activeBlock.x * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle(activeBlock.x * CELL_WIDTH, (activeBlock.y + 1) * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    DrawRectangle((activeBlock.x + 2) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    break;
  case Down: 
    DrawRectangle(activeBlock.x * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle(activeBlock.x * CELL_WIDTH, (activeBlock.y + 1) * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle(activeBlock.x * CELL_WIDTH, (activeBlock.y + 2) * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    break;
  case Left: 
    DrawRectangle(activeBlock.x * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 2) * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 2) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    break;
  }
}
void drawZBlock(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
  case Down:
    DrawRectangle(activeBlock.x * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    DrawRectangle((activeBlock.x + 2) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    break;
  case Right:
  case Left:

    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle(activeBlock.x * CELL_WIDTH, (activeBlock.y + 1) * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    DrawRectangle(activeBlock.x * CELL_WIDTH, (activeBlock.y + 2) * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    break;
  }
}
void drawSBlock(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
  case Down:
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 2) * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle(activeBlock.x * CELL_WIDTH, (activeBlock.y + 1) * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    break;
  case Right:
  case Left:
    DrawRectangle(activeBlock.x * CELL_WIDTH, activeBlock.y * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle(activeBlock.x * CELL_WIDTH, (activeBlock.y + 1) * CELL_HEIGHT,
                  CELL_WIDTH, CELL_HEIGHT, activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 1) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    DrawRectangle((activeBlock.x + 1) * CELL_WIDTH,
                  (activeBlock.y + 2) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                  activeBlock.color);
    break;
  }
}

void drawBarBlock(ActiveBlock activeBlock) {
  int barCells = 4;

  if (activeBlock.orientation == Up || activeBlock.orientation == Down) {
    for (int y = 0; y < barCells; y++) {
      DrawRectangle((activeBlock.x) * CELL_WIDTH,
                    (activeBlock.y + y) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                    activeBlock.color);
    }
  } else {
    for (int x = 0; x < barCells; x++) {
      DrawRectangle((activeBlock.x + x) * CELL_WIDTH,
                    (activeBlock.y) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                    activeBlock.color);
    }
  }
}

void drawBoxBlock(ActiveBlock activeBlock) {
  int boxcells = 2;
  for (int y = 0; y < boxcells; y++) {
    for (int x = 0; x < boxcells; x++) {
      DrawRectangle((activeBlock.x + x) * CELL_WIDTH,
                    (activeBlock.y + y) * CELL_HEIGHT, CELL_WIDTH, CELL_HEIGHT,
                    activeBlock.color);
    }
  }
}

bool canBlockGoDown(
    ActiveBlock activeBlock) { // checks if the row is free and if it is not
                               // occupied by a locked cell.
  switch (activeBlock.block) {
  case BarBlock:
    return canBarGoDown(activeBlock);
  case BoxBlock:
    return canBoxGoDown(activeBlock);
  case TBlock:
    return canTGoDown(activeBlock);
  case LBlock:
    return canLGoDown(activeBlock);
  case JBlock:
    return canJGoDown(activeBlock);
  case ZBlock:
    return canZGoDown(activeBlock);
  case SBlock:
    return canSGoDown(activeBlock);
  }
  return false;
}

bool canBoxGoDown(ActiveBlock activeBlock) {
  if (activeBlock.y + 2 >= ROWS)
    return false;
  if (cellInfo[activeBlock.y + 2][activeBlock.x] == 1)
    return false;
  if (cellInfo[activeBlock.y + 2][activeBlock.x + 1] == 1)
    return false;
  return true;
}

bool canBarGoDown(ActiveBlock activeBlock) {
  int barCells = 4;
  if (activeBlock.orientation == Up || activeBlock.orientation == Down) {
    if (activeBlock.y + barCells >= ROWS)
      return false;
    if (cellInfo[activeBlock.y + barCells][activeBlock.x] == 1)
      return false;
  } else {
    if (activeBlock.y + 1 >= ROWS)
      return false;
    for (int x = 0; x < barCells; x++) {
      if (cellInfo[activeBlock.y + 1][activeBlock.x + x] == 1)
        return false;
    }
  }
  return true;
}

bool canTGoDown(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    // we'll just check the cells on the bottom. i.e. y+1 and x+1
    if (activeBlock.y + 2 >=
        ROWS)       // would going down put the block off the board?
      return false; // we would if the cells below the block are occupied. That
                    // is why the 'y's are below the block.
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 2] == 1)
      return false;
    return true;
  case Right:
    if (activeBlock.y + 3 >= ROWS)
      return false;
    if (cellInfo[activeBlock.y + 3][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 1] == 1)
      return false;
    return true;
  case Down:
    if (activeBlock.y + 2 >= ROWS)
      return false;
    for (int x = 0; x < 3; x++) {
      if (cellInfo[activeBlock.y + 2][activeBlock.x + x] == 1)
        return false;
    }
    return true; // not exactly clear on this one.

  case Left:
    if (activeBlock.y + 3 >= ROWS)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 3][activeBlock.x + 1] == 1)
      return false;
    return true;
  }
  return true;
}

bool canLGoDown(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    if (activeBlock.y + 3 >= ROWS)
      return false;
    if (cellInfo[activeBlock.y + 3][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 3][activeBlock.x + 1] == 1)
      return false;
    return true;
  case Right:
    if (activeBlock.y + 2 >= ROWS)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 2] == 1)
      return false;
    return true;
  case Down:
    if (activeBlock.y + 3 >= ROWS)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 3][activeBlock.x + 1] == 1)
      return false;
    return true;
  case Left:
    if (activeBlock.y + 2 >= ROWS)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 2] == 1)
      return false;
    return true;
  }
  return true;
}

bool canJGoDown(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    if (activeBlock.y + 3 >= ROWS)
      return false;
    if (cellInfo[activeBlock.y + 3][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 3][activeBlock.x + 1] == 1)
      return false;
    return true;
  case Right:
    if (activeBlock.y + 2 >= ROWS)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 2] == 1)
      return false;
    return true;
  case Down:
    if (activeBlock.y + 3 >= ROWS)
      return false;
    if (cellInfo[activeBlock.y + 3][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 1] == 1)
      return false;
    return true;
  case Left:
    if (activeBlock.y + 2 >= ROWS)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 2] == 1)
      return false;
    return true;
  }
  return true;
}

bool canZGoDown(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
  case Down:
    if (activeBlock.y + 2 >= ROWS)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 2] == 1)
      return false;
    return true;
  case Right:
  case Left:
    if (activeBlock.y + 3 >= ROWS)
      return false;
    if (cellInfo[activeBlock.y + 3][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 1] == 1)
      return false;
    return true;
  }
  return true;
}

bool canSGoDown(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
  case Down:
    if (activeBlock.y + 2 >= ROWS)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 2] == 1)
      return false;
    return true;
  case Right:
  case Left:
    if (activeBlock.y + 3 >= ROWS)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 3][activeBlock.x + 1] == 1)
      return false;
    return true;
  }
  return true;
}

void lockActiveBlock(ActiveBlock activeBlock) {
  switch (activeBlock.block) {
  case BarBlock:
    lockBarBlock(activeBlock);
    break;
  case BoxBlock:
    lockBoxBlock(activeBlock);
    break;
  case TBlock:
    lockTBlock(activeBlock);
    break;
  case LBlock:
    lockLBlock(activeBlock);
    break;
  case JBlock:
    lockJBlock(activeBlock);
    break;
  case ZBlock:
    lockZBlock(activeBlock);
    break;
  case SBlock:
    lockSBlock(activeBlock);
    break;
  }
}

void lockBoxBlock(ActiveBlock activeBlock) {
  cellInfo[activeBlock.y][activeBlock.x] = 1;
  cellInfo[activeBlock.y][activeBlock.x + 1] = 1;
  cellInfo[activeBlock.y + 1][activeBlock.x] = 1; 
  cellInfo[activeBlock.y + 1][activeBlock.x + 1] = 1; 
}

void lockBarBlock(ActiveBlock activeBlock) {
  int barCells = 4;
  if (activeBlock.orientation == Up || activeBlock.orientation == Down) {
    for (int y = 0; y < barCells; y++) {
      cellInfo[activeBlock.y + y][activeBlock.x] = 1;
    }
  } else {
    for (int x = 0; x < barCells; x++) {
      cellInfo[activeBlock.y][activeBlock.x + x] = 1;
    }
  }
}

void lockTBlock(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    cellInfo[activeBlock.y][activeBlock.x] = 1;
    cellInfo[activeBlock.y][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y][activeBlock.x + 2] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 1] = 1;
    break;
  case Right:
    cellInfo[activeBlock.y][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 2][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 1] = 1;
    break;
  case Down:
    cellInfo[activeBlock.y][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 2] = 1;
    break;
  case Left:
    cellInfo[activeBlock.y][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 2][activeBlock.x + 1] = 1;
    break;
  }
}

void lockLBlock(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    cellInfo[activeBlock.y][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 2][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 2][activeBlock.x + 1] = 1;
    break;
  case Right:
    cellInfo[activeBlock.y][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x] = 1;
    cellInfo[activeBlock.y][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y][activeBlock.x + 2] = 1;
    break;
  case Down:
    cellInfo[activeBlock.y][activeBlock.x] = 1;
    cellInfo[activeBlock.y][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 2][activeBlock.x + 1] = 1;
    break;
  case Left:
    cellInfo[activeBlock.y + 1][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 2] = 1;
    cellInfo[activeBlock.y][activeBlock.x + 2] = 1;
    break;
  }
}

void lockJBlock(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    cellInfo[activeBlock.y][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 2][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 2][activeBlock.x] = 1;
    break;
  case Right:
    cellInfo[activeBlock.y][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 2] = 1;
    break;
  case Down:
    cellInfo[activeBlock.y][activeBlock.x] = 1;
    cellInfo[activeBlock.y][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 2][activeBlock.x] = 1;
    break;
  case Left:
    cellInfo[activeBlock.y][activeBlock.x] = 1;
    cellInfo[activeBlock.y][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y][activeBlock.x + 2] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 2] = 1;
    break;
  }
}

void lockZBlock(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
  case Down:
    cellInfo[activeBlock.y][activeBlock.x] = 1;
    cellInfo[activeBlock.y][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 2] = 1;
    break;
  case Left:
  case Right:
    cellInfo[activeBlock.y][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 2][activeBlock.x] = 1;
    break;
  }
}

void lockSBlock(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
  case Down:
    cellInfo[activeBlock.y + 1][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y][activeBlock.x + 2] = 1;
    cellInfo[activeBlock.y][activeBlock.x + 1] = 1;
    break;
  case Left:
  case Right:
    cellInfo[activeBlock.y][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x] = 1;
    cellInfo[activeBlock.y + 1][activeBlock.x + 1] = 1;
    cellInfo[activeBlock.y + 2][activeBlock.x + 1] = 1;
    break;
  }
}

void drawLockedCells() { // scans cell info and draws grey rectangle when cells
                         // = 1.
  for (int row = 0; row < ROWS; row++) {
    for (int col = 0; col < COLS; col++) {
      if (cellInfo[row][col] == 1) {
        DrawRectangle(col * CELL_WIDTH, row * CELL_HEIGHT, CELL_WIDTH,
                      CELL_HEIGHT, LOCKED_CELL_COLOR);
      }
    }
  }
}

int clearFullLines() {
  int linesCleared = 0;

  for (int row = ROWS - 1; row >= 0; row--) {
    bool isFull = true;
    for (int col = 0; col < COLS; col++) {
      if (cellInfo[row][col] == 0) {
        isFull = false;
        break;
      }
    }

    if (isFull) {
      linesCleared++;

      for (int r = row; r > 0; r--) {
        for (int c = 0; c < COLS; c++) {
          cellInfo[r][c] = cellInfo[r - 1][c];
        }
      }

      for (int c = 0; c < COLS; c++) {
        cellInfo[0][c] = 0;
      }

      row++;
    }
  }

  switch (linesCleared) {
  case 1:
    return 100;
  case 2:
    return 300;
  case 3:
    return 500;
  default:
    return linesCleared >= 4 ? 800 : 0;
  }
}

bool isGameOver() {
  for (int col = 0; col < COLS; col++) {
    if (cellInfo[0][col] == 1) {
      return true;
    }
  }
  return false;
}

void drawHud() {
  DrawText("TETRIS", BOARD_WIDTH + 20, 20, 30, RED);
  DrawText(TextFormat("Score: %i", score), BOARD_WIDTH + 20, 70, 24, BLACK);
}

bool canBlockGoLeft(ActiveBlock activeBlock) {
  switch (activeBlock.block) {
  case BarBlock:
    return canBarGoLeft(activeBlock);
  case BoxBlock:
    return canBoxGoLeft(activeBlock);
  case TBlock:
    return canTGoLeft(activeBlock);
  case LBlock:
    return canLGoLeft(activeBlock);
  case JBlock:
    return canJGoLeft(activeBlock);
  case ZBlock:
    return canZGoLeft(activeBlock);
  case SBlock:
    return canSGoLeft(activeBlock);
  }
  return false;
}

bool canBoxGoLeft(ActiveBlock activeBlock) {
  if (activeBlock.x - 1 < 0)
    return false;
  for (int i = 0; i < 2; i++)
    if (cellInfo[activeBlock.y + i][activeBlock.x - 1] == 1)
      return false;
  return true;
}

bool canBarGoLeft(ActiveBlock activeBlock) {
  int barCells = 4;
  if (activeBlock.orientation == Up || activeBlock.orientation == Down) {
    if (activeBlock.x - 1 < 0)
      return false;
    for (int y = 0; y < barCells; y++) {
      if (cellInfo[activeBlock.y + y][activeBlock.x - 1] == 1)
        return false;
    }
  } else {
    if (activeBlock.x - 1 < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x - 1] == 1)
      return false;
  }
  return true;
}

bool canTGoLeft(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    if (activeBlock.x - 1 < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x - 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    break;
  case Right:
    if (activeBlock.x - 1 < 0)
      return false;
    for (int i = 0; i < 3; i++) {
      if (cellInfo[activeBlock.y + i][activeBlock.x - 1] == 1)
        return false;
    }
    break;
  case Down:
    if (activeBlock.x - 1 < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x - 1] == 1)
      return false;
    break;
  case Left:
    if (activeBlock.x - 1 < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x - 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x] == 1)
      return false;
    break;
  }
  return true;
}

bool canLGoLeft(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    if (activeBlock.x - 1 < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x - 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x - 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x - 1] == 1)
      return false;
    break;
  case Right:
    if (activeBlock.x - 1 < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x - 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x - 1] == 1)
      return false;
    break;
  case Down:
    if (activeBlock.x - 1 < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x - 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x] == 1)
      return false;
    break;
  case Left:
    if (activeBlock.x - 1 < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x - 1] == 1)
      return false;
    break;
  }
  return true;
}

bool canJGoLeft(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    if (activeBlock.x - 1 < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x - 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    break;
  case Right:
    if (activeBlock.x - 1 < 0)
      return false;
    for (int i = 0; i < 3; i++) {
      if (cellInfo[activeBlock.y + i][activeBlock.x - 1] == 1)
        return false;
    }
    break;
  case Down:
    if (activeBlock.x - 1 < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x - 1] == 1)
      return false;
    break;
  case Left:
    if (activeBlock.x - 1 < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x - 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x] == 1)
      return false;
    break;
  }
  return true;
}

bool canZGoLeft(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
  case Down:
    if (activeBlock.x - 1 < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x - 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    break;
  case Right:
  case Left:
    if (activeBlock.x - 1 < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x - 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x - 1] == 1)
      return false;
  }
  return true;
}

bool canSGoLeft(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
  case Down:
    if (activeBlock.x - 1 < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x - 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x] == 1)
      return false;
    break;
  case Right:
  case Left:
    if (activeBlock.x - 1 < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x - 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x - 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x] == 1)
      return false;
    break;
  }
  return true;
}

//          Can Block go right.

bool canBlockGoRight(ActiveBlock activeBlock) {
  switch (activeBlock.block) {
  case BarBlock:
    return canBarGoRight(activeBlock);
  case BoxBlock:
    return canBoxGoRight(activeBlock);
  case TBlock:
    return canTGoRight(activeBlock);
  case LBlock:
    return canLGoRight(activeBlock);
  case JBlock:
    return canJGoRight(activeBlock);
  case ZBlock:
    return canZGoRight(activeBlock);
  case SBlock:
    return canSGoRight(activeBlock);
  }
  return false;
}

bool canTGoRight(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    if (activeBlock.x + 3 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 3] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 2] == 1)
      return false;
    break;
  case Right:
    if (activeBlock.x + 2 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 2] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 1] == 1)
      return false;
    break;
  case Down:
    if (activeBlock.x + 3 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 3] == 1)
      return false;
    break;
  case Left:
    if (activeBlock.x + 2 >= COLS)
      return false;
    for (int i = 0; i < 3; i++) {
      if (cellInfo[activeBlock.y + i][activeBlock.x + 2] == 1)
        return false;
    }
    break;
  }
  return true;
}

bool canLGoRight(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    if (activeBlock.x + 2 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 3] == 1)
      return false;
    break;
  case Right:
    if (activeBlock.x + 3 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 3] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 1] == 1)
      return false;
    break;
  case Down:
    if (activeBlock.x + 2 >= COLS)
      return false;
    for (int i = 0; i < 3; i++) {
      if (cellInfo[activeBlock.y + i][activeBlock.x + 2] == 1)
        return false;
    }
    break;
  case Left:
    if (activeBlock.x + 3 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 3] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 3] == 1)
      return false;
    break;
  }
  return true;
}

bool canJGoRight(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    if (activeBlock.x + 2 >= COLS)
      return false;
    for (int i = 0; i < 3; i++) {
      if (cellInfo[activeBlock.y + i][activeBlock.x + 2] == 1)
        return false;
    }
    break;
  case Right:
    if (activeBlock.x + 3 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 3] == 1)
      return false;
    break;
  case Down:
    if (activeBlock.x + 2 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 2] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 1] == 1)
      return false;
    break;
  case Left:
    if (activeBlock.x + 3 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 3] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 3] == 1)
      return false;
  }
  return true;
}

bool canZGoRight(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
  case Down:
    if (activeBlock.x + 3 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 2] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 3] == 1)
      return false;
    break;
  case Right:
  case Left:
    if (activeBlock.x + 2 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 2] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 2] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 1] == 1)
      return false;
    break;
  }
  return true;
}

bool canSGoRight(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
  case Down:
    if (activeBlock.x + 3 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 3] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 2] == 1)
      return false;
    break;
  case Right:
  case Left:
    if (activeBlock.x + 2 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 2] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 2] == 1)
      return false;
    break;
  }
  return true;
}

bool canBoxGoRight(ActiveBlock activeBlock) {
  if (activeBlock.x + 2 >= COLS)
    return false; // wall check
  for (int i = 0; i < 2; i++) {
    if (cellInfo[activeBlock.y + i][activeBlock.x + 2] == 1)
      return false;
  }
  return true;
}

bool canBarGoRight(ActiveBlock activeBlock) {
  int barCells = 4;
  if (activeBlock.orientation == Up || activeBlock.orientation == Down) {
    if (activeBlock.x + 1 >= COLS)
      return false;
    for (int y = 0; y < barCells; y++) {
      if (cellInfo[activeBlock.y + y][activeBlock.x + 1] == 1)
        return false;
    }
  } else {
    if (activeBlock.x + barCells >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + barCells] == 1)
      return false;
  }
  return true;
}

bool canBlockRotate(ActiveBlock activeBlock) {
  switch (activeBlock.block) {
  case BarBlock:
    return canBarRotate(activeBlock);
  case BoxBlock:
    return canBoxRotate(activeBlock);
  case TBlock:
    return canTRotate(activeBlock);
  case LBlock:
    return canLRotate(activeBlock);
  case JBlock:
    return canJRotate(activeBlock);
  case ZBlock:
    return canZRotate(activeBlock);
  case SBlock:
    return canSRotate(activeBlock);
  }
  return false;
}

bool canBoxRotate(ActiveBlock activeBlock) {
  return false; // A 2x2 square looks identical in all orientations — no
                // rotation needed
}

bool canBarRotate(ActiveBlock activeBlock) {
  int barCells = 4;
  if (activeBlock.orientation == Up || activeBlock.orientation == Down) {
    if ((activeBlock.x - 1) < 0) // checks if it hits the left wall.
      return false;
    if ((activeBlock.x - 1) + barCells >=
        COLS) // checks if it hits the right wall.
      return false;
    for (int x = -1; x < barCells - 1; x++) {
      if (cellInfo[activeBlock.y][activeBlock.x + x] == 1)
        return false;
    }
  } else {
    if (activeBlock.y + barCells >= ROWS) // checks if it hits the floor.
      return false;
    for (int y = 0; y < barCells; y++) {
      if (cellInfo[activeBlock.y + y][activeBlock.x + 1] == 1)
        return false;
    }
  }
  return true;
}

bool canTRotate(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up: // checkiing to see if it rotates to the right.
    if (activeBlock.y + 2 >= ROWS)
      return false; // Hits the floor
    if (activeBlock.x < 0)
      return false; // Hits left wall
    if (activeBlock.x + 1 >= COLS)
      return false; // Hits right wall
    if (cellInfo[activeBlock.y][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x] == 1)
      return false;
    break;
  case Right: // checking to see if it rotates downwards.
    if (activeBlock.y + 1 >= ROWS)
      return false;
    if (activeBlock.x < 0)
      return false;
    if (activeBlock.x + 2 >= COLS)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 2] == 1)
      return false;
    break;
  case Down: // checking to see if it rotates to the left.
    if (activeBlock.y + 2 >= ROWS)
      return false;
    if (activeBlock.x < 0)
      return false;
    if (activeBlock.x + 1 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 1] == 1)
      return false;
    break;
  case Left: // checks to see if it can rotate upwards.
    if (activeBlock.y + 1 >= ROWS)
      return false;
    if (activeBlock.x < 0)
      return false;
    if (activeBlock.x + 2 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 2] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 1] == 1)
      return false;
    break;
  }
  return true;
}
bool canLRotate(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    if (activeBlock.x + 2 >= COLS)
      return false;
    if (activeBlock.y + 1 >= ROWS)
      return false;
    if (activeBlock.x < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 2] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    break;
  case Right:
    if (activeBlock.x + 1 >= COLS)
      return false;
    if (activeBlock.y + 2 >= ROWS)
      return false;
    if (activeBlock.x < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 1] == 1)
      return false;
    break;
  case Down:
    if (activeBlock.x + 2 >= COLS)
      return false;
    if (activeBlock.y + 1 >= ROWS)
      return false;
    if (activeBlock.x < 0)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 2] == 1)
      return false;
    break;
  case Left:
    if (activeBlock.x + 2 >= COLS)
      return false;
    if (activeBlock.y + 1 >= ROWS)
      return false;
    if (activeBlock.x < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 2] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 1] == 1)
      return false;
    break;
  }
  return true;
}

bool canJRotate(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
    if (activeBlock.x + 2 >= COLS)
      return false;
    if (activeBlock.y + 1 >= ROWS)
      return false;
    if (activeBlock.x < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 2] == 1)
      return false;
    break;
  case Right:
    if (activeBlock.x + 1 >= COLS)
      return false;
    if (activeBlock.y + 2 >= ROWS)
      return false;
    if (activeBlock.x < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x] == 1)
      return false;
    break;
  case Down:
    if (activeBlock.x + 2 >= COLS)
      return false;
    if (activeBlock.y + 1 >= ROWS)
      return false;
    if (activeBlock.x < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 2] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 2] == 1)
      return false;
    break;
  case Left:
    if (activeBlock.x + 1 >= COLS)
      return false;
    if (activeBlock.y + 2 >= ROWS)
      return false;
    if (activeBlock.x < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 1] == 1)
      return false;
    break;
  }
  return true;
}

bool canZRotate(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
  case Down:
    if (activeBlock.x < 0)
      return false;
    if (activeBlock.y + 2 >= ROWS)
      return false;
    if (activeBlock.x + 1 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x] == 1)
      return false;
    break;
  case Right:
  case Left:
    if (activeBlock.y + 1 >= ROWS)
      return false;
    if (activeBlock.x + 2 >= COLS)
      return false;
    if (activeBlock.x < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 2] == 1)
      return false;
    break;
  }
  return true;
}

bool canSRotate(ActiveBlock activeBlock) {
  switch (activeBlock.orientation) {
  case Up:
  case Down:
    if (activeBlock.y + 2 >= ROWS)
      return false;
    if (activeBlock.x + 1 >= COLS)
      return false;
    if (activeBlock.x < 0)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x + 1] == 1)
      return false;
    if (cellInfo[activeBlock.y + 2][activeBlock.x + 1] == 1)
      return false;
    break;
  case Right:
  case Left:
    if (activeBlock.x < 0)
      return false;
    if (activeBlock.y + 1 >= ROWS)
      return false;
    if (activeBlock.x + 2 >= COLS)
      return false;
    if (cellInfo[activeBlock.y][activeBlock.x + 2] == 1)
      return false;
    if (cellInfo[activeBlock.y + 1][activeBlock.x] == 1)
      return false;
  }
  return true;
}

void rotateBlock(ActiveBlock &activeBlock) {
  switch (activeBlock.block) {
  case BarBlock:
    switch (activeBlock.orientation) {
    case Up:
    case Down:
      activeBlock.x--; // I don't understand why this is here.
      activeBlock.orientation = Right;
      break;
    case Right:
    case Left:
      activeBlock.x++;
      activeBlock.y--;
      activeBlock.orientation = Up;
      break;
    }
    break;
  case BoxBlock:
    break; // No rotation.
  case TBlock:
    switch (activeBlock.orientation) {
    case Up:
      activeBlock.orientation = Right;
      break;
    case Right:
      activeBlock.orientation = Down;
      break;
    case Down:
      activeBlock.orientation = Left;
      break;
    case Left:
      activeBlock.orientation = Up;
      break;
    }
    break;
  case LBlock:
    switch (activeBlock.orientation) {
    case Up:
      activeBlock.orientation = Right;
      break;
    case Right:
      activeBlock.orientation = Down;
      break;
    case Down:
      activeBlock.orientation = Left;
      break;
    case Left:
      activeBlock.orientation = Up;
      break;
    }
    break;
  case JBlock:
    switch (activeBlock.orientation) {
    case Up:
      activeBlock.orientation = Right;
      break;
    case Right:
      activeBlock.orientation = Down;
      break;
    case Down:
      activeBlock.orientation = Left;
      break;
    case Left:
      activeBlock.orientation = Up;
      break;
    }
    break;
  case ZBlock:
    switch (activeBlock.orientation) {
    case Up:
      activeBlock.orientation = Right;
      break;
    case Right:
      activeBlock.orientation = Down;
      break;
    case Down:
      activeBlock.orientation = Left;
      break;
    case Left:
      activeBlock.orientation = Up;
      break;
    }
    break;
  case SBlock:
    switch (activeBlock.orientation) {
    case Up:
      activeBlock.orientation = Right;
      break;
    case Right:
      activeBlock.orientation = Down;
      break;
    case Down:
      activeBlock.orientation = Left;
      break;
    case Left:
      activeBlock.orientation = Up;
      break;
    }
    break;
  }
}

