/*
# This file is part of Computer Vision Exam Project
#
# Copyright(c) 2012 Domenico Luciani
# domenico.luciani@email.it
#
#
# This file may be licensed under the terms of of the
# GNU General Public License Version 3 (the ``GPL'').
#
# Software distributed under the License is distributed
# on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
# express or implied. See the GPL for the specific language
# governing rights and limitations.
#
# You should have received a copy of the GPL along with this
# program. If not, go to http://www.gnu.org/licenses/gpl.html
# or write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include <cv.h>
#include <highgui.h>
#include "../lib/funzioni.h"

//File dove prendo le informazioni per il riconoscimento del volto
#define FILE_FACCIA_1 "haarcascade_frontalface_alt.xml"

#define FILE_OCCHIO_DESTRO "haarcascade_mcs_righteye.xml"
#define FILE_OCCHIO_SINISTRO "haarcascade_mcs_lefteye.xml"

//Nome della finestra
#define NOME_FINESTRA1 "Head tracking"
//Indica al programma se rilevare anche gli occhi, il naso e la bocca. (0 = no, 1 = si')
#define RILEVA_EXTRA 0

//Funzione per rilevare i dettagli, come parametro l'immagine e il nome del file cascade
void rilevaDettagli(IplImage *img,char *file)
{
    //Faccio spazio e prendo le informazioni dal file passato
    CvMemStorage *spazio = cvCreateMemStorage(0);
    CvHaarClassifierCascade *cascade = (CvHaarClassifierCascade*)cvLoad(file,spazio);
    // Analizzo gli elementi
    CvSeq *elementi = cvHaarDetectObjects(img,cascade,spazio,1.3,1,CV_HAAR_DO_CANNY_PRUNING,cvSize(40,40));
    //Controllo se ci sono elementi
    if(elementi->total > 0)
    {
        int i;
        //Creo i rettangoli
        for(i=0; i < elementi->total; i++)
        {
            //Prendo gli elementi
            CvRect *rettangolo = (CvRect*)cvGetSeqElem(elementi,i);
            //Creo i rettangoli di rosso nelle zone degli elementi
            cvRectangle(img,cvPoint(rettangolo->x,rettangolo->y),cvPoint(rettangolo->x+rettangolo->width,rettangolo->y+rettangolo->height),CV_RGB(255,0,0),1,8,0);
        }
    }
    //Mostro i rettangoli appena disegnati
    cvShowImage(NOME_FINESTRA1,img);
}


//Funzione per il rilevamento del volto, passiamo l'immagine, il file cascade e lo spazio da utilizzare
void rilevaVolto(IplImage *img,CvHaarClassifierCascade *cascade, CvMemStorage *spazio)
{
    //Analizzo gli elementi
    CvSeq *volti = cvHaarDetectObjects(img,cascade,spazio,1.1,3,CV_HAAR_DO_CANNY_PRUNING,cvSize(40,40));
    //Controllo se ci sono volti
    if(volti->total > 0)
    {
        int i;

            //Creo i rettangoli
            for(i=0; i < volti->total; i++)
            {
                //Prendo i volti trovati
                CvRect *rettangolo = (CvRect*)cvGetSeqElem(volti,i);
                //Se voglio cercare anche i dettagli
                if(RILEVA_EXTRA == 1)
                {
                    //Faccio il focus solo sulla faccia trovata escludendo il resto
                    cvSetImageROI(img,*rettangolo);
                    //Rilevo l'occhio sinistro
                    rilevaDettagli(img,(char*)FILE_OCCHIO_SINISTRO);
                    //Rilevo l'occhio destro
                    rilevaDettagli(img,(char*)FILE_OCCHIO_DESTRO);
                    //Faccio ritornare il focus sull'immagine normale
                    cvResetImageROI(img);
                }
                //Disegno il rettangolo del volto
                cvRectangle(img,cvPoint(rettangolo->x,rettangolo->y),cvPoint(rettangolo->x+rettangolo->width,rettangolo->y+rettangolo->height),CV_RGB(50,205,50),3,8,0);
            }
    }
}

int main(int argc,char **argv)
{
    if(argc != 2)
    {
        printf("usage: %s <mode>\n0 - integrate webcam\n1 - external webcam\n",argv[0]);
        exit(-1);
    }
    else
    {
        int web=atoi(argv[1]);
        if(web >= 0 && web <= 1)
        {
            //Questo è il frame
            IplImage *frame,*ridotta;
            //Immagine dalla cam
            CvCapture *immagine;
            //Creo uno spazio di memoria
            CvMemStorage *spazio=cvCreateMemStorage(0);
            //Carico il file cascade -IMPORTANTE PER IL FUNZIONAMENTO DEL TRACKING- 
            CvHaarClassifierCascade *cascade = (CvHaarClassifierCascade*) cvLoad(FILE_FACCIA_1,spazio);
            //Prendo i dati dalla cam
            immagine = cvCaptureFromCAM(web);
            
            cvSetCaptureProperty(immagine,CV_CAP_PROP_FRAME_WIDTH,640);
            cvSetCaptureProperty(immagine,CV_CAP_PROP_FRAME_HEIGHT,480);

            //Salvo il frame
            frame = cvQueryFrame(immagine);
            //Creo la finestra
            cvNamedWindow(NOME_FINESTRA1,CV_WINDOW_AUTOSIZE);
            
            //Ciclo finchè ci sono frame
            while(frame)
            {   
                cvFlip(frame,frame,1);       
                frame = diminuisci(frame,50);
                //Rilevo il volto e i relativi dettagli
                rilevaVolto(frame,cascade,spazio);
                //Mostra l'immagine
                cvShowImage(NOME_FINESTRA1,frame);
                //Libero lo spazio
                cvClearMemStorage(spazio);
                //Prende un altro frame
                frame = cvQueryFrame(immagine);
                //Aspetta 10 ms per controllare se viene premuto il tasto q (per uscire)
                char c = cvWaitKey(15);
                if(c == 'q') break;
            }
            
            cvReleaseHaarClassifierCascade(&cascade);
            cvReleaseCapture(&immagine);
            cvReleaseImage(&frame);
            cvDestroyWindow(NOME_FINESTRA1);
        }
        else
            puts("webcam not found");
    }
    return 0;
}


    
