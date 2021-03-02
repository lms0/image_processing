#include "opencv2/opencv.hpp"
#include <iostream>
#include <math.h>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>
#include <fstream>


using namespace std;
using namespace cv;

bool modoCalib=false;
bool modoUnion=false;

const string videoWindowName = "VIDEO";
const string trackbarWindowName = "AJUSTE DEL FILTRO";
const string thresholdWindowName = "IMAGEN FILTRADA";

int H_MIN= 0;
int S_MIN= 0;
int V_MIN= 0;
int H_MAX= 256;
int S_MAX= 256;
int V_MAX= 256;

int i=0;
vector<Point2f> clicks(3); //Vector de 3 puntos donde se clickeo
vector<Point2f> mci(3);    //Vector de 3 puntos de interes (centros de objetos de interes), asociados a los clicks


int aux=0;
int auxArch = 0;

int captura = 1;

void help(void)
{
printf( "\n\nEste programa permite ver un video y pausarlo en momentos de interes, para realizar operaciones de union y calculo de angulo entre objetos de un color a calibrar.\n\n"
 "Instrucciones de uso:\n\n"
 "./[nombre del ejecutable] [path al video] [opcional: carpeta de salida distinta]   EJEMPLO1: ./ejec /home/...video.mp4    EJEMPLO 2:./ejec /home/...video.mp4 /home/.../CarpetaSalida/  \n\n"
 "Por default, el video se ejecuta hasta terminarse. En cada momento puede pulsarse las siguientes teclas que pausaran el video y realizaran cierta accion:\n\n"
 " * Tecla 1 *: Entra al Modo Calibracion. Se ajustan los rangos de Matiz, Saturacion y Valor, y se ve el resultado en una imagen binaria.\n\n"
 " * Tecla Enter *: Sale del Modo Calibracion. Se guardan los valores de los filtros en un archivo, el cual se cargara cada vez que se ejecute el programa, para evitar tener que calibrar cada vez que se inicia."
 " * Tecla Barra Espaciadora *: Entra/Sale del Modo Union de puntos. Se clickea (click izquierdo) cerca de 3 objetos de interes y se trazan lineas entre los centros de cada objeto. Se mostrara el angulo de interes y se guardara en la carpeta contenedora una captura de pantalla y un archivo .csv con el nombre y los angulos\n\n"
 "Se puede reanudar el video saliendo del Modo Union de puntos y repetir el proceso para un nuevo marco del video.\n"
  );
}

void on_trackbar( int, void* )
{
}

void createTrackbars()
{

 namedWindow(trackbarWindowName,0);
 char TrackbarName[50];
 sprintf( TrackbarName, "H_MIN", H_MIN);
 sprintf( TrackbarName, "H_MAX", H_MAX);
 sprintf( TrackbarName, "S_MIN", S_MIN);
 sprintf( TrackbarName, "S_MAX", S_MAX);
 sprintf( TrackbarName, "V_MIN", V_MIN);
 sprintf( TrackbarName, "V_MAX", V_MAX);

 createTrackbar( "H_MIN", trackbarWindowName, &H_MIN, H_MAX, on_trackbar );
 createTrackbar( "H_MAX", trackbarWindowName, &H_MAX, H_MAX, on_trackbar );
 createTrackbar( "S_MIN", trackbarWindowName, &S_MIN, S_MAX, on_trackbar );
 createTrackbar( "S_MAX", trackbarWindowName, &S_MAX, S_MAX, on_trackbar );
 createTrackbar( "V_MIN", trackbarWindowName, &V_MIN, V_MAX, on_trackbar );
 createTrackbar( "V_MAX", trackbarWindowName, &V_MAX, V_MAX, on_trackbar );


}

void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN )
    {
        cout << "Punto clickeado en posicion (" << x << ", " << y << ")" << endl;
        clicks[i]=Point2f (x,y);
        i++;
    }
    if  ( event == EVENT_MBUTTONDOWN )
    {
        cout << "Se resetean los puntos clickeados" << endl;
        for (int a=0 ; a<=clicks.size()-1; a++)
        {
          clicks[a] = Point2f(0,0);
          mci[a]    = Point2f(0,0);
          i   = 0;
          aux = 0;
        }
    }

}

