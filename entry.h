#define FOV 1024

struct Entry {
	Window *window;
	Directx *directx;
	Camera *camera;
	Cube *cube;

	XMMATRIX projection;
};