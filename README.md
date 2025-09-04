# `ses` — Kullanım ve Çalıştırma Rehberi

ALSA tabanlı **loopback**, **PCM (codecsiz) gönderim/alım**, **Opus (codec'li) gönderim/alım** ve **iki portlu full-duplex** modlarını içeren komut satırı aracı.

- **TX (gönderici)**: Mikrofon → (PCM/Opus) → UDP  
- **RX (alıcı)**: UDP → (otomatik çözümleme) → hoparlör  
- **Full-Duplex (Yeni)**: Aynı anda **bir porttan dinler**, **diğer porta gönderir**

---

## Derleme (Build)

Ubuntu/Debian için örnek bağımlılıklar ve derleme adımları:

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libasound2-dev libopus-dev

# Hızlı yöntem (klasik)
mkdir -p build && cd build
cmake ..
make -j
```

Alternatif:

```bash
# Proje kökünden
cmake -S . -B build
cmake --build build -j
# ikili: build/ses
```

---

## Komut Özeti

```text
./ses --list-devices
./ses --device-info [capture] [playback]
./ses --loopback [capture] [playback] [frame_size] [channels] [sample_size] [sample_rate]

# Gönderici (TX)
./ses --sender [mode] [capture] [frame_size] [channels] [sample_size] [sample_rate] [port] [ip]

# Alıcı (RX)
./ses --receiver [mode] [playback] [port] [frame_size] [channels] [sample_size] [sample_rate]
./ses --full [playback] [port]     # oto alıcı için kısa yol (varsa)

# Full-Duplex (Yeni, iki port)
./ses --full-duplex <capture> <playback> <ip> <port_send> <port_listen> <frame> <ch> <sbytes> <rate> <mode>
```

### Parametre Sözlüğü

- **capture / playback**: ALSA aygıt adı (örn. `hw:1,0`, `plughw:2,0`).
- **frame_size / frame**: Her `snd_pcm_readi/writei` çağrısında işlenecek frame sayısı (örn. `960` → 20 ms @ 48 kHz).
- **channels / ch**: Kanal sayısı (`1`=mono, `2`=stereo).
- **sample_size / sbytes**: Örnek başına **bayt** (`2` → 16-bit `S16_LE`, `4` → 32-bit `S32_LE`).
- **sample_rate / rate**: Örnekleme hızı (örn. `48000`).
- **port / port_send / port_listen**: UDP port(lar)ı (örn. `5000`, `5001`).
- **ip**: Hedef IP (örn. `127.0.0.1`, ya da karşı cihazın LAN IP’si).
- **mode** (sender): `0 = codecsiz (ham PCM)`, `1 = codec'li (Opus)`  
- **mode** (receiver):  
  - `0 = oto` → İlk paketten parametreleri okuyup yapılandırır.  
  - `1 = codecsiz` → Ham PCM alımı bekler (parametreler CLI’dan alınır).  
  - `2 = codec` → Opus alımı bekler (parametreler CLI’dan alınır).  
  - `3 = full-oto` → Akışı izler, parametre değişirse yeniden yapılandırır.

> **Payload sınırı:** Pakette `AudioPacket.payload = 4000 B`. Bu nedenle **`frame * ch * sbytes ≤ 4000`** olmalı.  
> Örn: `960 * 1 * 2 = 1920` güvenli.

---

## Kullanım Örnekleri

### 1) Cihazları Listeleme

```
./ses --list-devices
```

Sistemdeki **capture** ve **playback** aygıtlarını listeler.

---

### 2) Cihaz Bilgisi

```
./ses --device-info [capture] [playback]
```

Örnek:

```
./ses --device-info hw:1,0 hw:1,0
```

Belirtilen aygıt(lar)ın özet bilgilerini gösterir.

---

### 3) Loopback (Mikrofondan al → Hoparlöre ver)

```
./ses --loopback [capture] [playback] [frame_size] [channels] [sample_size] [sample_rate]
```

Örnek:

```
./ses --loopback plughw:1,0 plughw:2,0 960 1 2 48000
```

---

### 4) Sender (Gönderici)

```
# mode: 0=codecsiz, 1=codec'li
./ses --sender [mode] [capture] [frame_size] [channels] [sample_size] [sample_rate] [port] [ip]
```

Örnekler:

```
./ses --sender 0 plughw:1,0 960 1 2 48000 5000 127.0.0.1
./ses --sender 1 plughw:1,0 960 1 2 48000 5000 127.0.0.1
```

---

### 5) Receiver (Alıcı)

```
# mode: 0=oto, 1=codecsiz, 2=codec, 3=full-oto
./ses --receiver [mode] [playback] [port] [frame_size] [channels] [sample_size] [sample_rate]
./ses --full [playback] [port]
```

Örnekler:

```
./ses --receiver 0 plughw:2,0 5000 960 1 2 48000
./ses --receiver 1 plughw:2,0 5000 960 1 2 48000
./ses --receiver 2 plughw:2,0 5000 960 1 2 48000
./ses --full plughw:2,0 5000
```

---

### 6) **Full-Duplex (Yeni)** — İki Port, Tek Komut

Tek komutta iki yönlü ses: bir portu **dinler** (`port_listen`), diğer porta **gönderir** (`port_send`).  
**Argüman sırası:**  
```
<capture> <playback> <ip> <port_send> <port_listen> <frame> <ch> <sbytes> <rate> <mode>
```

- **TX (gönderici)**: `mode=0` ise **PCM (udp_sender)**, `mode=1` ise **Opus (codec_sender)** kullanılır.  
- **RX (alıcı)**: **full-automatic** mekanizması; paket başlığından parametreleri okuyup uygun biçimde çalar (gelen akış değişirse yeniden ayarlar — projendeki “full-oto” mantığı).

#### 6.1) Yerel test (aynı makine, iki kart)

Aşağıda **A** süreci `127.0.0.1:5001`’e mikrofonu yollar, `5000`’i dinler.  
**B** süreci `127.0.0.1:5000`’e yollar, `5001`’i dinler.

**PCM (mode=0):**
```bash
# Terminal 1 (A):
./ses --full-duplex plughw:1,0 plughw:1,0 127.0.0.1 5001 5000 960 1 2 48000 0
# Terminal 2 (B):
./ses --full-duplex plughw:2,0 plughw:2,0 127.0.0.1 5000 5001 960 1 2 48000 0
```

**Opus (mode=1):**
```bash
# Terminal 1 (A):
./ses --full-duplex plughw:1,0 plughw:1,0 127.0.0.1 5001 5000 960 1 2 48000 1
# Terminal 2 (B):
./ses --full-duplex plughw:2,0 plughw:2,0 127.0.0.1 5000 5001 960 1 2 48000 1
```

#### 6.2) Uzak test (iki farklı cihaz)

Varsayalım **A**’nın IP’si `192.168.1.20`, **B**’nin IP’si `192.168.1.21`.

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

## Hızlı Başlangıç (Aynı Makinede Test)

1. **Terminal-1 (Receiver):**
   ```bash
   ./ses --receiver 0 plughw:2,0 5000
   # ya da codecsiz sabit parametrelerle:
   # ./ses --receiver 1 plughw:2,0 5000 960 1 2 48000
   ```

2. **Terminal-2 (Sender):**
   ```bash
   ./ses --sender 0 plughw:1,0 960 1 2 48000 5000 127.0.0.1
   ```

3. **(İsteğe bağlı) Full-Duplex hızlı test:**
   ```bash
   # A:
   ./ses --full-duplex plughw:1,0 plughw:1,0 127.0.0.1 5001 5000 960 1 2 48000 0
   # B:
   ./ses --full-duplex plughw:2,0 plughw:2,0 127.0.0.1 5000 5001 960 1 2 48000 0
   ```

4. **Durdurma:** `Ctrl+C`

---

## İpuçları & Sorun Giderme

- **Bozuk/uyumsuz ses**: Gönderici ve alıcıda `frame_size / channels / sample_size / sample_rate` uyumsuz olabilir. `plughw:` çoğu durumda otomatik dönüştürme sağlar; kesin eşleşme için `hw:` kullanın.  
- **Underrun/overrun (EPIPE)**: `frame_size` değerini artırın (örn. 960 → 1440/1920). `snd_pcm_recover()` kullanın, sistem yükünü azaltın, gerçek-zaman önceliği düşünebilirsiniz.  
- **Cihaz görünmüyor**: `aplay -l` ve `arecord -l` ile kart/cihaz numaralarını kontrol edin. PipeWire/PulseAudio köprüsü olan sistemlerde `plughw:` daha stabil olabilir.  
- **UDP portu kullanımda**: Farklı bir `port` seçin veya ilgili süreci sonlandırın (örn. `lsof -i :5000`).  
- **Payload sınırı**: `frame*ch*sbytes ≤ 4000` kuralına dikkat edin (ör. `960*1*2=1920` güvenlidir).  
- **Aynı kartı iki süreç**: Bazı ALSA kartları aynı anda çoklu erişime izin vermez; kartları ayırın veya önce bir süreci kapatın.

---

## Notlar

- Opus modunda 48 kHz ve 20 ms paket (960 frame) düşük gecikme için uygundur.  
- `sample_size=4` (32-bit) bazı donanımlarda `hw:` ile desteklenmeyebilir; `plughw:` üzerinden dönüştürme gerekebilir.  
- Ağ üzerinde paket kaybı/jitter varsa daha büyük `frame_size` ve tamponlar deneyin; gerekirse ring/jitter buffer katmanı ekleyin.  
- **Terminoloji:** *Full-duplex* = Aynı anda çift yönlü iletişim (eşzamanlı gönderim + alım).