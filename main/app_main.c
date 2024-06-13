//////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                       _              //
//               _    _       _      _        _     _   _   _    _   _   _        _   _  _   _          //
//           |  | |  |_| |\| |_| |\ |_|   |\ |_|   |_| |_| | |  |   |_| |_| |\/| |_| |  |_| | |   /|    //    
//         |_|  |_|  |\  | | | | |/ | |   |/ | |   |   |\  |_|  |_| |\  | | |  | | | |_ | | |_|   _|_   //
//                                                                                       /              //
//////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
*   Programa básico para controle da placa durante a Jornada da Programação 1
*   Permite o controle das entradas e saídas digitais, entradas analógicas, display LCD e teclado. 
*   Cada biblioteca pode ser consultada na respectiva pasta em componentes
*   Existem algumas imagens e outros documentos na pasta Recursos
*   O código principal pode ser escrito a partir da linha 86
*/

// Área de inclusão das bibliotecas
//-----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "ioplaca.h"   // Controles das Entradas e Saídas digitais e do teclado
#include "lcdvia595.h" // Controles do Display LCD
#include "hcf_adc.h"   // Controles do ADC
#include "MP_hcf.h"   // Controles do ADC


// Área de declaração de variáveis e protótipos de funções
//-----------------------------------------------------------------------------------------------------------------------

#define LIGAR_R1 saidas|0b00000001
#define DESLIGAR_R1 saidas&0b11111110

#define LIGAR_R2 saidas|0b00000010
#define DESLIGAR_R2 saidas&0b11111110

#define LIGAR_Q1 saidas|0b00000100
#define DESLIGAR_Q1 saidas&0b11111110

#define LIGAR_Q2 saidas|0b00001000
#define DESLIGAR_Q2 saidas&0b11111110

#define LIGAR_T1 saidas|0b00010000
#define DESLIGAR_T1 saidas&0b11111110

#define LIGAR_T3 saidas|0b00100000
#define DESLIGAR_T3 saidas&0b11111110

#define LIGAR_T5 saidas|0b01000000
#define DESLIGAR_T5 saidas&0b11111110

#define LIGAR_T6 saidas|0b10000000
#define DESLIGAR_T6 saidas&0b11111110


#define FIM_CURSO (entradas>>7)&1

#define TECLA_0 '0' ==le_teclado() 
#define TECLA_1 '1' ==le_teclado() 
#define TECLA_2 '2' ==le_teclado() 
#define TECLA_3 '3' ==le_teclado() 
#define TECLA_4 '4' ==le_teclado() 
#define TECLA_5 '5' ==le_teclado() 
#define TECLA_6 '6' ==le_teclado() 
#define TECLA_7 '7' ==le_teclado() 
#define TECLA_8 '8' ==le_teclado()

static const char *TAG = "Placa";
static uint8_t entradas, saidas = 0; //variáveis de controle de entradas e saídas

char mensagem[40];
uint32_t valor = 0;

int controle = 0;
int senha = 0;
int coluna = 0;
int resultado = 0;
int vezes = 0;
int tentativa = 0;

char operador;
char tecla;
char mostra[40];

uint32_t valoradc = 0;





// Funções e ramos auxiliares
//-----------------------------------------------------------------------------------------------------------------------

void analogica(void)
{
    esp_err_t read_result = hcf_adc_ler(&valor);

    sprintf(mensagem," %"PRIu32" ", valor);
    lcd595_write(1,0, mensagem);

}
// Programa Principal
//-----------------------------------------------------------------------------------------------------------------------

