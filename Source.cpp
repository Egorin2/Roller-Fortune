//#define _USE_MATH_DEFINES
#include<SDL.h>
#include<SDL_ttf.h>
#include <SDL2_gfxPrimitives.h>
#include<iostream>
#include<string.h>

#include <string>
#include <curl/curl.h>


#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"


const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGTH = 700;

const int maxSectors = 25;

const float radius = 400; //Радиус пирога
int sectorCirc = maxSectors; //Количество секторов
double speed = 0; //"Угловая" скорость

const char* Name_List[50] = { "Welcum to hell!", "Item 2", "Item 3", u8"Пошёл нахер, козёл!", u8"Гнида", u8"Петух", u8"The FUCK!!!", u8"УМРУ, НО НЕ СДАМСЯ!", "Item 9", "Item 10","Item 11", "Item 12","Item 13", "Item 14" ,"Item 15", "Item 16" ,"Item 17", "Item 18" ,"Item 19", "Item 20", "Item 21", "Item 22", "Item 23","Item 24", "Item 25","Item 26", "Item 27" ,"Item 28", "Item 29" ,"Item 30", "Item 31" };


SDL_Color BackGround;

SDL_Window* win = NULL;
SDL_Renderer* ren = NULL;

int win_width = SCREEN_WIDTH, win_heigth = SCREEN_HEIGTH;


