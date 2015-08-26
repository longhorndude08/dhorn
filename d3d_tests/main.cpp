/*
 * Duncan Horn
 *
 * main.cpp
 *
 * Functional tests for the dhorn::d3d namespace. Namely the d3d_window class
 */

#include <dhorn/d3d.h>
#include <dhorn/d3d/colors.h>
#include <dhorn/d3d/shapes.h>

#include "geometry.h"
#include "globals.h"

// Define the globals
dhorn::d3d::d3d_window globals::window;
dhorn::d3d::camera globals::camera;
dhorn::win32::com_ptr<ID3D11VertexShader> globals::vertex_shader;
dhorn::win32::com_ptr<ID3D11PixelShader> globals::pixel_shader;
dhorn::win32::com_ptr<ID3D11InputLayout> globals::input_layout;
dhorn::win32::com_ptr<ID3D11Buffer> globals::cuboid_vertices;
dhorn::win32::com_ptr<ID3D11Buffer> globals::cuboid_indices;
dhorn::win32::com_ptr<ID3D11Buffer> globals::object_data;

bool globals::forward = false;
bool globals::backward = false;
bool globals::up = false;
bool globals::down = false;
bool globals::left = false;
bool globals::right = false;

#if defined(DEBUG) || defined(_DEBUG)
#define OUTPUT_PATH "Debug\\"
#else
#define OUTPUT_PATH "Release\\"
#endif


struct object_data
{
    DirectX::XMFLOAT4X4 viewProjectionMatrix;
};


static void load_shaders(void)
{
    std::vector<uint8_t> vertexShaderBytecode;
    globals::vertex_shader = dhorn::d3d::load_vertex_shader(
        globals::window.device(),
        OUTPUT_PATH "VertexShader.cso",
        vertexShaderBytecode);

    D3D11_INPUT_ELEMENT_DESC inputDesc[] =
    {
        dhorn::d3d::input_element_desc(&vertex::position, DXGI_FORMAT_R32G32B32_FLOAT, "POSITION"),
        dhorn::d3d::input_element_desc(&vertex::normal, DXGI_FORMAT_R32G32B32_FLOAT, "NORMAL"),
        dhorn::d3d::input_element_desc(&vertex::color, DXGI_FORMAT_R32G32B32A32_FLOAT, "COLOR")
    };
    dhorn::win32::throw_if_failed(globals::window.device()->CreateInputLayout(
        inputDesc, dhorn::array_size(inputDesc),
        vertexShaderBytecode.data(), vertexShaderBytecode.size(),
        &globals::input_layout));

    globals::pixel_shader = dhorn::d3d::load_pixel_shader(globals::window.device(), OUTPUT_PATH "PixelShader.cso");
}


static void load_geometry(void)
{
    std::vector<dhorn::d3d::vertex> cuboidVertices;
    std::vector<UINT> indices;
    dhorn::d3d::cuboid(1.0f, 1.0f, 1.0f, cuboidVertices, indices);

    // We need to convert each vertex to our own vertex format
    std::vector<vertex> vertices;
    for (auto &v : cuboidVertices)
    {
        vertex next;
        next.position = v.position;
        next.normal = v.normal;

        // Use the position as the color
        DirectX::XMVECTOR color = DirectX::XMLoadFloat3(&v.position);
        color = DirectX::XMVectorAdd(color, DirectX::XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f));
        DirectX::XMStoreFloat4(&next.color, color);

        vertices.push_back(next);
    }

    globals::cuboid_vertices = dhorn::d3d::create_buffer(globals::window.device(), vertices, D3D11_BIND_VERTEX_BUFFER);
    globals::cuboid_indices = dhorn::d3d::create_buffer(globals::window.device(), indices, D3D11_BIND_INDEX_BUFFER);

    // Create the buffer that we will use for the cbuffer for 'ObjectData'
    globals::object_data = dhorn::d3d::create_constant_buffer<object_data>(globals::window.device());
}


static std::pair<bool, intptr_t> key_press_handler(
    _In_ dhorn::win32::window * /*sender*/,
    _In_ uintptr_t wparam,
    _In_ intptr_t lparam)
{
    bool handled = false;
    bool key_down = (lparam & 0x80000000) == 0;

    switch (static_cast<dhorn::win32::virtual_key>(wparam))
    {
    case dhorn::win32::virtual_key::up:
    case dhorn::win32::virtual_key::w:
        globals::forward = key_down;
        handled = true;
        break;

    case dhorn::win32::virtual_key::down:
    case dhorn::win32::virtual_key::s:
        globals::backward = key_down;
        handled = true;
        break;

    case dhorn::win32::virtual_key::left:
    case dhorn::win32::virtual_key::a:
        globals::left = key_down;
        handled = true;
        break;

    case dhorn::win32::virtual_key::right:
    case dhorn::win32::virtual_key::d:
        globals::right = key_down;
        handled = true;
        break;

    case dhorn::win32::virtual_key::space:
        globals::up = key_down;
        handled = true;
        break;

    case dhorn::win32::virtual_key::shift:
        globals::down = key_down;
        handled = true;
        break;
    }

    return std::make_pair(handled, 0);
}


