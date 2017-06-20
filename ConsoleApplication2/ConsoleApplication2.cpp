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

