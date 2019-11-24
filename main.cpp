//
//  threadTrem.cpp
//
//
//  Created by Affonso on 25/10/16.
//
//

// http: pubs.opengroup.org/onlinepubs/7908799/xsh/pthread_mutex_init.html

// Programa que sincroniza threads utilizando-se mutexes
// Para compilá-lo utilise: g++ -o threadTrem threadTrem.cpp -lpthread

//   Os trens circulam em sentido horário entre os trilhos

//   + - - - 1 - - - + - - - 2 - - - + - - - 3 - - - +
//   |               |               |               |
//   |               |               |               |
//   |               |               |               |
//   1     trem1    4/4     trem2   5/5     trem3    3
//   |               |               |               |
//   |               |               |               |
//   |               |               |               |
//   + - - - 6/6 - - + - - -7/7 - -  + - - - 8/8- -  +
//   |                                               |
//   |                                               |
//   |                                               |
//   9                    trem4                      9
//   |                                               |
//   |                                               |
//   |                                               |
//   + - - - - - - - - - - - 9- - - - - - - - - - - +

#include "BlackGPIO/BlackGPIO.h"
#include "ADC/Adc.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>


#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <arpa/inet.h>

#include <netinet/in.h>
#include <time.h>


using namespace BlackLib;
// ----------- leds trem 1 ----------
BlackGPIO l1(GPIO_67, output);       //P8_8
BlackGPIO l4_trem1(GPIO_66, output); //P8_7
BlackGPIO l6_trem1(GPIO_60, output); //P9_12
// ----------- leds trem 2 ----------
BlackGPIO l4_trem2(GPIO_27, output); //P8_17
BlackGPIO l2(GPIO_65, output);       //P8_18
BlackGPIO l5_trem2(GPIO_69, output); //P8_9
BlackGPIO l7_trem2(GPIO_68, output); //P8_10
// ----------- leds trem 3 ----------
BlackGPIO l5_trem3(GPIO_51, output); //P9_14->16 ok
BlackGPIO l3(GPIO_20, output);       //P9_41
BlackGPIO l8_trem3(GPIO_5, output);  //P9_30->17 ok
// ----------- leds trem 4 ----------
BlackGPIO l7_trem4(GPIO_14, output); //P9_27->26 ok
BlackGPIO l9(GPIO_31, output);       //P9_13
BlackGPIO l6_trem4(GPIO_30, output); //P9_11
BlackGPIO l8_trem4(GPIO_49, output); //P9_23

//------------ Adc -----------------
ADC vel_trem1(AINx::AIN0);
ADC vel_trem2(AINx::AIN1);
ADC vel_trem3(AINx::AIN2);
ADC vel_trem4(AINx::AIN3);

pthread_mutex_t t5;  /* proteção para o trilho 5 */
pthread_mutex_t t6;  /* proteção para o trilho 6 */
pthread_mutex_t t8;  /* proteção para o trilho 6 */
pthread_mutex_t t9;  /* proteção para o trilho 6 */
pthread_mutex_t t10; /* proteção para o trilho 6 */

#define MULTICAST_ADDR "225.0.0.37"

void acenderLocalizacao(BlackGPIO *loc_atual, BlackGPIO *loc_anterior, int sleepTime)
{
  loc_anterior->setValue(low);
  loc_atual->setValue(high);
  sleep(sleepTime);
}

int normalizeAdc(int trem, ADC *ADC)
{
  int leituraADC = ADC->getIntValue();
  //printf("\ntrem %d: %d\n", trem, (leituraADC % 5) + 1);
  return (leituraADC % 5) + 1;
}

void L(int trem, int trilho, int sleepTime)
{
  printf("trem %d no trilho %d, vel:%d\n", trem, trilho,sleepTime);
  sleep(sleepTime);
}

