//#define _USE_MATH_DEFINES
#include<SDL.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include <SDL2_gfxPrimitives.h>
#include<iostream>
#include<string.h>
#include<thread>

#include <string>
#include <curl/curl.h>


#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"


const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGTH = 700;

const int maxSectors = 25;

const float radius = 400; //������ ������
int sectorCirc = maxSectors; //���������� ��������
double speed = 0; //"�������" ��������

const char* Name_List[50] = { "Welcum to hell!", "Item2", "Item3", u8"��� ������", u8"��� ����� ����", u8"�����", u8"Item 7", "Item8", "Item 9", u8"��� ������","Item 11", u8"����� � ��� ����� � ���","Item 13", "Item 14", u8"��� ����� ���������� �� ��������", "Item 16" ,u8"����, �� �� ������!", "Item 18" ,u8"��������", "Item 20", "Item 21", "Item 22", "Item 23","Item 24", "Item 25","Item 26", "Item 27" ,"Item 28", "Item 29" ,"Item 30", "Item 31" };


SDL_Color BackGround;

SDL_Window* win = NULL;
SDL_Renderer* ren = NULL;

int win_width = SCREEN_WIDTH, win_heigth = SCREEN_HEIGTH;


//�������������� �������� ��� ������ ���������
struct Sector {
	int sector;
	char str[100];
	SDL_Rect rect;
	SDL_Color color;
	SDL_Texture* texture;
};

//������� ������������� � ��������������� ����������
void DeInit(int error)
{
	if (ren != NULL) SDL_DestroyRenderer(ren);
	if (win != NULL) SDL_DestroyWindow(win);
	Mix_Quit();
	TTF_Quit();
	SDL_Quit();
	exit(error);
}