static void move_cursor(void)
{
    // Move the cursor to position (100, 100)
    auto rect = globals::window.client_rect();
    POINT pt = { static_cast<LONG>(rect.x), static_cast<LONG>(rect.y) };
    ::ClientToScreen(globals::window.handle(), &pt);
    ::SetCursorPos(pt.x + 300, pt.y + 200);
}


static std::pair<bool, intptr_t> mouse_move_handler(
    _In_ dhorn::win32::window * /*sender*/,
    _In_ uintptr_t /*wparam*/,
    _In_ intptr_t lparam)
{
    bool handled = false;
    size_t x = static_cast<uint16_t>(LOWORD(lparam));
    size_t y = static_cast<uint16_t>(HIWORD(lparam));

    float delta_x = static_cast<float>(x) - 300;
    if (delta_x != 0)
    {
        globals::camera.rotate_y(delta_x * DirectX::XM_PI / 520);
    }

    float delta_y = static_cast<float>(y) - 200;
    if (delta_y != 0)
    {
        auto v = globals::camera.right();
        globals::camera.rotate(v, delta_y * DirectX::XM_PI / 520);
    }

    // Reset the cursor position
    if (x != 300 || y != 200)
    {
        move_cursor();
    }

    return std::make_pair(handled, 0);
}


int WINAPI wWinMain(
    _In_ dhorn::win32::instance_handle instance,
    _In_ dhorn::win32::instance_handle /*prevInstance*/,
    _In_ wchar_t * /*cmdLine*/,
    _In_ int cmdShow)
{
    // Initialize the window
    dhorn::win32::window_class windowClass(L"D3D Test Window");
    windowClass.use_defaults();
    windowClass.instance = instance;

    dhorn::win32::window_options options(L"D3D Test Window");
    options.width = 600;
    options.height = 400;

    globals::window.set_background(dhorn::d3d::colors::burnt_orange);

    globals::window.on_initialized([&](void)
    {
        load_shaders();
        load_geometry();
        move_cursor();

        // Initialize the camera
        auto rect = globals::window.client_rect();
        float ratio = static_cast<float>(rect.width) / static_cast<float>(rect.height);
        globals::camera.configure_frustum(0.1f, 100.0f, DirectX::XM_PIDIV2, ratio);
        globals::camera.set_position(DirectX::XMVectorSet(0, 0, -2, 0));
        globals::camera.look_at(DirectX::g_XMZero, DirectX::XMVectorSet(0, 1, 0, 0));
    });

    globals::window.on_update([&]()
    {
        static const float delta = 0.0001f;

        if (globals::up ^ globals::down)
        {
            globals::camera.translate_up(delta * (globals::up ? 1.0f : -1.0f));
        }

        if (globals::right ^ globals::left)
        {
            globals::camera.translate_right(delta * (globals::right ? 1.0f : -1.0f));
        }

        if (globals::forward ^ globals::backward)
        {
            globals::camera.translate_forward(delta * (globals::forward ? 1.0f : -1.0f));
        }
    });

    globals::window.on_draw([&](ID3D11Device * /*device*/, ID3D11DeviceContext *context)
    {
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->IASetInputLayout(globals::input_layout);
        context->VSSetShader(globals::vertex_shader, nullptr, 0);
        context->PSSetShader(globals::pixel_shader, nullptr, 0);

        UINT stride = sizeof(vertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, globals::cuboid_vertices.get_address_of(), &stride, &offset);
        context->IASetIndexBuffer(globals::cuboid_indices, DXGI_FORMAT_R32_UINT, 0);

        // Update the constant buffer
        object_data objData;
        DirectX::XMStoreFloat4x4(
            &objData.viewProjectionMatrix,
            DirectX::XMMatrixTranspose(globals::camera.view_projection_matrix()));
        context->UpdateSubresource(globals::object_data, 0, nullptr, &objData, 0, 0);
        context->VSSetConstantBuffers(0, 1, globals::object_data.get_address_of());

        // Draw the geometry!
        context->DrawIndexed(36, 0, 0);
    });

    globals::window.on_resize([&](const dhorn::rect<size_t> &clientArea)
    {
        float ratio = static_cast<float>(clientArea.width) / static_cast<float>(clientArea.height);
        globals::camera.configure_frustum(0.1f, 100.0f, DirectX::XM_PIDIV2, ratio);
    });

    // Set window message handlers
    globals::window.add_callback_handler(dhorn::win32::window_message::key_down, &key_press_handler);
    globals::window.add_callback_handler(dhorn::win32::window_message::key_up, &key_press_handler);
    globals::window.add_callback_handler(dhorn::win32::window_message::mouse_move, &mouse_move_handler);

    // Run the application!
    globals::window.run(windowClass, options, cmdShow);
}