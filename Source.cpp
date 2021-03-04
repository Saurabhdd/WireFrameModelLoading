
#include<Windows.h>
#include<stdio.h>
#include<math.h>
#include<gl/gl.h>
#include<gl/GLU.h>
#include"Header.h"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

#define GL_PI 3.14159
#define WINWIDTH 800
#define WINHEIGHT 600

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

//variable declaration for file
FILE *pFile = NULL;
FILE *pLogFile = NULL;
FILE *gbFile_SSD = NULL;

HWND ghwnd_SSD;
BOOL gbActiveWindow_SSD = FALSE;
BOOL gbFullScreen_SSD = FALSE;

HGLRC hglrc_SSD;
HDC ghdc_SSD;
DWORD dwStyle_SSD;
WINDOWPLACEMENT wPrev_SSD;
int WINDOWMAXWIDTH_SSD, WINDOWMAXHEIGHT_SSD, X_SSD, Y_SSD;
GLfloat angle_SSD = 0.0f;

GLfloat xRotate = -90.0f;
GLfloat yRotate = 0.0f;
GLfloat zRotate = 0.0f;

//mesh loading variables and declarations
#define NR_NORMAL_COORDS 3
#define NR_TEXTURE_COORDS 2
#define NR_VERTEX_COORDS 3
#define NR_FACE_TOKENS 3
#define NR_TRIANGLE_VERTICES 3

#define LINE_SIZE 128
#define S_EQUAL 0

typedef struct _vec_int {

	int **pp_arr;
	size_t size;
}vec_int;

typedef struct _vec_float {

	float **pp_arr;
	size_t size;
}vec_float;

//vectors
vec_float *p_vec_float_vertext = NULL;
vec_float *p_vec_float_normal = NULL;
vec_float *p_vec_float_texture = NULL;

vec_int *p_vec_int_vertex = NULL;
vec_int *p_vec_int_texture = NULL;
vec_int *p_vec_int_normal = NULL;



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	//Function declaration
	void Display(void);
	void Initialize(void);

	//variable declaration
	HWND hwnd_SSD;
	BOOL bDone_SSD = FALSE;
	TCHAR szAppName_SSD[] = TEXT("MyWIndow");
	WNDCLASSEX wndclass_SSD;
	MSG msg_SSD;

	//Creation of the log file
	if (fopen_s(&gbFile_SSD, "logApp.txt", "w"))
	{
		MessageBox(NULL, TEXT("Log file creation unsuccessful, exiting"), TEXT("Error"), MB_OK);
		exit(0);
	}

	wPrev_SSD = { sizeof(WINDOWPLACEMENT) };

	wndclass_SSD.cbClsExtra = 0;
	wndclass_SSD.cbSize = sizeof(WNDCLASSEX);
	wndclass_SSD.cbWndExtra = 0;
	wndclass_SSD.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass_SSD.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass_SSD.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass_SSD.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass_SSD.hInstance = hInstance;
	wndclass_SSD.lpfnWndProc = WndProc;
	wndclass_SSD.lpszClassName = szAppName_SSD;
	wndclass_SSD.lpszMenuName = NULL;
	wndclass_SSD.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	RegisterClassEx(&wndclass_SSD);

	WINDOWMAXWIDTH_SSD = GetSystemMetrics(SM_CXMAXIMIZED);
	WINDOWMAXHEIGHT_SSD = GetSystemMetrics(SM_CYMAXIMIZED);

	X_SSD = (WINDOWMAXWIDTH_SSD / 2) - (WINWIDTH / 2);
	Y_SSD = (WINDOWMAXHEIGHT_SSD / 2) - (WINHEIGHT / 2);

	hwnd_SSD = CreateWindowEx(WS_EX_APPWINDOW, szAppName_SSD, TEXT("practice"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		X_SSD, Y_SSD,
		WINWIDTH,
		WINHEIGHT,
		NULL,
		NULL,
		hInstance,
		NULL);

	ghwnd_SSD = hwnd_SSD;

	Initialize();
	SetForegroundWindow(hwnd_SSD);
	SetFocus(hwnd_SSD);

	while (bDone_SSD == FALSE)
	{
		if (PeekMessage(&msg_SSD, NULL, 0, 0, PM_REMOVE))
		{
			if (msg_SSD.message == WM_QUIT)
				bDone_SSD = TRUE;

			else
			{
				TranslateMessage(&msg_SSD);
				DispatchMessage(&msg_SSD);
			}
		}
		else
		{
			if (gbActiveWindow_SSD == TRUE)
			{
				Display();
			}
		}
	}

	return (int)msg_SSD.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	void ToggleFullScreen(void);
	void Uninitialize(void);
	void Resize(int, int);
	void ChangeDisplay(void);
	void Display(void);

	switch (iMsg)
	{


	case WM_SETFOCUS:
		gbActiveWindow_SSD = TRUE;
		break;

	case WM_KILLFOCUS:
		gbActiveWindow_SSD = FALSE;
		break;

	case WM_ERASEBKGND:
		return 0;

	case WM_SIZE:
		Resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_CHAR:
		switch (wParam)
		{
		case 'x':
			xRotate -= 1.0f;
			break;

		case 'X':
			xRotate += 1.0f;
			break;

		case 'y':
			yRotate -= 1.0f;
			break;

		case 'Y':
			yRotate += 1.0f;
			break;

		case 'z':
			zRotate -= 1.0f;
			break;

		case 'Z':
			zRotate += 1.0f;
				break;

		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);

		case 0x46:
		case 0x66:
			ToggleFullScreen();
			break;

		default:
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_PAINT:
		//Display();
		break;



	case WM_DESTROY:
		Uninitialize();
		PostQuitMessage(0);
		break;

	}
	return (DefWindowProc(hwnd, iMsg, wParam, lParam));

}

