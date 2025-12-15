#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H


#define configUSE_PREEMPTION 1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0 
#define configUSE_IDLE_HOOK 0 
#define configUSE_TICK_HOOK 0 
#define configCPU_CLOCK_HZ ( ( unsigned long ) 100000000 )
#define configTICK_RATE_HZ ( ( TickType_t ) 1000 )
#define configMAX_PRIORITIES ( 30 ) // Maksimum Öncelik Sayısı: 30 . Yüksek bir sayı verdik tüm öncelikleri kontrol edebilelim diye
#define configMINIMAL_STACK_SIZE ( ( unsigned short ) 128 )
#define configTOTAL_HEAP_SIZE ( ( size_t ) ( 65 * 1024 ) )
#define configMAX_TASK_NAME_LEN ( 16 )
#define configUSE_TRACE_FACILITY 1
#define configUSE_16_BIT_TICKS 0
#define configIDLE_SHOULD_YIELD 1
#define configUSE_MUTEXES 1
#define configCHECK_FOR_STACK_OVERFLOW 0
#define configUSE_RECURSIVE_MUTEXES 1
#define configQUEUE_REGISTRY_SIZE 20
#define configUSE_APPLICATION_TASK_TAG 0
#define configUSE_COUNTING_SEMAPHORES 1
#define configUSE_ALTERNATIVE_API 0
#define configUSE_QUEUE_SETS 1
#define configUSE_TASK_NOTIFICATIONS 1
#define configUSE_TIMERS 1
#define configTIMER_TASK_PRIORITY ( configMAX_PRIORITIES - 1 )
#define configTIMER_QUEUE_LENGTH 5
#define configTIMER_TASK_STACK_DEPTH ( configMINIMAL_STACK_SIZE * 2 )

#define INCLUDE_vTaskPrioritySet 1 // Öncelik değiştirme fonksiyonu
#define INCLUDE_uxTaskPriorityGet 1 // Öncelik okuma fonksiyonu
#define INCLUDE_vTaskDelete 1 // Görev silme (vTaskDelete)
#define INCLUDE_vTaskCleanUpResources 0 
#define INCLUDE_vTaskSuspend 1 // Görev askıya alma (vTaskSuspend)
#define INCLUDE_vTaskDelayUntil 1 // Mutlak zamanlı bekleme
#define INCLUDE_vTaskDelay 1 // Bağıl zamanlı bekleme (vTaskDelay)
#define INCLUDE_xTaskGetSchedulerState 1 // Scheduler durumunu okuma

#endif