void app_main(void)
{
    /////////////////////////////////////////////////////////////////////////////////////   Programa principal


    MP_init();

    // a seguir, apenas informações de console, aquelas notas verdes no início da execução
    ESP_LOGI(TAG, "Iniciando...");
    ESP_LOGI(TAG, "Versão do IDF: %s", esp_get_idf_version());

    /////////////////////////////////////////////////////////////////////////////////////   Inicializações de periféricos (manter assim)
    
    // inicializar os IOs e teclado da placa
    ioinit();      
    entradas = io_le_escreve(saidas); // Limpa as saídas e lê o estado das entradas

    ioinit();
    entradas = io_le_escreve (0b00000100);

    printf("Entradas ativas em hexadecimal [binário]: %x [%x %x %x %x %x %x %x %x]\n", entradas, 
                                                                                        (entradas>>7)&1,(entradas>>6)&1,(entradas>>5)&1,(entradas>>4)&1,
                                                                                        (entradas>>3)&1,(entradas>>2)&1,(entradas>>1)&1,entradas&1);
                                                                                

    // inicializar o display LCD 
    lcd595_init();
    lcd595_write(1,1, "JORNADA 1");
    lcd595_write(2,1, "PROGRAMA BASICO");

    
    // Inicializar o componente de leitura de entrada analógica
    esp_err_t init_result = hcf_adc_iniciar();
    if (init_result != ESP_OK) {
        ESP_LOGE("MAIN", "Erro ao inicializar o componente ADC personalizado");
    }

    //delay inicial
    vTaskDelay(1000 / portTICK_PERIOD_MS); 
    lcd595_clear();

    /////////////////////////////////////////////////////////////////////////////////////   Periféricos inicializados

        lcd595_write(1,5, "SENAI");
        vTaskDelay(500 / portTICK_PERIOD_MS); 
        lcd595_clear();
        lcd595_write(1,1, "Isabella Rocha");
        vTaskDelay(1000 / portTICK_PERIOD_MS); 
        lcd595_clear();

    /////////////////////////////////////////////////////////////////////////////////////   Início do ramo principal  

   


    while (1) 
       {
        
        if(valoradc > 450)
    {
        hcf_adc_ler(&valoradc);
        while(valoradc > 450)
        {
            hcf_adc_ler(&valoradc);
            rotacionar_DRV(0,10,saidas);
        }
    }

        if(tecla == 'C')
        {
            senha = 0;
            vezes = 0;
        }


        hcf_adc_ler(&valoradc);

        tecla = le_teclado();

        lcd595_write(1,1, "DIGITE A SENHA");
        lcd595_write(2,1, "[ ] [ ] [ ] [ ]");
        
        if(tecla>='0' && tecla <='9')
        {
            if(controle == 0)
            {
                senha = senha * 10 + tecla - '0';
                vezes = vezes + 1;
            }
        }

        

        if(vezes == 0)
        {
            lcd595_write(1,1, "DIGITE A SENHA");
            lcd595_write(2,1, "[ ] [ ] [ ] [ ]");
        }

        if(vezes == 1)
        {
            lcd595_write(1,1, "DIGITE A SENHA");
            lcd595_write(2,1, "[*] [ ] [ ] [ ]");
        }

        if(vezes == 2)
        {
            lcd595_write(1,1, "DIGITE A SENHA");
            lcd595_write(2,1, "[*] [*] [ ] [ ]");
        }

        if(vezes == 3)
        {
            lcd595_write(1,1, "DIGITE A SENHA");
            lcd595_write(2,1, "[*] [*] [*] [ ]");
        }

         if(vezes == 4)
        {
            lcd595_write(1,1, "DIGITE A SENHA");
            lcd595_write(2,1, "[*] [*] [*] [*]");
        }
      




        if(senha == 1234)

        {
                lcd595_clear();
                lcd595_write(1,1, "ABRINDO COFRE");
                rotacionar_DRV(1,100, saidas);
                lcd595_clear();
                lcd595_write(1,1, "COFRE ABERTO");
                vTaskDelay(8000 / portTICK_PERIOD_MS);
                lcd595_clear();
                lcd595_write(1,1, "FECHANDO COFRE");
                rotacionar_DRV(0,100, saidas);
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                lcd595_clear();
                lcd595_write(1,1, "COFRE FECHADO");
                vTaskDelay(2000 / portTICK_PERIOD_MS);
                lcd595_clear();

                vezes = 0;
                senha = 0;

        }
       

        else if(vezes == 4)
        
        {
            tentativa = tentativa + 1;

            if(tentativa == 1 && vezes == 4)
            {
                lcd595_clear();
                lcd595_write(1,1, "SENHA INCORRETA");
                lcd595_write(2,1, "RESTAM 2 TENTA.");
                vTaskDelay(2000 / portTICK_PERIOD_MS);

                senha = 0;
                vezes = 0;
            }
        
            if(tentativa == 2 && vezes == 4)
            {
                lcd595_clear();
                lcd595_write(1,1, "SENHA INCORRETA");
                lcd595_write(2,1, "RESTA 1 TENTA.");
                vTaskDelay(2000 / portTICK_PERIOD_MS);

                senha = 0;
                vezes = 0;
            }

            if(tentativa == 3 && vezes == 4)
            {
                lcd595_clear();
                lcd595_write(1,0, "MUITAS TENTA.");
                lcd595_write(2,0, "ACESSO BLOQUEADO");
                vTaskDelay(3000 / portTICK_PERIOD_MS);
                lcd595_clear();
                lcd595_write(1,0, "INSIRA SENHA ADM");
                lcd595_write(2,0, "PARA DESBLOQUEAR");
                vTaskDelay(3000 / portTICK_PERIOD_MS);

                senha = 0;
                vezes = 0;
                tentativa = 0;

            if(senha == 1111)
                {
                    
                    lcd595_clear();
                    lcd595_write(1,1, "SENHA CORRETA");
                    vTaskDelay(4000 / portTICK_PERIOD_MS);
                    lcd595_clear();
                    lcd595_write(1,1, "ABRINDO COFRE");
                    rotacionar_DRV(1,100, saidas);
                    lcd595_clear();
                    lcd595_write(1,1, "COFRE ABERTO");
                    vTaskDelay(8000 / portTICK_PERIOD_MS);
                    lcd595_clear();
                    lcd595_write(1,1, "FECHANDO COFRE");
                    rotacionar_DRV(0,100, saidas);
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    lcd595_clear();
                    lcd595_write(1,1, "COFRE FECHADO");
                    vTaskDelay(2000 / portTICK_PERIOD_MS);
                    lcd595_clear();

                    vezes = 0;
                    senha = 0;
                    tentativa = 0;
                  
                }

                else if(vezes == 4 && senha != 1111 && tentativa == 3)
                {
                    lcd595_clear();
                    lcd595_write(1,0, "ACESSO BLOQUEADO");
                    vTaskDelay(3000 / portTICK_PERIOD_MS);
                    lcd595_clear();
                    lcd595_write(1,0, "INSIRA SENHA ADM");
                    lcd595_write(2,0, "PARA DESBLOQUEAR");
                    vTaskDelay(4000 / portTICK_PERIOD_MS);

                    vezes = 0;
                    senha = 0;
                    tentativa = 0;

             
               }
        
        }

       
    }
            
        


       

        /* if (tecla!= '_')
        {
            sprintf(&mostra[0], "%c", tecla);
            lcd595_write(1,coluna,&mostra[0]);
            coluna++;

        }
        */


    }
        vTaskDelay(100 / portTICK_PERIOD_MS);    // delay mínimo obrigatório, se retirar, pode causar reset do ESP
    
    // caso erro no programa, desliga o módulo ADC
        hcf_adc_limpar();

}                                                                                                                    
    

    
   
    /////////////////////////////////////////////////////////////////////////////////////   Fim do ramo principal
    


