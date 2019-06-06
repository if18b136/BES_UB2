#include "vtg.h"

int main(int argc, char* argv[]){
   message_t msg;	  //Buffer fuer Message
   int msgid = -1;	 //Message Queue ID 
   int kfz = (int)argv[1][0] - 64;    //Autoname wird in integer verwandelt
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
   msg.mType=100;   //Anfangs wird auf 100 gesendet um sich als neuer client erkennen zu lassen
   msg.mPID=getpid();   //zum Ruecksenden wird eigene PID mitgeschickt
   strncpy(msg.mText,argv[1],MAX_DATA);      //überprüft wieviele Param erlaubt sind
   if (msgsnd(msgid,&msg,sizeof(msg)-sizeof(long), 0) == -1){   //gewuenschter Buchstabe wird auf Kanal 100 an Server gesendet
         /* error handling */
         fprintf(stderr,"%s: Can't send message\n",argv[0]);
         return EXIT_FAILURE;
   }
   while (1){
      if (msgrcv(msgid,&msg,sizeof(msg)-sizeof(long), (int)getpid() , 0) == -1){    //wartet auf antwort ob Auto erstellt
         fprintf(stderr,"%s: Can't receive from message queue\n",argv[0]);
         return EXIT_FAILURE;
      }
      else{   
        if(msg.mType == (int)getpid()){   //Wenn Erstellungsantwort von Server (unter unserer PID) kommt
          cout << msg.mText << endl;    //message des servers wird geprintet
          int comp = strcmp(msg.mText, "Auto bereits vergeben");    //Abfrage ob Auto bereits vergeben
          if(comp == 0){
            return EXIT_SUCCESS;
          }
          else{   //Auto wurde erstellt
            while(1){   //Jetzt nur noch Richtungsinput
                char dir;
                cout << "Enter Move: ";
                cin  >> dir;

                msg.mText[0] = dir;   //erste stelle der message wird auf himmelsrichtung gesetzt
                msg.mType = kfz;    //ab jetzt wird nur noch ueber Autonamen als integer gesendet/empfangen
                if (msgsnd(msgid,&msg,sizeof(msg)-sizeof(long), 0) == -1){    //message wird gesendet
                      /* error handling */
                      fprintf(stderr,"%s: Can't send message\n",argv[0]);
                      return EXIT_FAILURE;
                }
                if(dir == 'T'){
                  cout << "Prozess wird beendet.." << endl;
                  return EXIT_SUCCESS;
                }
            }
          }
        }
      }
   }
   return EXIT_SUCCESS;
}
