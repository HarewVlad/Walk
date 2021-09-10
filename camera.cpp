static Camera* CreateCamera(const XMFLOAT3& position) {
  Camera* result = new Camera;

  result->yaw = 0;
  result->pitch = 0;
  result->look_direction = {0, 0, -1};
  result->up_direction = {0, 1, 0};
  result->move_speed = 20.0f;
  result->turn_speed = XM_PIDIV2;
  result->position = position;
  return result;
}

static void CameraUpdate(Camera* camera, float dt) {
  auto move_speed = camera->move_speed;
  auto turn_speed = camera->turn_speed;
  auto& yaw = camera->yaw;
  auto& pitch = camera->pitch;
  auto& position = camera->position;
  auto& look_direction = camera->look_direction;
  auto& view = camera->view;
  auto& up_direction = camera->up_direction;

  XMFLOAT3 move = {};

  if (GetAsyncKeyState('A') & 0x8000) {
    move.x -= 1.0f;
  }
  if (GetAsyncKeyState('D') & 0x8000) {
    move.x += 1.0f;
  }
  if (GetAsyncKeyState('S') & 0x8000) {
    move.z -= 1.0f;
  }
  if (GetAsyncKeyState('W') & 0x8000) {
    move.z += 1.0f;
  }

  if (fabs(move.x) > 0.1f && fabs(move.z) > 0.1f) {
    XMVECTOR vector = XMVector3Normalize(XMLoadFloat3(&move));
    move.x = XMVectorGetX(vector);
    move.z = XMVectorGetZ(vector);
  }

  float move_interval = move_speed * dt;
  float rotate_interval = turn_speed * dt;

  if (GetAsyncKeyState(VK_UP) & 0x8000) {
    pitch += rotate_interval;
  }
  if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
    pitch -= rotate_interval;
  }
  if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
    yaw += rotate_interval;
  }
  if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
    yaw -= rotate_interval;
  }

  // Prevent looking to far up and down
  pitch = std::min(pitch, XM_PIDIV4);
  pitch = std::max(-XM_PIDIV4, pitch);

  // Move camera
  float x = move.x * -cosf(yaw) - move.z * sinf(yaw);
  float z = move.x * sinf(yaw) - move.z * cos(yaw);
  position.x += x * move_interval;
  position.z += z * move_interval;

  // Calculate look direction
  float r = cosf(pitch);
  look_direction.x = r * sinf(yaw);
  look_direction.y = sinf(pitch);
  look_direction.z = r * cosf(yaw);

  if (look_direction.x != 0 || look_direction.y != 0 || look_direction.z != 0) {
    view = XMMatrixLookToRH(XMLoadFloat3(&position),
                          XMLoadFloat3(&look_direction),
                          XMLoadFloat3(&up_direction));
  }
}