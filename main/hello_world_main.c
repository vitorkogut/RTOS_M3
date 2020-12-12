/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "freertos/task.h"
#include "esp_timer.h"
#define LIMITE_DA_ESTEIRA 150


#ifdef CONFIG_IDF_TARGET_ESP32
#define CHIP_NAME "ESP32"
#endif

#ifdef CONFIG_IDF_TARGET_ESP32S2BETA
#define CHIP_NAME "ESP32-S2 Beta"
#endif


int ESTEIRA_1;
int ESTEIRA_2;
int ESTEIRA_3;
TaskHandle_t handler_esteira_01 = NULL;
TaskHandle_t handler_esteira_02 = NULL;
TaskHandle_t handler_esteira_03 = NULL;
TaskHandle_t handler_contador = NULL;
TaskHandle_t handler_monitor = NULL;
float VETOR_DADOS[LIMITE_DA_ESTEIRA];




void taskEsteira01( void * pvParameters )
{
    while(1 == 1){
        
        vTaskDelay(pdMS_TO_TICKS(1000)); // -> Delay pre definido para leitura

        portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED; // Variavel Mutex
        portENTER_CRITICAL(&mux); // Entrada na seção critica

        VETOR_DADOS[ESTEIRA_1+ESTEIRA_2+ESTEIRA_3] = 5.0;
        ESTEIRA_1 = ESTEIRA_1 + 1;

        portEXIT_CRITICAL(&mux); // Saida da seção critica
   
    }
}

void taskEsteira02( void * pvParameters )
{
    while(1 == 1){
        vTaskDelay(pdMS_TO_TICKS(500));
        portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
        portENTER_CRITICAL(&mux);

        VETOR_DADOS[ESTEIRA_1+ESTEIRA_2+ESTEIRA_3] = 2.0;
        ESTEIRA_2 = ESTEIRA_2 + 1;

        portEXIT_CRITICAL(&mux);
            
    }
}

void taskEsteira03( void * pvParameters )
{
    while(1 == 1){
        vTaskDelay(pdMS_TO_TICKS(100));
        portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
        portENTER_CRITICAL(&mux);

        VETOR_DADOS[ESTEIRA_1+ESTEIRA_2+ESTEIRA_3] = 0.5;
        ESTEIRA_3 = ESTEIRA_3 + 1;

        portEXIT_CRITICAL(&mux);
            
    }
}

void confereValores( void * pvParameters )
{
    while(1 == 1){

        bool STOP = false;
        portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
        portENTER_CRITICAL(&mux);

        if((ESTEIRA_1 + ESTEIRA_2 + ESTEIRA_3) >= LIMITE_DA_ESTEIRA){
            STOP = true;
        }else{
            
        }
        portEXIT_CRITICAL(&mux);

        
        
        if(STOP){
            vTaskSuspend(handler_esteira_01);
            vTaskSuspend(handler_esteira_02);
            vTaskSuspend(handler_esteira_03);

            float peso_total = 0.0;
            for(int i=0; i<LIMITE_DA_ESTEIRA; i++){
                peso_total = peso_total + VETOR_DADOS[i];
            }

            printf("\n------------------------------------------\n");
            printf("ESTEIRA NUMERO 1 CHEGOU A %d\n", ESTEIRA_1);
            printf("ESTEIRA NUMERO 2 CHEGOU A %d\n", ESTEIRA_2);
            printf("ESTEIRA NUMERO 3 CHEGOU A %d\n", ESTEIRA_3);
            printf("\nPESO TOTAL DOS ITENS: %f\n", peso_total);
            printf("------------------------------------------\n");

            ESTEIRA_1 = 0;
            ESTEIRA_2 = 0;
            ESTEIRA_3 = 0;
            
            vTaskResume(handler_esteira_01);
            vTaskResume(handler_esteira_02);
            vTaskResume(handler_esteira_03);

            STOP = false;
            
            //vTaskDelay(pdMS_TO_TICKS(5000));

            
        }
        vTaskDelay(pdMS_TO_TICKS(15));

    }
}

void taskMonitor( void * pvParameters )
{  
    while(1 == 1){

        printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n-------------------------\n");
        printf("ESTEIRA NUMERO 1: %d\n", ESTEIRA_1);
        printf("ESTEIRA NUMERO 2: %d\n", ESTEIRA_2);
        printf("ESTEIRA NUMERO 3: %d\n", ESTEIRA_3);
        printf("-------------------------\n");

        vTaskDelay(pdMS_TO_TICKS(2000));    
    }
}


void app_main(void)
{
    
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    
    ESTEIRA_1 = 0;
    ESTEIRA_2 = 0;
    ESTEIRA_3 = 0;

    for(int i=0; i<LIMITE_DA_ESTEIRA; i++){
        VETOR_DADOS[i] = 0;
    }
    

    printf("VOU CHAMAR AS TASK EM 2 SEC\n");
    vTaskDelay(pdMS_TO_TICKS(2000));

    xTaskCreate( taskEsteira01, "esteira01", 2048, NULL, 2, &handler_esteira_01 );
    xTaskCreate( taskEsteira02, "esteira02", 2048, NULL, 2, &handler_esteira_02 );
    xTaskCreate( taskEsteira03, "esteira03", 2048, NULL, 2, &handler_esteira_03 );
    xTaskCreate( confereValores, "conferidor", 4048, NULL, 1, &handler_contador );
    xTaskCreate( taskMonitor, "monitor", 2048, NULL, 2, &handler_monitor );
   
    
}




