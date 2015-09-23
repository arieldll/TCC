#include <opencv2/imgcodecs.hpp>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <stdlib.h>
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include <string.h>
#include <sstream>
#include <fstream>
#include <vector>
#include "opencv2/core/core.hpp"
#include <algorithm>
#include <unistd.h>
#include <postgresql/libpq-fe.h>
#include "/usr/include/pqxx/pqxx"


using namespace cv;
using namespace std;

//hide the local functions in an anon namespace
namespace {
	int quantidade_aguardo = 1; //provavelmente vá usar isso depois
	int threshold = 90, max_deslocamento_frame = 100;
	int contour_length_threshold= 90;
	
	typedef struct quadrado{
		Point inicio;
		Point fim;
		int id;
		int zona_deteccao;
	} quadrado;
	
	
	float quant_carros = 0;
	int quant_carros_zona[60];
	int min_tamanho_zona[60];
	
	
	float distancia_minima = 0; //tava 30
	int veiculos_leves = 0;
	int veiculos_pesados = 0;
	
	//int linha_x1 = 168, linha_y1 = 190;
	//int linha_x2 = 502, linha_y2 = 190;
	
	int linha_x1 = 174, linha_y1 = 188;
	int linha_x2 = 478, linha_y2 = 184;
	
	Point ini_linha(linha_x1, linha_y1), fim_linha(linha_x2, linha_y2);
	
	int morph_elem = 0;
	int morph_size = 0;
	int morph_operator = 0;
	int const max_operator = 4;
	int const max_elem = 2;
	int const max_kernel_size = 21;
	
	unsigned long long int mostrar;
	
	Ptr<BackgroundSubtractor> pMOG2; //MOG2 Background subtractor
	std::ofstream arquivo_escrita("posicoes.oar");
	
	void CallBackFunc(int event, int x, int y, int flags, void* userdata){
		if  ( event == EVENT_LBUTTONDOWN ){
			cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
			arquivo_escrita << "(" << x << ", " << y << ")" << endl;
			threshold+=5;
			//printf("\n\n\n\n\n\n\nmostrar vale: %lld\n\n\n\n\n\n\n", mostrar);
		}else if  ( event == EVENT_RBUTTONDOWN ){
			cout << "Right button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
			if(threshold > 0) threshold--;			
		}else if  ( event == EVENT_MBUTTONDOWN ){
			quantidade_aguardo += 100;
			cout << "Middle button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		}else if ( event == EVENT_MOUSEMOVE){
			
			//cout << "Mouse move over the window - position (" << x << ", " << y << ")" << endl;
		}
	}
	
	//std::vector< std::pair< Point, int> > ultimas_centroides;
	
	std::vector< vector<Point> > ultimas_centroides;
	std::vector< vector<Point> > atual_centroides;
	
	std::vector<quadrado> zonas_deteccao;
	
	std::vector<int> coletou_centroides;
	int id_carros = 0;	
	
	void adicionar_zona(int inicio_x, int inicio_y, int fim_x, int fim_y, int id, int zona_deteccao){
		quadrado q;
		q.inicio.x = inicio_x;
		q.inicio.y = inicio_y;
		q.fim.x = fim_x;
		q.fim.y = fim_y;
		q.id = id;
		q.zona_deteccao = zona_deteccao;
		zonas_deteccao.push_back(q);
		vector<Point> p;
		ultimas_centroides.push_back(p);
		atual_centroides.push_back(p);
		coletou_centroides.push_back(0);
	}
	
	/*int inicio_x = 316, inicio_y = 198;
			int fim_x = 420, fim_y = 248;
			
			/*no meio*/
			//int inicio_x = 238, inicio_y = 197;
			//int fim_x = 302, fim_y = 217;
			
			/*esquerdo*/
			//int inicio_x = 144, inicio_y = 156;
			//int fim_x = 227, fim_y = 220;*/
	
	
	
