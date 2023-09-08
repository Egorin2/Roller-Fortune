#define _USE_MATH_DEFINES
#include<SDL.h>
#include<SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>
#include<iostream>

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"


const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGTH = 1000;

const int maxSectors = 22;

const float radius = 400; //Радиус пирога
int sectorCirc = maxSectors; //Количество секторов
double speed = 50; //"Угловая" скорость

const char* Name_List[50] = { "Welcum to hell!", "Item 2", "Item 3", u8"Пошёл нахер, козёл!", u8"Гнида", u8"Петух", u8"The FUCK!!!", u8"УМРУ, НО НЕ СДАМСЯ!", "Item 9", "Item 10","Item 11", "Item 12","Item 13", "Item 14" ,"Item 15", "Item 16" ,"Item 17", "Item 18" ,"Item 19", "Item 20", "Item 21", "Item 22", "Item 23","Item 11", "Item 12","Item 13", "Item 14" ,"Item 15", "Item 16" ,"Item 17", "Item 18" };


SDL_Color BackGround;

SDL_Window* win = NULL;
SDL_Renderer* ren = NULL;

int win_width = SCREEN_WIDTH, win_heigth = SCREEN_HEIGTH;


//функции инициализации и деинициализации приложения
void DeInit(int error)
{
	if (ren != NULL) SDL_DestroyRenderer(ren);
	if (win != NULL) SDL_DestroyWindow(win);
	TTF_Quit();
	SDL_Quit();
	exit(error);
}