void *trem1(void *arg)
{
  int trem = 1;

  while (true)
  {
    int sleepTime = normalizeAdc(trem, &vel_trem1);
    acenderLocalizacao(&l1, &l1, sleepTime);
    L(trem, 1, sleepTime);
    pthread_mutex_lock(&t5);
    pthread_mutex_lock(&t8);
    acenderLocalizacao(&l4_trem1, &l1, sleepTime);
    L(trem, 5, sleepTime);
    pthread_mutex_unlock(&t5);
    acenderLocalizacao(&l6_trem1, &l4_trem1, sleepTime);
    L(trem, 8, sleepTime);
    pthread_mutex_unlock(&t8);
    acenderLocalizacao(&l1, &l6_trem1, sleepTime);
    L(trem, 4, sleepTime);
  }
  pthread_exit(0);
}

void *trem2(void *arg)
{
  int trem = 2;

  while (true)
  {
    int sleepTime = normalizeAdc(trem, &vel_trem2);
    acenderLocalizacao(&l2, &l4_trem2,sleepTime);
    L(trem, 2,sleepTime);
    pthread_mutex_lock(&t6);
    acenderLocalizacao(&l5_trem2, &l2,sleepTime);
    L(trem, 6,sleepTime);
    pthread_mutex_unlock(&t6);
    pthread_mutex_lock(&t9);
    acenderLocalizacao(&l7_trem2, &l5_trem2,sleepTime);
    L(trem, 9,sleepTime);
    pthread_mutex_unlock(&t9);
    pthread_mutex_lock(&t5);
    acenderLocalizacao(&l4_trem2, &l7_trem2,sleepTime);
    L(trem, 5,sleepTime);
    pthread_mutex_unlock(&t5);
  }
  pthread_exit(0);
}

void *trem3(void *arg)
{
  int trem = 3;
  while (true)
  {
    int sleepTime = normalizeAdc(trem, &vel_trem3);
    acenderLocalizacao(&l3, &l5_trem3, sleepTime);
    L(trem, 3, sleepTime);
    acenderLocalizacao(&l3, &l3, sleepTime);
    L(trem, 7, sleepTime);
    pthread_mutex_lock(&t10);
    pthread_mutex_lock(&t6);
    acenderLocalizacao(&l8_trem3, &l3, sleepTime);
    L(trem, 10, sleepTime);
    pthread_mutex_unlock(&t10);
    acenderLocalizacao(&l5_trem3, &l8_trem3, sleepTime);
    L(trem, 6, sleepTime);
    pthread_mutex_unlock(&t6);
  }
  pthread_exit(0);
}

void *trem4(void *arg)
{
  int trem = 4;
  while (true)
  {
    int sleepTime =  normalizeAdc(trem, &vel_trem4);
    acenderLocalizacao(&l9, &l9, sleepTime);
    L(trem, 13, sleepTime);
    acenderLocalizacao(&l9, &l9, sleepTime);
    L(trem, 11, sleepTime);
    pthread_mutex_lock(&t8);
    acenderLocalizacao(&l6_trem4, &l9, sleepTime);
    L(trem, 8, sleepTime);
    pthread_mutex_unlock(&t8);
    pthread_mutex_lock(&t9);
    acenderLocalizacao(&l7_trem4, &l6_trem4, sleepTime);
    L(trem, 9, sleepTime);
    pthread_mutex_lock(&t10);
    acenderLocalizacao(&l8_trem4, &l7_trem4, sleepTime);
    pthread_mutex_unlock(&t9);
    L(trem, 10, sleepTime);
    pthread_mutex_unlock(&t10);
    acenderLocalizacao(&l9, &l8_trem4, sleepTime);
    L(trem, 12, sleepTime);
  }
  pthread_exit(0);
}

