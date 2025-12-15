#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"

int main(int argc, char *argv[]) {
    // Kullanıcının programı çalıştırırken bir giriş dosyası belirtip belirtmediğini kontrol eder. (giris.txt)
    if (argc < 2) {
        printf("Kullanim: ./freertos_sim <giris_dosyasi>\n");
        return 1;
    }

    // Komut satırından gelen dosya adını (argv[1]) kullanarak readInputFile fonksiyonunu çağırır. Bu fonksiyon, tasks dizisini doldurur.
    readInputFile(argv[1]);
    xTaskCreate(schedulerTask, "Scheduler", 2048, NULL, 4, NULL); // Simülasyonu yönetecek olan ana görevi (Master Task) oluşturur.
    printf("Simulasyon baslatiliyor...\n");
    vTaskStartScheduler(); // FreeRTOS zamanlayıcısını başlatır.
    return 0;
}