void trackFilteredObject(Mat threshold, Mat &frame, string nombreArchivo, string direccionCarpeta)
{
    cout << "ENTRE A TRACKEAR\n";
    Mat temp;
    threshold.copyTo(temp);
	  Mat imgLines = Mat::zeros( temp.size(), CV_8UC3 );

    vector< vector<Point> > contours; //Vector de los contornos encontrados
    vector<Vec4i> hierarchy;
    //Canny( temp, temp, thresh, thresh*2, 3 );
    findContours(temp,contours,hierarchy,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_SIMPLE);

    vector<Moments> mu(contours.size() );
    vector<Point2f> mc(contours.size() );


	  for( int i = 0; i < contours.size(); i++ )
    { mu[i] = moments( contours[i], false ); }
    for( int i = 0; i < contours.size(); i++ )
    { mc[i] = Point2f( mu[i].m10/mu[i].m00 , mu[i].m01/mu[i].m00 );}

    //Script que compara cada punto del Point2f "clicks" con los centros de masa mc.


    for( int i = 0; i < contours.size(); i++ )
    {
      if ( abs(mc[i].x-clicks[0].x)<=60  && abs(mc[i].y-clicks[0].y)<=60)
      {
        mci[0] = mc[i];

      }
      if ( abs(mc[i].x-clicks[1].x)<=60  && abs(mc[i].y-clicks[1].y)<=60)
      {
        mci[1] = mc[i];
      }
      if ( abs(mc[i].x-clicks[2].x)<=60  && abs(mc[i].y-clicks[2].y)<=60)
      {
        mci[2] = mc[i];
      }
    }

  cout << mci[0] << endl;
  cout << mci[1] << endl;
  cout << mci[2] << endl;

  for( int i = 0; i <2; i++ )
    {
       line(imgLines, Point(mci[i]), Point(mci[i+1]), Scalar(255,0,0), 2);
    }

    //Script que coloca angulos entre las lineas (TEOREMA DEL COSENO)

    double p1=mci[0].x-mci[2].x;
    double p2=mci[0].y-mci[2].y;
    double p3=mci[2].x-mci[1].x;
    double p4=mci[2].y-mci[1].y;
    double p5=mci[0].x-mci[1].x;
    double p6=mci[0].y-mci[1].y;
    double a = sqrt(pow(abs(p1),2)+pow(abs(p2),2));
    double b = sqrt(pow(abs(p3),2)+pow(abs(p4),2));
    double c = sqrt(pow(abs(p5),2)+pow(abs(p6),2));
    cout << a << endl;
    cout << b << endl;
    cout << c << endl;
    double angle = acos((pow(b,2)+pow(c,2)-pow(a,2))/(2*b*c))*180/3.14159265359;
    cout << angle;
    string anguloTexto = to_string(angle);
    putText(frame,anguloTexto,Point(mci[1]),1, 2,Scalar(64,64,245));


    frame = frame + imgLines;
    aux=1;

    //GUARDO UNA CAPTURA CON UN ID + NOMBRE DEL PACIENTE EN LA CARPETA CONTENEDORA DEL ARCHIVO
    string camino;
    camino.append(direccionCarpeta);
    camino.append(nombreArchivo);
    camino.append(to_string(captura));
    camino.append(".jpg") ;
    imwrite(camino,frame);
    cout <<"GUARDO CAPTURA N" << captura << endl;
    captura++;

    //GUARDO UN ARCHIVO .csv CON LOS ANGULOS MEDIDOS
    ofstream archivo;
    string nombreCSV;
    nombreCSV.append(direccionCarpeta);
    nombreCSV.append(nombreArchivo);
    nombreCSV.append(".csv");
    archivo.open (nombreCSV,std::ios::app);
      if (auxArch == 0)
      {archivo << nombreArchivo << endl;
      auxArch++;
      }
    archivo << angle << ",";

}


void morphOps(Mat &thresh)
{

 //TRATAMIENTO DE SENAL, EROSION - DILATACION PARA ELIMINAR IMPUREZAS DE LA CAMARA

 Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
 Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));

 erode(thresh,thresh,erodeElement);
 erode(thresh,thresh,erodeElement);


 dilate(thresh,thresh,dilateElement);
 dilate(thresh,thresh,dilateElement);

}