void *socketServer(void* arg){

  int server_sockfd;
  size_t server_len;
  socklen_t client_len;
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;

  struct ip_mreq mreq;  // para endereco multicast

  unsigned short porta = 8109;

  if ( (server_sockfd = socket(AF_INET, SOCK_DGRAM, 0) )  < 0  )  // cria um novo socket
  {
      printf(" Houve erro na ebertura do socket ");
      exit(1);
  }
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY);
  server_address.sin_port = htons(porta);


  server_len = sizeof(server_address);

  if(  bind(server_sockfd, (struct sockaddr *) &server_address, server_len) < 0 )
  {
      perror("Houve error no Bind");
      exit(1);
  }

  //use setsockopt() para requerer inscricap num grupo multicast

  mreq.imr_multiaddr.s_addr=inet_addr(MULTICAST_ADDR);
  mreq.imr_interface.s_addr=htonl(INADDR_ANY);
  
  if (setsockopt(server_sockfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
      perror("setsockopt");
      exit(1);
  }

  printf(" IPPROTO_IP = %d\n", IPPROTO_IP);
  printf(" SOL_SOCKET = %d\n", SOL_SOCKET);
  printf(" IP_ADD_MEMBERSHIP = %d \n", IP_ADD_MEMBERSHIP);


  while(1){
      int valor;
      
      printf("Servidor esperando ...\n");
      
      client_len = sizeof(client_address);
      if(recvfrom(server_sockfd, &valor, sizeof(valor),0,
                  (struct sockaddr *) &client_address, &client_len) < 0 )
      {
          perror(" erro no RECVFROM( )");
          exit(1);
      }
      printf(" Valor recebido foi = %d\n", valor);
      //close(server_sockfd);
      
  }
  }

void* socketClient(void* arg){
  int sockfd;
  int len;
  struct sockaddr_in address;
  unsigned short porta = 8109;
    
  sockfd  = socket(AF_INET, SOCK_DGRAM,0);  // criacao do socket
    
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(MULTICAST_ADDR);
  address.sin_port = htons(porta);
  len = sizeof(address);
  while(1){
  int adcValues[] = {normalizeAdc(1,&vel_trem1),normalizeAdc(2,&vel_trem2),normalizeAdc(3,&vel_trem3),normalizeAdc(4,&vel_trem4)};
  for(int i=0;i<4;i++)
  {
      sendto(sockfd, &adcValues[i],sizeof(adcValues[i]),0,(struct sockaddr *) &address, len);
      sleep(1);
  }}
  
}

