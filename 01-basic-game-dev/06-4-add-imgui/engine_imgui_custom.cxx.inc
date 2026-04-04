// Dear ImGui: custom rendering through om::engine (textured indexed triangles), SDL3 input via imgui_impl_sdl3 only.

static float                        g_imgui_time            = 0.0F;
static bool                         g_imgui_mouse_down[3] = { false, false, false };
static float                        g_imgui_mouse_wheel   = 0.0F;
static om::shader_gl_es20*         g_imgui_shader         = nullptr;
static om::texture*                 g_imgui_font_texture   = nullptr;
static void imgui_destroy_font_texture()
{
    if (g_imgui_font_texture != nullptr && om::g_imgui_engine != nullptr)
    {
        om::g_imgui_engine->destroy_texture(g_imgui_font_texture);
        g_imgui_font_texture = nullptr;
    }
}

static void imgui_invalidate_device_objects()
{
    imgui_destroy_font_texture();
    delete g_imgui_shader;
    g_imgui_shader = nullptr;
}

static bool imgui_create_font_texture()
{
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels = nullptr;
    int            w      = 0;
    int            h      = 0;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &w, &h);
    g_imgui_font_texture = om::g_imgui_engine->create_texture_rgba32(
        pixels, static_cast<size_t>(w), static_cast<size_t>(h));
    io.Fonts->SetTexID(static_cast<ImTextureID>(
        reinterpret_cast<uintptr_t>(g_imgui_font_texture)));
    return true;
}

static bool imgui_create_device_objects()
{
    const GLchar* vertex_shader = "#if defined(GL_ES)\n"
                                  "precision highp float;\n"
                                  "#endif\n"
                                  "uniform mat3 ProjMtx;\n"
                                  "attribute vec2 Position;\n"
                                  "attribute vec2 UV;\n"
                                  "attribute vec4 Color;\n"
                                  "varying vec2 Frag_UV;\n"
                                  "varying vec4 Frag_Color;\n"
                                  "void main()\n"
                                  "{\n"
                                  "  Frag_UV = UV;\n"
                                  "  Frag_Color = Color;\n"
                                  "  gl_Position = vec4(ProjMtx * vec3(Position.xy,1), 1);\n"
                                  "}\n";

    const GLchar* fragment_shader = "#if defined(GL_ES)\n"
                                    "precision highp float;\n"
                                    "#endif\n"
                                    "uniform sampler2D Texture;\n"
                                    "varying vec2 Frag_UV;\n"
                                    "varying vec4 Frag_Color;\n"
                                    "void main()\n"
                                    "{\n"
                                    "  gl_FragColor = Frag_Color * texture2D(Texture, Frag_UV);\n"
                                    "}\n";

    g_imgui_shader = new om::shader_gl_es20(
        vertex_shader,
        fragment_shader,
        { { 0, "Position" }, { 1, "UV" }, { 2, "Color" } });

    return imgui_create_font_texture();
}

static void imgui_ensure_device_objects()
{
    if (g_imgui_shader != nullptr)
    {
        return;
    }
    if (!imgui_create_device_objects())
    {
        std::cerr << "imgui_create_device_objects failed\n";
    }
}

static void imgui_to_engine_render(ImDrawData* draw_data)
{
    if (draw_data == nullptr || !draw_data->Valid || draw_data->CmdListsCount == 0)
    {
        return;
    }

    ImGuiIO& io       = ImGui::GetIO();
    ImVec2   fb_scale = draw_data->FramebufferScale;
    int      fb_width =
        static_cast<int>(draw_data->DisplaySize.x * fb_scale.x);
    int fb_height = static_cast<int>(draw_data->DisplaySize.y * fb_scale.y);
    if (fb_width <= 0 || fb_height <= 0)
    {
        return;
    }
    draw_data->ScaleClipRects(fb_scale);

    om::mat2x3 orto_matrix =
        om::mat2x3::scale(2.0F / io.DisplaySize.x, -2.0F / io.DisplaySize.y) *
        om::mat2x3::move(om::vec2(-1.0F, 1.0F));

    g_imgui_shader->use();
    g_imgui_shader->set_uniform("ProjMtx", orto_matrix);

    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list          = draw_data->CmdLists[n];
        const ImDrawIdx*  idx_buffer_offset = nullptr;

        const auto* vertex_data =
            reinterpret_cast<const om::v2*>(cmd_list->VtxBuffer.Data);
        auto vert_count = static_cast<size_t>(cmd_list->VtxBuffer.Size);

        om::vertex_buffer* vertex_buff =
            om::g_imgui_engine->create_vertex_buffer(vertex_data, vert_count);

        const std::uint16_t* indexes = cmd_list->IdxBuffer.Data;
        auto index_count = static_cast<size_t>(cmd_list->IdxBuffer.Size);

        om::index_buffer* index_buff =
            om::g_imgui_engine->create_index_buffer(indexes, index_count);

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != nullptr)
            {
                pcmd->UserCallback(cmd_list, pcmd);
                continue;
            }

            const ImTextureID tid = pcmd->GetTexID();
            auto*               tex = reinterpret_cast<om::texture*>(
                static_cast<uintptr_t>(tid));

            om::g_imgui_engine->render(vertex_buff,
                                       index_buff,
                                       tex,
                                       idx_buffer_offset,
                                       static_cast<size_t>(pcmd->ElemCount));

            idx_buffer_offset += pcmd->ElemCount;
        }
        om::g_imgui_engine->destroy_vertex_buffer(vertex_buff);
        om::g_imgui_engine->destroy_index_buffer(index_buff);
    }
}