string getFilePath(const string& s) {

   char sep = '/';

#ifdef _WIN32
   sep = '\\';
#endif

   size_t i = s.rfind(sep, s.length());
   if (i != string::npos) {
      return(s.substr(0, i+1));
   }

}


string getFileName(const string& s) {

   char sep = '/';

#ifdef _WIN32
   sep = '\\';
#endif

   size_t i = s.rfind(sep, s.length());
   if (i != string::npos) {
      return(s.substr(i+1, s.length() - i));
   }

   return("");
}


string remove_extension(const std::string& filename) {
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos) return filename;
    return filename.substr(0, lastdot);
}




int main(int argc, char* argv[])
{

  help();

  //DECLARACION DE MATRICES
  Mat frame;            //fotograma
  Mat HSV;
  Mat threshold;        //img binaria blanco negro
  Mat imgTemp;
  Mat imgLines;         //img con lineas y angulos

  //ENTRA ARGUMENTO 1, SEPARO NOMBRE DEL ARCHIVO
  string path = argv[1];
  string nombreArchivo = remove_extension(getFileName(path));

  //SI HAY ARGUMENTO 2, SE GUARDAN LAS SALIDAS EN ESA DIRECCION. SI NO HAY, SE GUARDA EN LA CARPETA CONTENEDORA DEL ARCHIVO
  string direccionCarpeta;
  if (argv[2] == NULL)
   {
   direccionCarpeta = getFilePath(path);
   }
   else
   {
   direccionCarpeta = argv[2];
   }

  cout << "El nombre del archivo es " << nombreArchivo << "y las salidas del programa se guardaran en" << direccionCarpeta << endl;

  VideoCapture cap(path);
  if(!cap.isOpened())
   {
   cout << "Error opening video stream or file" << endl;
   return -1;
   }

  cap >> frame;

  // CADA VEZ QUE SE CLICKEA SE INTERRUMPE EL PROGRAMA
  imshow( videoWindowName, frame );
  setMouseCallback(videoWindowName, CallBackFunc, NULL);


  while(cap.isOpened())
    {

       cap >> frame;

       if (frame.empty())
       break;

       //MODO CALIBRACION: FILTRO LA IMAGEN, LA TRATO Y MUESTRO LA IMAGEN BINARIA MIENTRAS TUNEO PARAMETROS HSV (video parado hasta salir)
       if (modoCalib==true)
       {
        createTrackbars();
        cvtColor(frame,HSV,COLOR_BGR2HSV);

         while (modoCalib==true)
         {
          inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold);
          morphOps(threshold);
          imshow(thresholdWindowName,threshold);
          if (waitKey(60)==49) modoCalib=!modoCalib;
         }
       }

       //MODO UNION, IDEM ANTERIOR PERO ENTRA A LA FUNCION DE TRACKEO UNA VEZ QUE YA CLICKIE 3 PTOS
       if (modoUnion==true)
       {
        cvtColor(frame,HSV,COLOR_BGR2HSV);
        inRange(HSV,Scalar(H_MIN,S_MIN,V_MIN),Scalar(H_MAX,S_MAX,V_MAX),threshold);
        morphOps(threshold);
        imshow(thresholdWindowName,threshold);
        imshow(videoWindowName, frame);

         while (modoUnion==true)
		     {
           if (clicks[0] != Point2f(0,0) && clicks[1] != Point2f(0,0) && clicks[2] != Point2f(0,0) && aux==0) //IF (YA TENGO 3 PUNTOS DE INTERES COMO MINIMO, HAGO EL TRACKEO)
           { trackFilteredObject(threshold,frame, nombreArchivo, direccionCarpeta);
             imshow(videoWindowName, frame);

           }
          if (waitKey(60)==32) modoUnion=!modoUnion;
		     }
	     }


     imshow( videoWindowName, frame );


      switch(waitKey(60))
      {

      case 49:  //SE APRIETA '1', SE PAUSA EL VIDEO Y SE CALIBRA LA IMAGEN
      modoCalib=!modoCalib;
      modoUnion=false;

      break;

      case 32:  //SE APRIETA 'BARRA ESPACIADORA', SE PAUSA EL VIDEO Y SE UNEN PUNTOS
      modoUnion=!modoUnion;
      modoCalib=false;

      break;

      case 27:
      cap.release();

      break;


      }

    }


  cap.release();
  destroyAllWindows();

  return 0;
}
