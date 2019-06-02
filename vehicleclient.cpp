#include "vtg.h"

int main(int argc, char* argv[]){
   message_t msg;	/* Buffer fuer Message */
   int msgid = -1;	/* Message Queue ID */
   int kfz = (int)argv[1][0] - 64;
   /* Argument Handling */
   if (argc!=2){
      fprintf(stderr,"Usage: %s <Message>\n",argv[0]);
      return EXIT_FAILURE;
   }

   /* Message Queue oeffnen */
   if( (msgid = msgget(KEY,PERM))==-1 ){
      /* error handling */
      fprintf(stderr,"%s: Can't access message queue\n",argv[0]);
      return EXIT_FAILURE;
   }

   /* Nachricht verschicken */
   msg.mType=100;
   msg.mPID=getpid();
   strncpy(msg.mText,argv[1],MAX_DATA);      //überprüft wieviele Param erlaubt sind
   if (msgsnd(msgid,&msg,sizeof(msg)-sizeof(long), 0) == -1){
         /* error handling */
         fprintf(stderr,"%s: Can't send message\n",argv[0]);
         return EXIT_FAILURE;
   }
   while (1){
      if (msgrcv(msgid,&msg,sizeof(msg)-sizeof(long), (int)getpid() , 0) == -1){
         fprintf(stderr,"%s: Can't receive from message queue\n",argv[0]);
         return EXIT_FAILURE;
      }
      else{
        if(msg.mType == (int)getpid()){
          cout << msg.mText << endl;
          int comp = strcmp(msg.mText, "Auto bereits vergeben");
          if(comp == 0){
            return EXIT_SUCCESS;
          }
          else{
            while(1){
                char dir;
                cout << "Enter Move: ";
                cin  >> dir;
                /*
                if (msgrcv(msgid,&msg,sizeof(msg)-sizeof(long), kfz, 0) == -1){
                  fprintf(stderr,"%s: Can't receive from message queue\n",argv[0]);
                  return EXIT_FAILURE;
                }
                else {
                  int d_car = strcmp(msg.mText, "Vehicle has been eliminated.");
                  if (d_car = 0){
                    cout <<  "Vehicle has been eliminated." << endl;
                    return EXIT_SUCCESS;
                  }
                }
                  */
                  msg.mText[0] = dir;
                  msg.mType = kfz;
                  if (msgsnd(msgid,&msg,sizeof(msg)-sizeof(long), 0) == -1){
                        /* error handling */
                        fprintf(stderr,"%s: Can't send message\n",argv[0]);
                        return EXIT_FAILURE;
                  }
            }
          }
        }
      }
   }
   return EXIT_SUCCESS;
}
