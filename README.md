# A Moving Robot

電腦圖學 Homework #2：A Moving Robot。以 C++17、OpenGL 與 GLUT 實作（Windows / Linux 使用 FreeGLUT，macOS 使用系統內建的 GLUT）。

## 建置

需要 CMake 3.16 以上、支援 C++17 的編譯器，以及 OpenGL / GLU / GLUT。

| 平台                     | 相依套件                                                                                  |
| ------------------------ | ----------------------------------------------------------------------------------------- |
| macOS                    | Xcode Command Line Tools（`xcode-select --install`）與 CMake（例如 `brew install cmake`） |
| Linux（Debian / Ubuntu） | `sudo apt install cmake freeglut3-dev libglu1-mesa-dev`                                   |
| Windows                  | MinGW-w64 + FreeGLUT（MinGW 會靜態連結執行檔）                                            |

> macOS 使用系統內建的 GLUT、OpenGL framework（含 GLU），有硬體加速，不需要 Homebrew 的 FreeGLUT、Mesa 或 XQuartz。Apple GLUT 與 FreeGLUT 的差異（關閉視窗、主循環、修飾鍵、Command 快捷鍵等）由平台層處理，見 [`src/platform/README.md`](src/platform/README.md)。

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

```bash
cmake --build build
```

```bash
./build/main
```

建置完成後，`assets/` 會自動複製到執行檔旁邊；程式啟動時從執行檔所在目錄的 `assets/fonts` 載入字型，請保持兩者放在一起。

`Debug` 組態會開啟 `-fsanitize=undefined`。

## 操作

啟動後為主選單，選「開始」進入世界。世界是依 seed 生成的地形，以玩家為中心載入半徑 12 個 chunk 的圓形範圍，地上散布方塊、球與圓柱等障礙物。玩家操控一隻藍色機器人，可以走、跑、跳，並切換站立、潛行、坐下三種姿勢；撞到障礙物或太陡的坡會被擋下。

機器人與相機分開操作：左手控制機器人，右手控制相機。相機有六個自由度，平移與旋轉都沿著相機自己的軸，所以 `gluLookAt` 的 eye、focus、up 都會改變。

相機有兩種模式，按 Tab 切換：

- 跟隨（預設）：相機與機器人的相對位置固定，機器人移動、轉向時相機跟著走
- 自由：相機留在原地，只由相機的按鍵控制

投影方式按 F1–F5 直接切換（macOS 預設要同時按住 fn）：

| 按鍵 | 投影方式                                              |
| ---- | ----------------------------------------------------- |
| F1   | 沿 x 軸的平行投影（側視）                             |
| F2   | 沿 y 軸的平行投影（俯視）                             |
| F3   | 沿 z 軸的平行投影（前視）                             |
| F4   | 透視投影（預設）                                      |
| F5   | 四個 viewport：左上俯視、右上透視、左下前視、右下側視 |

平行投影的視線固定沿世界的座標軸，畫面中心則跟著相機的 focus（F）移動，只畫中心前後 40 公尺內的東西，並疊上透視相機的 view volume（黃色四角錐，洋紅色短線是相機的上方向）。所有畫面都會畫出世界座標的原點與 x、y、z 軸（紅、綠、藍）。

| 按鍵           | 功能                                                |
| -------------- | --------------------------------------------------- |
| W / S          | 遊玩中：機器人往前 / 後走（沿機器人自己的朝向）     |
| A / D          | 遊玩中：機器人往左 / 右平移                         |
| Q / E          | 遊玩中：機器人原地左 / 右轉                         |
| 左 Shift       | 遊玩中：按住跑步                                    |
| 空白鍵         | 遊玩中：跳躍                                        |
| 1 / 2 / 3      | 遊玩中：切換為站立 / 潛行 / 坐下                    |
| I / K          | 遊玩中：相機前進 / 後退（surge）                    |
| J / L          | 遊玩中：相機往左 / 右平移（sway）                   |
| U / O          | 遊玩中：相機上升 / 下降（heave）                    |
| ← / →          | 遊玩中：相機左 / 右轉（yaw）                        |
| ↑ / ↓          | 遊玩中：相機抬頭 / 低頭（pitch）                    |
| , / .          | 遊玩中：相機往左 / 右傾（roll）                     |
| 左鍵拖曳       | 遊玩中：相機轉向，往哪拖就往哪看                    |
| = / -          | 遊玩中：放大 / 縮小（改變 fovY）；平行投影的範圍也由 fovY 換算，所有畫面一起縮放 |
| R              | 遊玩中：把相機放回機器人後方                        |
| Tab            | 遊玩中：切換相機模式（跟隨 / 自由）                 |
| F1–F5          | 遊玩中：切換投影方式                                |
| Esc            | 遊玩中：打開暫停選單；選單中：返回上一層            |
| ↑ ↓ / Tab      | 在選單中選擇按鈕                                    |
| Enter / 空白鍵 | 按下選擇的按鈕                                      |

選單也可以用滑鼠點擊。

GLUT 無法使用輸入法，注音等輸入法開啟時單鍵按鍵會失效。macOS 上程式在前景時會自動切換為英文鍵盤配置，切換到其他程式或結束時還原；其他平台請自行切換為英文輸入。

## 專案結構

```text
src/
├── main.cpp
├── engine/     視窗、主迴圈（60 Hz 固定 tick）、輸入狀態
├── game/       遊戲流程（Session 與畫面的切換），組裝其他模組
├── world/      世界的狀態與規則，不涉及繪製與輸入
├── view/       依世界狀態繪製場景與 HUD
├── screen/     選單畫面（主選單、暫停、操作說明）
├── gui/        通用介面元件（按鈕）
├── graphics/   不認識世界的繪圖工具：2D 畫布、座標軸、繪製階段、相機與投影參數、畫出網格
├── common/     顏色、座標、矩形與 UTF-8
├── font/       字型的量測與排版（見 src/font/README.md）
├── input/      按鍵代碼與輸入狀態
├── io/         檔案讀寫（字型、設定檔）
├── math/       向量、數學常數與角度轉換、內插等工具函式
├── geometry/   不涉及繪圖 API 的幾何：曲線與路徑、三角形網格與 MeshBuilder
└── platform/   GLUT 平台層：FreeGLUT 與 Apple GLUT 的差異
assets/fonts/   字型檔與字型設定（fonts.ini）
docs/           報告各章節、圖片與圖片原始檔
```

## 報告

報告主檔為 [`hw2-robot.tex`](hw2-robot.tex)，各章節位於 `docs/`。使用 XeLaTeX 編譯，需安裝 Noto Serif、Noto Serif / Sans CJK TC、Menlo 與 Kaiti TC 字型。

```bash
latexmk -xelatex hw2-robot.tex
```

`docs/figures-src/` 存放圖片原始檔：`.mmd` 由 `render-mermaid.sh` 透過 Mermaid CLI（`mmdc`）輸出為 PNG。

## 授權

`assets/fonts/` 內的字型依各自目錄中的 OFL 授權條款散布。
