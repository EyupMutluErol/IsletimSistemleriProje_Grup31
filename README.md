# İşletim Sistemleri Projesi - FreeRTOS Scheduler Simülasyonu (Grup 31)

Bu proje, **FreeRTOS Kernel** kullanılarak Linux (POSIX) ortamında çalışan bir **Görev Planlayıcı (Scheduler)** simülasyonudur. Proje, verilen bir giriş dosyasındaki (`giris.txt`) görevleri okur ve belirlenen algoritmalarla (FCFS, Round Robin, Feedback Queue) bu görevleri yönetir.

## Özellikler

Proje aşağıdaki isterleri karşılayacak şekilde tasarlanmıştır:
* **Hibrit Planlama:** Gerçek Zamanlı (Real-Time) ve Kullanıcı (User) görevlerinin birlikte yönetimi.
* **FCFS (First Come First Served):** Önceliği 0 olan RT görevler varış sırasına göre çalıştırılır.
* **Geri Beslemeli Kuyruk (Feedback Queue):** Kullanıcı görevleri (Öncelik > 0) çalışıp sürelerini doldurduklarında öncelikleri düşürülür (sayısal değeri artar).
* **Zaman Aşımı Kontrolü (20 sn):** Bir görev sisteme giriş yaptığı andan (Arrival Time) itibaren işlem görmeden 20 saniye beklerse, **"proses zamanasimi"** hatası ile sonlandırılır.
* **Askıya Alma (Suspend/Resume):** Sırası gelen görev `vTaskResume` ile uyandırılır, sırası geçen görev `vTaskSuspend` ile askıya alınır.
* **Renkli Çıktı:** Her görev terminalde farklı bir renkle temsil edilir.

## Dosya Yapısı

* **FreeRTOS/**: Orijinal FreeRTOS çekirdek dosyaları ve kütüphaneleri.
* **src/**: Projenin kaynak kodları.
    * `main.c`: Programın giriş noktası ve dosya okuma başlatıcı.
    * `scheduler.c`: Planlama algoritması (Task seçimi ve zaman aşımı kontrolü).
    * `tasks.c`: Görevlerin simülasyonu ve ekrana yazdırma işlemleri.
    * `scheduler.h`: Veri yapıları ve prototipler.
    * `FreeRTOSConfig.h`: FreeRTOS sistem ayarları.
* **Makefile**: Derleme kuralları.
* **giris.txt**: Test için kullanılan görev listesi.

## Derleme ve Çalıştırma

Projeyi derlemek ve çalıştırmak için terminalde aşağıdaki komutları kullanın:

1.  **Projeyi Derle:**
    \`\`\`bash
    make
    \`\`\`

2.  **Simülasyonu Başlat:**
    \`\`\`bash
    ./freertos_sim giris.txt
    \`\`\`

3.  **Temizle (İsteğe Bağlı):**
    \`\`\`bash
    make clean
    \`\`\`

## Grup Üyeleri
* Eyüp Mutlu Erol
* Ömer Yazıcıgil
* Muhammet Mustafa Kangal
* Batuhan Bektaş
* Alperen Somuncuoğlu

---
*Bu proje İşletim Sistemleri dersi kapsamında geliştirilmiştir.*
