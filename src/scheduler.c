#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"

// Konsol çıktısında her görevin (task) farklı bir renkle görünmesini sağlayan renk kodları dizisi
const char *colors_sched[] = {
    "\033[0;31m", "\033[0;32m", "\033[0;33m", "\033[0;34m", "\033[0;35m", "\033[0;36m"
};
#define RESET_COLOR "\033[0m" // renk sıfırlama

TaskInfo tasks[MAX_TASKS]; // Tüm görevlerin bilgilerini tutan ana dizi
int taskCount = 0; // Toplam okunan görev sayısı
int systemTime = 0; // Simülasyonun o anki zamanı (saniye cinsinden)


// Verilen giriş dosyasını (giris.txt) satır satır okur ve tasks dizisine kaydeder
void readInputFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) { perror("Dosya Hatasi"); exit(1); }
    char line[256];
    int idCounter = 0; // Görevlere 0'dan başlayarak Id atamak için sayaç
    
    while (fgets(line, sizeof(line), file)) {
        if (taskCount >= MAX_TASKS) break; // Dizi taşmasını önlemek için
        char *token = strtok(line, ","); // Satırı virgüllere göre parçalama
        if (token) {
            tasks[taskCount].arrivalTime = atoi(token); // Varış zamanı
            tasks[taskCount].priority = atoi(strtok(NULL, ",")); // Öncelik
            tasks[taskCount].burstTime = atoi(strtok(NULL, ",")); // Toplam çalışma süresi
            // Diğer özelliklere ilk değerlerini atıyoruz
            tasks[taskCount].id = idCounter; 
            tasks[taskCount].remainingTime = tasks[taskCount].burstTime; // Kalan süre başta toplama eşittir 
            tasks[taskCount].dynamicPriority = tasks[taskCount].priority; // Dinamik öncelik başlangıçta statik olana eşittir
            tasks[taskCount].state = 0; // 0: Başlamadı, 1: Çalışıyor, 2: Bitti, 3: Askıda --> Başlangıçta başlamadığı için 0 veriyoruz
            tasks[taskCount].colorID = idCounter++; // Renk seçimi için Id
            tasks[taskCount].taskHandle = NULL; // FreeRTOS görev tutucusu henüz yok
            tasks[taskCount].lastActiveTime = tasks[taskCount].arrivalTime; // lastActiveTime: Zaman aşımı kontrolü ve kuyruk sıralaması için kullanılır
            taskCount++;
        }
    }
    fclose(file);
}

