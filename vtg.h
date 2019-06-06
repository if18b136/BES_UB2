#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <assert.h>
#include <bits/stdc++.h>
#include <utility> 
#include <vector>
using namespace std;

#define KEY 420257884	/* eindeutiger Key z.B. Matrikelnummer */
#define PERM 0660

#define MAX_DATA 255

typedef struct 		//struct einer message
{
	long mType;		//braucht immer long als typfeld/priority. Später danach filtern um fahrzeuge zu unterscheiden(1-26 reincoderien)
	char mText[MAX_DATA];		//meistens Text, aber auch alle anderen Werte möglich
	int mPID;		//Zur Identifikation bevor ein Buchstabe zugeteilt ist notwendig

} message_t;
