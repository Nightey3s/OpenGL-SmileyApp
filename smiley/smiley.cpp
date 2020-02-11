/*
Done by: Brian
*/
#ifdef _MSC_VER                         // Check if MS Visual C compiler
#  pragma comment(lib, "opengl32.lib")  // Compiler-specific directive to avoid manually configuration
#  pragma comment(lib, "glu32.lib")     // Link libraries
#endif

//#include "framework.h"
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "smiley.h"
#include <gl\gl.h> // Header File For The OpenGL32 Library
#include <gl\glu.h> // Header File For The GLu32 Library
#include <math.h>
#include "Smileyclass.cpp"
#include <windowsx.h>
#include <vector>

#define MAX_LOADSTRING 100

using namespace std;

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

int width = 400, height = 400;//set height and width of window to 400px
float xPosSquare = 0.0f;
float yPosSquare = 0.0f;


//store smileys under (std::vector<smiley *> t
vector<smiley*> vSmileys;

int save_smileys()//save the coordinates of smileys into a txt file
{
	ofstream out("smileys.txt");
	if (!out)
		return -1;
	for (int i = 0; i < vSmileys.size(); ++i)//loop through all the smileys
	{
		int r = int(vSmileys[i]->get_radius());//radius of smiley
		int x = int(vSmileys[i]->get_x()) - r + 200;//x coordinate of smiley - windows unit
		int y = 200 - r - int(vSmileys[i]->get_y());//y coordinate of smiley - windows unit
		out << x << endl << y << endl << 2*r << endl << "*******\n";
	}
	return (out.bad()? -1: 0);
}

int read_smileys()//read in the smiley coordinates if txt file exist
{
	ifstream in("smileys.txt");
	if (!in)
		return -1;
	smiley *sm = 0;
	while (!in.eof())
	{
		float x, y, r;
		if (sm)
			sm->set_happy(0);
		in >> x >> y >> r;
		r /= 2;
		if (in.eof())
			break;
		in.ignore(15, '\n');
		in.ignore(15, '\n'); // asterisk line
		sm = new smiley(r, x + r - 200.0f, 200.0f - r - y);
		if (sm)
			vSmileys.push_back(sm);
	}
	return 0;
}

int InitOpenGL();
void DrawGLScene();
void ReSizeGLScene(GLsizei width, GLsizei height);

static PIXELFORMATDESCRIPTOR pfd{
	sizeof(PIXELFORMATDESCRIPTOR),
	1,// Version Number
	PFD_DRAW_TO_WINDOW | // Format Must Support Window
	PFD_SUPPORT_OPENGL | // Format Must Support OpenGL
	PFD_DOUBLEBUFFER,// Must Support Double Buffering
	PFD_TYPE_RGBA, // Request An RGBA Format
	16, // Select Our Color Depth = 16
	0, 0, 0, 0, 0, 0, // Color Bits Ignored
	0,// No Alpha Buffer
	0,// Shift Bit Ignored
	0,// No Accumulation Buffer
	0, 0, 0, 0, // Accumulation Bits Ignored
	32, // 32 Bit Z-Buffer (Depth Buffer)
	0,// No Stencil Buffer
	0,// No Auxiliary Buffer
	PFD_MAIN_PLANE,// Main Drawing Layer
	0,// Reserved
	0, 0, 0// Layer Masks Ignored
};

GLuint PixelFormat;
HDC hDC = NULL;
HWND hWnd = NULL;
HGLRC hRC = NULL;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


