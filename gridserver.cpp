#include "vtg.h"

void display(vector<vector<char> > grid){
  int fd;
  char * myfifo = "/tmp/myfifo";

  /* create the FIFO (named pipe) */
  mkfifo(myfifo, 0666);

  /* write "Hi" to the FIFO */
  fd = open(myfifo, O_WRONLY);
  write(fd, "Hi", sizeof("Hi"));
  close(fd);

  /* remove the FIFO */
  unlink(myfifo);

}


int main(int argc, char* argv[]){
   srand(time(0));
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
              width = atoi(optarg);
              break;
          case 'y':
              height = atoi(optarg);
              break;
          default:
            assert(0);
      }
  }

  if(width < 1 || height < 1){
    cout << "Invalid Parameters." << endl;
    return EXIT_FAILURE;
  }
  /*
  vector<vector<char> > grid(width+1);

  for(int i = 0; i < width+1; i++){
    vector<char> RowVector(height+1);
    // ... fill the row vector ...
    grid.push_back(RowVector);
  }
  */

  vector<char> row(height+2,' ');
  vector<vector<char> > grid(width+2,row);

  //vektorbefuellung
  for(int columns = 0; columns < grid.size(); columns++){
    for (int rows = 0; rows < grid[columns].size(); rows++){
      if(columns == 0 || columns == grid.size()-1 || rows == 0 || rows == grid[columns].size()-1){
        grid[columns][rows] = '#';
      }
      /* not necessary anymore
     else{
        grid[columns][rows] = ' ';
      }*/
    }
  }
/*
print out grid
  for(int columns = 0; columns< grid.size(); columns++){
    for (int rows = 0; rows < grid[columns].size(); rows++){
      cout << grid[columns][rows];
    }
    cout << endl;
  }
*/
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
           int randX = rand()%width+1;
           int randY = rand()%height+1;
           do{   //zuerst schauen, ob dort schon was liegt
             randX = rand()%width+1;
             randY = rand()%height+1;
           }while(grid[randX][randY] != ' ');
           //an der stelle fuegen wir jetzt das auto ein
          grid[randX][randY] = msg.mText[0];
          cout << grid[randX][randY];

          for(int columns = 0; columns< grid.size(); columns++){
            for (int rows = 0; rows < grid[columns].size(); rows++){
              cout << grid[columns][rows];
            }
            cout << endl;
          }

          strncpy(msg.mText,"Auto initialisiert.",MAX_DATA);

         }
         if (msgsnd(msgid,&msg,sizeof(msg)-sizeof(long), 0) == -1) //Errorhandling
         {
               /* error handling */
               fprintf(stderr,"%s: Can't send message\n",argv[0]);
               return EXIT_FAILURE;
         }
      }
      else if(msg.mType >= 1 && msg.mType <= 26){
        switch(msg.mText[0]){
          case 78:
            cout << msg.mText[0] << endl;
            break;
          case 79:
            cout << msg.mText[0] << endl;
            break;
          case 87:
            cout << msg.mText[0] << endl;
            break;
          case 83:
            cout << msg.mText[0] << endl;
            break;
          default:
            cout << msg.mText[0] << endl;
            break;
        }
      }
      display(grid);
   }
   return EXIT_SUCCESS;
}