void Init()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Couldn`t init SDL! Error: %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}

	if (TTF_Init())
	{
		printf("Couldn`t init SDL_TTF! Error: %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}
	//Включить флаги
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN); //Что значит SDL_WINDOW_ALLOW_HIGHDPI?
	win = SDL_CreateWindow("Name window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_width, win_heigth, window_flags);
	if (win == NULL)
	{
		printf("Couldn`t create window! Error: %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED); //Что значит SDL_RENDERER_PRESENTVSYNC?
	if (ren == NULL)
	{
		printf("Couldn`t create renderer! Error: %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}

}


void mathCordsToScrean(double x, double y, double scale, int centerX, int centerY, int& sx, int& sy)
{
	double rx = centerX + x * scale;
	double ry = centerY - y * scale;
	sy = round(centerY - y * scale);
	sx = round(centerX + x * scale);
	/*printf("\nНовые значения");
	printf("\nСравнение значенией. Чистый x: %i , Переделанный: %i", (int)rx, (int)sx);
	printf("\nСравнение значенией. Чистый y: %i , Переделанный: %i", (int)ry, (int)sy);
	if (rx>sx) printf("\nНачальный x больше на: %i", (int)(rx-sx));*/
	sx = (int)rx;
	sy = (int)ry;
}


//Для рисование основной текстуры
void DrawCircle(SDL_Renderer* render, int point_count, double radius, SDL_Point center, Uint8 width) {

	SDL_Point* points = (SDL_Point*)malloc(sizeof(SDL_Point) * (point_count + 1));
	double alphas = 0;
	for (int i = 0; i < point_count; i++)
	{
		alphas += 2 * M_PI / point_count;
		mathCordsToScrean(cos(alphas) * (radius), sin(alphas) * (radius), 1.0, center.x, center.y, points[i].x, points[i].y);
	}
	points[point_count] = points[0];
	for (int i = 0; i < point_count; i++) {
		thickLineRGBA(render, points[i].x, points[i].y, points[i + 1].x, points[i + 1].y, width, 0, 0, 0, 255);
	}
	free(points);
}

void DrawPie(SDL_Renderer* render, int radius, int sector, SDL_Color* color, int countColor, SDL_Point center)
{
	double lengSector = 360.0 / sector;
	for (int i = 0; i < sector; i++) {
		int mod = i % countColor;
		filledPieRGBA(render, center.x, center.y, radius, lengSector * i, lengSector * (i + 1), color[mod].r, color[mod].g, color[mod].b, color[mod].a);
	}
	for (int i = 0; i < sector; i++) {
		int sx, sy;

		double x = radius * cos(((lengSector * (i))) * (M_PI / 180) * (-1));
		double y = radius * sin(((lengSector * (i))) * (M_PI / 180) * (-1));
		mathCordsToScrean(x, y, 1.0, center.x, center.y, sx, sy);
		//SDL_RenderDrawLine(ren, center.x, center.y, sx, sy);

		thickLineRGBA(render, center.x, center.y, sx, sy, 3, 0, 0, 0, 255);
		thickLineRGBA(render, center.x, center.y, center.x + x, center.y - y, 3, 0, 0, 0, 255);
	}
	DrawCircle(render, 70, radius, center, 3);
}
//Основная текстура
SDL_Texture* CreatePieTexture(SDL_Renderer* render, int radius, int sector, SDL_Color* color, int countColor, SDL_Color bg) {
	int delta = 0;
	SDL_Texture* textur = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, radius * 2 + delta, radius * 2 + delta);
	if (textur == NULL) {
		printf("Couldn`t create texture! Error: %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}
	SDL_SetRenderTarget(render, textur);

	//Заливаем фон
	SDL_SetRenderDrawColor(render, bg.r, bg.g, bg.b, bg.a); //Серый цвет
	SDL_RenderClear(render);

	DrawPie(render, radius, sector, color, countColor, { radius + delta,radius + delta });

	SDL_SetRenderTarget(render, NULL);

	return textur;
}
//------------------------------------------------

//Функция создания текстуры текста
SDL_Texture* CreateTextureFromText(SDL_Renderer* render, const char* str, TTF_Font* font, SDL_Rect* rect, SDL_Color fg) {
	SDL_Surface* surface = TTF_RenderUTF8_Blended(font, str, fg);
	rect->w = surface->w;
	rect->h = surface->h;
	//*rect = { 0, 0, surface->w, surface->h};
	SDL_Texture* textur = SDL_CreateTextureFromSurface(render, surface);
	SDL_FreeSurface(surface);
	return textur;
}

int main(int argc, char* argv[])
{
	//Инициализция приложения
	Init();
	SDL_Event ev;

	//Назначаем цвет фона
	BackGround = { 200, 200, 200,255 };

	//Инициализируем переменные для таймера
	int newtick = 0;
	int lasttick = SDL_GetTicks();
	int dt = 0;

	//инициализация шрифта
	int font_size = 25;
	TTF_Font* font = TTF_OpenFont("./fonts/Candara.ttf", font_size);


	char str[100] = u8"This program is not work! It is a bad job! Плохая программа!";
	SDL_Rect str_rect;
	SDL_Texture* str_text = CreateTextureFromText(ren, str, font, &str_rect, { 120,20,20,255 });
	str_rect.x = 10;
	str_rect.y = 10;

	//Временно создаём массив цветов
	SDL_Color color[] = {
		{255,160,122,255},
		{50,205,0,255},
		{255,215,0,255},
		{254,255,255,255},
		{186,85,211,255},
		{240,248,255,255},
		{128,128,128,255},
		{105,105,105,255}
	};

	//Информация для пирога
	double lengSector = 360.0 / sectorCirc;
	double sectorCenter = lengSector / 2;

	bool isDrawTexture = true; //Для перерисовки текстуры

	double dspeed = 0; //Изменение скорости

	//Создание основной текстуры
	SDL_Texture* textur = NULL;
	SDL_Point center = { win_width / 2,win_heigth / 2 };

	//создание текстовых текстур секторов
	SDL_Texture** item_str_text = (SDL_Texture**)malloc(sizeof(SDL_Texture*) * sectorCirc);
	SDL_Rect* item_dstrect = (SDL_Rect*)malloc(sizeof(SDL_Rect) * sectorCirc);
	//Создание текстуры текста
	for (int i = 0; i < sectorCirc; i++) {
		item_str_text[i] = CreateTextureFromText(ren, Name_List[i], font, &item_dstrect[i], { 120,20,20,255 });
		item_dstrect[i].x = center.x + 100;
		item_dstrect[i].y = center.y - font_size / 2;
	}
	//----------

	double angle = 0; //угол поворота колеса

	bool isRunning = true;
	while (isRunning)
	{
		//Проверка сообщений
		while (SDL_PollEvent(&ev)) { //Проверка событий	
			//ImGui_ImplSDL2_ProcessEvent(&ev);
			switch (ev.type)
			{
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_WINDOWEVENT:
				if (ev.window.event == SDL_WINDOWEVENT_RESIZED)
				{
					win_width = ev.window.data1;
					center.x = win_width / 2;
					win_heigth = ev.window.data2;
					center.y = win_heigth / 2;

					for (int i = 0; i < sectorCirc; i++) {
						item_dstrect[i].x = center.x + 100;
						item_dstrect[i].y = center.y - font_size / 2;
					}
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (ev.button.button == SDL_BUTTON_LEFT) {
					speed += 15;
				}
				if (ev.button.button == SDL_BUTTON_RIGHT) {
					speed -= 15;
				}
				break;
			case SDL_KEYDOWN:
				switch (ev.key.keysym.scancode)
				{
				case SDL_SCANCODE_LEFT:
					if (sectorCirc < maxSectors) {
						sectorCirc++;
						isDrawTexture = true;
					}
					//dspeed = speed / 5 * (-1);
					//stop = true;
					break;
				case SDL_SCANCODE_RIGHT:
					if (sectorCirc > 2) {
						sectorCirc--;
						isDrawTexture = true;
					}
					//stop = false;
					//dspeed = 5000 / 6;
					break;
				case SDL_SCANCODE_SPACE:
					if (speed != 0) {
						speed = 0;
					}
					else {
						speed = 50;
					}
				default:
					break;
				}
				break;

			default:
				break;
			}
		}
		
		//вычисляем угол
		lengSector = 360.0 / sectorCirc;
		sectorCenter = lengSector / 2;


		//счётчик тиков
		newtick = SDL_GetTicks();
		dt = newtick - lasttick;
		lasttick = newtick;



		/*
		if (speed > 5000 & !stop) {
			dspeed = 0;
		}
		if ((speed < 0 & speed >(-1 * dspeed)) | (speed < (-1 * dspeed) & speed>0)) {
			dspeed = speed / 5 * -1;
		}
		if ((0 < speed & speed<1 | 0>speed & speed > -1) & stop) {
			speed = 0;
			dspeed = 0;
			stop = false;
		}
		*/
		speed += dspeed / 1000 * dt;





		angle = angle + (speed / 1000) * dt;
		if (angle > 360) angle -= 360;
		if (angle < -360) angle += 360;

		SDL_Point falseCenter = { center.x - item_dstrect[0].x, 25 / 2 }; //Необходим как центр вращения для текста

		SDL_Rect renderPie = { center.x - radius, center.y - radius, radius * 2,radius * 2 };
		SDL_Point centrPie = { radius, radius };

		// ------------------------------------
		// Отрисовка
		// Очиска экрана
		SDL_SetRenderDrawColor(ren, BackGround.r, BackGround.g, BackGround.b, BackGround.a);
		SDL_RenderClear(ren);
 
#pragma region Draw_Pie
		if (isDrawTexture) {
			if (textur != NULL)	SDL_DestroyTexture(textur);
			textur = CreatePieTexture(ren, radius, sectorCirc, color, 8, BackGround);
			//textur = newCreatePieTexture(ren, radius, 8, , clear_color);
		}
#pragma endregion		

		
		//Поворот текстуры		
		SDL_RenderCopyEx(ren, textur, NULL, &renderPie, angle + sectorCenter, &centrPie, SDL_FLIP_NONE);

		//Печать повёрнутых слов
		//Рисование линий
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255); //Чёрный цвет
		for (int i = 0; i < sectorCirc; i++) {
			//Центр вращения относительно координат dstrect
			SDL_RenderCopyEx(ren, item_str_text[i], NULL, &item_dstrect[i], angle + (lengSector * (i + 1)), &falseCenter, SDL_FLIP_NONE);
		}

		SDL_RenderCopy(ren, str_text, NULL, &str_rect);

		//Рисование остальных элементов
		filledCircleRGBA(ren, center.x, center.y, 35, 0, 0, 0, 255);
		

		//Обновление картинки на экране
		SDL_RenderPresent(ren);

		//SDL_Delay(16);		


		//Окончание цикла
		isDrawTexture = false;
	}



	for (int i = 0; i < sectorCirc; i++) {
		SDL_DestroyTexture(item_str_text[i]);
	}
	TTF_CloseFont(font);
	SDL_DestroyTexture(textur);
	SDL_DestroyTexture(str_text);

	DeInit(0);
	return 0;
}