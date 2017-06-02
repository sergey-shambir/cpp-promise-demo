## Как собрать

Склонируйте репозиторий, проинициализируйте подмодуль, соберите через CMake. На Linux:

```bash
git clone https://github.com/sergey-shambir/cpp-promise-demo.git
git submodule update --init
cmake -DCMAKE_BUILD_TYPE=Release .
cmake --build .
```

На Windows с Visual Studio 2017:

```bash
git clone https://github.com/sergey-shambir/cpp-promise-demo.git
git submodule update --init
cmake -G "Visual Studio 2017" .
cmake --build .
```

## Структура проектов

- `libpromise` &mdash; реализует Promise (подобный Promise/A+) и базовую платформу (event loop, thread pool)
- `promise-app` &mdash; демонстрирует, как можно применять самописные Promise
- `boost-future-pain` &mdash; демонстрирует, как не надо использовать boost.future
    - изменяйте значение макроса OPTION_PAIN_LEVEL для изменения уровня подводных камней Boost
- `libhttp` &mdash; поверх CURL реализует AsyncHttpClient, выполняющий обработку множества сетевых запросов в одном выделенном потоке
- `httpbin-app` &mdash; демонстрирует лёгкость обработки сетевых запросов через Promise

## Почему Promise/A+?

- [Об этом расскажет отдельный документ rationale.md](docs/rationale.md)

## Статус порта Promise/A+

Структура классов:

- класс `PromiseObject<Value>` предоставляет реализацию так называемого shared state, хранящего текущее состояние и его данные, а также прикреплённые колбеки
- класс `Promise<Value>` предоставляет похожий на shared_ptr фасад над PromiseObject, предназначенный для использования снаружи

Порт неполный, и содержит сознательно допущенные отклонения от стандарта:

- Then/Catch могут быть вызваны лишь по одному разу для каждого Promise, и не выполняют продолжение (continuation) задачи
- Для выполнения продолжения в Promise реализован отдельный метод ThenDo, который имеет несколько перегрузок, от простых до максимально гибких
- Отсутствуют аналоги функций [all](https://developer.mozilla.org/ru/docs/Web/JavaScript/Reference/Global_Objects/Promise/all) и [race](https://developer.mozilla.org/ru/docs/Web/JavaScript/Reference/Global_Objects/Promise/race), хотя принципально ничто не мешает их написать, за исключением необходимости выбора между двумя вариантами реализации all:
    - с фиксированным числом разнородных значений внутри `tuple<T1, T2, T3>`
    - с произвольным числом однородных значений внутри `vector<T>`

## Статус AsyncHttpClient

Класс предоставляет API для запросов к сети:

- API асинхронный и использует `Promise<HttpResponse>` вместо блокирования/возврата `HttpResponse`
- Один Network поток одновременно выполняет много запросов, используя интерфейс multi библиотеки libcurl
