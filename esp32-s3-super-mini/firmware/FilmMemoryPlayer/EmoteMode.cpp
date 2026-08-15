#include "EmoteMode.h"

#include "AppState.h"

// ============================================================
// Robot Expression System
// ============================================================

void drawThickLine(int16_t x0, int16_t y0,
                   int16_t x1, int16_t y1,
                   uint16_t color) {
  tft.drawLine(x0, y0 - 1, x1, y1 - 1, color);
  tft.drawLine(x0, y0,     x1, y1,     color);
  tft.drawLine(x0, y0 + 1, x1, y1 + 1, color);
}


// ============================================================
// ROBOT BODY
// ============================================================

void drawRobotShell() {
  const uint16_t white = ST77XX_WHITE;

  // ----------------------------------------------------------
  // 主頭殼
  // ----------------------------------------------------------

  tft.drawRoundRect(
    26, 20,
    76, 68,
    18,
    white
  );

  tft.drawFastHLine(
    42, 87,
    44,
    white
  );


  // ----------------------------------------------------------
  // 左右耳罩
  // ----------------------------------------------------------

  tft.drawRoundRect(
    19, 41,
    8, 27,
    4,
    white
  );

  tft.drawRoundRect(
    101, 41,
    8, 27,
    4,
    white
  );

  tft.drawFastHLine(23, 53, 4, white);
  tft.drawFastHLine(101, 53, 4, white);


  // ----------------------------------------------------------
  // 臉部顯示區
  // ----------------------------------------------------------

  tft.drawRoundRect(
    34, 32,
    60, 45,
    11,
    white
  );


  // ----------------------------------------------------------
  // 脖子
  // ----------------------------------------------------------

  tft.drawRoundRect(
    52, 89,
    24, 9,
    3,
    white
  );


  // ----------------------------------------------------------
  // 小型肩膀 / 底座
  // ----------------------------------------------------------

  tft.drawRoundRect(
    39, 97,
    50, 11,
    5,
    white
  );
}


// ============================================================
// EYES
// ============================================================

// 普通圓眼
void drawDotEye(int16_t x, int16_t y) {
  tft.fillCircle(
    x, y,
    3,
    ST77XX_WHITE
  );
}


// 比較大的圓眼
void drawBigEye(int16_t x, int16_t y) {
  const uint16_t white = ST77XX_WHITE;

  tft.drawCircle(
    x, y,
    5,
    white
  );

  tft.fillCircle(
    x, y,
    2,
    white
  );
}


// 開心瞇眼
void drawHappyEye(int16_t x, int16_t y) {
  const uint16_t white = ST77XX_WHITE;

  drawThickLine(
    x - 7, y + 2,
    x - 3, y - 2,
    white
  );

  drawThickLine(
    x - 3, y - 2,
    x, y - 3,
    white
  );

  drawThickLine(
    x, y - 3,
    x + 3, y - 2,
    white
  );

  drawThickLine(
    x + 3, y - 2,
    x + 7, y + 2,
    white
  );
}


// 平眼 / 無奈
void drawFlatEye(int16_t x, int16_t y) {
  drawThickLine(
    x - 6, y,
    x + 6, y,
    ST77XX_WHITE
  );
}


// X 眼
void drawXEye(int16_t x, int16_t y) {
  const uint16_t white = ST77XX_WHITE;

  drawThickLine(
    x - 5, y - 5,
    x + 5, y + 5,
    white
  );

  drawThickLine(
    x + 5, y - 5,
    x - 5, y + 5,
    white
  );
}


// 生氣眼
void drawAngryEyes() {
  const uint16_t white = ST77XX_WHITE;

  // 左眼
  drawThickLine(
    42, 46,
    53, 52,
    white
  );

  // 右眼
  drawThickLine(
    75, 52,
    86, 46,
    white
  );

  // 小眼睛本體
  tft.fillCircle(49, 54, 2, white);
  tft.fillCircle(79, 54, 2, white);
}


// ============================================================
// MOUTH
// ============================================================

// 普通小微笑
void drawSmallSmile() {
  const uint16_t white = ST77XX_WHITE;

  drawThickLine(
    55, 64,
    60, 67,
    white
  );

  drawThickLine(
    60, 67,
    64, 68,
    white
  );

  drawThickLine(
    64, 68,
    68, 67,
    white
  );

  drawThickLine(
    68, 67,
    73, 64,
    white
  );
}


// 很淡的微笑
void drawTinySmile() {
  const uint16_t white = ST77XX_WHITE;

  drawThickLine(
    58, 65,
    64, 67,
    white
  );

  drawThickLine(
    64, 67,
    70, 65,
    white
  );
}


// 平嘴
void drawFlatMouth() {
  drawThickLine(
    58, 66,
    70, 66,
    ST77XX_WHITE
  );
}


// 難過嘴
void drawSmallFrown() {
  const uint16_t white = ST77XX_WHITE;

  drawThickLine(
    55, 69,
    60, 65,
    white
  );

  drawThickLine(
    60, 65,
    64, 64,
    white
  );

  drawThickLine(
    64, 64,
    68, 65,
    white
  );

  drawThickLine(
    68, 65,
    73, 69,
    white
  );
}


