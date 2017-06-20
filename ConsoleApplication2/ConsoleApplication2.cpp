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
<<<<<<< HEAD

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
=======

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
>>>>>>> 381bc626dbbb01ec43852f026048bc8b76826033
}