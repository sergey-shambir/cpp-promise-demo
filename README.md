## Структура проектов

- `libpromise` &mdash; реализует Promise (подобный Promise/A+) и базовую платформу (event loop, thread pool)
- `promise-app` &mdash; демонстрирует, как можно применять самописные Promise
- `boost-future-pain` &mdash; демонстрирует, как не надо использовать boost.future
    - изменяйте значение макроса OPTION_PAIN_LEVEL для изменения уровня подводных камней Boost

## Почему Promise/A+?

- [Об этом расскажет отдельный документ rationale.md](docs/rationale.md)

## Статус порта Promise/A+

Структура классов:

- класс `IPromise<Value>` предоставляет тонкий интерфейс запущенного Promise, и может быть реализован mock'ом в автоматизированных тестах
- класс `Promise<Value>` наследует IPromise и предоставляет реализацию по-умолчанию
- класс `IPromisePtr<Value>` предоставляет фасад над IPromise, предназначенный для использования извне вместо IPromise, и хранящий IPromise по shared_ptr

Порт неполный, и содержит сознательно допущенные отклонения от стандарта:

- Then/Catch могут быть вызваны лишь по одному разу для каждого Promise, и не выполняют продолжение (continuation) задачи
- Для выполнения продолжения в IPromisePtr реализована отдельная функция ThenDo, которая опционально может явно принимать диспетчеры для запуска новой подзадачи и для вызова callback по её завершению.
- Отсутствуют аналоги функций [all](https://developer.mozilla.org/ru/docs/Web/JavaScript/Reference/Global_Objects/Promise/all) и [race](https://developer.mozilla.org/ru/docs/Web/JavaScript/Reference/Global_Objects/Promise/race), хотя принципально ничто не мешает их написать, за исключением необходимости выбора между двумя вариантами реализации all:
    - с фиксированным числом разнородных значений внутри `tuple<T1, T2, T3>`
    - с произвольным числом однородных значений внутри `vector<T>`