int main()
{
  printf("MAIN() --> Inicio do teste de leds\n");
  for (int i = 0; i < 2; i++)
  {
    l1.setValue(high);
    l4_trem1.setValue(high);
    l4_trem2.setValue(high);
    l2.setValue(high);
    l5_trem2.setValue(high);
    l7_trem2.setValue(high);
    l5_trem3.setValue(high);
    l3.setValue(high);
    l6_trem1.setValue(high);
    l6_trem4.setValue(high);
    l7_trem4.setValue(high);
    l8_trem3.setValue(high);
    l8_trem4.setValue(high);
    l9.setValue(high);
    sleep(2);

    // ----------- Testando LEDS ----------
    l1.setValue(low);
    l4_trem1.setValue(low);
    l4_trem2.setValue(low);
    l2.setValue(low);
    l5_trem2.setValue(low);
    l7_trem2.setValue(low);
    l5_trem3.setValue(low);
    l3.setValue(low);
    l6_trem1.setValue(low);
    l6_trem4.setValue(low);
    l7_trem4.setValue(low);
    l8_trem3.setValue(low);
    l8_trem4.setValue(low);
    l9.setValue(low);

    sleep(2);
  }

  printf("MAIN() --> Fim do teste de leds\n");

  srand(time(0));
  int res;
  pthread_t thread1, thread2, thread3, thread4,thread5,thread6;

  void *thread_result;

  // ------ criando multex t5 ------
  res = pthread_mutex_init(&t5, NULL);
  if (res != 0)
  {
    perror("Iniciação do Mutex t5 falhou");
    exit(EXIT_FAILURE);
  }

  // ------ criando multex t6 ------
  res = pthread_mutex_init(&t6, NULL);
  if (res != 0)
  {
    perror("Iniciação do Mutex t6 falhou");
    exit(EXIT_FAILURE);
  }

  // ------ criando multex t8 ------
  res = pthread_mutex_init(&t8, NULL);
  if (res != 0)
  {
    perror("Iniciação do Mutex t8 falhou");
    exit(EXIT_FAILURE);
  }

  // ------ criando multex t9 ------
  res = pthread_mutex_init(&t9, NULL);
  if (res != 0)
  {
    perror("Iniciação do Mutex t9 falhou");
    exit(EXIT_FAILURE);
  }

  // ------ criando multex t10 ------
  res = pthread_mutex_init(&t10, NULL);
  if (res != 0)
  {
    perror("Iniciação do Mutex t10 falhou");
    exit(EXIT_FAILURE);
  }

  //------ Thread 1 (executa a fn: trem 1) ------
  res = pthread_create(&thread1, NULL, trem1, NULL);
  if (res != 0)
  {
    perror("Criação da thread 1 falhou");
    exit(EXIT_FAILURE);
  }

  //------ Thread 2 (executa a fn: trem 2) ------
  res = pthread_create(&thread2, NULL, trem2, NULL);
  if (res != 0)
  {
    perror("Criação da thread 2 falhou");
    exit(EXIT_FAILURE);
  }

  //------ Thread 3 (executa a fn: trem 3) ------
  res = pthread_create(&thread3, NULL, trem3, NULL);
  if (res != 0)
  {
    perror("Criação da thread 3 falhou");
    exit(EXIT_FAILURE);
  }

  //------ Thread 4 (executa a fn: trem 4) ------
  res = pthread_create(&thread4, NULL, trem4, NULL);
  if (res != 0)
  {
    perror("Criação da thread 4 falhou");
    exit(EXIT_FAILURE);
  }

  //------ Thread 5 (executa a fn: socketServer) ------
  res = pthread_create(&thread5, NULL, socketServer, NULL);
  if (res != 0)
  {
    perror("Criação da thread 5 falhou");
    exit(EXIT_FAILURE);
  }

    //------ Thread 6 (executa a fn: socketClient) ------
  res = pthread_create(&thread6, NULL, socketClient, NULL);
  if (res != 0)
  {
    perror("Criação da thread 6 falhou");
    exit(EXIT_FAILURE);
  }

  // ----- Espera termino das threads
  res = pthread_join(thread1, &thread_result);
  if (res != 0)
  {
    perror("Juncao da Thread 1 falhou");
    exit(EXIT_FAILURE);
  }
  res = pthread_join(thread2, &thread_result);
  if (res != 0)
  {
    perror("Juncao da Thread 2 falhou");
    exit(EXIT_FAILURE);
  }
  res = pthread_join(thread3, &thread_result);
  if (res != 0)
  {
    perror("Juncao da Thread 3 falhou");
    exit(EXIT_FAILURE);
  }
  res = pthread_join(thread4, &thread_result);
  if (res != 0)
  {
    perror("Juncao da Thread 4 falhou");
    exit(EXIT_FAILURE);
  }

    res = pthread_join(thread5, &thread_result);
  if (res != 0)
  {
    perror("Juncao da Thread 5 falhou");
    exit(EXIT_FAILURE);
  }
      res = pthread_join(thread6, &thread_result);
  if (res != 0)
  {
    perror("Juncao da Thread 6 falhou");
    exit(EXIT_FAILURE);
  }

  printf("MAIN() --> Thread foi juntada com sucesso\n");

  //----- destruíndo mutex
  pthread_mutex_destroy(&t5);
  pthread_mutex_destroy(&t6);
  pthread_mutex_destroy(&t8);
  pthread_mutex_destroy(&t9);
  pthread_mutex_destroy(&t10);
  exit(EXIT_SUCCESS);
}