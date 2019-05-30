#include "vtg.h"

int main(int argc, char* argv[]){
   message_t msg;	/* Buffer fuer Message */
   int msgid = -1, c = 0, width = 0, height = 0;
   bool carArray[26] = {0};
   /* Message Queue neu anlegen */
   if( (msgid = msgget(KEY,PERM | IPC_CREAT | IPC_EXCL ))==-1 )   //1. eindeutige Kennung 2. Flags, Bitweise mit oder verknüpft
   {
      /* error handling */
      fprintf(stderr,"%s: Error creating message queue\n",argv[0]);
      return EXIT_FAILURE;
   }

   while( (c = getopt( argc, argv, "x:y:" )) != EOF ){
     switch (c) {
          case 'x':
              width = (long int)optarg;
              break;
          case 'y':
              height = (long int)optarg;
              break;
          default:
            assert(0);
      }
  }

  if(width == 0 || height == 0){
    cout << "Invalid Parameters." << endl;
    return EXIT_FAILURE;
  }


   while (1){
      if (msgrcv(msgid,&msg,sizeof(msg)-sizeof(long), 0 , 0) == -1){
         fprintf(stderr,"%s: Can't receive from message queue\n",argv[0]);
         return EXIT_FAILURE;
      }
      if(msg.mType == 100){
         int spot = (int)msg.mText[0];
         spot -= 65;
         msg.mType = msg.mPID;
         if(carArray[spot] == true){
           strncpy(msg.mText,"Auto bereits vergeben",MAX_DATA);
         }
         else{
           carArray[spot] = true;
           strncpy(msg.mText,"Auto initialisiert",MAX_DATA);
         }
         if (msgsnd(msgid,&msg,sizeof(msg)-sizeof(long), 0) == -1) //Errorhandling
         {
               /* error handling */
               fprintf(stderr,"%s: Can't send message\n",argv[0]);
               return EXIT_FAILURE;
         }
      }
   }
   return EXIT_SUCCESS;
}