void ToggleFullScreen(void)
{
	//local variable declaration
	MONITORINFO mi_SSD = { sizeof(MONITORINFO) };
	//code

	if (gbFullScreen_SSD == FALSE)
	{
		dwStyle_SSD = GetWindowLong(ghwnd_SSD, GWL_STYLE); //getWindowLong(ghwnd, GWL_STYLE);

		if (dwStyle_SSD & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd_SSD, &wPrev_SSD) && GetMonitorInfo(MonitorFromWindow(ghwnd_SSD, MONITORINFOF_PRIMARY), &mi_SSD))
			{
				SetWindowLong(ghwnd_SSD, GWL_STYLE, (dwStyle_SSD & ~WS_OVERLAPPEDWINDOW));

				SetWindowPos(ghwnd_SSD, HWND_TOP, mi_SSD.rcMonitor.left, mi_SSD.rcMonitor.top, (mi_SSD.rcMonitor.right - mi_SSD.rcMonitor.left), (mi_SSD.rcMonitor.bottom - mi_SSD.rcMonitor.top), SWP_NOZORDER | SWP_FRAMECHANGED);  //SetWindowPosition(ghwnd, HWND_TOP, mi.rcmonitor.left, mi.rcMonitor.top, (mi.rcMonitor.right - mi.rcMonitor.left), (mi.rcMonitor.bottom - mi.rcMonitor.top), SWP_NOZORDER | SWP_FRAMECHANGED); //WM_NCCALCSIZE

			}
		}

		ShowCursor(FALSE);
		gbFullScreen_SSD = TRUE;
	}

	else
	{
		SetWindowLong(ghwnd_SSD, GWL_STYLE, (dwStyle_SSD | WS_OVERLAPPEDWINDOW));
		SetWindowPlacement(ghwnd_SSD, &wPrev_SSD);
		SetWindowPos(ghwnd_SSD, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
		ShowCursor(TRUE);
		gbFullScreen_SSD = FALSE;
	}

}

