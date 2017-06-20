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