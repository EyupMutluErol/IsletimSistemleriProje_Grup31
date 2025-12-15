#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"

const char *colors[] = {
    "\033[0;31m", "\033[0;32m", "\033[0;33m", "\033[0;34m", "\033[0;35m", "\033[0;36m"
};
#define RESET_COLOR "\033[0m" 


/*
Bu fonksiyon, sistemdeki her bir Prosesin davranışını simüle eder
FreeRTOS'un xTaskCreate fonksiyonu ile her görev için bu fonksiyondan bir kopya (instance) oluşturulur
*/
void simulationTask(void *pvParameters) {
    TaskInfo *task = (TaskInfo *)pvParameters; // xTaskCreate ile gönderilen görev bilgilerini (TaskInfo yapısını) alıyoruz
    int colorIndex = task->colorID % 6; // Görevin Id'sine göre renk seçimi
    char buffer[128]; // Ekrana yazılacak mesaj için geçici bellek
    TickType_t lastWakeTime;
    const TickType_t frequency = pdMS_TO_TICKS(1000); // Simülasyonda 1 saniye = 1000 Tick olarak ayarladık

    lastWakeTime = xTaskGetTickCount(); // Mevcut zamanı al (Zamanlama referansı için).
    
    // İl başlama mesajı
    // Görev ilk kez oluşturulduğunda ve Scheduler tarafından seçildiğinde burası çalışır.
    snprintf(buffer, sizeof(buffer), "%s%d.0000 sn proses basladi\t(id:%04d\toncelik:%d\tkalan sure:%d sn)%s",
             colors[colorIndex], systemTime, task->id, task->dynamicPriority, task->remainingTime, RESET_COLOR);
    printf("%s\n", buffer);

    // Çıktının tampon bellekte (buffer) beklemeden anında terminale basılmasını sağlar
    fflush(stdout);

    // Görevin kalan süresi bitene kadar bu döngü devam eder.
    while (task->remainingTime > 0) {
        // vTaskDelayUntil , görevi tam olarak frequency (1 sn) kadar uyutur. Bu süre zarfında görev blocked durumundadır ve CPU kullanmaz.
        vTaskDelayUntil(&lastWakeTime, frequency);
        
        // Zaman güncellemeleri
        task->remainingTime--; // Kalan süreyi azalt
        systemTime++; // Sistem saatini ilerlet
        
        /*
        Görev çalıştığı (aktif olduğu) an zaman damgasını güncelliyoruz
        Bu işlem Scheduler'daki en çok bekleyen (Fair Queue) algoritması için gereklidir
        Zamanı güncellenen görev yeni çalışmış sayılır ve kuyruğun sonuna atılmış olur
        */
        task->lastActiveTime = systemTime;

        // Durum kontrolü
        if (task->remainingTime == 0) {
            // Görev tamamlandı
            snprintf(buffer, sizeof(buffer), "%s%d.0000 sn proses sonlandi\t(id:%04d\toncelik:%d\tkalan sure:0 sn)%s",
                     colors[colorIndex], systemTime, task->id, task->dynamicPriority, RESET_COLOR);
            printf("%s\n", buffer);
            fflush(stdout);
            task->state = 2; // Durumu Bitti (2) yap
            vTaskDelete(NULL); // Görevi bellekten ve FreeRTOS listesinden sil
        } else {
            /*
            Eğer kullanıcı görevi (User Task, Öncelik > 0) ise
            Geri Beslemeli Kuyruk (Feedback Queue) mantığını uyguluyoruz
            */
            if (task->priority > 0) {
                
                // 1 Saniyelik Quantum süresi doldu . Öncelik değerini artırıyoruz yani önceliği düşürüyoruz aslında
                task->dynamicPriority++; 
                
                // Askıya alma mesajı
                snprintf(buffer, sizeof(buffer), "%s%d.0000 sn proses askida\t(id:%04d\toncelik:%d\tkalan sure:%d sn)%s",
                     colors[colorIndex], systemTime, task->id, task->dynamicPriority, task->remainingTime, RESET_COLOR);
                printf("%s\n", buffer);
                fflush(stdout);

                task->state = 3; // 3 durumuna ayarlıyoruz (askıda durumu)
                vTaskSuspend(NULL); // Durduruyoruz

                // Kod akışı burada durur. Scheduler başka bir görevi seçer. Scheduler tekrar vTaskResume(bu_gorev) diyene kadar alt satıra geçmez
                
                // Tekrar devam kısmı
                task->state = 1; // Durumu tekrar çalışıyor yap (State 1 çalışıyor durumu)
                lastWakeTime = xTaskGetTickCount(); // Zamanlamayı sıfırla
                
                // Tekrar başladı mesajı
                snprintf(buffer, sizeof(buffer), "%s%d.0000 sn proses basladi\t(id:%04d\toncelik:%d\tkalan sure:%d sn)%s",
                     colors[colorIndex], systemTime, task->id, task->dynamicPriority, task->remainingTime, RESET_COLOR);
                printf("%s\n", buffer);
                fflush(stdout);

            } else {
                // Eğer Gerçek Zamanlı Görev (RT) Öncelik 0 ise askıya alınmaz, kesintisiz çalışmaya devam eder
                snprintf(buffer, sizeof(buffer), "%s%d.0000 sn proses yurutuluyor\t(id:%04d\toncelik:%d\tkalan sure:%d sn)%s",
                     colors[colorIndex], systemTime, task->id, task->dynamicPriority, task->remainingTime, RESET_COLOR);
                printf("%s\n", buffer);
                fflush(stdout);
            }
        }
    }
    vTaskDelete(NULL); // Güvenlik önlemi olarak döngüden çıkarsa siliyoruz
}
