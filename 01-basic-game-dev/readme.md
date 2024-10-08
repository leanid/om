## Д.З. 01

1. собрать с++ hello world через cmake + ninja (если нужно, пройти обучалку на официальном сайте [cmake tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/index.html)).
2. собрать отдельно библиотеки с функцией приветствия: статическую (\*.a) и динамическую (\*.so).
3. сделать два исполняемых файла, один слинковать со статической версией библиотеки, другой - с динамической.
4. добавить тест (CTest) в CMakeLists.txt и убедиться, что он работает.
5. создать репозиторий на любой платформе, типа bitbucket, github. Добавить туда ваш репозиторий со сделанным домашним заданием.
6. (опционально) добавить автоматичекусю сборку вашего проекта и в readme.md файл добавить визуально картинки, что бы было видно что сборка прошла успешно. Каждый ваш commit в репозиторий, стартует автоматическую сборку и проверку.
7. (опционально) почитать про Docker сделать свой образ заиспользовать его в сборке. Так же можно использовать что-то более новое и современное.

## Д.З. 01 - частые проблемы вопросы:

1. не знают про кэширование, про ```std::cout << "test" << std::endl << '\n'```.
2. не отличают ошибки своей программы и вызова в терминале(shell).
3. не понимают как работает перенаправление ввода-вывода.
4. не знают как проверить негативный случай, когда не отработал вывод в поток - и это хорошо (как в этом убедиться) - подсказка - /dev/video0.

## Д.З. 02:
1. собрать локально из исходников последнюю версию SDL3 и статически и динамически.
2. сделать программу которая линкуется со статической версией - проверить версии вывести.
3. сделать программу которая линкуется динамически с SDL3 - тоже проверить и вывести результат.
4. разобраться с вызовами функции из динамической библиотеке, посмотреть под отладчиком, заметить в чем прикол, первого вызова.

## Д.З. 02 - частые проблемы:
1. cmake не находит пакет с SDL3Config.cmake - возможно дело в регистре find(sdl3 REQUIRED) и find(SDL3 REQUIRED) - могут отрабатывать по разному
2. для удобства можно поставить пакет из репозитория под вашу систему: ```sudo dnf install SDL3-devel SDL3-static```
3. потом можно их удалить без зависимостей (что бы зависимости остались и с ними не возиться): ```rpm -e --nodeps <pkg>```

## Д.З. 03:
1. сделать программу которая в бесконечном цикле выводит события нажатия виртуальной консоли (up, down, left, ... first_button, second_button) - в одной программе через SDL_Event
2. переделать предыдущую программу в библиотеку (статическую) (engine) и игру (game) которая использует engine а сама непосредвстенно не работает напрямую с SDL
3. переделать предыдущую программу engine -> dll game -> exe - собрать убедиться что все работает и так тоже.
4. переделать предыдущую программу engine -> exe, game -> dll - игра загружается из dll-ки(so-шки) на лету после старта программы, а также снова перезагружается, если и изменился на диске файл - game.so. (будет проверяться только эта финальная программа)

## Д.З. 03 - частые проблемы:
1. нет разделения на интерфейсы и SDL торчит отовсюду
2. забыли указать - экспортировать символы для исполняемого файла в cmake

## Д.З. 04 - софтовый рендер, свои шейдеры.
1. - научиться сохронять-загружать *.ppm формат
2. - научиться рисовать линую с помощью алгоритма Брезенхейма (желательно получать список всех точек линии, пригодится дальше)
3. - научиться рисовать треугольник
4. - научиться рисовать много треугольников, использую БуферВершин и БуферИндексовВершин
5. - научиться рисовать сплошной треугольник (залитый-растеризованный, интерполированными значениями из вершин)
6. - создать свою абстракцию - GfxProgram в которой можно установить - вершинный шейдер, фрагментный шейдер, юниформы - сделать с помощью этого что-то такое (https://bitbucket.org/b_y/om/src/master/class-01-basic-game-dev/04-0-render-basic/gfx_shaders.mp4)
Это домашнее задание большое и сложное, поэтому его врядли успеем сделать за 1-неделю, поэтому будем доделывать в классе, и (или) на следующей неделе. (но если большинство - сделает - то смело идем вперед, больше узнаете нового).

## Д.З. 05 - контекст OpenGL, первый шейдер, RenderDoc знакомство.
1. - с помощью glad https://glad.dav1d.de/ сгенерируйте загрузчик OpenGL нужной вам версии (на Fedora OpenGL ES 3.2 на встроенном Intel 2017-го работает из коробки)
2. - создайте debug OpenGL контекст, загрузите все указатели на функции, почистите экран любым цветом, убедитесь, что работает.
3. - создайте свой первый вершинный и пиксельный шейдер на языке GLSL, можете сами, можете по книге, можете по моему примеру.
4. - скачайте RenderDoc https://renderdoc.org/ и переделайте вашу программу по моему примеру, где используется VertexBuffer+VAO под OpenGL 3.3, захватите кадр и постарайтесь освоится в этой программе.
5. - результат вашего домашнего задания - у вашего движка появляются функции, через которые вы можете рисовать треугольники, применяя вершинные и пиксельные шейдеры.

## Д.З. 06 - двигаем текстурированный танк клавишами по экрану, вращаем
1. - разобрать пример морфинга вершин
2. - разобрать пример загрузки текстуры
3. - разобрать пример математической библиотеки вектор 2д матрица 2д
4. - научиться делать матрицу переноса, масштабирования и вращения в 2д
5. - разобрать пример с масштабированием на экране (соотношение стророн и NDC координаты)
6. - поняв все предыдущее сделать перемещение 2д танка, вращение и все что захочется дополнительно по желанию.

## Выпускной проект
1. - в вашей мини игре (или демке) должен быть геймплей (можно любой клон, идея не важна)
2. - должны быть звуки, музыка и сделаны они должны быть через ваш собственный микшер на базе SDL3
3. - меню в игре (или редактор) сделан через вашу обертку для ImGui, которая работает через API вашего движка.
4. - игра должна собираться и запускаться под Linux (Fedora 38), Windows 10, Android (API level 24)
5. - подготовить мини презу, где минимум есть 3 ссылки на билды под каждую платформу + ссылка на репозиторий с кодом. Видео гейплея на одной десктопной платформе и на мобильной(Андроид) - регламент 10 минут.
6. - все что было на курсе - необходимо использовать, если чего-то не было, а вам это надо и хочется - то можно (например физический движок)
7. - в корне должен быть readme.md в котором описана сборка и установка зависимостей под каждую платформу. сборка должна быть легкой простой и повторяемой, сборка всегда из одного и того же репозитория одной ветки (но можно из разных папок)
8. - Выпускной проект должен быть допущен к сдаче, в нем минимум должнен быть геймплей и рабочий билд на Linux, в котором есть использование графики, звука, ввода, ImGui, все через ваш микро-движок. (Допуск на предыдущем занятии перед выступлением, или раньше)
