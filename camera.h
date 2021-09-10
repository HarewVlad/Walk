struct Camera {
	XMFLOAT3 position;

	float yaw;
	float pitch;

	XMFLOAT3 look_direction;
	XMFLOAT3 up_direction;

	float move_speed;
	float turn_speed;

	XMMATRIX view;
};