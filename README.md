# ALSA UDP Audio (Raw PCM & Opus)

Bu proje, ALSA üzerinden ses yakalayıp UDP ile gönderen ve alan **CLI** araçları içerir. Kodda **ham PCM (codec_type=0)** ve **Opus (codec_type=1)** paketleme mantığı mevcuttur ve `AudioPacket` yapısı üzerinden çalışır.

## İçerik (dosyalar)
- `main.c / main.h` — Komut satırı arayüzü ve alt modüllere yönlendirme
- `alsa.c / alsa.h` — ALSA cihaz açma/kapama, loopback ve cihaz bilgisi yazdırma
- `udp.c / udp.h` — UDP üzerinden **ham PCM** gönderme/alma
- `udp_codec.c / udp_codec.h` — UDP üzerinden **Opus** ile kodlayıp gönderme ve alıcıda çözme
- `audiopacket.h` — Ortak paket yapısı
  
`AudioPacket` yapısı (koddaki haliyle):
```c
typedef struct{
    uint8_t  codec_type;   // 0: raw PCM, 1: Opus
    uint16_t data_length;  // payload uzunluğu (byte)
    char     payload[4000];
} AudioPacket;
```

## Gerekli paketler (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install -y build-essential pkg-config libasound2-dev libopus-dev
```

## Derleme
Tek satırda `gcc` ile (mevcut kaynaklara göre):
```bash
gcc -O2 -Wall -Wextra -o ses \
  main.c alsa.c udp.c udp_codec.c \
  -lasound -lopus
```
> İstersen CMake/Make ekleyebilirsin; mevcut kod doğrudan yukarıdaki komutla derlenebilir.

## Komut Satırı Kullanımı (koddaki argüman sırasına göre)

> **Not:** Aşağıdaki alt komutlar `main.c` içindeki `strcmp(argv[1], "...")` bloklarına göre listelenmiştir.

### 1) Cihazları listeleme
```
./ses --list-devices
```
ALSA **PCM** cihaz adlarını listeler (ör. `hw:0,0`, `plughw:1,0`, `default`, vb.).

### 2) Cihaz bilgisi (yakalama + oynatma)
```
./ses --device-info <capture_dev> <playback_dev>
# örnek:
./ses --device-info plughw:2,0 plughw:2,0
```
Seçilen cihazların isimleri ve desteklenen format/oran bilgileri yazdırılır.

### 3) Yerel loopback (mikrofon -> hoparlör)
```
./ses --loopback <capture_dev> <playback_dev> <frame_size> <channels> <sample_size> <sample_rate>

# örnek (mono, 48kHz):
./ses --loopback plughw:2,0 plughw:2,0 960 1 2 48000
```
`frame_size` ve örnekleme ayarları kodda belirtilen sırada fonksiyonlara aktarılır.

### 4) Gönderici (UDP)
**Mod:** `0` = PCM (ham), `1` = Opus

```
./ses --sender <mode> <capture_dev> <frame_size> <channels> <sample_size> <sample_rate> <port> <ip>

# ham PCM örnek:
./ses --sender 0 plughw:2,0 960 1 2 48000 5000 192.168.1.50

# Opus örnek:
./ses --sender 1 plughw:2,0 960 1 2 48000 5000 192.168.1.50
```

### 5) Alıcı (UDP)
**Mod:** `0` = otomatik (packet.header `codec_type`’a göre çözüm),  
`1` = sadece ham PCM,  
`2` = sadece Opus

```
./ses --receiver <mode> <playback_dev> <frame_size> <channels> <sample_size> <sample_rate> <port>

# otomatik örnek:
./ses --receiver 0 plughw:2,0 960 1 2 48000 5000

# sadece PCM:
./ses --receiver 1 plughw:2,0 960 1 2 48000 5000

# sadece Opus:
./ses --receiver 2 plughw:2,0 960 1 2 48000 5000
```

## Önemli Notlar
- **Çerçeve boyutu (frame_size):** 48 kHz için `960` değerinde ~20 ms kare büyüklüğü yaygın VOIP ayarıdır.
- **Örnek boyutu (sample_size):** Kod genelinde **S16_LE** akışı hedeflenir; bu format için `sample_size=2` kullanılır.
- **Aynı parametreler:** Gönderici ve alıcı tarafında `channels / sample_rate / sample_size` uyumlu olmalıdır.
- **Port/IP:** UDP iletimi için alıcı makinede firewall kuralı gerekiyorsa izin verin.

## Hata Ayıklama İpuçları
- `snd_pcm_*` hatalarında `snd_pcm_recover(...)` çağrıları vardır; yine de cihaz ismi (`hw` vs `plughw`) ve izinler (mic erişimi) kontrol edilmelidir.
- Paket tarafında `recvfrom/sendto` hataları konsola yazdırılır.
- `data_length` alanı, `payload`’a yazılan byte sayısıdır; alıcı tarafta frame hesabı `payload_len / (sample_size*channels)` şeklindedir.

---

Bu README, **mevcut kodun arayüz ve akışına göre** hazırlanmıştır; herhangi bir kaynak dosyada değişiklik varsaymaz. 