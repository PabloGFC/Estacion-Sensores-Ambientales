#include "sensores.h"
#include "modem.h"
#include "main.h"
#include "stdio.h"
#include "string.h"

#ifndef SRC_PRINCIPAL_H_
#define SRC_PRINCIPAL_H_

extern osMessageQueueId_t mid_PRINCIPALQueue;

typedef struct {
	char info[64];
	int act;
} MSGQUEUE_PRINCIPAL_t;

int Thread_Principal(void);

void MX_GPIO_Init(void);
void MX_RTC_Init(void);


void Get_RTC_Time_Date(void);
void guardaMedidas(void);

#endif /* SRC_PRINCIPAL_H_ */
