#ifndef _BANK_H
#define _BANK_H
#include <semaphore.h>

typedef struct Bank {
  unsigned int numberBranches;
  /*The number of all workers who are participating in this process.*/
  int numberWorkers;
  /*Semaphore to make checking operation atomic.*/
  sem_t chechking;
  /*Semaphore to control the preparation for the next day.*/
  sem_t nextDay;
  /*Semaphore ro make transfer process atomic.*/
  sem_t reportTransfer;
  struct Branch  *branches;
  struct Report  *report;
} Bank;

#include "account.h"

int Bank_Balance(Bank *bank, AccountAmount *balance);

Bank *Bank_Init(int numBranches, int numAccounts, AccountAmount initAmount,
                AccountAmount reportingAmount,
                int numWorkers);

int Bank_Validate(Bank *bank);
int Bank_Compare(Bank *bank1, Bank *bank2);



#endif /* _BANK_H */
