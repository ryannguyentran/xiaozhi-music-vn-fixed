# 🎵 Hướng Dẫn Cài Đặt Xiaozhi Music - Phát Nhạc Việt 2.0.3.0.1

> **Xiaozhi Music Vietnamese Edition** - Trợ lý giọng nói thông minh hỗ trợ phát nhạc tiếng Việt

---

## 📋 Mục Lục
- [Yêu Cầu Hệ Thống](#yêu-cầu-hệ-thống)
- [Bước 1: Tải Source Code](#bước-1-tải-source-code)
- [Bước 2: Cài Đặt ESP-IDF](#bước-2-cài-đặt-esp-idf)
- [Bước 3: Cấu Hình Dự Án](#bước-3-cấu-hình-dự-án)
- [Bước 4: Build Firmware](#bước-4-build-firmware)
- [Bước 5: Flash vào ESP32](#bước-5-flash-vào-esp32)
- [Xử Lý Lỗi Thường Gặp](#xử-lý-lỗi-thường-gặp)
- [Lời Cảm Ơn](#lời-cảm-ơn)

---

## 🛠️ Yêu Cầu Hệ Thống

### Phần Cứng
- **ESP32-S3** 
- **Màn hình LCD** tương thích
- **Cáp USB** kết nối ESP32 với máy tính

### Phần Mềm
- **ESP-IDF 5.5** (bắt buộc)
- **Driver USB-to-UART** (CP210x, CH340, FTDI,...)

### Hệ Điều Hành
- ✅ Windows 10/11
- ✅ macOS 12+
- ✅ Linux (Ubuntu 20.04+)

---

## 📥 Bước 1: Tải Source Code

### Từ GitHub

```bash
git clone https://github.com/ryannguyentran/xiaozhi-music-vietnamese.git
cd xiaozhi-music-vietnamese
```

### Hoặc Tải ZIP

1. Truy cập: `https://github.com/ryannguyentran/xiaozhi-music-vietnamese`
2. Nhấn nút **Code** → **Download ZIP**
3. Giải nén vào thư mục `C:\xiaozhi-music` (Windows) hoặc `~/xiaozhi-music` (Mac/Linux)

---

## 🔧 Bước 2: Cài Đặt ESP-IDF

### Windows

#### 2.1. Tải ESP-IDF Installer
```
https://dl.espressif.com/dl/esp-idf/?idf=5.5
```

#### 2.2. Chạy Installer
- Chọn **ESP-IDF 5.5**
- Chọn thư mục cài đặt (khuyến nghị: `C:\esp\esp-idf`)
- Chờ cài đặt hoàn tất (~10-15 phút)

#### 2.3. Mở ESP-IDF Command Prompt
- Tìm "ESP-IDF 5.5 CMD" trong Start Menu
- Hoặc chạy: `C:\esp\esp-idf\export.bat`

### macOS

```bash
# Cài Homebrew (nếu chưa có)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Cài dependencies
brew install cmake ninja dfu-util python3

# Tải ESP-IDF
mkdir -p ~/esp
cd ~/esp
git clone --recursive -b v5.5 https://github.com/espressif/esp-idf.git

# Cài đặt
cd esp-idf
./install.sh esp32s3

# Kích hoạt môi trường
. ./export.sh
```

### Linux (Ubuntu/Debian)

```bash
# Cài dependencies
sudo apt update
sudo apt install -y git wget flex bison gperf python3 python3-pip \
    python3-venv cmake ninja-build ccache libffi-dev libssl-dev \
    dfu-util libusb-1.0-0

# Tải ESP-IDF
mkdir -p ~/esp
cd ~/esp
git clone --recursive -b v5.5 https://github.com/espressif/esp-idf.git

# Cài đặt
cd esp-idf
./install.sh esp32s3

# Kích hoạt môi trường
. ./export.sh
```

### ✅ Kiểm Tra Cài Đặt

```bash
idf.py --version
```

**Kết quả mong đợi:**
```
ESP-IDF v5.5
```

---

## ⚙️ Bước 3: Cấu Hình Dự Án

### 3.1. Di Chuyển Đến Thư Mục Source

```bash
cd /path/to/xiaozhi-music-vietnamese
```

**Ví dụ:**
- Windows: `cd C:\xiaozhi-music`
- macOS/Linux: `cd ~/xiaozhi-music`

### 3.2. Thiết Lập Target ESP32-S3

```bash
idf.py set-target esp32s3
```

**Output:**
```
Adding "set-target"'s dependency "fullclean" to list of commands...
Executing action: fullclean
Build directory '...' not found. Nothing to clean.
Executing action: set-target
Set Target to: esp32s3
```

### 3.3. Mở Menu Cấu Hình

```bash
idf.py menuconfig
```

### 3.4. Cấu Hình Quan Trọng

#### 📱 **Từ Đánh Thức**
```
ESP Speech Recognition → Load Multiple .... (WakeNet9)
├─ [*] Chọn từ Wake Up bạn muốn
```

#### 🖥️ **Display Configuration**
```
Xiaozhi Configuration → Display
├─ Display Driver: ST7789 (hoặc tùy màn hình)
├─ Screen Width: 240
├─ Screen Height: 320
└─ [*] Enable Display Rotation
```

#### 🎤 **Config Board, Màn hình**
```
Xiaozhi Assistant
├─ Default Language > Vietnamese
├─ Board Type : Bread Compact Wifi + LCD ( hoặc Board của bạn )
└─ LCD Type > ST7789 240x240 ( này 8 pin - hoặc màn của bạn nhé ) 
└─ Select Display stype : Đây là chọn cách hiển thị màn hình , icon ngay giữa hoặc cách giống chat wechat từng dòng )
└─ Wake Word Imp.. > Wakenet Model with AFE
```

### 3.5. Lưu Cấu Hình
- Nhấn `S` (Save)
- Nhấn `Q` (Quit)

---

## 🔨 Bước 4: Build Firmware

```bash
idf.py build
```

### Thời Gian Build
- **Lần đầu:** ~15-30 phút (tùy cấu hình máy)
- **Lần sau:** ~2-5 phút (chỉ build phần thay đổi)

### ✅ Build Thành Công

```
Project build complete. To flash, run:
 idf.py -p (PORT) flash

or run:
 idf.py -p (PORT) flash monitor
```

---

## 🚀 Bước 5: Flash vào ESP32

### 5.1. Kiểm Tra Cổng COM

#### Windows
1. Mở **Device Manager** (`Win + X` → Device Manager)
2. Tìm **Ports (COM & LPT)**
3. Tìm thiết bị: `Silicon Labs CP210x USB to UART Bridge (COM3)` hoặc tương tự
4. Ghi nhớ số cổng (ví dụ: `COM3`)

**Hoặc dùng lệnh:**
```cmd
mode
```

#### macOS
```bash
ls /dev/cu.*
```

**Kết quả:**
```
/dev/cu.usbserial-0001
/dev/cu.SLAB_USBtoUART
```

#### Linux
```bash
ls /dev/ttyUSB*
ls /dev/ttyACM*
```

**Kết quả:**
```
/dev/ttyUSB0
```

### 5.2. Cấp Quyền (Linux/macOS)

```bash
sudo usermod -a -G dialout $USER  # Linux
sudo dseditgroup -o edit -a $USER -t user dialout  # macOS
```

**Sau đó logout và login lại**

### 5.3. Flash Firmware

#### Flash + Monitor (Khuyến nghị)

**Windows:**
```cmd
idf.py -p COM3 flash monitor
```

**macOS:**
```bash
idf.py -p /dev/cu.usbserial-0001 flash monitor
```

**Linux:**
```bash
idf.py -p /dev/ttyUSB0 flash monitor
```

#### Chỉ Flash (không theo dõi log)

```bash
idf.py -p COM3 flash
```

### 5.4. Theo Dõi Log (nếu flash riêng)

```bash
idf.py -p COM3 monitor
```

**Thoát monitor:** `Ctrl + ]`

---

## ✅ Xác Nhận Hoạt Động

### Log Khởi Động Thành Công

```
I (123) main: Xiaozhi Music Vietnamese Edition
I (234) wifi: WiFi connected
I (345) Esp32Music: Music player initialized
I (456) main: Say "Sophia" to wake up
```

### Test Wake Word
1. Nói: **"Sophia"**
2. Đèn LED sáng + màn hình hiển thị "Listening..."
3. Nói: **"Phát bài Hoa của Tú Na"** ( hiện tại bản này phải nói cả ca sĩ mới được thì phải. 
4. Nhạc bắt đầu phát 🎵

---

## 🐛 Xử Lý Lỗi Thường Gặp

### ❌ Lỗi: `idf.py: command not found`

**Nguyên nhân:** Chưa kích hoạt ESP-IDF environment

**Giải pháp:**
```bash
# Windows
C:\esp\esp-idf\export.bat

# macOS/Linux
. ~/esp/esp-idf/export.sh
```

---

### ❌ Lỗi: `Failed to open port COM3`

**Nguyên nhân:**
- Cổng COM sai
- Thiếu driver USB
- ESP32 chưa kết nối

**Giải pháp:**
1. Kiểm tra lại cổng COM (xem [Bước 5.1](#51-kiểm-tra-cổng-com))
2. Cài driver:
   - **CP210x:** https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
   - **CH340:** http://www.wch-ic.com/downloads/CH341SER_EXE.html
3. Thử cắm lại ESP32

---

### ❌ Lỗi: `A fatal error occurred: Failed to connect`

**Nguyên nhân:** ESP32 không ở chế độ download

**Giải pháp:**
1. Giữ nút **BOOT** trên board
2. Nhấn nút **RESET** 1 lần
3. Thả nút **BOOT**
4. Chạy lại lệnh flash

---

### ❌ Lỗi Build: `undefined reference to...`

**Nguyên nhân:** Thiếu thư viện hoặc sai cấu hình

**Giải pháp:**
```bash
idf.py fullclean
idf.py build
```

---

## 📊 Kiểm Tra Heap Memory

### Trong Serial Monitor

```
I (362563) SystemInfo: free sram: 67363 minimal sram: 60727
```

- **Free SRAM:** RAM còn trống
- **Minimal SRAM:** RAM thấp nhất đã gặp

**Khuyến nghị:**
- ✅ Minimal SRAM > 50KB → OK
- ⚠️ Minimal SRAM 30-50KB → Cảnh báo
- ❌ Minimal SRAM < 30KB → Nguy cơ crash

---

## 📚 Tài Liệu Tham Khảo

- **ESP-IDF Docs:** https://docs.espressif.com/projects/esp-idf/en/v5.5/
- **ESP32-S3 Datasheet:** https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf
- **Xiaozhi GitHub:** https://github.com/78/xiaozhi-esp32

---

## 🎉 Lời Cảm Ơn

### 👏 Đặc Biệt Cảm Ơn

> **Bạn Cường Mod** - Người đã phát triển và chia sẻ bản **Xiaozhi Music Nhạc Việt** này cho cộng đồng. Nhờ có sự cống hiến và tâm huyết của bạn mà chúng ta có thể thưởng thức nhạc Việt trên thiết bị Xiaozhi một cách hoàn hảo! 🇻🇳🎵

### 🌟 Credits

- **Original Xiaozhi Project:** [@78/xiaozhi-esp32](https://github.com/78/xiaozhi-esp32)
- **Vietnamese Music Adapter:** Cường Mod
- **ESP-IDF Framework:** Espressif Systems
- **Community Contributors:** Cộng đồng Xiaozhi Việt Nam

---

## 💬 Hỗ Trợ & Đóng Góp

### Báo Lỗi
- Tạo **Issue** tại: https://github.com/ryannguyentran/issues
- Cung cấp:
  - Log đầy đủ từ Serial Monitor
  - Thông tin board và màn hình
  - Các bước tái hiện lỗi

### Đóng Góp Code
```bash
git checkout -b feature/your-feature
git commit -am "Add your feature"
git push origin feature/your-feature
```
Sau đó tạo **Pull Request**

---

**Made with ❤️ by Cường Mod and Xiaozhi Vietnam Community**

**Phiên bản:** 1.0.0  
**Cập nhật lần cuối:** 05/11/2025