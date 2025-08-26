# `ses` — Kullanım ve Çalıştırma Rehberi

ALSA tabanlı **loopback**, **PCM (codecsiz) gönderim/alım** ve **Opus (codec'li) gönderim/alım** modlarını içeren komut satırı aracı.

---

## Derleme (Build)

Ubuntu/Debian için örnek bağımlılıklar ve derleme adımları:

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libasound2-dev libopus-dev

cmake ..
make
```

Çalıştırma (ikili dosya: `ses`):

```bash
./build/ses --list-devices
```

> Not: Projeyi farklı klasörde derlediyseniz ikili dosya yolunu buna göre değiştirin.

---

## Komut Özeti

```text
./ses --list-devices
./ses --device-info [capture] [playback]
./ses --loopback [capture] [playback] [frame_size] [channels] [sample_size] [sample_rate]
./ses --sender [mode] [capture] [frame_size] [channels] [sample_size] [sample_rate] [port] [ip]
./ses --receiver [mode] [playback] [port] [frame_size] [channels] [sample_size] [sample_rate]
```

### Parametre Sözlüğü

- **capture / playback**: ALSA aygıt adı (örn. `hw:1,0`, `plughw:2,0`).
- **frame_size**: Her `snd_pcm_readi/writei` çağrısında işlenecek frame sayısı (örn. `960`).
- **channels**: Kanal sayısı (`1`=mono, `2`=stereo).
- **sample_size**: Örnek başına **bayt** (`2` → 16‑bit `S16_LE`, `4` → 32‑bit `S32_LE`).
- **sample_rate**: Örnekleme hızı (örn. `48000`).
- **port**: UDP portu (örn. `5000`).
- **ip**: Hedef IP (örn. `127.0.0.1`).

### Modlar

- **Sender `mode`**: `0 = codecsiz (ham PCM)`, `1 = codec'li (Opus)`
- **Receiver `mode`**:
  - `0 = oto` → İlk paketten parametreleri okuyup yapılandırır.
  - `1 = codecsiz` → Ham PCM alımı bekler (parametreler komut satırından alınır).
  - `2 = codec` → Opus alımı bekler (parametreler komut satırından alınır).
  - `3 = full-oto` → Akışı izler, parametre değişirse yeniden yapılandırır.

> **İpucu:** `oto` ve `full-oto` modlarında çoğu durumda yalnızca `[playback]` ve `[port]` vermeniz yeterlidir. Diğer parametreler paketten algılanıyorsa yok sayılabilir.

---

## Kullanım Örnekleri

### 1) Cihazları Listeleme

```
./ses --list-devices
```

Sistemdeki **capture** ve **playback** aygıtlarını isimleriyle listeler.

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
```

Örnekler:

```
./ses --receiver 0 plughw:2,0 5000 960 1 2 48000
./ses --receiver 1 plughw:2,0 5000 960 1 2 48000
./ses --receiver 2 plughw:2,0 5000 960 1 2 48000
./ses --receiver 3 plughw:2,0 5000
```

---

## Hızlı Başlangıç (Aynı Makinede Test)

1. **Terminal‑1 (Receiver):**
   ```bash
   ./ses --receiver 0 plughw:2,0 5000
   # ya da codecsiz sabit parametrelerle:
   # ./ses --receiver 1 plughw:2,0 5000 960 1 2 48000
   ```

2. **Terminal‑2 (Sender):**
   ```bash
   ./ses --sender 0 plughw:1,0 960 1 2 48000 5000 127.0.0.1
   ```

3. **Durdurma:** `Ctrl+C`

---

## İpuçları & Sorun Giderme

- **Bozuk/uyumsuz ses**: Gönderici ve alıcıda `frame_size / channels / sample_size / sample_rate` uyumsuz olabilir. `plughw:` çoğu durumda otomatik dönüştürme sağlar; kesin eşleşme için `hw:` kullanın.
- **Underrun/overrun (EPIPE)**: `frame_size` değerini artırın (örn. 960 → 1920). Sistem yükünü azaltın, gerçek‑zaman önceliği düşünün.
- **Cihaz görünmüyor**: `aplay -l` ve `arecord -l` ile kart/cihaz numaralarını kontrol edin. PipeWire/PulseAudio köprüsü olan sistemlerde `plughw:` daha stabil olabilir.
- **UDP portu kullanımda**: Farklı bir `port` seçin veya ilgili süreci sonlandırın (örn. `lsof -i :5000`).

---

## Notlar

- Opus modunda 48 kHz ve 20 ms paket (960 frame) düşük gecikme için uygundur.
- `sample_size=4` (32‑bit) bazı donanımlarda `hw:` ile desteklenmeyebilir; `plughw:` üzerinden dönüştürme gerekebilir.
- Ağ üzerinde paket kaybı/jitter varsa daha büyük `frame_size` ve tamponlar deneyin.