void Initialize()
{
	//function declaration
	void Resize(int, int);
	void LoadMeshData(void);

	//variable declarations
	PIXELFORMATDESCRIPTOR pfd_SSD;
	int iPixelFormatIndex_SSD;

	//code
	ghdc_SSD = GetDC(ghwnd_SSD);

	ZeroMemory(&pfd_SSD, sizeof(PIXELFORMATDESCRIPTOR));
	pfd_SSD.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd_SSD.nVersion = 1;
	pfd_SSD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd_SSD.iPixelType = PFD_TYPE_RGBA;
	pfd_SSD.cColorBits = 32;
	pfd_SSD.cRedBits = 8;
	pfd_SSD.cGreenBits = 8;
	pfd_SSD.cBlueBits = 8;
	pfd_SSD.cAlphaBits = 8;

	iPixelFormatIndex_SSD = ChoosePixelFormat(ghdc_SSD, &pfd_SSD);
	if (iPixelFormatIndex_SSD == 0)
	{
		fprintf_s(gbFile_SSD, "choosepixel()  failed\n");
		DestroyWindow(ghwnd_SSD);
	}

	if (SetPixelFormat(ghdc_SSD, iPixelFormatIndex_SSD, &pfd_SSD) == FALSE)
	{
		fprintf_s(gbFile_SSD, "SetPixel()  failed\n");
		DestroyWindow(ghwnd_SSD);
	}

	hglrc_SSD = wglCreateContext(ghdc_SSD);
	if (hglrc_SSD == NULL)
	{
		fprintf_s(gbFile_SSD, "wglCreateContext()  failed\n");
		DestroyWindow(ghwnd_SSD);
	}

	if (wglMakeCurrent(ghdc_SSD, hglrc_SSD) == FALSE)
	{
		fprintf_s(gbFile_SSD, "wglMakeCurrent()  failed\n");
		DestroyWindow(ghwnd_SSD);
	}

	glShadeModel(GL_SMOOTH);

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	LoadMeshData();

	//glColor3f(0.0f, 1.0f, 0.0f);
	//glShadeModel(GL_FLAT);
	//glFrontFace(GL_CW);

	Resize(WINWIDTH, WINHEIGHT);

}

void Resize(int width, int height)
{
	if (height <= 0) {
		height = 1;
	}

	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f);

}

void Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -15.0f);
	glTranslatef(0.0f, -6.0f, 0.0f);

	glRotatef(xRotate, 1.0f, 0.0f, 0.0f);
	glRotatef(yRotate, 0.0f, 1.0f, 0.0f);
	glRotatef(zRotate, 0.0f, 0.0f, 1.0f);

	glColor3f(1.0f, 1.0f, 1.0f);

	glFrontFace(GL_CCW);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if (p_vec_int_vertex == NULL)
	{
		MessageBox(ghwnd_SSD, TEXT("int vertext not initialised"), TEXT("Error:"), MB_OK);
		//DestroyWindow(ghwnd_SSD);
	}

	if (p_vec_float_vertext == NULL)
	{
		MessageBox(ghwnd_SSD, TEXT("float vertext not initialised"), TEXT("Error:"), MB_OK);
		DestroyWindow(ghwnd_SSD);
	}

	for (int i = 0; i != p_vec_int_vertex->size; ++i)
	{
		glBegin(GL_TRIANGLES);

		for (int j = 0; j != NR_TRIANGLE_VERTICES; j++)
		{
			int vi = p_vec_int_vertex->pp_arr[i][j] - 1;
			int vi_t = p_vec_int_texture->pp_arr[i][j] - 1;

			glTexCoord2f(p_vec_float_texture->pp_arr[vi_t][0], p_vec_float_texture->pp_arr[vi_t][1]);
			glVertex3f(p_vec_float_vertext->pp_arr[vi][0], p_vec_float_vertext->pp_arr[vi][1],
				p_vec_float_vertext->pp_arr[vi][2]);
		}
		glEnd();
	}

	SwapBuffers(ghdc_SSD);

}

void Update(void)
{
	angle_SSD = angle_SSD + 0.1f;
	if (angle_SSD >= 360.0f)
		angle_SSD = 0.0f;
}