//Дополнительные элементы для работы программы
struct Sector {
	int sector;
	char str[100];
	SDL_Rect rect;
	SDL_Color color;
	SDL_Texture* texture;
};

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
void DrawCircle(SDL_Renderer* render, int point_count, double radius, SDL_Point center, Uint8 width, SDL_Color color) {

	SDL_Point* points = (SDL_Point*)malloc(sizeof(SDL_Point) * (point_count + 1));
	double alphas = 0;
	for (int i = 0; i < point_count; i++)
	{
		alphas += 2 * M_PI / point_count;
		mathCordsToScrean(cos(alphas) * (radius), sin(alphas) * (radius), 1.0, center.x, center.y, points[i].x, points[i].y);
	}
	points[point_count] = points[0];
	for (int i = 0; i < point_count; i++) {
		thickLineRGBA(render, points[i].x, points[i].y, points[i + 1].x, points[i + 1].y, width, color.r, color.g, color.b, color.a);
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

		//thickLineRGBA(render, center.x, center.y, sx, sy, 3, 0, 0, 0, 255);
		thickLineRGBA(render, center.x, center.y, center.x + x, center.y - y, 3, 0, 0, 0, 255);
	}
	DrawCircle(render, 70, radius, center, 3, {0,0,0,255});
}
void DrawPieFromSector(SDL_Renderer* render, Sector* items, int count, int radius, SDL_Point center) {
	double lengSector = 360.0 / count;
	for (int i = 0; i < count; i++) {
		filledPieRGBA(render, center.x, center.y, radius, lengSector * i, lengSector * (i + 1), items[i].color.r, items[i].color.g, items[i].color.b, items[i].color.a);
	}
	for (int i = 0; i < count; i++) {
		int sx, sy;

		double x = radius * cos(((lengSector * (i))) * (M_PI / 180) * (-1));
		double y = radius * sin(((lengSector * (i))) * (M_PI / 180) * (-1));
		mathCordsToScrean(x, y, 1.0, center.x, center.y, sx, sy);
		//SDL_RenderDrawLine(ren, center.x, center.y, sx, sy);

		//thickLineRGBA(render, center.x, center.y, sx, sy, 3, 0, 0, 0, 255);
		thickLineRGBA(render, center.x, center.y, center.x + x, center.y - y, 3, 0, 0, 0, 255);
	}
	DrawCircle(render, 70, radius, center, 3, {0,0,0,255});
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
void UpdatePieTexture(SDL_Renderer* render, SDL_Texture* texture, int radius, int sector, SDL_Color* color, int countColor, SDL_Color bg) {
	
	SDL_SetRenderTarget(render, texture);
	//Заливаем фон
	SDL_SetRenderDrawColor(render, bg.r, bg.g, bg.b, bg.a); //Серый цвет
	SDL_RenderClear(render);

	DrawPie(render, radius, sector, color, countColor, { radius, radius});

	SDL_SetRenderTarget(render, NULL);
}
void UpdatePieTextureFromSector(SDL_Renderer* render, SDL_Texture* texture, int radius,  Sector* items, int count, SDL_Color bg) {
	SDL_SetRenderTarget(render, texture);
	//Заливаем фон
	SDL_SetRenderDrawColor(render, bg.r, bg.g, bg.b, bg.a);
	SDL_RenderClear(render);

	DrawPieFromSector(render, items, count, radius, { radius, radius });

	SDL_SetRenderTarget(render, NULL);
}
//------------------------------------------------
SDL_Texture* CreatePieTextureFromSector(SDL_Renderer* render, Sector* items, int count, int radius, SDL_Color bg) {
	SDL_Texture* textur = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, radius * 2, radius * 2);
	if (textur == NULL) {
		printf("Couldn`t create texture! Error: %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}
	SDL_SetRenderTarget(render, textur);

	//Заливаем фон
	SDL_SetRenderDrawColor(render, bg.r, bg.g, bg.b, bg.a); //Серый цвет
	SDL_RenderClear(render);
	DrawPieFromSector(render,items,count,radius, { radius , radius });

	SDL_SetRenderTarget(render, NULL);

	return textur;
}


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

void CreateTextureFromSector(SDL_Renderer* render, Sector* sector, TTF_Font* font, SDL_Color fg) {
	SDL_Surface* surface = TTF_RenderUTF8_Blended(font, sector->str, fg);
	sector->rect.w = surface->w;
	sector->rect.h = surface->h;
	sector->texture = SDL_CreateTextureFromSurface(render, surface);
	SDL_FreeSurface(surface);
}
void UpdateTexturText(SDL_Renderer* render, Sector* sector, TTF_Font* font, SDL_Color fg) {
	SDL_DestroyTexture(sector->texture);
	if (sector->str == u8"") {
		return;
	}
	SDL_Surface* surface = TTF_RenderUTF8_Blended(font, sector->str, fg);
	if (surface == NULL) {
		return;
	}
	sector->rect.w = surface->w;
	sector->rect.h = surface->h;
	sector->texture = SDL_CreateTextureFromSurface(render, surface);
	SDL_FreeSurface(surface);
}

//Фунцкии работы imGui
bool ColorEdit3U32(const char* label, SDL_Color& color, ImGuiColorEditFlags flags = 0) {
	float col[4];
	col[0] = (float)((color.r) & 0xFF) / 255.0f;
	col[1] = (float)((color.g) & 0xFF) / 255.0f;
	col[2] = (float)((color.b) & 0xFF) / 255.0f;
	col[3] = (float)((color.a) & 0xFF) / 255.0f;

	bool result = ImGui::ColorEdit3(label, col, flags);

	color = { (Uint8)(col[0] * 255), (Uint8)(col[1] * 255), (Uint8)(col[2] * 255), (Uint8)(col[3] * 255) };

	return result;
}

//cURL

size_t curlWriteFunc(char* data, size_t size, size_t nmemb, std::string* buffer)
{
	size_t result = 0;

	if (buffer != NULL)
	{
		buffer->append(data, size * nmemb);
		result = size * nmemb;
	}
	return result;
}

std::string curlBuffer;

bool request( std::string* buffer) {
	
	//SetConsoleOutputCP(CP_UTF8);
	
	// запрашиваемая страничка(путь до login screen)
	const char* url = R"(https://api.notion.com/v1/databases/65c17b98615f4ffb85f3b0b366950b92/query)";
	// передаваемые параметры
	const char* urlPOST = u8R"({"filter": {"and": [{"property": "Сезон", "select" : {"equals": "2 сезон"}}, {"property": "Status", "status": {"equals": "В предложке"}}] }})";

	//Передаваемый headers
	struct curl_slist* list = NULL;
	// буфер для сохранения текстовых ошибок
	char curlErrorBuffer[CURL_ERROR_SIZE];

	CURL* curl = curl_easy_init();
	if (curl) {
		//
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curlErrorBuffer);

		list = curl_slist_append(list, "Authorization: Bearer secret_fn3KKxMMz6vShXrPMj4MESqj9C4ApSEJ0Bbbc3CxQho");
		list = curl_slist_append(list, "Notion-Version: 2022-06-28");
		list = curl_slist_append(list, "content-type: application/json");
		list = curl_slist_append(list, "accept: application/json");

		// задаем URL...
		curl_easy_setopt(curl, CURLOPT_URL, url);
		// переходить по "Location:" указаному в HTTP заголовке  
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		// не проверять сертификат удаленного сервера
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		// использовать метод POST для отправки данных
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		// параметры POST
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, urlPOST);
		//параметры Headers
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		// функция, вызываемая cURL для записи полученых данных
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunc);

		// выполнить запрос
		CURLcode curlResult = curl_easy_perform(curl);
		// завершение сеанса
		curl_easy_cleanup(curl);

		if (curlResult == CURLE_OK)
		{
			return true;
		}
		else {
			std::cout << "Ошибка(" << curlResult << "): " << curlErrorBuffer << std::endl;	
			return false;
		}
	}
	return false;
}
void CopyBuffer(std::string buffer, Sector* items, int &count) {
	std::string find = u8"Название книги";	
	std::string find_content = "content";
	count = 0;
	std::size_t pos_init = 0;
	int len = find_content.length() + 3;
	std::size_t found = buffer.find(find, pos_init);
	while (found != std::string::npos) {
		found = buffer.find(find_content, found);
		pos_init = found + len;
		found = buffer.find('"', pos_init);
		int counts = found - pos_init;
		//curlBuffer.substr(pos_init, counts).data()
		std::string copy_string = buffer.substr(pos_init, counts);
		strcpy_s(items[count].str, copy_string.data());
		pos_init = found;
		found = buffer.find(find, pos_init);
		count++;
	}
}

