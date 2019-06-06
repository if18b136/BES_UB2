#include "vtg.h"

void display(vector<vector<char> > grid){
  int fd;   
  char * myfifo = "/tmp/myfifo";    //Ablagestelle fuer Pipe File
  string grid_string = "";    //String zum senden an Displayprozess wird erstellt

    //String zum senden an Displayprozess wird gefuellt
   for(int i =0 ; i< grid.size();i++)
   {
      for(int j =0 ;j<grid[i].size();j++)
      {
          grid_string += grid[i][j];
      }
      grid_string += "\n";    //Absaetze zur Veranschaulichung

   }
   const char * grid_msg = grid_string.c_str();   //string wird in const char* verwandelt um mit write() kompatibel zu sein





  mkfifo(myfifo, 0666);   //Pipe wird erstellt



  fd = open(myfifo, O_WRONLY);    //Pipe wird geoeffnet

  write(fd, grid_msg, 10000);   //msg wird gesendet
  close(fd);    //Pipe wird geschlossen

  unlink(myfifo);   //Fifo wird entfernt

}


int main(int argc, char* argv[]){
   srand(time(0));    //randomseed
   message_t msg;	  //Buffer fuer Message 
   int msgid = -1, c = 0, width = 0, height = 0;
   bool carArray[26] = {0};   //Array zur feststellung ob Buchstabe bereits belegt ist
   vector< pair<int,int> > car_Array;   //Vektor zum festhalten der Position der autos, als pairs

   for (int i = 0; i < 26; i++){
    car_Array.push_back( pair<int,int>(-1,-1));   //Standardposition wenn Auto noch nicht initialisiert = -1/-1
   }

   /* Message Queue neu anlegen */
   if( (msgid = msgget(KEY,PERM | IPC_CREAT | IPC_EXCL ))==-1 )   //1. eindeutige Kennung 2. Flags, Bitweise mit oder verknüpft
   {
      /* error handling */
      fprintf(stderr,"%s: Error creating message queue\n",argv[0]);
      return EXIT_FAILURE;
   }
   // Auslesen der Terminaluebergabe bzgl Groesse des Grids
   while( (c = getopt( argc, argv, "x:y:" )) != EOF ){
     switch (c) {
          case 'x':
              width = atoi(optarg);   //parsing auf int ueber atoi
              break;
          case 'y':
              height = atoi(optarg);    //parsing auf int ueber atoi
              break;
          default:    //debugging
            assert(0);
      }
  }

  if(width < 1 || height < 1){    //wenn Grid unmoegliche Groesse erhaelt wird beendet
    cout << "Invalid Parameters." << endl;
    return EXIT_FAILURE;
  }
  //Initialisierung des Grids
  vector<char> row(height+2,' ');
  vector<vector<char> > grid(width+2,row);

  //vektorbefuellung
  for(int columns = 0; columns < grid.size(); columns++){
    for (int rows = 0; rows < grid[columns].size(); rows++){
      if(columns == 0 || columns == grid.size()-1 || rows == 0 || rows == grid[columns].size()-1){
        grid[columns][rows] = '#';
      }
    }
  }
  while (1){    //endlosschleife bis server beendet wird
    if (msgrcv(msgid,&msg,sizeof(msg)-sizeof(long), 0 , 0) == -1){    //blockt bis message ankommt
       fprintf(stderr,"%s: Can't receive from message queue\n",argv[0]);
       return EXIT_FAILURE;
    }   
    if(msg.mType == 100){   //die ID 100 wurde von uns zur initialisierung neuer Autos gewaehlt
       int spot = (int)msg.mText[0];    //Gewuenschter Buchstabe wird in integer umgewandelt
       spot -= 65;    //-65 bringt alle Grossbuchstabe auf die Werte 0-25
       msg.mType = msg.mPID;    //Zum zuruecksenden wird die PID des Clients als Identifier gewaehlt, somit koennen mehrere Clients simultan ein Auto erstellen
       if(carArray[spot] == true){    //Ueberpruefung ob Buchstabe bereits einem anderen Client gehoert
         strncpy(msg.mText,"Auto bereits vergeben",MAX_DATA);
       }
       else{    //wenn noch verfuegbar
         carArray[spot] = true;   //stelle im Array wird belegt um spaeteren Clients den Buchstaben zu 'verbieten'
         int randX = rand()%width+1;    //auto wird auf random stelle im Grid gesetzt
         int randY = rand()%height+1;
         do{   //zuerst schauen, ob dort schon was liegt
           randX = rand()%width+1;
           randY = rand()%height+1;
         }while(grid[randX][randY] != ' ');
        grid[randX][randY] = msg.mText[0];    //an der stelle im Grid fuegen wir jetzt das auto ein
        car_Array[spot] = pair<int,int>(randX,randY);   //Koordinaten des Autos werden als pair in unser Koordinatenarray eingefuegt
        strncpy(msg.mText,"Auto initialisiert.",MAX_DATA);    //Messagerueckgabe wird vorbereitet
       }
       if (msgsnd(msgid,&msg,sizeof(msg)-sizeof(long), 0) == -1) //message wird gesendet
       {
             /* error handling */
             fprintf(stderr,"%s: Can't send message\n",argv[0]);
             return EXIT_FAILURE;
       }
    }
    else if(msg.mType >= 1 && msg.mType <= 26 && carArray[msg.mType-1] == true){    //falls Auto bereits erstellt wurde
      switch(msg.mText[0]){   //es wird nach Input differenziert
        case 78:    //=Norden
          /*Check ob Weg frei ist, bewege Auto*/
          if(grid[car_Array[msg.mType-1].first-1][car_Array[msg.mType-1].second]==' '){
             grid[car_Array[msg.mType-1].first-1][car_Array[msg.mType-1].second] = grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second];
             grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second] = ' ';
             car_Array[msg.mType-1].first--;
          }
          /*Check ob Wand im Weg ist, zerstoere Auto*/
          else if(grid[car_Array[msg.mType-1].first-1][car_Array[msg.mType-1].second]=='#'){
            grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second]=' ';
            car_Array[msg.mType-1].first = -1;
            car_Array[msg.mType-1].second = -1;
            carArray[msg.mType-1] = false;
          }
          /*Alles andere muss ein Fremdes Auto sein, Zerstoere beide*/
          else{
            int collider = (int)grid[car_Array[msg.mType-1].first-1][car_Array[msg.mType-1].second];
            collider -= 65;
            
            grid[car_Array[collider].first][car_Array[collider].second]=' ';
            car_Array[collider].first = -1;
            car_Array[collider].second = -1;
            carArray[collider] = false;

            grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second]=' ';
            car_Array[msg.mType-1].first = -1;
            car_Array[msg.mType-1].second = -1;
            carArray[msg.mType-1] = false;
          }
          break;
        case 79:    //=Osten
          cout << msg.mText[0] << endl;
          if(grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second+1]==' '){
             grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second+1] = grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second];
             grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second] = ' ';
             car_Array[msg.mType-1].second++;
          }
          else if(grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second+1]=='#'){
            grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second]=' ';
            car_Array[msg.mType-1].first = -1;
            car_Array[msg.mType-1].second = -1;
            carArray[msg.mType-1] = false;
          }
          else{
            int collider = (int)grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second+1];
            collider -= 65;
            
            grid[car_Array[collider].first][car_Array[collider].second]=' ';
            car_Array[collider].first = -1;
            car_Array[collider].second = -1;
            carArray[collider] = false;

            grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second]=' ';
            car_Array[msg.mType-1].first = -1;
            car_Array[msg.mType-1].second = -1;
            carArray[msg.mType-1] = false;
          }
          break;
        case 83:    //=Sueden
          cout << msg.mText[0] << endl;
          if(grid[car_Array[msg.mType-1].first+1][car_Array[msg.mType-1].second]==' '){
             grid[car_Array[msg.mType-1].first+1][car_Array[msg.mType-1].second] = grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second];
             grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second] = ' ';
             car_Array[msg.mType-1].first++;
          }
          else if(grid[car_Array[msg.mType-1].first+1][car_Array[msg.mType-1].second]=='#'){
            grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second]=' ';
            car_Array[msg.mType-1].first = -1;
            car_Array[msg.mType-1].second = -1;
            carArray[msg.mType-1] = false;
          }
          else{
            int collider = (int)grid[car_Array[msg.mType-1].first+1][car_Array[msg.mType-1].second];
            collider -= 65;
            
            grid[car_Array[collider].first][car_Array[collider].second]=' ';
            car_Array[collider].first = -1;
            car_Array[collider].second = -1;
            carArray[collider] = false;

            grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second]=' ';
            car_Array[msg.mType-1].first = -1;
            car_Array[msg.mType-1].second = -1;
            carArray[msg.mType-1] = false;
          }
          break;
        case 87:    //=Westen
          cout << msg.mText[0] << endl;
          if(grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second-1]==' '){
             grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second-1] = grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second];
             grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second] = ' ';
             car_Array[msg.mType-1].second--;
          }
          else if(grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second-1]=='#'){
            grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second]=' ';
            car_Array[msg.mType-1].first = -1;
            car_Array[msg.mType-1].second = -1;
            carArray[msg.mType-1] = false;
          }
          else{
            int collider = (int)grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second-1];
            collider -= 65;
            
            grid[car_Array[collider].first][car_Array[collider].second]=' ';
            car_Array[collider].first = -1;
            car_Array[collider].second = -1;
            carArray[collider] = false;

            grid[car_Array[msg.mType-1].first][car_Array[msg.mType-1].second]=' ';
            car_Array[msg.mType-1].first = -1;
            car_Array[msg.mType-1].second = -1;
            carArray[msg.mType-1] = false;
          }
          break;
        default:
          cout << msg.mText[0] << endl;
          break;
      }
    }
    display(grid);    //In jeder Whileschleifeniteration werden die aktuellsten werte an die Displayfunktion uebergeben
   }
   return EXIT_SUCCESS;
}
