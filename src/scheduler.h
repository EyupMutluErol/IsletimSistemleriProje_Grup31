#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define MAX_TASKS 100 // Sistemin kabul edebileceği maksimum görev sayısı 
#define TIME_QUANTUM_MS 1000 // Kullanıcı görevlerinin (User Tasks) kesintisiz çalışabileceği süre (1 sn)


// Bir görevin (task/process) tüm bilgilerini tutan yapı
typedef struct {
    int id; // Görevin benzersiz kimlik numarası
    int arrivalTime; // Sisteme varış zamanı (Giriş dosyasından okuyoruz)
    int priority; // Başlangıç önceliği
    int burstTime; // Toplam çalışma süresi (saniye olarak)
    int remainingTime; // Tamamlanması için kalan süre
    int startTime; // Görevin ilk CPU aldığı zaman
    int completionTime; // Görevin bittiği zaman
    int state; // Görevin anlık durumu (0:Bekliyor , 1:Calışıyor , 2:Bitti , 3:Askıda)
    int dynamicPriority; // Değişken öncelik değeri. Geri Beslemeli Kuyruk (Feedback Queue) mantığı gereği bu değer çalıştırıldıkça artar (öncelik düşer)
    TaskHandle_t taskHandle; // FreeRTOS'un bu görevi yönetmek için kullandığı işaretçi
    int colorID; // Konsol çıktısında göreve atanacak renk kodu indeksi
    int lastActiveTime;  // Görevin en son ne zaman çalıştığı veya sisteme girdiği zaman. Zaman Aşımı (20 sn) kontrolü için kullanılır
    //Round Robin / Fair Queue sıralamasında En Çok Bekleyeni bulmak için kullanılır
} TaskInfo;


// Fonksiyon Prototipleri
void readInputFile(const char *filename); // Dosyayı okur, tasks dizisini doldurur
void schedulerTask(void *pvParameters); // Ana Zamanlayıcı (Scheduler) görevi
void simulationTask(void *pvParameters); // İşçi görev simülasyonu

// Global Değişkenler
extern TaskInfo tasks[MAX_TASKS]; // Tüm görevlerin listesi
extern int taskCount; // Yüklenen toplam görev sayısı
extern int systemTime; // Global simülasyon saati


#endif