// 小張嘴大笑
void drawLaughMouth() {
  const uint16_t white = ST77XX_WHITE;

  // 上方
  drawThickLine(
    55, 63,
    73, 63,
    white
  );

  // U
  drawThickLine(
    55, 63,
    58, 69,
    white
  );

  drawThickLine(
    58, 69,
    64, 71,
    white
  );

  drawThickLine(
    64, 71,
    70, 69,
    white
  );

  drawThickLine(
    70, 69,
    73, 63,
    white
  );
}


// 驚訝小 O 嘴
void drawOMouth() {
  const uint16_t white = ST77XX_WHITE;

  tft.drawCircle(
    64, 66,
    5,
    white
  );

  tft.drawCircle(
    64, 66,
    6,
    white
  );
}


// ============================================================
// HEART EYES
// ============================================================

void drawHeartOutline(int16_t x, int16_t y) {
  const uint16_t white = ST77XX_WHITE;

  drawThickLine(
    x, y + 5,
    x - 6, y,
    white
  );

  drawThickLine(
    x - 6, y,
    x - 5, y - 4,
    white
  );

  drawThickLine(
    x - 5, y - 4,
    x - 2, y - 5,
    white
  );

  drawThickLine(
    x - 2, y - 5,
    x, y - 2,
    white
  );

  drawThickLine(
    x, y - 2,
    x + 2, y - 5,
    white
  );

  drawThickLine(
    x + 2, y - 5,
    x + 5, y - 4,
    white
  );

  drawThickLine(
    x + 5, y - 4,
    x + 6, y,
    white
  );

  drawThickLine(
    x + 6, y,
    x, y + 5,
    white
  );
}


// ============================================================
// ROBOT DETAILS
// ============================================================

// 臉頰小燈
void drawCheeks() {
  const uint16_t white = ST77XX_WHITE;

  tft.drawFastHLine(
    39, 64,
    6,
    white
  );

  tft.drawFastHLine(
    83, 64,
    6,
    white
  );
}


// ============================================================
// MAIN EXPRESSION
// ============================================================

void drawExpression() {

  const uint16_t white = ST77XX_WHITE;

  tft.fillScreen(ST77XX_BLACK);

  // 機器人的頭、耳罩、脖子、底座固定不變
  drawRobotShell();


  switch (currentExpression) {

    // ========================================================
    // 0. 普通 / 開心
    //
    //     •       •
    //
    //        ︶
    // ========================================================

    case 0:

      drawDotEye(48, 51);
      drawDotEye(80, 51);

      drawSmallSmile();

      break;


    // ========================================================
    // 1. 溫柔微笑
    //
    //      ^     ^
    //
    //        ︶
    // ========================================================

    case 1:

      drawHappyEye(48, 52);
      drawHappyEye(80, 52);

      drawTinySmile();

      drawCheeks();

      break;


    // ========================================================
    // 2. 大笑
    // ========================================================

    case 2:

      drawHappyEye(48, 50);
      drawHappyEye(80, 50);

      drawLaughMouth();

      break;


    // ========================================================
    // 3. 暈掉 / 系統當機
    //
    //      X     X
    //
    //        O
    // ========================================================

    case 3:

      drawXEye(48, 51);
      drawXEye(80, 51);

      drawOMouth();

      break;


    // ========================================================
    // 4. 難過
    // ========================================================

    case 4:

      drawDotEye(48, 53);
      drawDotEye(80, 53);

      // 八字眉
      drawThickLine(
        41, 45,
        51, 48,
        white
      );

      drawThickLine(
        77, 48,
        87, 45,
        white
      );

      drawSmallFrown();

      break;


    // ========================================================
    // 5. 無奈 / 累
    //
    //      —     —
    //
    //        —
    // ========================================================

    case 5:

      drawFlatEye(48, 52);
      drawFlatEye(80, 52);

      drawFlatMouth();

      break;


    // ========================================================
    // 6. 生氣
    // ========================================================

    case 6:

      drawAngryEyes();

      drawSmallFrown();

      break;


    // ========================================================
    // 7. 驚訝
    // ========================================================

    case 7:

      drawBigEye(48, 51);
      drawBigEye(80, 51);

      drawOMouth();

      break;


    // ========================================================
    // 8. 喜歡
    // ========================================================

    case 8:

      drawHeartOutline(48, 51);
      drawHeartOutline(80, 51);

      drawTinySmile();

      break;


    // ========================================================
    // 9. 得意
    // ========================================================

    case 9:

      // 左眼
      drawDotEye(48, 51);

      // 右眼眨眼
      drawThickLine(
        74, 53,
        80, 49,
        white
      );

      drawThickLine(
        80, 49,
        86, 53,
        white
      );

      // 歪嘴
      drawThickLine(
        57, 66,
        63, 67,
        white
      );

      drawThickLine(
        63, 67,
        70, 64,
        white
      );

      break;
  }
}

void changeExpression(int direction) {
  currentExpression =
      (currentExpression + EXPRESSION_COUNT + direction) % EXPRESSION_COUNT;
  const uint32_t now = millis();
  expressionSelectionDirty = true;
  expressionSelectionChangedAt = now;
  drawExpression();
}