void LoadMeshData(void)
{
	//fuction declaration
	vec_int *CreateVector_int(void);
	vec_float* CreateVector_float(void);
	void PushBack_vec_int(vec_int*, int *);
	void PushBack_vec_float(vec_float*, float*);
	void Destroy_vec_int(vec_int *);
	void Destroy_vec_float(vec_float*);

	//opening the file
	if (fopen_s(&pFile, "MeshOBJ.obj", "r"))
	{
		MessageBox(ghwnd_SSD, TEXT("Log file creation unsuccessful, exiting"), TEXT("Error"), MB_OK);
		DestroyWindow(ghwnd_SSD);
	}
	

	if (fopen_s(&pLogFile, "log.txt", "w"))
	{
		MessageBox(ghwnd_SSD, TEXT("Log file creation unsuccessful, exiting"), TEXT("Error"), MB_OK);
		DestroyWindow(ghwnd_SSD);
	}
	

	//variable declaration when file is successfully opened
	//vec_int* p_vec_int = NULL;
	char *token = NULL;
	char *firstToken = NULL;
	const char *space_seperator = " ";
	const char *slash_seperator = "/";
	char *nextToken1 = NULL;
	char *nextToken2 = NULL;
	char arr_line[LINE_SIZE];

	float vertex[3];
	float texture[3];
	float normal[3];

	int vertex_i[3];
	int texture_i[3];
	int normal_i[3];

	char *vertex_token = NULL;
	char *texture_token = NULL;
	char *normal_token = NULL;

	char *face_tokens[NR_FACE_TOKENS];

	p_vec_float_normal = CreateVector_float();
	p_vec_float_vertext = CreateVector_float();
	p_vec_float_texture = CreateVector_float();

	p_vec_int_normal = CreateVector_int();
	p_vec_int_texture = CreateVector_int();
	p_vec_int_vertex = CreateVector_int();

	while (fgets(arr_line, LINE_SIZE, pFile))
	{
		firstToken = strtok_s(arr_line, space_seperator, &nextToken1);

		if (strcmp(firstToken, "v") == S_EQUAL)
		{
			for (int i = 0; i < NR_VERTEX_COORDS; i++)
			{
				token = strtok_s(NULL, space_seperator, &nextToken1);
				vertex[i] = atof(token);
			}
			PushBack_vec_float(p_vec_float_vertext, vertex);
			memset(vertex, (float)'\0', sizeof(float)* 3);
		}

		else if (strcmp(firstToken, "vt") == S_EQUAL)
		{
			for (int i = 0; i < NR_TEXTURE_COORDS; i++)
			{
				token = strtok_s(NULL, space_seperator, &nextToken1);
				texture[i] = atof(token);
			}
			PushBack_vec_float(p_vec_float_texture, texture);
			memset(texture, (float)'\0', sizeof(float)* 3);
		}

		else if (strcmp(firstToken, "vn") == S_EQUAL)
		{
			for (int i = 0; i < NR_NORMAL_COORDS; i++)
			{
				token = strtok_s(NULL, space_seperator, &nextToken1);
				normal[i] = atof(token);
			}
			PushBack_vec_float(p_vec_float_normal, normal);
			memset(normal, 0, sizeof(float) * 3);
		}

		else if (strcmp(firstToken, "f") == S_EQUAL)
		{
			int nr_tokens = 0;

			while ((token = strtok_s(NULL, space_seperator, &nextToken1)) != NULL && nr_tokens < 3)
			{
				if (strlen(token) < 3)
					break;
				face_tokens[nr_tokens] = token;
				nr_tokens++;
			}

			for (int i = 0; i < NR_FACE_TOKENS; i++)
			{
				vertex_token = strtok_s(face_tokens[i], slash_seperator, &nextToken2);
				texture_token = strtok_s(NULL, slash_seperator, &nextToken2);
				normal_token = strtok_s(NULL, slash_seperator, &nextToken2);

				vertex_i[i] = atoi(vertex_token);
				texture_i[i] = atoi(texture_token);
				normal_i[i] = atoi(normal_token);

			}

			PushBack_vec_int(p_vec_int_normal, normal_i);
			PushBack_vec_int(p_vec_int_texture, texture_i);
			PushBack_vec_int(p_vec_int_vertex, vertex_i);

			memset(vertex_i, 0, sizeof(int) * 3);
			memset(texture_i, 0, sizeof(int) * 3);
			memset(normal_i, 0, sizeof(int) * 3);
			
		}

		memset(arr_line, (int)'\0', LINE_SIZE);
		nextToken1 = NULL;
		nextToken2 = NULL;
	}


	fprintf_s(pLogFile, "vertices->size:%llu, texture->size:%llu, normal->size:%llu\n", p_vec_int_vertex->size,
		p_vec_int_texture->size, p_vec_int_normal->size);



}

vec_int* CreateVector_int(void)
{
	//local variable declaration
	vec_int* p_vec_int = NULL;

	p_vec_int = (vec_int*)calloc(1, sizeof(vec_int));
	if (p_vec_int == NULL)
		DestroyWindow(ghwnd_SSD);

	return p_vec_int;
}

vec_float* CreateVector_float(void)
{
	//local variable
	vec_float* p_vec_float = NULL;

	p_vec_float = (vec_float*)calloc(1, sizeof(vec_float));
	if (p_vec_float == NULL)
		DestroyWindow(ghwnd_SSD);

	return p_vec_float;

}