void Init()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO) != 0)
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
	if (Mix_Init(MIX_INIT_OGG)==0) {
		printf("Couldn`t init SDL_mixer! Error: %s\n", SDL_GetError());
		system("pause");
		DeInit(1);
	}
	//�������� �����
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN); //��� ������ SDL_WINDOW_ALLOW_HIGHDPI?
	win = SDL_CreateWindow("Name window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_width, win_heigth, window_flags);
	if (win == NULL)
	{
		printf("Couldn`t create window! Error: %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}

	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED| SDL_RENDERER_PRESENTVSYNC);
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
	/*printf("\n����� ��������");
	printf("\n��������� ���������. ������ x: %i , ������������: %i", (int)rx, (int)sx);
	printf("\n��������� ���������. ������ y: %i , ������������: %i", (int)ry, (int)sy);
	if (rx>sx) printf("\n��������� x ������ ��: %i", (int)(rx-sx));*/
	sx = (int)rx;
	sy = (int)ry;
}


//��� ��������� �������� ��������
int pieRGBA_fix(SDL_Renderer* renderer, Sint16 x, Sint16 y, Sint16 rad, double start, double end, Uint8 r, Uint8 g, Uint8 b, Uint8 a, Uint8 filled)
{
	int result;
	double angle, start_angle, end_angle;
	double deltaAngle;
	double dr;
	int numpoints, i;
	Sint16* vx, * vy;

	/*
	* Sanity check radii
	*/
	if (rad < 0) {
		return (-1);
	}

	/*
	* Fixup angles
	*/
	//start = start % 360;
	//end = end % 360;

	/*
	* Special case for rad=0 - draw a point
	*/
	if (rad == 0) {
		return (pixelRGBA(renderer, x, y, r, g, b, a));
	}

	/*
	* Variable setup
	*/
	dr = (double)rad;
	deltaAngle = 3.0 / dr;
	start_angle = (double)start * (2.0 * M_PI / 360.0);
	end_angle = (double)end * (2.0 * M_PI / 360.0);
	if (start > end) {
		end_angle += (2.0 * M_PI);
	}

	/* We will always have at least 2 points */
	numpoints = 2;

	/* Count points (rather than calculating it) */
	angle = start_angle;
	while (angle < end_angle) {
		angle += deltaAngle;
		numpoints++;
	}

	/* Allocate combined vertex array */
	vx = vy = (Sint16*)malloc(2 * sizeof(Uint16) * numpoints);
	if (vx == NULL) {
		return (-1);
	}

	/* Update point to start of vy */
	vy += numpoints;

	/* Center */
	vx[0] = x;
	vy[0] = y;

	/* First vertex */
	angle = start_angle;
	vx[1] = x + (int)(dr * cos(angle));
	vy[1] = y + (int)(dr * sin(angle));

	if (numpoints < 3)
	{
		result = lineRGBA(renderer, vx[0], vy[0], vx[1], vy[1], r, g, b, a);
	}
	else
	{
		/* Calculate other vertices */
		i = 2;
		angle = start_angle;
		while (angle < end_angle) {
			angle += deltaAngle;
			if (angle > end_angle)
			{
				angle = end_angle;
			}
			vx[i] = x + (int)(dr * cos(angle));
			vy[i] = y + (int)(dr * sin(angle));
			i++;
		}

		/* Draw */
		if (filled) {
			result = filledPolygonRGBA(renderer, vx, vy, numpoints, r, g, b, a);
		}
		else {
			result = polygonRGBA(renderer, vx, vy, numpoints, r, g, b, a);
		}
	}

	/* Free combined vertex array */
	free(vx);

	return (result);
}
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
		//filledPieRGBA(render, center.x, center.y, radius, lengSector * i, lengSector * (i + 1), color[mod].r, color[mod].g, color[mod].b, color[mod].a);
		pieRGBA_fix(render, center.x, center.y, radius, lengSector * i, lengSector * (i + 1), color[mod].r, color[mod].g, color[mod].b, color[mod].a, 1);
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
		//filledPieRGBA(render, center.x, center.y, radius, lengSector * i, lengSector * (i + 1), items[i].color.r, items[i].color.g, items[i].color.b, items[i].color.a);
		pieRGBA_fix(render, center.x, center.y, radius, lengSector * i, lengSector * (i + 1), items[i].color.r, items[i].color.g, items[i].color.b, items[i].color.a, 1);
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
//�������� ��������
SDL_Texture* CreatePieTexture(SDL_Renderer* render, int radius, int sector, SDL_Color* color, int countColor, SDL_Color bg) {
	int delta = 0;
	SDL_Texture* textur = SDL_CreateTexture(render, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, radius * 2 + delta, radius * 2 + delta);
	if (textur == NULL) {
		printf("Couldn`t create texture! Error: %s", SDL_GetError());
		system("pause");
		DeInit(1);
	}
	SDL_SetRenderTarget(render, textur);

	//�������� ���
	SDL_SetRenderDrawColor(render, bg.r, bg.g, bg.b, bg.a); //����� ����
	SDL_RenderClear(render);

	DrawPie(render, radius, sector, color, countColor, { radius + delta,radius + delta });

	SDL_SetRenderTarget(render, NULL);

	return textur;
}
void UpdatePieTexture(SDL_Renderer* render, SDL_Texture* texture, int radius, int sector, SDL_Color* color, int countColor, SDL_Color bg) {
	
	SDL_SetRenderTarget(render, texture);
	//�������� ���
	SDL_SetRenderDrawColor(render, bg.r, bg.g, bg.b, bg.a); //����� ����
	SDL_RenderClear(render);

	DrawPie(render, radius, sector, color, countColor, { radius, radius});

	SDL_SetRenderTarget(render, NULL);
}
void UpdatePieTextureFromSector(SDL_Renderer* render, SDL_Texture* texture, int radius,  Sector* items, int count, SDL_Color bg) {
	SDL_SetRenderTarget(render, texture);
	//�������� ���
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

	//�������� ���
	SDL_SetRenderDrawColor(render, bg.r, bg.g, bg.b, bg.a); //����� ����
	SDL_RenderClear(render);
	DrawPieFromSector(render,items,count,radius, { radius , radius });

	SDL_SetRenderTarget(render, NULL);

	return textur;
}


//������� �������� �������� ������
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
	if (sector->texture != NULL) {
		SDL_DestroyTexture(sector->texture);
		sector->texture = NULL;
	}
	if (sector->str == "\0") {
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

//������� ������ imGui
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
	
	// ������������� ���������(���� �� login screen)
	const char* url = R"(https://api.notion.com/v1/databases/65c17b98615f4ffb85f3b0b366950b92/query)";
	// ������������ ���������
	const char* urlPOST = u8R"({"filter": {"and": [{"property": "�����", "select" : {"equals": "2 �����"}}, {"property": "Status", "status": {"equals": "� ���������"}}] }})";

	//������������ headers
	struct curl_slist* list = NULL;
	// ����� ��� ���������� ��������� ������
	char curlErrorBuffer[CURL_ERROR_SIZE];

	CURL* curl = curl_easy_init();
	if (curl) {
		//
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curlErrorBuffer);

		list = curl_slist_append(list, "Authorization: Bearer secret_fn3KKxMMz6vShXrPMj4MESqj9C4ApSEJ0Bbbc3CxQho");
		list = curl_slist_append(list, "Notion-Version: 2022-06-28");
		list = curl_slist_append(list, "content-type: application/json");
		list = curl_slist_append(list, "accept: application/json");

		// ������ URL...
		curl_easy_setopt(curl, CURLOPT_URL, url);
		// ���������� �� "Location:" ��������� � HTTP ���������  
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
		// �� ��������� ���������� ���������� �������
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		// ������������ ����� POST ��� �������� ������
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		// ��������� POST
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, urlPOST);
		//��������� Headers
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
		// �������, ���������� cURL ��� ������ ��������� ������
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlWriteFunc);

		// ��������� ������
		CURLcode curlResult = curl_easy_perform(curl);
		// ���������� ������
		curl_easy_cleanup(curl);

		if (curlResult == CURLE_OK)
		{
			return true;
		}
		else {
			std::cout << "������(" << curlResult << "): " << curlErrorBuffer << std::endl;	
			return false;
		}
	}
	return false;
}
void CopyBuffer(std::string buffer, Sector* items, int &count) {
	std::string find = u8"�������� �����";	
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

std::thread t1;

int main(int argc, char* argv[])
{
	
	//������������ ����������
	Init();
	SDL_Event ev;
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		printf("Couldn`t open audio! Error: %s\n", SDL_GetError());
		system("pause");
		DeInit(1);
	}

	Mix_Music* music = Mix_LoadMUS_RW(SDL_RWFromFile(".\\music\\Dead-Or-Alive.ogg","r"), SDL_TRUE);
	if (music == NULL) {
		printf("Couldn`t load music! Error: %s\n", SDL_GetError());
		system("pause");
		DeInit(1);
	}
	#pragma region Initialization ImGui
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION(); //��� ������?
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(win, ren);
	ImGui_ImplSDLRenderer2_Init(ren);

	io.Fonts->Clear();//���������� ����� ������
	ImFont* fontForGui = io.Fonts->AddFontFromFileTTF(".\\fonts\\Candara.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic());
	IM_ASSERT(fontForGui != nullptr);
	#pragma endregion
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;

	//��������� ���� ����
	BackGround = { 200, 200, 200,255 };

	//�������������� ���������� ��� �������
	int newtick = 0;
	int lasttick = SDL_GetTicks();
	int dt = 0;

	//������������� ������
	int font_size = 25;
	TTF_Font* font = TTF_OpenFont("./fonts/Candara.ttf", font_size);
	TTF_Font* font_for_choes = TTF_OpenFont("./fonts/Candara.ttf", font_size*2);

	char str[100] = u8"This program is not work! It is a bad job! ������ ���������!";
	SDL_Rect str_rect;
	SDL_Texture* str_text = CreateTextureFromText(ren, str, font, &str_rect, { 120,20,20,255 });
	str_rect.x = 10;
	str_rect.y = 10;

	//�������� ������ ������ ������
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

	//���������� ��� ������
	double lengSector = 360.0 / sectorCirc;
	double sectorCenter = lengSector / 2;

	bool isDrawTexture = false; //��� ����������� ��������

	double dspeed = 0; //��������� ��������

	int inCURL = 0;
	bool res = false;
	char** req = (char**)malloc(sizeof(char*)*50);
	for (int i = 0; i < 50; i++) {
		char a[200] = "";
		req[i] = (char*)a;
	}
	//�������� �������� ��������
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


	//�������� ��������� ������� ��������
	//SDL_Texture** item_str_text = (SDL_Texture**)malloc(sizeof(SDL_Texture*) * sectorCirc);
	//SDL_Rect* item_dstrect = (SDL_Rect*)malloc(sizeof(SDL_Rect) * sectorCirc);
	//�������� �������� ������
	//for (int i = 0; i < sectorCirc; i++) {
		//item_str_text[i] = CreateTextureFromText(ren, Name_List[i], font, &item_dstrect[i], { 120,20,20,255 });
		//item_dstrect[i].x = center.x + 100;
		//item_dstrect[i].y = center.y - font_size / 2;
	//}
	//----------
	bool firstDraw = true;
	double angle = 0; //���� �������� ������
	int choese = 0;
	int choese_item = 0;
	char choese_str[10];
	SDL_Texture* cho_text = NULL;
	SDL_Rect cho_rect = {0,0,0,0};
	int small_radius = 35;

	bool isPlayMusic = false;

	bool endTread = false;

	bool globalItem = false;
	bool endChoes = false;
	char end_str[100] = u8"����� ������ �������� ��";
	SDL_Rect end_rect = {100,100,0,0};
	SDL_Texture* end_str_text = CreateTextureFromText(ren, end_str, font_for_choes, &end_rect, { 255,255,255,255 });
	SDL_Texture* choes_text = NULL;
	SDL_Rect choes_rect = {100,125,0,0};

	float second = 0;
	int choeseStep = 0;
	int mouse_pos_x = 0;
	int mouse_pos_y = 0;
	bool isCoese = false;
	bool isInCircle = false;
	bool isRunning = true;
	//Mix_PlayMusic(music, -1);
	
	while (isRunning)
	{
		//�������� ���������
		while (SDL_PollEvent(&ev)) { //�������� �������	
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
					isInCircle = false;
					isCoese = true;
					//Mix_PlayMusic(music, 0);
				}
				break;
			case SDL_KEYDOWN:
				switch (ev.key.keysym.scancode)
				{
				case SDL_SCANCODE_UP:					
					speed += 15;
					if (!isPlayMusic) {
						isPlayMusic = true;
						Mix_PlayMusic(music, -1);
					}
					
					break;
				case SDL_SCANCODE_DOWN:
					speed -= 15;
					if (!isPlayMusic) {
						isPlayMusic = true;
						Mix_PlayMusic(music, -1);
					}
					break;
				case SDL_SCANCODE_SPACE:
					if (speed != 0) {
						speed = 0;
						isPlayMusic = false;
						Mix_PauseMusic();
					}
					else {
						speed = 50;
						//Mix_PlayMusic(music, -1);
						isPlayMusic = true;
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
					if (globalItem)
						globalItem = false;
					break;
				}
				break;
			}
		}
		
		if (res) {
			
			std::thread t1([&]()
				{
					if (request(&curlBuffer)) {
						CopyBuffer(curlBuffer, items, inCURL);						
						sectorCirc = inCURL;
						//isDrawTexture = true;
						curlBuffer = "";
						endTread = true;
					}
				});
			t1.detach();
			/*
			if (request(&curlBuffer)) {
				CopyBuffer(curlBuffer, items, inCURL);
				for (int i = 0; i < sectorCirc; i++) {
					UpdateTexturText(ren, &items[i], font, fg_str);
				}
				sectorCirc = inCURL;
				isDrawTexture = true;
				curlBuffer = "";
			}*/			
			res = false;
		}

		if (endTread) {
			for (int i = 0; i < sectorCirc; i++) {
				UpdateTexturText(ren, &items[i], font, fg_str);
			}
			isDrawTexture = true;
			endTread = false;
		}

		//��������� ����
		lengSector = 360.0 / sectorCirc;
		sectorCenter = lengSector / 2;


		//������� �����
		newtick = SDL_GetTicks();
		dt = newtick - lasttick;
		lasttick = newtick;

		//�������� "����������" ������
		if (isCoese) {

			if (choeseStep==0) {
				Mix_FadeInMusic(music, 0, 2000);
				dspeed = 700 / 2;
				choeseStep++;
			}
			if ((choeseStep == 1)&(second>2)) {
				dspeed = 0;
				choeseStep++;
			}
			if ((choeseStep == 2)& (second > 7)) {
				dspeed = -1*speed/4;
				choeseStep++;
			}
			if ((choeseStep == 3) & ((speed + dspeed/1000*dt)<=0)) {
				speed = 0;
				dspeed = 0;
				isCoese = false;
				choeseStep = 0;
				second = 0;
				endChoes = true;
			}
			if (second > 9)
				Mix_FadeOutMusic(2000);;
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

		SDL_Point falseCenter = { center.x - items[0].rect.x, 25 / 2 }; //��������� ��� ����� �������� ��� ������
		
		SDL_Rect renderPie = { center.x - radius, center.y - radius, radius * 2,radius * 2 };
		SDL_Point centrPie = { radius, radius };

		//������ ��������� ���������
		choese = (int)floor((angle+sectorCenter) / lengSector);
		if (choese >= 0) {
			for (choese; choese > sectorCirc; choese - sectorCirc);
			choese_item = sectorCirc - choese;
			if (choese_item == 0)
				choese_item = sectorCirc;
		}
		if (choese < 0)
			choese_item = choese * -1;
		//�������� �������� ���������
		sprintf_s(choese_str, "%02i", choese_item);
		if (cho_text != NULL) SDL_DestroyTexture(cho_text);
		cho_text = CreateTextureFromText(ren, choese_str, font, &cho_rect, { 255,255,255,255 });
		cho_rect.x = center.x - 10;
		cho_rect.y = center.y - font_size / 2;

		if (endChoes) {
			choes_text = CreateTextureFromText(ren, items[choese_item - 1].str, font_for_choes, &choes_rect, { 255,255,255,255 });
			endChoes = false;
			globalItem = true;
		}

		#pragma region Start ImGui Setup
		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		if (!isCoese&!globalItem) {
			ImGui::SetNextWindowPos(ImVec2(0, 0), 0);
			ImGui::SetNextWindowSize(ImVec2(300, win_heigth));
			ImGui::Begin("GlobalMenus", 0, window_flags);

			if (ImGui::Button(u8"������ � ��"))
				res = true;

			for (int i = 0; i < sectorCirc; i++) {
				ImGui::PushID(i);
				if (ImGui::InputText("##input_text", items[i].str, 100))
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
		// ���������
		// ������ ������
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

		
		//������� ��������		
		SDL_RenderCopyEx(ren, textur, NULL, &renderPie, angle + sectorCenter, &centrPie, SDL_FLIP_NONE);

		//������ ��������� ����
		//��������� �����
		SDL_SetRenderDrawColor(ren, 0, 0, 0, 255); //׸���� ����
		for (int i = 0; i < sectorCirc; i++) {
			//����� �������� ������������ ��������� dstrect
			//SDL_RenderCopyEx(ren, item_str_text[i], NULL, &item_dstrect[i], angle + (lengSector * (i + 1)), &falseCenter, SDL_FLIP_NONE);
			if(items[i].texture != NULL)
				SDL_RenderCopyEx(ren, items[i].texture, NULL, &items[i].rect, angle + (lengSector * (items[i].sector + 1)), &falseCenter, SDL_FLIP_NONE);

		}
		//����� "������ ���������"
		//SDL_RenderCopy(ren, str_text, NULL, &str_rect);

		//��������� ��������� ���������
		filledCircleRGBA(ren, center.x, center.y, small_radius, 0, 0, 0, 255);	//��������� ���� � ������	
		filledPieRGBA(ren,center.x+radius-20,center.y,50,-20,20,0,0,0,255); //���������
		SDL_RenderCopy(ren, cho_text, NULL, &cho_rect);
		if (isInCircle|isCoese)
			DrawCircle(ren, 70, small_radius, center, 3, { 0,240,0,255 });

		if (globalItem) {
			SDL_SetRenderDrawColor(ren,100,100,100,255);
			end_rect.x = center.x - end_rect.w /2;
			end_rect.y = center.y - (radius / 2)+10;
			choes_rect.x = end_rect.x;
			choes_rect.y = end_rect.y + end_rect.h;
			SDL_Rect gl = { end_rect.x - 10, end_rect.y - 10 , end_rect.w+20, end_rect.h + choes_rect.h+20 };
			SDL_RenderFillRect(ren,&gl);
			SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
			SDL_RenderDrawRect(ren, &gl);
			SDL_RenderCopy(ren,end_str_text,NULL,&end_rect);
			SDL_RenderCopy(ren, choes_text, NULL, &choes_rect);
		}

		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
		//���������� �������� �� ������
		SDL_RenderPresent(ren);

		//SDL_Delay(16);		


		//��������� �����
		isDrawTexture = false;
		firstDraw = false;
		
	}

	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();



	//for (int i = 0; i < sectorCirc; i++) {
		//SDL_DestroyTexture(item_str_text[i]);
	//}
	Mix_FreeMusic(music);
	for (int i = 0; i < init_items; i++) {
		SDL_DestroyTexture(items[i].texture);
	}
	if(choes_text != NULL)
		SDL_DestroyTexture(choes_text);
	SDL_DestroyTexture(end_str_text);
	free(items);
	TTF_CloseFont(font);
	SDL_DestroyTexture(textur);
	SDL_DestroyTexture(str_text);

	DeInit(0);
	return 0;
}