/*
    Ana Zamanlayıcı (Sistemin Ana Mantığı)
    * Sürekli döngü içinde çalışır ve sırasıyla şu işlemleri yapar 
    1.Zaman aşımına uğrayan görevleri sonlandırır
    2.Yeni gelen görevleri sisteme dahil eder
    3.Hangi görevin çalışacağına karar verir (Scheduling Algoritması)
    4.FreeRTOS görevlerini durdurur veya devam ettirir
*/
void schedulerTask(void *pvParameters) {
    (void)pvParameters;
    TaskInfo *runningTask = NULL;

    while (1) {
        /* Zamanaşımı kontrolü (20 sn)
        Kural: Bir görev sisteme girdikten sonra veya en son çalıştıktan sonra
        20 saniye boyunca işlem görmezse sonlandırılacak şekilde sistemi tasarladık.
        */
        for (int i = 0; i < taskCount; i++) {
            if (tasks[i].state != 2 && (systemTime - tasks[i].lastActiveTime) >= 20) {
                int colorIndex = tasks[i].colorID % 6;
                printf("%s%d.0000 sn proses zamanasimi\t(id:%04d\toncelik:%d\tkalan sure:%d sn)%s\n",
                     colors_sched[colorIndex], systemTime, tasks[i].id, tasks[i].dynamicPriority, tasks[i].remainingTime, RESET_COLOR);
                fflush(stdout);
                
                tasks[i].state = 2; // Durumu Bitti veya Ölü olarak işaretle
                if (tasks[i].taskHandle != NULL) {
                    vTaskDelete(tasks[i].taskHandle); // FreeRTOS tarafından sil
                    tasks[i].taskHandle = NULL;
                }
                if (runningTask == &tasks[i]) runningTask = NULL; // Eğer çalışan görev öldüyse, çalışan göstergesini sıfırla
            }
        }

        // Yeni gelen görevi başlat 
        // O anki zamana (systemTime) eşit varış zamanı olan görevleri oluşturur
        for (int i = 0; i < taskCount; i++) {
            if (tasks[i].arrivalTime == systemTime && tasks[i].taskHandle == NULL) {
                // FreeRTOS görevi oluşturulur ancak hemen askıya alınır Çünkü çalışıp çalışmayacağına Scheduler karar verecektir
                xTaskCreate(simulationTask, "SimTask", 1024, &tasks[i], 1, &tasks[i].taskHandle);
                vTaskSuspend(tasks[i].taskHandle);
                tasks[i].state = 1; // Durumu hazır yapıyoruz
            }
        }

        /* 
        Eğer şu an çalışan bir görev varsa ve durumu hala 1 (Çalışıyor) ise
        Scheduler araya girmez. Görevin 1 saniyelik işini bitirip Askıda (State 3)
        veya Bitti (State 2) durumuna geçmesini bekler
        */
        if (runningTask != NULL && runningTask->state == 1) {
            vTaskDelay(pdMS_TO_TICKS(10)); // Kısa bir süre bekle ve döngü başına dön
            continue; 
        }
        
        // Görev seçimi
        int selectedIdx = -1;
        
        // Önceliği 0 olan görevler en yüksek önceliğe sahiptir
        // Kendi aralarında FCFS mantığına göre seçilirler
        for (int i = 0; i < taskCount; i++) {
            if (tasks[i].state != 2 && tasks[i].taskHandle != NULL && tasks[i].dynamicPriority == 0) {
                // En erken varış zamanına sahip olanı buluruz
                if (selectedIdx == -1 || tasks[i].arrivalTime < tasks[selectedIdx].arrivalTime)
                    selectedIdx = i;
            }
        }

        // Eğer RT görev yoksa, kullanıcı görevlerine bakılır
        // Aynı öncelikteki görevlerden lastActiveTime değeri en küçük olanı (en eskisini) seçeriz
        // Bu şekilde doğal bir Round Robin (Sıralı Döngü) sağlamış oluruz
        if (selectedIdx == -1) {
            for (int p = 1; p <= 30; p++) { // Öncelik 1'den 30'a kadar tara
                int bestIdx = -1;
                long minTime = 2147483647; // Başlangıç için çok büyük bir sayı atadık

                for(int i = 0; i < taskCount; i++) {
                     if (tasks[i].state != 2 && tasks[i].taskHandle != NULL && tasks[i].dynamicPriority == p) {
                         // Kim daha uzun süredir bekliyor? Onu seç
                         if (tasks[i].lastActiveTime < minTime) {
                             minTime = tasks[i].lastActiveTime;
                             bestIdx = i;
                         }
                         // Eğer bekleme süreleri eşitse mesela aynı anda geldilerse ID'si küçük olana (Listedeki sıraya) öncelik veriyoruz
                         else if (tasks[i].lastActiveTime == minTime) {
                             if (bestIdx == -1 || tasks[i].id < tasks[bestIdx].id) {
                                 bestIdx = i;
                             }
                         }
                     }
                }

                if (bestIdx != -1) {
                    selectedIdx = bestIdx;
                    goto TASK_FOUND; // Görev bulundu aramayı bitirdik
                }
            }
        }
        
        TASK_FOUND:
        if (selectedIdx != -1) {
            TaskInfo *nextTask = &tasks[selectedIdx];
            
            // Eğer seçilen görev şu an çalışandan farklıysa değişim yap
            if (runningTask != nextTask) {
                // Eski görevi askıya al (Tabi hala geçerliyse)
                if (runningTask != NULL && runningTask->state != 2) {
                   vTaskSuspend(runningTask->taskHandle);
                }
                runningTask = nextTask;
                
                /*
                Görev çalışmaya başladığı an lastActiveTime güncellenir
                Bu sayede bir sonraki seçimde kuyruğun en sonuna atılmış olur
                Temel Round Robin mantığını uyguluyoruz
                */
                runningTask->lastActiveTime = systemTime; 

                // Yeni görevi uyandır veya başlat hangisi gerekiyorsa
                vTaskResume(runningTask->taskHandle);
            }
        }
        
        // Döngünün çok hızlı dönüp CPU'yu kilitlememesi için bir önlem
        vTaskDelay(pdMS_TO_TICKS(100));
        
        // Simülsayon bitiş kontrolü
        int allFinished = 1;
        for(int i=0; i<taskCount; i++) if(tasks[i].state != 2) allFinished = 0;
        
        // Eğer tüm görevler bittiyse (State 2 olduysa) simülasyonu bitir
        if(allFinished && systemTime > 0) {
            printf("Tum gorevler tamamlandi. Simulasyon bitiyor.\n");
            fflush(stdout);
            vTaskEndScheduler(); // FreeRTOS Kernel'i durdur
            exit(0);
        }
    }
}