    int process(VideoCapture& capture) {
		
	  pqxx::connection conexao_postgres("dbname=tcc-ariel host=127.0.0.1 user=postgres password=postgres");

	 /* pqxx::result r = txn.exec(
		"SELECT id "
		"FROM Employee "
		"WHERE name =");

	  if (r.size() != 1)
	  {
		std::cerr
		  << "Expected 1 employee with name " << " " << ", "
		  << "but found " << r.size() << std::endl;
		return 1;
	  }

	  int employee_id = r[0][0].as<int>();
	  std::cout << "Updating employee #" << employee_id << std::endl;

	  txn.exec(
		"UPDATE EMPLOYEE "
		"SET salary = salary + 1 "
		"WHERE id = " + txn.quote(employee_id)); 

	  txn.commit(); */
		//PGconn *conn; //conexao com o postgres
		//conn = PQconnectdb("dbname=tcc-ariel host=127.0.0.1 user=postgres password=postgres");
		
		//adicionar_zona(317, 199, 418, 248, 1, 1);
		//adicionar_zona(200, 198, 310, 246, 2, 1);
		
		//adicionar_zona(102, 193, 184, 253, 3, 1);
		//adicionar_zona(19, 177, 71, 241, 4, 1);
		
		//adicionar_zona(316, 198, 420, 248, 1, 1);
		//adicionar_zona(238, 197, 302, 217, 2, 1);
		//adicionar_zona(144, 156, 227, 220, 3, 1);
		//adicionar_zona(11, 879, 228, 1003, 1, 1);
		
		//adicionar_zona(618, 843, 514, 917, 2, 2);
		
		//adicionar_zona(219, 451, 532, 819, 3, 3);
		
		//adicionar_zona(641, 850, 514, 912, 4, 4);
		
		
		
		/*novo video*/
		//zona 1
		//adicionar_zona(96, 938, 155, 956, 1, 1);
		//adicionar_zona(129, 967, 200, 999, 2, 1);
		
		//zona 2
		//adicionar_zona(564, 835, 619, 888, 3, 2);
		//adicionar_zona(551, 877, 579, 931, 4, 2);
	
		//adicionar_zona(84, 773, 137, 790, 5, 2);
		//adicionar_zona(42, 782, 95, 799, 6, 2);
		
		/*(340, 726)
(376, 763)
(379, 737)
(412, 771)*/
		
		//zona 3
		//adicionar_zona(340, 726, 376, 763, 7, 3);
		//adicionar_zona(379, 737, 412, 771, 8, 3);
		
		adicionar_zona(99, 383, 137, 413, 1, 1);
		adicionar_zona(141, 416, 176, 462, 2, 1);
		adicionar_zona(573, 299, 609, 353, 3, 2);
		adicionar_zona(533, 339, 576, 394, 4, 2);
		adicionar_zona(327, 203, 359, 230, 5, 3);
		adicionar_zona(370, 210, 412, 241, 6, 3);
		adicionar_zona(104, 244, 145, 257, 7, 2);
		adicionar_zona(61, 257, 103, 270, 8, 2);
		
		min_tamanho_zona[1] = 70;
		min_tamanho_zona[2] = 60;
		min_tamanho_zona[3] = 60;

		
        int n = 0;
        char filename[200];
        string window_name = "video | q or esc to quit";
        cout << "press space to save a picture. q or esc to quit" << endl;
        namedWindow("original", WINDOW_AUTOSIZE );
        namedWindow(window_name, WINDOW_AUTOSIZE ); //resizable window;
        namedWindow("substraction", WINDOW_AUTOSIZE );
        namedWindow("canny", WINDOW_AUTOSIZE ); //resizable window;
        Mat frame, frame_gray, fundo_gray, substraction, resultado_canny, frame_original;
        CvArr* arrayx;
		setMouseCallback(window_name, CallBackFunc, NULL);
		
		//carregar o fundo
		fundo_gray = imread("./fundo.png", CV_BGR2GRAY);
		int pi = 0;
		
		pMOG2 = createBackgroundSubtractorMOG2(350,16,false); //MOG2 approach
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		RNG rng(12345);
		int frames = 0;
		
		for(int pnk=0; pnk<60; pnk++){
			quant_carros_zona[pnk] = -50;
		}
		
        for(;;){
						
			
			
			printf("frame %d\n", frames);
			//getchar();
			int largest_area = 0;
			int largest_contour_index = 0;
			Rect bounding_rect;
			
			//mostrar++;

			// Crop the full image to that image contained by the rectangle myROI
			// Note that this doesn't copy the data
            capture >> frame >> frame_original;            
            cv::Rect myROI(2, 540, 709, 550);
			cv::Mat cropeedImage;
			cropeedImage = frame(myROI);
			frame = cropeedImage;
            
            if (frame.empty())
                break;
            arrayx =  &capture;
			
            //cv::line(frame, ini_linha, fim_linha, Scalar(0,255,255));

            // Converte a imagem para cinza e aplica uma mascara de media
		    cvtColor(frame, frame_gray, CV_BGR2GRAY);
		    blur(frame_gray, frame_gray, Size(3,3)); //operação gaussiana
		    
		    //fundo_gray = frame_gray;
		    /*if(mostrar==97){
				image2 = frame_gray.clone();
				pi++;
				printf("\ncapturou quadro");
			}else{
				if(pi==0){
					image2 = frame_gray.clone();
				}
			} */
			
		    // Cria uma janela
		    char source_window[7];
		    strcpy(source_window, "Source");

			//faz o background substraction
			pMOG2->apply(frame_gray, substraction);
			
			int operation = morph_operator + 2;			
			Mat element = getStructuringElement(2, Size(7,7), Point(0,0)); //com 7 vai de boas
						
			morphologyEx(substraction, substraction, MORPH_OPEN, element );
			
			element = getStructuringElement(0, Size(5,5), Point(0,0));
			morphologyEx(substraction, substraction, MORPH_CLOSE, element);			
			
			element = getStructuringElement(0, Size(25,25), Point(0,0));
			dilate(substraction, substraction, element);						
			//morphologyEx(substraction, substraction, MORPH_TOPHAT, element );
			//dilate(substraction, substraction, 0, Point(0,0), 2, 1, 1);
			
			//fastNlMeansDenoising(substraction, substraction);
			
			//cv::absdiff(frame_gray, fundo_gray, substraction); // Absolute differences between the 2 images
			//cv::threshold(substraction, substraction, 5, 255, CV_THRESH_BINARY); // set threshold to ignore small differences you can also use inrange function
			Canny(substraction, resultado_canny, 20, 100, 3);
			//Canny(frame, frame_gray, 100, 200, 3); // => fica doidão
			
			
			
		
			findContours(resultado_canny, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0) ); //CV_RETR_EXTERNAL
			/// Draw contours
			/*Mat drawing = Mat::zeros(frame_gray.size(), CV_8UC3 );
			for( int i = 0; i< contours.size(); i++ )
			{
				Scalar color = Scalar(0,255,0);
				drawContours(frame, contours, i, color, 2, 8, hierarchy, 0, Point() );
			} */
			
			/*for (vector<vector<Point> >::iterator it = contours.begin(); it!=contours.end(); )
			{
				if (it->size()<contour_length_threshold)
					it=contours.erase(it);
				else
					++it;
			} */
			
			// iterate through each contour.
			//int inicio_x = 123, inicio_y = 183;
			//int inicio_x = 144, inicio_y = 180; //-> tela inteira
			/*lado direito*/
			for(int gera = 0; gera < zonas_deteccao.size(); gera++){
				if(ultimas_centroides[gera].size()==0) coletou_centroides[gera] = 0;
				
				//int inicio_x = 316, inicio_y = 198;
				//int fim_x = 420, fim_y = 248;
				quadrado quad = zonas_deteccao[gera];
				int inicio_x = quad.inicio.x;
				int inicio_y = quad.inicio.y;
				int fim_x = quad.fim.x;
				int fim_y = quad.fim.y;
				int zona_detectando = quad.zona_deteccao;
				
				/*no meio*/
				//int inicio_x = 238, inicio_y = 197;
				//int fim_x = 302, fim_y = 217;
				
				/*esquerdo*/
				//int inicio_x = 144, inicio_y = 156;
				//int fim_x = 227, fim_y = 220;
				
				Rect desenho_linhas;
				desenho_linhas.x = inicio_x;
				desenho_linhas.y = inicio_y;
				desenho_linhas.width = fim_x - inicio_x;
				desenho_linhas.height = fim_y - inicio_y;
				
				rectangle(frame, desenho_linhas,  Scalar(255,0,0),2, 8,0);
				
				std::ostringstream astr;
				astr << "(" << zona_detectando << ")";
				cv::putText(frame, astr.str(), Point(inicio_x, inicio_y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, CV_RGB(255,255,0), 2, 1);
				
				for( int i = 0; i< contours.size(); i++){
					//  Find the area of contour
					double a = arcLength(contours[i],true); //contourArea é a área e arcLength é o perímetro
					//printf("contornos valem: %d\n", contours[i].size());
					if(a >= threshold){
						vector<Moments> mu(contours[i].size());
						vector<Point2f> mc( contours[i].size() );
						
						//copia contornos para os momentos da centroid
						for(int j=0; j<contours[i].size(); j++){
							mu[j] = moments(contours[i], false);
						}
						
						float quantidade_acumula = 0;
						//calcula a centroid e desenha ela na tela
						float centroid_x = 0, centroid_y = 0;
						for(size_t n = 0; n < contours.size(); n++){ 						
							mc[n] = Point2f( static_cast<float>(mu[n].m10/mu[n].m00) , static_cast<float>(mu[n].m01/mu[n].m00) ); 																
							centroid_x += mc[n].x;
							centroid_y += mc[n].y;											
							/*float calc1 = fim_linha.y - centroid_y / fim_linha.x - centroid_x;
							float calc2 = centroid_y - ini_linha.y / centroid_x - ini_linha.x;					
							float diferenca = abs(calc1 - calc2);	
							//if(diferenca < 0 ) diferenca *= -1;
							if(calc1==calc2){
								quantidade_acumula++;
								cout << "--------------------------- Contou um carro --------------------------- [" << diferenca << "]\n";
							}else{
								//cout << ">> calc 1: " << calc1 << " ====== calc 2: " << calc2 << " ========== \n";
							}*/
						}
						centroid_x /= contours.size();
						centroid_y /= contours.size();
						
						//bounding_rect = boundingRect(contours[i]);
						//rectangle(frame, bounding_rect,  Scalar(120,252,252),2, 8,0);				
						
						if(inicio_x <= centroid_x && fim_x>=centroid_x && inicio_y<=centroid_y && fim_y>=centroid_y){
							cout << "> Entrou na centróide <\n";
							Scalar color = Scalar(0,255,0);
							circle(frame, Point(centroid_x, centroid_y), 4, color, -1, 8, 0 );
							largest_area = a;
							largest_contour_index = i;
							Rect extremos = boundingRect(contours[i]);
							
							float tamanho_veiculo = sqrt(pow(extremos.width, 2) + pow(extremos.height, 2));
							cout << "Tamanho do veículo: " << tamanho_veiculo << "\n";
							
							bounding_rect = boundingRect(contours[i]);
							//Draw the contour and rectangle
							//drawContours( frame, contours,largest_contour_index, color, CV_FILLED,8,hierarchy);					
													
							rectangle(frame, bounding_rect,  Scalar(255,255,0),2, 8,0);				
							//quantidade_acumula++;
							//cout << "--------------------------- Contou um carro --------------------------- [" << quant_carros << "]\n";
							Point ponto = Point(centroid_x, centroid_y);
							
							//grava a atual
							atual_centroides[gera].push_back(ponto);
							
							if(coletou_centroides[gera]){
								float menor_dist = 999999;
								Point chave;
								int removedor = 0;
								for(int b=0; b < ultimas_centroides[gera].size() && ultimas_centroides[gera].size() > 0; b++){									
									Point p1 = ultimas_centroides[gera][b];
									Point p2 = ponto;
									float dist = sqrt(pow(p1.x - p1.y, 2) + pow(p2.x - p2.y, 2));
									cout << "Distância calculada " << dist << "\n";
									if(dist < menor_dist){ //encontrou um carro correspondente
										menor_dist = dist;
										chave.x = p1.x;
										chave.y = p1.y;
										removedor = b;
									}
								}
								
								char posi_string[10000];
								sprintf(posi_string, "%lf;%lf", centroid_x, centroid_y);								
								string posicao_string = posi_string;
								
								
								cout << "Menor distância " << menor_dist << "\n";
								cv::line(frame, ponto, chave, cv::Scalar(200,0,0), 3);
								//printf("menor distância: %f\n", menor_dist);							
								if(menor_dist > distancia_minima && atual_centroides[gera].size() > 1 && ultimas_centroides[gera].size() >= 1 && (tamanho_veiculo >= (float)min_tamanho_zona[zona_detectando])){
									cout << "\n================= CONTOU UM VEÍCULO =====================\n";
									id_carros++;
									int eh_leve = 0;
									if(tamanho_veiculo < 200){
										veiculos_leves++;
										eh_leve = 1;
									}else{
										veiculos_pesados++;
										eh_leve = 0;
									}
									if(quant_carros_zona[zona_detectando]==-50) quant_carros_zona[zona_detectando] = 0;
									quant_carros_zona[zona_detectando] += 1;
									/*pqxx::work txn(conexao_postgres);
									txn.exec("INSERT INTO veiculos_passagem(veiculo, posicao, classificacao, tempo, zona) values("+
										txn.quote(id_carros) + ", " + txn.quote(posicao_string) + ", " + txn.quote(eh_leve) + ", current_timestamp , " + txn.quote(zona_detectando) + ")");
									txn.commit();*/
									//ultimas_centroides[gera].push_back(ponto);
									//getchar();
										//"WHERE id = " + txn.quote(employee_id)); 				
									//getchar();
									//printf(" ---> Contou %d\n", id_carros);
								}else{
									//getchar();
								}
							}
							
							
							//grava a anterior
							if(!coletou_centroides[gera]){
								/*id_carros++;
								if(tamanho_veiculo < 90){
									veiculos_leves++;
								}else{
									veiculos_pesados++;
								} */
								//ultimas_centroides.push_back(make_pair(ponto, id_carros));
								ultimas_centroides[gera].push_back(ponto);
							}
						}
						
						//cout << "\n\n\n >>>>> Quantidade de carros contados até o momento: " << quant_carros;
						

						//if(a > largest_area){
							
							//cout << i << " area  " << a << endl;
							// Store the index of largest contour
							
							// Find the bounding rectangle for biggest contour
							
						//} 
						//Scalar color( 255,255,255);  // color of the contour in the
					}
				}
				//imshow( "Display window", src );    
				
				
				
				//imshow("", image2);

				//cv::line(substraction, cv::Point(160, 200), cv::Point(472, 220), cv::Scalar(220,0,0), 10, 4); //traçar a linha
				rectangle(substraction, desenho_linhas,  Scalar(255,0,0),2, 8,0);
				
				printf("\n Tamanho %d %d - %d", (int)atual_centroides[gera].size(), (int) ultimas_centroides[gera].size(), id_carros);
				if(coletou_centroides[gera]){
					
					/*for(int c=0; c<atual_centroides.size(); c++){
						float menor_dist = 999999;
						for(int b=0; b<ultimas_centroides.size(); b++){
							Point p1 = ultimas_centroides[b];
							Point p2 = atual_centroides[c];
							float dist = sqrt(pow(p1.x - p1.y, 2) + pow(p2.x - p2.y, 2));
							cv::line(frame, p1, p2, cv::Scalar(200,0,0), 10);
							//cout << "Distância " << dist << "\n";
							if(dist < menor_dist){ //encontrou um carro correspondente
								menor_dist = dist;
							}
						}
						if(menor_dist>distancia_minima){
							id_carros++;
							//printf(" ---> Contou %d\n", id_carros);
						}
					} */
					
					ultimas_centroides[gera].erase(ultimas_centroides[gera].begin(), ultimas_centroides[gera].end());
					ultimas_centroides[gera].clear();
					for(int b = 0; b<atual_centroides[gera].size(); b++){
						ultimas_centroides[gera].push_back(atual_centroides[gera][b]);
					}
					atual_centroides[gera].clear();
				}				
				
				coletou_centroides[gera] = 1;
				
			}
			
			
			std::ostringstream str;
			str << "(Leves: " << veiculos_leves << " / Pesados: " << veiculos_pesados << ")  - " << id_carros << "";
			cv::putText(frame, str.str(), Point(0,30), cv::FONT_HERSHEY_SIMPLEX, 1, CV_RGB(255,0,0), 3, 8);
			
			int alinha_zonas = 1;
			for(int i=0; i<60; i++){
				if(quant_carros_zona[i]!=-50){
					alinha_zonas+=25;
					std::ostringstream astr;
					astr << "> Zona: " << i << ": " << quant_carros_zona[i];
					cv::putText(frame, astr.str(), Point(0,30+alinha_zonas), cv::FONT_HERSHEY_SIMPLEX, 0.7, CV_RGB(0,0,0), 2, 6);
				}
			}
				
			imshow("original", frame_original);
			imshow(window_name, frame);			
		    imshow("substraction", substraction);
		    imshow("canny", resultado_canny);


            char key = (char) waitKey(quantidade_aguardo); //delay N millis, usually long enough to display and capture input  -fa

            switch (key) {
            case 'q':
            case 'Q':
            case 27: //escape key
                return 0;
            case ' ': //Save an image
                sprintf(filename,"filename%.3d.jpg",n++);
                imwrite(filename,frame);
                cout << "Saved " << filename << endl;
                break;
            default:
                break;
            }
			
            
            //frames++;
            //if(frames==310) break;
        }
        
        cout << "\n\n\n >>>>> Quantidade final de carros contados: " << round(id_carros) << "\n Veículos leves: " << veiculos_leves << "\n Veículos pesados: " << veiculos_pesados;
        return 0;
    }
}

int main(int ac, char** av) {

    if (ac != 2) {
        return 1;
    }
    std::string arg = av[1];
    VideoCapture capture(arg); //try to open string, this will attempt to open it as a video file or image sequence
    if (!capture.isOpened()) //if this fails, try to open as a video camera, through the use of an integer param
        capture.open(atoi(arg.c_str()));
    if (!capture.isOpened()) {
        cerr << "Failed to open the video device, video file or image sequence!\n" << endl;
        return 1;
    }
    cout << "capturando em " << capture.get(CV_CAP_PROP_FPS) << " fps\n";
    //exit(0);
    return process(capture);
}
