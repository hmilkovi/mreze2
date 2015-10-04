
#include <stdlib.h> /* atoi(), exit()*/
#include <arpa/inet.h>
#include <stdio.h> /* printf(); alternativno cout i iostream*/
#include <string.h> /* memset() */
#include <ctype.h>
 
#define MAX_MSG 10000

 
int main() {
  int opisnikUticnice, rc, n;
  socklen_t cliLen;
  struct sockaddr_in cliAddr, servAddr;
  char meduspremnik[MAX_MSG];
  short portNum = 8080;
 
  /* stvaranje socketa */
  opisnikUticnice=socket(AF_INET, SOCK_DGRAM, 0);

  if(opisnikUticnice<0) {
    printf("%d: ne mogu otvoriti soket \n",portNum);
    exit(1);
  }
 
  /* povezivanje posluzitelja s portom */
  servAddr.sin_family = AF_INET;
  servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servAddr.sin_port = htons(portNum);
  rc = bind (opisnikUticnice, (struct sockaddr *) &servAddr,sizeof(servAddr));
  if(rc<0) {
    printf("Ne mogu se povezati s portom broj: %d \n", portNum);
    exit(1);
  }
 
  printf("Cekam podatke na udp portu %u\n",portNum);
 
  while(1) {
    /* popuni meduspremnik nulama */
    memset(meduspremnik,0,MAX_MSG);
 
    /* primi poruku */
    cliLen = sizeof(cliAddr);
    n = recvfrom(opisnikUticnice, meduspremnik, MAX_MSG, 0/*flags=0*/,(struct sockaddr *) &cliAddr, &cliLen);

    if(n<0) {
      printf("%u: ne mogu primiti podatke \n",portNum);
      continue;
    }
    /* ispis primljene poruke */
    printf("Od %s:UDP%u : %s \n",inet_ntoa(cliAddr.sin_addr),ntohs(cliAddr.sin_port),meduspremnik);
       /*posalji poruku*/
    for(int i = 0; meduspremnik[i]; i++){
      meduspremnik[i] = tolower(meduspremnik[i]);
    }

    if(meduspremnik[0] == 'h' && meduspremnik[1] == 'i') {
        sendto(opisnikUticnice,"Hi, try to ask a question   :)",30,0,(struct sockaddr *)&cliAddr,cliLen);
        memset(meduspremnik,0,MAX_MSG);
    } else if (meduspremnik[0] == 'w' && meduspremnik[1] == 'h' && meduspremnik[2] == 'o') {
        sendto(opisnikUticnice,"I'm and UDP server arka.foi.hr.",30,0,(struct sockaddr *)&cliAddr,cliLen);
        memset(meduspremnik,0,MAX_MSG);
    } else {
        sendto(opisnikUticnice,"I don't understand, try again!",30,0,(struct sockaddr *)&cliAddr,cliLen);
        memset(meduspremnik,0,MAX_MSG);
    }
  }
return 0;
}