int main(int argc, char* argv[])
{
	//Инициализция приложения
	Init();
	SDL_Event ev;

	#pragma region Initialization ImGui
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION(); //Что делает?
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(win, ren);
	ImGui_ImplSDLRenderer2_Init(ren);

	io.Fonts->Clear();//Переписать адрес шрифта
	ImFont* fontForGui = io.Fonts->AddFontFromFileTTF("F:\\ProgramProject\\Roller Fortune\\Debug\\fonts\\Candara.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
	IM_ASSERT(fontForGui != nullptr);
	#pragma endregion
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

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

	SDL_Color fg_str = { 120,20,20,255 };

	//Информация для пирога
	double lengSector = 360.0 / sectorCirc;
	double sectorCenter = lengSector / 2;

	bool isDrawTexture = false; //Для перерисовки текстуры

	double dspeed = 0; //Изменение скорости

	int inCURL = 0;
	bool res = false;
	char** req = (char**)malloc(sizeof(char*)*50);
	for (int i = 0; i < 50; i++) {
		char a[200] = "";
		req[i] = (char*)a;
	}
	//Создание основной текстуры
	SDL_Texture* textur = NULL;
	SDL_Point center = { win_width / 2,win_heigth / 2 };


	int max_items = 50;
	int init_items = 0;
	Sector* items = (Sector*)malloc(sizeof(Sector) * max_items);
	for (int i = 0; i < sectorCirc; i++) {
		int mod = i % 8;
		items[i].color = SDL_Color({ color[mod].r,color[mod].g,color[mod].b,color[mod].a });
		items[i].sector = i;
		strcpy_s(items[i].str, Name_List[i]);
		items[i].rect = SDL_Rect({ center.x + 100, center.y - font_size / 2, 0, 0 });
		CreateTextureFromSector(ren, &items[i], font, { 120,20,20,255 });

		init_items++;
	}


	//создание текстовых текстур секторов
	//SDL_Texture** item_str_text = (SDL_Texture**)malloc(sizeof(SDL_Texture*) * sectorCirc);
	//SDL_Rect* item_dstrect = (SDL_Rect*)malloc(sizeof(SDL_Rect) * sectorCirc);
	//Создание текстуры текста
	//for (int i = 0; i < sectorCirc; i++) {
		//item_str_text[i] = CreateTextureFromText(ren, Name_List[i], font, &item_dstrect[i], { 120,20,20,255 });
		//item_dstrect[i].x = center.x + 100;
		//item_dstrect[i].y = center.y - font_size / 2;
	//}
	//----------
	bool firstDraw = true;
	double angle = 0; //угол поворота колеса
	int choese = 0;
	int choese_item = 0;
	char choese_str[10];
	SDL_Texture* cho_text = NULL;
	SDL_Rect cho_rect = {0,0,0,0};
	int small_radius = 35;


	float second = 0;
	int choeseStep = 0;
	int mouse_pos_x = 0;
	int mouse_pos_y = 0;
	bool isCoese = false;
	bool isInCircle = false;
	bool isRunning = true;
	while (isRunning)
	{
		//Проверка сообщений
		while (SDL_PollEvent(&ev)) { //Проверка событий	
			ImGui_ImplSDL2_ProcessEvent(&ev);
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
						//item_dstrect[i].x = center.x + 100;
						//item_dstrect[i].y = center.y - font_size / 2;
						items[i].rect.x = center.x + 100;
						items[i].rect.y = center.y - font_size / 2;
					}
				}
				break;
			case SDL_MOUSEMOTION:
				if (!isCoese) {
					mouse_pos_x = ev.motion.x - center.x;
					mouse_pos_y = ev.motion.y - center.y;
					if (sqrt(mouse_pos_x * mouse_pos_x + mouse_pos_y * mouse_pos_y) < small_radius) {
						isInCircle = true;
					}
					else {
						isInCircle = false;
					}
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if ((ev.button.button == SDL_BUTTON_LEFT)&isInCircle) {
					isCoese = true;
				}
				break;
			case SDL_KEYDOWN:
				switch (ev.key.keysym.scancode)
				{
				case SDL_SCANCODE_UP:					
					speed += 15;
					
					//dspeed = speed / 5 * (-1);
					//stop = true;
					break;
				case SDL_SCANCODE_DOWN:
					speed -= 15;
					
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
					break;
				case SDL_SCANCODE_KP_PLUS:
					if (sectorCirc < maxSectors) {
						sectorCirc++;
						isDrawTexture = true;
					}
					break;
				case SDL_SCANCODE_KP_MINUS:
					if (sectorCirc > 2) {
						sectorCirc--;
						isDrawTexture = true;
					}
					break;
				default:
					break;
				}
				break;
			}
		}
		
		if (res) {
			if (request(&curlBuffer)) {
				CopyBuffer(curlBuffer, items, inCURL);
				for (int i = 0; i < sectorCirc; i++) {
					UpdateTexturText(ren, &items[i], font, fg_str);
				}
				sectorCirc = inCURL;
				isDrawTexture = true;
				curlBuffer = "";
			}			
			res = false;
		}


		//вычисляем угол
		lengSector = 360.0 / sectorCirc;
		sectorCenter = lengSector / 2;


		//счётчик тиков
		newtick = SDL_GetTicks();
		dt = newtick - lasttick;
		lasttick = newtick;

		//Алгоритм "Случайного" выбора
		if (isCoese) {
			if (choeseStep==0) {
				dspeed = 500 / 2;
				choeseStep++;
			}
			if ((choeseStep == 1)&(second>2)) {
				dspeed = 0;
				choeseStep++;
			}
			if ((choeseStep == 2)& (second > 7)) {
				dspeed = -1*speed/3;
				choeseStep++;
			}
			if ((choeseStep == 3) & ((speed + dspeed/1000*dt)<=0)) {
				speed = 0;
				dspeed = 0;
				isCoese = false;
				choeseStep = 0;
				second = 0;
			}
			second += ((float)dt) / 1000;
		}

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

		SDL_Point falseCenter = { center.x - items[0].rect.x, 25 / 2 }; //Необходим как центр вращения для текста
		
		SDL_Rect renderPie = { center.x - radius, center.y - radius, radius * 2,radius * 2 };
		SDL_Point centrPie = { radius, radius };

		//Расчёт положения указателя
		choese = (int)floor((angle+sectorCenter) / lengSector);
		if (choese >= 0) {
			for (choese; choese > sectorCirc; choese - sectorCirc);
			choese_item = sectorCirc - choese;
			if (choese_item == 0)
				choese_item = sectorCirc;
		}
		if (choese < 0)
			choese_item = choese * -1;
		//Создание текстуры указателя
		sprintf_s(choese_str, "%02i", choese_item);
		if (cho_text != NULL) SDL_DestroyTexture(cho_text);
		cho_text = CreateTextureFromText(ren, choese_str, font, &cho_rect, { 255,255,255,255 });
		cho_rect.x = center.x - 10;
		cho_rect.y = center.y - font_size / 2;


		#pragma region Start ImGui Setup
		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		if (!isCoese) {
			ImGui::SetNextWindowPos(ImVec2(0, 0), 0);
			ImGui::SetNextWindowSize(ImVec2(300, win_heigth));
			ImGui::Begin("GlobalMenus", 0, window_flags);

			if (ImGui::Button(u8"Запрос в БД"))
				res = true;

			for (int i = 0; i < sectorCirc; i++) {
				ImGui::PushID(i);
				if (ImGui::InputText("##input_text", items[i].str, 200))
					UpdateTexturText(ren, &items[i], font, fg_str);
				ImGui::SameLine();
				if (ColorEdit3U32("##color_edit", items[i].color, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
					isDrawTexture = true;
				ImGui::PopID();
			}
			ImGui::End();
		}
		ImGui::Render();
		#pragma endregion
		// ------------------------------------
		// Отрисовка
		// Очиска экрана
		SDL_SetRenderDrawColor(ren, BackGround.r, BackGround.g, BackGround.b, BackGround.a);
		SDL_RenderClear(ren);
 
#pragma region Draw_Pie
		if (firstDraw) {
			if (textur != NULL)	
				SDL_DestroyTexture(textur);
			//textur = CreatePieTexture(ren, radius, sectorCirc, color, 8, BackGround);

			textur = CreatePieTextureFromSector(ren, items, sectorCirc, radius, BackGround);
		}
		if (isDrawTexture) {
			//UpdatePieTexture(ren, textur, radius, sectorCirc, color, 8, BackGround);
			UpdatePieTextureFromSector(ren, textur, radius, items, sectorCirc, BackGround);
		}
#pragma endregion		

		
		//Поворот текстуры		
		SDL_RenderCopyEx(ren, textur, NULL, &renderPie, angle + sectorCenter, &centrPie, SDL_FLIP_NONE);

		//Печать повёрнутых слов
		//Рисование линий
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255); //Чёрный цвет
		for (int i = 0; i < sectorCirc; i++) {
			//Центр вращения относительно координат dstrect
			//SDL_RenderCopyEx(ren, item_str_text[i], NULL, &item_dstrect[i], angle + (lengSector * (i + 1)), &falseCenter, SDL_FLIP_NONE);
			SDL_RenderCopyEx(ren, items[i].texture, NULL, &items[i].rect, angle + (lengSector * (items[i].sector + 1)), &falseCenter, SDL_FLIP_NONE);

		}
		//Текст "плохпя программа"
		//SDL_RenderCopy(ren, str_text, NULL, &str_rect);

		//Рисование остальных элементов
		filledCircleRGBA(ren, center.x, center.y, small_radius, 0, 0, 0, 255);	//Маленький круг в центре	
		filledPieRGBA(ren,center.x+radius-20,center.y,50,-20,20,0,0,0,255); //Указатель
		SDL_RenderCopy(ren, cho_text, NULL, &cho_rect);
		if (isInCircle)
			DrawCircle(ren, 70, small_radius, center, 3, { 0,240,0,255 });

		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
		//Обновление картинки на экране
		SDL_RenderPresent(ren);

		//SDL_Delay(16);		


		//Окончание цикла
		isDrawTexture = false;
		firstDraw = false;
	}

	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();



	//for (int i = 0; i < sectorCirc; i++) {
		//SDL_DestroyTexture(item_str_text[i]);
	//}
	for (int i = 0; i < init_items; i++) {
		SDL_DestroyTexture(items[i].texture);
	}
	free(items);
	TTF_CloseFont(font);
	SDL_DestroyTexture(textur);
	SDL_DestroyTexture(str_text);

	DeInit(0);
	return 0;
}