void PushBack_vec_int(vec_int* p_vec_int, int *entry_array)
{
	if (p_vec_int != NULL)
	{
		p_vec_int->pp_arr = (int**)realloc(p_vec_int->pp_arr, (p_vec_int->size + 1) * sizeof(int*));
		if (p_vec_int->pp_arr == NULL)
		{
			MessageBox(ghwnd_SSD, TEXT("int array vector not created"), TEXT("ERROR"), MB_OK);
			DestroyWindow(ghwnd_SSD);
		}

		p_vec_int->size++;
		p_vec_int->pp_arr[p_vec_int->size - 1] = (int*)malloc(3 * sizeof(int));
		memcpy(p_vec_int->pp_arr[p_vec_int->size - 1], entry_array, 3 * sizeof(int));

	}
}

void PushBack_vec_float(vec_float *p_vec_float, float *entry_array)
{
	if (p_vec_float != NULL)
	{
		p_vec_float->pp_arr = (float**)realloc(p_vec_float->pp_arr, (p_vec_float->size + 1) * sizeof(int*));
		if (p_vec_float->pp_arr == NULL)
		{
			MessageBox(ghwnd_SSD, TEXT("int array vector not created"), TEXT("ERROR"), MB_OK);
			DestroyWindow(ghwnd_SSD);
		}

		p_vec_float->size++;
		p_vec_float->pp_arr[p_vec_float->size - 1] = (float*)malloc(3 * sizeof(int));
		if (p_vec_float->pp_arr[p_vec_float->size - 1] == NULL)
		{
			
				MessageBox(ghwnd_SSD, TEXT("int array vector not created"), TEXT("ERROR"), MB_OK);
				DestroyWindow(ghwnd_SSD);
			
		}

		memcpy(p_vec_float->pp_arr[p_vec_float->size - 1], entry_array, 3 * sizeof(int));
	}
}

void Destroy_vec_int(vec_int* p_vec_int)
{
	if (p_vec_int != NULL)
	{
		if (p_vec_int->pp_arr != NULL)
		{
			for (int i = 0; i < p_vec_int->size; i++)
			{
				free(p_vec_int->pp_arr[i]);
			}

			free(p_vec_int->pp_arr);
		}

		free(p_vec_int);
		p_vec_int = NULL;
	}
}

void Destroy_vec_float(vec_float* p_vec_float)
{
	if (p_vec_float != NULL)
	{
		if (p_vec_float->pp_arr)
		{
			for (int i = 0; i < p_vec_float->size; i++)
			{
				free(p_vec_float->pp_arr[i]);
			}
			free(p_vec_float->pp_arr);
		}

		free(p_vec_float);
		p_vec_float = NULL;
	}
}

void Uninitialize(void)
{
	//file IO code
	void Destroy_vec_int(vec_int* p_vec_int);

	void Destroy_vec_float(vec_float* p_vec_float);

	Destroy_vec_float(p_vec_float_normal);
	Destroy_vec_float(p_vec_float_texture);
	Destroy_vec_float(p_vec_float_vertext);

	Destroy_vec_int(p_vec_int_normal);
	Destroy_vec_int(p_vec_int_texture);
	Destroy_vec_int(p_vec_int_vertex);

	if (gbFullScreen_SSD == TRUE)
	{
		dwStyle_SSD = GetWindowLong(ghwnd_SSD, GWL_STYLE);
		SetWindowLong(ghwnd_SSD, GWL_STYLE, (dwStyle_SSD | WS_OVERLAPPEDWINDOW));

		SetWindowPlacement(ghwnd_SSD, &wPrev_SSD);
		SetWindowPos(ghwnd_SSD, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOSIZE);
		ShowCursor(TRUE);
		gbFullScreen_SSD = FALSE;
	}

	if (wglGetCurrentContext() == hglrc_SSD)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (hglrc_SSD)
	{
		wglDeleteContext(hglrc_SSD);
		hglrc_SSD = NULL;
	}

	if (ghdc_SSD)
	{
		ReleaseDC(ghwnd_SSD, ghdc_SSD);
		ghdc_SSD = NULL;
	}


	if (gbFile_SSD)
	{
		fprintf_s(gbFile_SSD, "log file closed successfully\n");
		fclose(gbFile_SSD);
		gbFile_SSD = NULL;
	}

	if (pFile)
	{
		fprintf_s(pFile, "obj file closed\n");
		fclose(pFile);
		pFile = NULL;
	}

	if (pLogFile)
	{
		fprintf_s(pLogFile, "MeshLog file closed\n");
		fclose(pLogFile);
		pLogFile = NULL;
	}

}






