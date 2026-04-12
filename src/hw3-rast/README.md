# Computer Graphics HW3 - Software Rasterization & Diffusion Shading

本專案包含了一個基於 C++ 實作的 Z-buffer 軟體光柵器 (Software Rasterizer)，未來計畫算繪出的結果與基於 Python 的擴散模型 (Diffusion Model) 進行結合，以生成具有豐富細節與真實感的紋理陰影 (Texture Shading)。

為了解決 C++ 圖形學庫與 Python 深度學習庫常常會遇到的系統環境衝突問題，推薦統一透過 **Conda** 來管理我們所有的依賴套件。

## 系統環境與套件安裝 (Conda 環境)

我們提供了一份 `environment.yml` 檔案，不僅包含了圖形介面編譯所需的 C++ 依賴 (`cmake`, `glfw`, `glew`, `glm`, `freeglut`)，也一併準備好 Diffusion Model 常用的 AI 運算推論套件 (`pytorch`, `diffusers`)。

### 快速建立 Conda 環境
開啟終端機，執行以下指令，Conda 將會自動安裝所有需要用到的 C++ 圖形學編譯鏈以及 Python 生成模型套件：

```bash
# 1. 依照 environment.yml 建立所有環境與相依性套件
conda env create -f environment.yml

# 2. 啟動環境
conda activate your-env-name
```
*(備註：若尚未安裝顯卡的 CUDA 驅動或有版本限制，請視情況至 `environment.yml` 中修改 `pytorch-cuda` 的對應版本)*

---

## 專案編譯與執行

當上述所有套件安裝完成，使用 CMake 進行編譯：

### 1. 編譯 C++ rendering 程式
```bash
# 移動到 hw3-rast 目錄 (若您尚未在此目錄中)
# cd src/hw3-rast

# 利用 cmake 尋找環境中的依賴套件並產生編譯檔
cmake -B build

# 指示系統開始編譯出執行檔
cmake --build build
```

### 2. 執行程式
編譯成功後，就可以去啟動執行檔並開啟測試視窗：
```bash
# 執行
./build/rast
```


## 未來擴充想法：結合 Diffusion Model (Texture Shading)

既然我們已經把 C++ 與 Python AI 之間的系統環境「打通」，未來串接兩者就會有更多彈性的空間！若要在光柵化後透過 Diffusion 生成精美的 Shading，有幾種常見的整合實作方式供您評估：

1. **圖檔存取過渡 (File I/O + ControlNet)**
   在程式的 `swClearZbuffer` 並且像素填滿之後，透過 `OpenCV-C++` 或簡單的 `stb_image` 庫將 Z-buffer 陣列存為深度圖 (`Depth Map`) 檔案。
   接著呼叫寫好的 Python AI 腳本，利用 `diffusers` 中極度熱門的 `StableDiffusionControlNetPipeline` 並載入 Depth 權重模型，基於此深度圖做邊緣控制的影像生成 (Image-to-Image / Depth-to-Image)！

2. **C++ 直接作為 Python 擴充模組 (Pybind11 整合)**
   這是一個更高級但也更一體化的作法。我們可以透過 `pybind11` 將我們寫好的 `swInitZbuffer`, `swTriangle` 等 C++ 程式碼編譯並打包為 Python 可載入的 `C-extension`。
   如此一來，這個光柵化引擎將會變成一個 Python 庫。您可以直接在 Python 中將 3D 模型餵給自製 Rasterizer，產出的影像陣列即可原地直接成為 PyTorch 張量 (Tensors)，做到一鍵打通 Rasterizer -> Diffusion -> Render Image 無縫串接，甚至還能加上自動微分機制 (Differentiable Rasterization)！
