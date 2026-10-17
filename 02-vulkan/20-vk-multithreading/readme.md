# 20-vk-multithreading — многопоточная запись командных буферов

По мотивам главы `Multithreading with Vulkan` из туториала
`docs.vulkan.org`. База — урок `19-vk-objects`, старые уроки не менялись.

## Часть 1: worker-потоки записывают compute-буферы

Многопоточность здесь на **CPU**: несколько потоков **параллельно
записывают (record) разные командные буферы**, а реальный рендеринг и
симуляция частиц по-прежнему выполняются на GPU.

- Каждый worker-поток владеет **своим `CommandPool`** и своими буферами
  (`[thread][frame_in_flight]`) — пулы нельзя трогать из разных потоков,
  поэтому запись идёт вообще без блокировок
  (`render::compute_thread_pools`, `compute_thread_cmd_buffers`).
- Частицы делятся на группы — по одной на поток
  (`render::particle_groups`). Диапазон группы передаётся в compute-шейдер
  через **push constants** `{start_index, count}` (см. `compute.slang`),
  dispatch запускается только на свою группу.
- Главный поток в `render::draw(parts, ...)`: сигналит воркерам
  (`thread_work_ready`) → пока они пишут, сам записывает graphics-буфер →
  ждёт всех (`thread_work_done` + condition variable) → делает **один
  submit со всеми compute-буферами разом** в `compute_queue`.
- Синхронизация GPU-стороны не изменилась: timeline semaphore между
  compute и graphics, барьер storage → vertex input теперь стоит в каждом
  буфере на свой диапазон частиц.
- Потоки создаются один раз (при первом `draw` с частицами) и живут до
  деструктора `render` — создавать потоки на каждый кадр дорого.

Два отличия от кода главы (в оригинале — скрытые гонки):

1. Воркер сбрасывает `work_ready` **до** установки `work_done`, а не
   после: иначе главный поток мог бы успеть засигналить следующий кадр
   (`work_ready=true`) между этими двумя store — и воркер затёр бы сигнал,
   главный уснул бы в ожидании навсегда.
2. `work_done` выставляется под тем же мьютексом, под которым главный
   проверяет предикат condition variable: без этого возможен lost wakeup
   (notify проскакивает между проверкой предиката и засыпанием).

## Базовый урок (19): несколько объектов в одном кадре

По мотивам главы `Rendering Multiple Objects` из туториала
`docs.vulkan.org`. База — урок `18-vk-gltf-ktx2`, старые уроки не менялись.

Идея главы: разделить ресурсы на **общие** (один раз на всех) и
**per-object** (свои у каждого объекта), а дальше рисовать объекты по
очереди в одном render pass, переключая только descriptor set.

```text
shared (один раз):            per-object (у каждого свой):
- mesh (vertex/index buffer)  - model-матрица
- image + sampler             - uniform buffer с ней
- pipeline + pipeline layout  - descriptor set на этот buffer
- render pass, command pool   - (у нас: + свой descriptor pool)
```

## Три дескрипторные сущности

Новичка путает, что их три. Каждая отвечает за своё, и это следствие
главного принципа Vulkan: **драйвер ничего не аллоцирует и не угадывает
в момент отрисовки** — всё описано и выделено заранее.

```text
шейдер (*.slang)                C++ сторона
────────────────                ──────────
binding 0: ConstantBuffer<ubo>  ──контракт──> DescriptorSetLayout
binding 1: Sampler2D texture    ──контракт──> (binding 0 = UBO/shader-vertex,
                                               binding 1 = sampler/fragment)
                                                      │
DescriptorPool ──выделяет из себя──> DescriptorSet ───┘
(сколько сетов и                 (конкретная привязка:
 каких типов можно                «сет №i = UBO №i + texture №i»)
 выдать, задаётся
 при создании)
```

- **DescriptorSetLayout** — «чертёж»: какие binding'и ждёт шейдер
  (0 — uniform buffer для вершинного, 1 — sampler для фрагментного).
  Один на всех, создаётся в `render::create_descriptor_set_layout()`,
  зашивается в `pipeline_layout`. Если поменять порядок binding'ов —
  будет validation-ошибка (см. комментарий в `shader.vert.frag.slang`).
