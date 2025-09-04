# `ses` — Kullanım ve Çalıştırma Rehberi

ALSA tabanlı **loopback**, **UDP üzerinden PCM (codecsiz) gönderim/alım**, **Opus (codec’li) gönderim** ve **iki portlu full-duplex** modlarını içeren komut satırı aracı.

- **TX (gönderici)**: Mikrofon → (PCM/Opus) → UDP  
- **RX (alıcı)**: UDP → (otomatik çözümleme) → hoparlör  
- **Full-Duplex (Yeni)**: Aynı anda **bir porttan dinler**, **diğer porta gönderir**

---

## Derleme (Build)

Gereksinimler (Ubuntu/Debian):
```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libasound2-dev libopus-dev
```

Derleme:
```bash
# proje kökünde
cmake -S . -B build
cmake --build build
# ikili: build/ses
```

(Alternatif klasik yöntem:)
```bash
mkdir -p build && cd build
cmake ..
make -j
```

---

## Komut Özeti

```text
# Cihaz listeleme (varsa)
./ses --list-devices

# Loopback (mikrofondan al, aynı makinede anında çal)
./ses --loopback <capture> <playback> <frame> <ch> <sbytes> <rate>

# Gönderici (TX): mode=0 PCM (udp_sender), mode=1 Opus (codec_sender)
./ses --sender <mode> <capture> <frame> <ch> <sbytes> <rate> <port> <ip>

# Alıcı (RX):
#  - 0: PCM parametrelerini CLI'dan alır
#  - 2: full_automatic_receiver (paket başlığından parametreleri okur)  ← ÖNERİLEN
./ses --receiver 0 <playback> <frame> <ch> <sbytes> <rate> <port>
./ses --receiver 2 <playback> <port>

# **Full-Duplex (Yeni)** — iki portlu, tek komut
# mode: 0=PCM(udp_sender), 1=Opus(codec_sender)
./ses --full-duplex <capture> <playback> <ip> <port_send> <port_listen> <frame> <ch> <sbytes> <rate> <mode>
```

### Parametre Sözlüğü
- **capture / playback**: ALSA cihaz adı (örn. `plughw:1,0`, `plughw:2,0`).
- **frame**: Frame başına örnek sayısı (örn. 960 → 20 ms @ 48 kHz).
- **ch**: Kanal sayısı (1=mono, 2=stereo).
- **sbytes**: Örnek başına byte sayısı (1=8-bit, 2=16-bit, 4=32-bit).
- **rate**: Örnekleme hızı (örn. 48000).
- **ip/port**: UDP hedef IP ve port.
- **mode**: 0=PCM (codecsiz), 1=Opus (codec’li).

---

## Kullanım Örnekleri

### 1) Loopback (aynı makinede geri dinletme)
```bash
./ses --loopback plughw:1,0 plughw:2,0 960 1 2 48000
```

### 2) PCM — Yerel (127.0.0.1)
**Terminal 1 (RX):**
```bash
./ses --receiver 0 plughw:2,0 960 1 2 48000 5000
```
**Terminal 2 (TX):**
```bash
./ses --sender 0 plughw:1,0 960 1 2 48000 5000 127.0.0.1
```

### 3) Opus — Yerel (127.0.0.1)
**Terminal 1 (RX otomatik):**
```bash
./ses --receiver 2 plughw:2,0 5000
```
**Terminal 2 (TX Opus):**
```bash
./ses --sender 1 plughw:1,0 960 1 2 48000 5000 127.0.0.1
```

### 4) Full-Duplex (İki Port) — **Yeni**
Tek komutta iki yönlü ses: bir portu dinler (**port_listen**), diğer porta gönderir (**port_send**).  
**Argüman sırası:**  
`<capture> <playback> <ip> <port_send> <port_listen> <frame> <ch> <sbytes> <rate> <mode>`

- **TX (gönderici)**: `mode=0` ise **udp_sender** (PCM), `mode=1` ise **codec_sender** (Opus) kullanılır.  
- **RX (alıcı)**: **full_automatic_receiver**; paket başlığından parametreleri okuyup uygun biçimde çalar.

> **Not:** `frame * ch * sbytes` değeri paket `payload` kapasitesini aşmamalıdır (projedeki `AudioPacket.payload` 4000 B). Örn: `960 * 1 * 2 = 1920` güvenli.

#### 4.1) Yerel test (aynı makine, iki kart)
Aşağıda **A** süreci `127.0.0.1:5001`’e mikrofonu yollar, `5000`’i dinler (card1).  
**B** süreci `127.0.0.1:5000`’e yollar, `5001`’i dinler (card2).

**PCM (mode=0):**
```bash
# Terminal 1 (A / card1):
./ses --full-duplex plughw:1,0 plughw:1,0 127.0.0.1 5001 5000 960 1 2 48000 0
# Terminal 2 (B / card2):
./ses --full-duplex plughw:2,0 plughw:2,0 127.0.0.1 5000 5001 960 1 2 48000 0
```

**Opus (mode=1):**
```bash
# Terminal 1 (A / card1):
./ses --full-duplex plughw:1,0 plughw:1,0 127.0.0.1 5001 5000 960 1 2 48000 1
# Terminal 2 (B / card2):
./ses --full-duplex plughw:2,0 plughw:2,0 127.0.0.1 5000 5001 960 1 2 48000 1
```

#### 4.2) Uzak test (iki farklı cihaz)
Varsayalım A’nın IP’si `192.168.1.20`, B’nin IP’si `192.168.1.21`.

**A cihazı** (mic→B:5001, dinle B→A:5000):
```bash
./ses --full-duplex plughw:1,0 plughw:1,0 192.168.1.21 5001 5000 960 1 2 48000 0   # PCM
# veya
./ses --full-duplex plughw:1,0 plughw:1,0 192.168.1.21 5001 5000 960 1 2 48000 1   # Opus
```

**B cihazı** (mic→A:5000, dinle A→B:5001):
```bash
./ses --full-duplex plughw:2,0 plughw:2,0 192.168.1.20 5000 5001 960 1 2 48000 0   # PCM
# veya
./ses --full-duplex plughw:2,0 plughw:2,0 192.168.1.20 5000 5001 960 1 2 48000 1   # Opus
```

---

## İpuçları & Sorun Giderme

- **Cızırtı/underrun (EPIPE)**: `frame` değerini büyütmeyi deneyin (960 → 1440/1920). Sistem yükünü azaltın.  
- **Format uyuşmazlığı**: `plughw:` çoğu durumda otomatik dönüştürür; tam kontrol istiyorsanız `hw:` kullanın (cihaza birebir ayar gerekir).  
- **Cihaz görünmüyor**: `aplay -l` ve `arecord -l` ile kart/cihaz numaralarını kontrol edin.  
- **UDP portu kullanımda**: Başka port deneyin veya ilgili süreci sonlandırın (`lsof -i :5000`).  
- **Payload sınırı**: `frame*ch*sbytes ≤ 4000` kuralına dikkat edin (ör. 960*1*2=1920 güvenlidir).

---

## Notlar

- Opus için önerilen ayar: **48 kHz**, **20 ms** paket (frame=960), **mono**.  
- Aynı ALSA kartını iki süreç aynı anda kullanamıyorsa kartları ayırın ya da önce bir süreci kapatın.  
- Gelişmiş senaryolarda jitter/ring buffer katmanı ekleyebilirsiniz (projekodunda altyapısı mevcut).