void ReSizeGLScene(GLsizei width, GLsizei height) {
	if (height == 0)// Prevent A Divide By Zero By
		height = 1; // Making Height Equal One

		// Reset The Current Viewport
	glViewport(0, 0, width, height);

	// Select The Projection Matrix
	glMatrixMode(GL_PROJECTION);

	// Reset The Projection Matrix
	glLoadIdentity();

	//// Calculate The Aspect Ratio Of The Window
	//gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);//3D projection

	gluOrtho2D(-1, 1, -1, 1);//Change to 2D Projection (orthogonal projection)
}
int InitOpenGL() {
	// Get Device Dontext
	if (!(hDC = GetDC(hWnd))) {
		MessageBox(NULL, L"Can't Create A GL Device Context.",
			L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}
	// Check if Windows can find a matching Pixel Format
	if (!(PixelFormat = ChoosePixelFormat(hDC, &pfd))) {
		MessageBox(NULL, L"Can't Find A Suitable PixelFormat.",
			L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}
	// Try to set pixel format
	if (!SetPixelFormat(hDC, PixelFormat, &pfd)) {
		MessageBox(NULL, L"Can't Set The PixelFormat.", L"ERROR",
			MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}
	// Get a Rendering context
	if (!(hRC = wglCreateContext(hDC))) {
		MessageBox(NULL, L"Can't Create A GL Rendering Context.",
			L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}
	// Activate Rendering Context
	if (!wglMakeCurrent(hDC, hRC)) {
		MessageBox(NULL, L"Can't Activate The GL Rendering Context.", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}
	// TO SOLVE DEPTH-BUFFER ISSUE
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	return 1;
}
void Draw2D_AxisSystem() {
	glBegin(GL_LINES);
	// Draw positive axis as gray 
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex2f(0, 0);
	glVertex2f(width/2, 0);//x-axis

	glVertex2f(0, 0);
	glVertex2f(0, height/2);//y-axis
	glEnd();
}
void convertWindowToOpenGLCoordinates(int xWinPos, int yWinPos, float& xOpenGLPos, float& yOpenGLPos) {

	xOpenGLPos = xWinPos - 200.0f;
	yOpenGLPos = 200.0f - yWinPos;
}

float MatrixGL[16] = {1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 200.0f};

void DrawGLScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0f, 1.0f, 1.0f, 1);//Color: white
	glLoadMatrixf(MatrixGL);
	Draw2D_AxisSystem();
	for (int i = 0; i < vSmileys.size(); i++) {
		vSmileys[i]->draw();
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	read_smileys();

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_SMILEY, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SMILEY));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			DrawGLScene();
			SwapBuffers(hDC);
		}
	}

	save_smileys();

	return (int)msg.wParam;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDC_SMILEY));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SMILEY);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	RECT r;
	r.left = 20;
	r.top = 20;
	r.right = width + 20;
	r.bottom = height + 20;
	if(!AdjustWindowRectEx(&r, WS_SYSMENU|WS_CAPTION, 0, WS_EX_LTRREADING))
		exit(0);

	/*Changed : hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);*/
	hWnd = CreateWindow(szWindowClass,
		szTitle, WS_SYSMENU|WS_CAPTION,
		20, 20, r.right - r.left, r.bottom - r.top,
		nullptr, nullptr, hInstance, nullptr);
	SetMenu(hWnd, NULL);//remove menu bar
	SetWindowText(hWnd, TEXT("\r\nSmiley\r\n"));
	
	InitOpenGL();

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	ReSizeGLScene(width, height);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static smiley* smileyFace = 0;
	static int oldx, smileyindex = -1, ctrlpress = 0, delpress = 0, bckspace = 0;
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_SIZE:
	{
		ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));
		break;
	}
	case WM_LBUTTONDOWN:
	{
		if (smileyFace)
			smileyFace->set_happy(0);
		// Get the left mouse press coordinates
		int mouseX = GET_X_LPARAM(lParam);
		int mouseY = GET_Y_LPARAM(lParam);
		convertWindowToOpenGLCoordinates(mouseX, mouseY, xPosSquare, yPosSquare);
		smileyindex = -1;
		for (int i = 0; i < vSmileys.size(); ++i)
		{
			float dx = xPosSquare - vSmileys[i]->get_x();
			float dy = yPosSquare - vSmileys[i]->get_y();
			float r = vSmileys[i]->get_radius();
			if (r*r > dx*dx + dy*dy)
				smileyindex = i;
		}
		if (smileyindex < 0)
			smileyFace = 0;
		else
		{
			smileyFace = vSmileys[smileyindex];
			smileyFace->set_happy(1);
			vSmileys.erase(vSmileys.begin() + smileyindex); // erase the pointer from the vector
			smileyindex = vSmileys.size();
			vSmileys.push_back(smileyFace); // the pointer is at the end of the vector
		}
		break;
	}
	case WM_RBUTTONDOWN:
	{
		int mouseX = GET_X_LPARAM(lParam);
		int mouseY = GET_Y_LPARAM(lParam);
		convertWindowToOpenGLCoordinates(mouseX, mouseY, xPosSquare, yPosSquare);
		if (smileyFace)
			smileyFace->set_happy(0);
		smileyFace = new smiley(25.0f, xPosSquare, yPosSquare);
		smileyindex = vSmileys.size();
		vSmileys.push_back(smileyFace);
		break;
	}
	case WM_MOUSEMOVE:
	{
		int mouseX = GET_X_LPARAM(lParam);
		if ((GetKeyState(VK_CONTROL)&~1) && smileyFace && ctrlpress)
		{
			if (ctrlpress&1)
				smileyFace->resize(smileyFace->get_radius() + mouseX - oldx);
			else
				ctrlpress = 1;
		}
		oldx = mouseX;
		break;
	}
	case WM_KEYDOWN:
	{
		if (GetKeyState(VK_CONTROL) & ~1)
			ctrlpress = 2;
		else if ((GetKeyState(VK_DELETE) & ~1))
			delpress = 1;
		else if ((GetKeyState(VK_BACK) & ~1))
			bckspace = 1;
		break;
	}
	case WM_KEYUP:
	{
		if (delpress && smileyFace)//delete one smiley at a time and smiley must be selected
		{
			vSmileys.erase(vSmileys.begin() + smileyindex);
			delete smileyFace;
			smileyFace = 0;
			smileyindex = -1;
		}
		if (bckspace) {//backspace to clear all smileys
			vSmileys.clear();
			delete smileyFace;
			smileyFace = 0;
			smileyindex = -1;
		}
		delpress = ctrlpress = bckspace = 0;
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

