#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace std;
using namespace cv;

int thresh = 50, N = 5;  //zmienne pozwalajace na konfiguracje / dostosowanie oczytywania kwadratow z ekranu (kalibracja)
const char* wndname = "Wykrywanie kart";  //nazwa okienka

										  //dodatkowa funkcja odkrywajaca kat miedzy wektorami
static double angle(Point pt1, Point pt2, Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

//funkcja odpowiedzialna za wyszukiwanie kwadratow na ekranie
static void findSquares(const Mat& image, vector<vector<Point> >& squares)
{
	//czyszczenie poprzednich wyszukan
	squares.clear();

	//rozmycie ekranu = zwieksza wykrywanie krawedzi
	Mat timg(image);
	medianBlur(image, timg, 9);
	Mat gray0(timg.size(), CV_8U), gray;

	vector<vector<Point> > contours;

	//znajdywanie kwadratow w kazdej plaszczyznie obrazu
	for (int c = 0; c < 3; c++)
	{
		int ch[] = { c, 0 };
		mixChannels(&timg, 1, &gray0, 1, ch, 1);

		//sprawdzanie kilku poziomow
		for (int l = 0; l < N; l++)
		{
			//Canny pomaga zlapac kwadraty z gradientem cieniowania
			if (l == 0)
			{
				//zastosowanie Canny
				Canny(gray0, gray, 5, thresh, 5);
				dilate(gray, gray, Mat(), Point(-1, -1));
			}
			else
			{
				gray = gray0 >= (l + 1) * 255 / N;
			}

			//Znalezienie kontorow i zapisanie ich do listy
			findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

			vector<Point> approx;

			//testowanie kazdego konturu
			for (size_t i = 0; i < contours.size(); i++)
			{
				approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
				//sprawdzanie czy ma 4 wierzcholki
				//sprawdzaie na wiekszym obszarze, zeby filtrowalo wiekszosc "halasow"
				if (approx.size() == 4 &&
					fabs(contourArea(Mat(approx))) > 1000 &&
					isContourConvex(Mat(approx)))
				{
					double maxCosine = 0;

					for (int j = 2; j < 5; j++)
					{
						//znajdywanie maksymalnego cosinusa miedzy krawedziami
						double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
						maxCosine = MAX(maxCosine, cosine);
					}
					//jezeli wszystkie katy sa mniejwiecej wielkosci ~90 stopni -> zapisanie ich do listy
					if (maxCosine < 0.3)
						squares.push_back(approx);
				}
			}
		}
	}
}

//funkcja odpowiedzialna za wyszukiwanie kwadratow w danym obszarze na ekranie
void checkSquaresPositions(const vector<vector<Point> >& squares, bool &LT, bool &LM, bool &LB, bool &RT, bool &RM, bool &RB)
{
	system("cls");
	//pobieranie kwadratow i zapis koordynatow ich wierzcholkow
	for (size_t i = 0; i < squares.size(); i++)
	{
		auto p1 = &squares[i][0];
		auto p2 = &squares[i][1];
		auto p3 = &squares[i][2];
		auto p4 = &squares[i][3];
		int x1 = p1->x;
		int y1 = p1->y;
		int x2 = p2->x;
		int y2 = p2->y;
		int x3 = p3->x;
		int y3 = p3->y;
		int x4 = p4->x;
		int y4 = p4->y;

		//sprawdzanie w jakiej pozycji sa kwadraty za pomoca koordynatow ich wierzcholkow
		//lewo
		if ((x1 > 200) && (x2 > 200) && (x3 > 200) && (x4 > 200) && (x1 < 322) && (x2 < 322) && (x3 < 322) && (x4 < 322))
		{
			//gora
			if ((y1 > 80) && (y2 > 80) && (y3 > 80) && (y4 > 80) && (y1 < 200) && (y2 < 200) && (y3 < 200) && (y4 < 200))  LT = true;
			//srodek
			else if ((y1 > 200) && (y2 > 200) && (y3 > 200) && (y4 > 200) && (y1 < 300) && (y2 < 300) && (y3 < 300) && (y4 < 300)) LM = true;
			//dol
			else if ((y1 > 300) && (y2 > 300) && (y3 > 300) && (y4 > 300) && (y1 < 420) && (y2 < 420) && (y3 < 420) && (y4 < 420)) LB = true;
		}

		//prawo
		if ((x1 > 322) && (x2 > 322) && (x3 > 322) && (x4 > 322) && (x1 < 450) && (x2 < 450) && (x3 < 450) && (x4 < 450))
		{
			//gora
			if ((y1 > 80) && (y2 > 80) && (y3 > 80) && (y4 > 80) && (y1 < 200) && (y2 < 200) && (y3 < 200) && (y4 < 200)) RT = true;
			//srodek
			else if ((y1 > 200) && (y2 > 200) && (y3 > 200) && (y4 > 200) && (y1 < 300) && (y2 < 300) && (y3 < 300) && (y4 < 300)) RM = true;
			//dol
			else if ((y1 > 300) && (y2 > 300) && (y3 > 300) && (y4 > 300) && (y1 < 420) && (y2 < 420) && (y3 < 420) && (y4 < 420)) RB = true;
		}
	}
}


//funkcja pobiera dane zawierajace polozenie wszystkich kwadratow i za ich pomoca sprawdza jaka to jest karta ( rozpisane w .doc = wzor na dana karte)
void checkCards(bool LT, bool LM, bool LB, bool RT, bool RM, bool RB, char &currentCard, char &currentSuit)
{
	//pik
	if (LT == false && RT == false)currentSuit = char(006);
	//trefl
	if (LT == true && RT == false)currentSuit = char(005);
	// karo
	if (LT == false && RT == true)currentSuit = char(004);
	//kier
	if (LT == true && RT == true)currentSuit = char(003);
	// A
	if (LM == false && RM == false && LB == false && RB == true)currentCard = 'A';
	// 2
	if (LM == false && RM == false && LB == true && RB == false)currentCard = '2';
	// 3
	if (LM == false && RM == false && LB == true && RB == true)currentCard = '3';
	// 4
	if (LM == false && RM == true && LB == false && RB == false)currentCard = '4';
	// 5
	if (LM == false && RM == true && LB == false && RB == true)currentCard = '5';
	// 6
	if (LM == false && RM == true && LB == true && RB == false)currentCard = '6';
	// 7
	if (LM == false && RM == true && LB == true && RB == true)currentCard = '7';
	// 8
	if (LM == true && RM == false && LB == false && RB == false)currentCard = '8';
	// 9
	if (LM == true && RM == false && LB == false && RB == true)currentCard = '9';
	// 10
	if (LM == true && RM == false && LB == true && RB == false)currentCard = '1';
	// J
	if (LM == true && RM == false && LB == true && RB == true)currentCard = 'J';
	// D
	if (LM == true && RM == true && LB == false && RB == false)currentCard = 'D';
	// K
	if (LM == true && RM == true && LB == false && RB == true)currentCard = 'K';
	// poza zakresem, brak karty
	if (LM == true && RM == true && LB == true && RB == false)
	{
		cout << "brak" << endl;
	}
	// poza zakresem, brak karty
	if (LM == true && RM == true && LB == true && RB == true)
	{
		cout << "brak" << endl;
	}
}


//funckja ktora rysuje karte w konsoli
void drawCards(char currentCard, char currentSuit)
{
	//stala wysokosc i szerokosc karty
	const int cardHeight = 14;
	const int cardWidth = 14;
	char tenT = ' ';
	char tenB = ' ';
	system("cls");

	//tablica zawierajaca wstepne narysowanie karty, jak ma karta wygladac
	char cardView[cardHeight][cardWidth] = {
		"1555555555552",
		"6           6",
		"6 cd      S 6",
		"6           6",
		"6           6",
		"6           6",
		"6           6",
		"6           6",
		"6           6",
		"6           6",
		"6           6",
		"6 S      DC 6",
		"6           6",
		"3555555555554" };


	//zaleznie od tego czy to karta czarna czy czerwona, tak bedzie narysowana w konsoli
	if (currentSuit == char(006) || currentSuit == char(005))
	{
		system("color F0");
	}
	if (currentSuit == char(004) || currentSuit == char(003))
	{
		system("color F4");
	}


	//rysowanie karty w konsoli za pomoca tablicy i rysowania kazdego 'chara' po koleji
	for (int y = 0; y < cardHeight; ++y) {
		for (int x = 0; x < cardWidth; ++x) {
			char curChar = cardView[y][x];
			switch (curChar) {
			case '1':
				cardView[y][x] = char(201);
				break;
			case '2':
				cardView[y][x] = char(187);
				break;
			case '3':
				cardView[y][x] = char(200);
				break;
			case '4':
				cardView[y][x] = char(188);
				break;
			case '5':
				cardView[y][x] = char(205);
				break;
			case '6':
				cardView[y][x] = char(186);
				break;

				// jezeli jest to '10' to narysowanie liczby bedzie wygladalo inaczej, tak aby liczba byla narysowana poprawnie w odpowiednim miejscu

				//dol
			case 'C':
				if (currentCard == '1')
				{
					tenB = '0';
					cardView[y][x] = tenB;
				}
				else
				{
					cardView[y][x] = currentCard;
				}
				break;
			case 'D':
				if (tenT == '0')
				{
					cardView[y][x] = '1';
				}
				else
				{
					cardView[y][x] = tenB;
				}
				break;
				//gora
			case 'c':
				if (currentCard == '1')
				{
					cardView[y][x] = currentCard;
					tenT = '0';
				}
				else
				{
					cardView[y][x] = currentCard;
				}
				break;
			case 'd':
				cardView[y][x] = tenT;
				break;
			case 'S':
				cardView[y][x] = currentSuit;
				break;
			default:
				break;
			}
			cout << cardView[y][x];
		}
		cout << endl;
	}

}

//funkcja ktora rysuje zielone kwadraty na ekranie by pomoc w ich zlokalizowaniu i odpowiednim ustawieniu
static void drawSquares(Mat& image, const vector<vector<Point> >& squares)
{
	//pobiera dane kwadratow (ich wierzcholkow) i laczy 4 wierzcholki linia tworzac prostokaty / kwadraty
	for (size_t i = 0; i < squares.size(); i++)
	{
		const Point* p = &squares[i][0];
		int n = static_cast<int>(squares[i].size());
		if (p->x > 3 && p->y > 3)
			polylines(image, &p, &n, 1, true, Scalar(0, 255, 0), 3, LINE_AA);
	}
	imshow(wndname, image);
}

//funkcja do rysowania linii na ekranie - pomagajaca ustawic odpowiedni kat i odleglosc kamery
void interface(Mat image)
{
	Scalar lineColor = Scalar(0, 255, 0);
	line(image, Point(322, 100), Point(322, 400), lineColor, 3); //pionowa linia
	line(image, Point(222, 200), Point(422, 200), lineColor, 3); //pozioma gorna
	line(image, Point(222, 300), Point(422, 300), lineColor, 3); //pozioma dolna
}

/*
zrodlo / core https://github.com/alyssaq/opencv/blob/master/squares.cpp

-jezeli nie wyswietla znakow (pik, trefl itd) -> console -> options -> use old console
-zeby lepiej wyswietlalo zmiana konsoli na Lucida Console.
*/

int main(int /*argc*/, char** /*argv*/)
{
	char currentCard; //aktualna figura / liczba
	char currentSuit; //aktualna figura / liczba
	namedWindow(wndname, 1);  //tworzenie okna 
	vector<vector<Point> > squares; //tworzenie listy potrzebnej do zapisu kwadratow
	bool LT, LM, LB, RT, RM, RB; //czy w danej czesci ekranu jest kwadrat

	bool continueCapture = true;
	VideoCapture cap(0); //pobieranie obrazu z kamery

						 //jezeli nie odczytuje kamery to program sie wylacza
	if (!cap.isOpened())
	{
		return -1;
	}

	//jezeli odczytuje to kontynuuje
	while (continueCapture) {
		Mat image;
		//wczytanie obrazu
		cap >> image;

		//jezeli wczytalo obraz
		if (cap.read(image))
		{
			//reset booli przy kazdym odswiezeniu
			LT = false, LM = false, LB = false, RT = false, RM = false, RB = false;

			//uruchomienie funkcji w odpowiedniej kolejnosci
			findSquares(image, squares);
			interface(image);
			checkSquaresPositions(squares, LT, LM, LB, RT, RM, RB);
			checkCards(LT, LM, LB, RT, RM, RB, currentCard, currentSuit);
			drawCards(currentCard, currentSuit);
			drawSquares(image, squares);

			//czekanie na klikniecie klawisza
			auto c = waitKey();

			//jezeli 'ESC' -> wylacza program
			if (static_cast<char>(c) == 27)
			{
				return 0;
			}
			//resetowanie aktualnej karty
			currentCard = ' ';
			currentSuit = ' ';
		}
		else continueCapture = false;
	}
	return 0;
}