- **DescriptorPool** — явный аллокатор сетов. Vulkan заставляет сказать
  заранее: «мне нужно столько-то сетов, столько-то UBO-дескрипторов,
  столько-то sampler'ов». Зато потом `allocate` — это просто выдача
  готовых слотов, без сюрпризов для драйвера.
- **DescriptorSet** — заполненный бланк: «в binding 0 лежит вот этот
  `vk::Buffer`, в binding 1 — вот эти `ImageView`+`Sampler`».
  Заполняется один раз через `updateDescriptorSets`, дальше только
  биндится в командный буфер.

Почему нельзя было обойтись одним объектом на всех? Потому что UBO —
это конкретные байты в конкретном `vk::Buffer`. Два объекта с разными
матрицами в один буфер не влезут (точнее, влезут, но это уже dynamic
UBO / push constants — другие главы). Поэтому: **один объект — один
набор UBO + один набор сетов** (по одному на каждый frame in flight).

## Почему UBO три штуки на объект

`max_frames_in_flight = 3`: пока GPU рисует кадр N, CPU уже пишет UBO
для кадра N+1. Если бы буфер был один, CPU перезаписал бы матрицу
прямо под руками у GPU. Поэтому кольцевой буфер:

```text
кадр:        N        N+1        N+2        N+3 (=N)
              │          │          │          │
UBO:        buf[0]     buf[1]     buf[2]     buf[0]  <- переиспользование
сет:        set[0]     set[1]     set[2]     set[0]  <- сеты ссылаются
                                                     на свои буферы,
                                                     обновлять их не надо
```

Сеты создаются один раз в конструкторе `object` и ссылаются каждый на
свой UBO — в `draw()` делается только `memcpy` матрицы в уже
смапленный буфер текущего кадра. Никаких `updateDescriptorSets` в
цикле отрисовки.

## Как это выглядит в коде (без `vk::*` снаружи)

Весь Vulkan спрятан в `om::vulkan::object`
(`vulkan/render.cxx`). Из `main()`:

```cpp
om::vulkan::mesh room_mesh = om::gltf::load_model("viking_room.gltf", render);
om::vulkan::object room_left(render, room_mesh, image_mip_on,
                             sizeof(uniform_buffer_object), "room_left");
om::vulkan::object room_right(render, room_mesh, image_mip_off,
                              sizeof(uniform_buffer_object), "room_right");

// в цикле: view/proj общие, model у каждого своя
render.begin_frame();
render.draw(room_left, ubo_left_span);    // memcpy UBO + bind set + drawIndexed
render.draw(room_right, ubo_right_span); // то же для второго объекта
render.draw(parts, compute_ubo_span);    // частицы своим пайплайном
render.end_frame();
```

Внутри `render::draw(obj, ubo)`:

```text
draw(obj):
  obj.update_uniform_buffer(current_frame, ubo)   // memcpy в buf[current_frame]
  record_mesh_commands(cmd, obj.set[current_frame], obj.mesh)
    ├─ bindPipeline(graphics_pipeline)           // общий для всех
    ├─ bindVertexBuffers / bindIndexBuffer       // mesh объекта
    ├─ bindDescriptorSets(set[current_frame])    // <<< переключение объекта
    └─ drawIndexed()                             // поехали
```

## Отличие от туториала

В оригинале — `struct GameObject` с векторами UBO и сетов + **один
общий** `descriptorPool` размером `MAX_OBJECTS * MAX_FRAMES_IN_FLIGHT`,
а в `main` торчат `vk::raii::Buffer`, `vk::DescriptorSet` и индекс
кадра.

Здесь то же самое по смыслу, но владение перевёрнуто: у каждого
`object` **свой** маленький пул (`max_frames_in_flight` сетов) и свои
UBO. Итог:

- из `main()` не видно ни пулов, ни сетов, ни `current_frame`;
- объект создаётся одной строкой и сразу готов к `draw()`;
- `mesh` и `image` разделяются между объектами (указатели, не владеет),
  время жизни: `mesh`/`image` должны жить дольше `object`;
- лимит «сколько объектов» пропал как константа — сколько создал,
  столько и рисуешь.
