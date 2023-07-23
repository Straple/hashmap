# hashmap

Хеш таблица на открытой адресации.

Типы ключа и значения должны быть `MoveConstructible`, `MoveAssignable`, `Destructible`. Для `.operator[](key)` значение должно быть `DefaultConstructible`, чтобы можно было создать элемент по умолчанию и добавить его.

Обработки исключений нет. Гарантии безопасности исключений нет. Это в TODO.

TODO: написать про инвалидацию ссылок и итераторов после операций

### Внутри

Хранит все элементы в `std::vector<std::optional<std::pair<Key, Value>>>`. Этот вектор всегда хотя бы в 2 раза больше
чем количество элементов, чтобы поддерживать хорошую разбросанность и быстрые операции. Пара из ключ-значения хранятся
в `std::optional`, чтобы именно он заботился о создании объекта и его удалении на этом участке памяти. Таким образом,
если будет добавлен новый элемент, то `std::optional` вызовет конструктор, чтобы его создать, аналогично с удалением
элемента.

### Операции

В конструкторах хеш-объект опционально передается последним параметром и копируется. Если нужно использовать прям его,
то почитайте про `std::reference_wrapper`

#### Конструкторы

1) Конструктор по умолчанию: корзина размера 8
2) Конструктор от `elements_count`: корзина размера `2 * elements_count` (это как `reserve`)
3) Конструктор от итераторов `first, last`: заранее выделяет корзину размера `2 * std::distance(first, last)`, после
   чего закидывает все элементы от `first` до `last`
4) Конструктор от `std::initializer_list`, то есть `{...}`: `first=begin(), last=end()` и вызвали конструктор выше

#### Геттеры

1) `size()` возвращает количество элементов в структуре
2) `empty()` возвращает "верно ли, что в структуре нет элементов?"
3) `hash_function()` возвращает `const&` на хеш функцию

#### Редакторы

Аккуратно сделал с использованием `perfect forwarding`, чтобы не писать кучу однотипных перегрузок

1) `insert(key, value)` если не было элемента с ключом `key`, то добавляет пару `key->value`, иначе ничего не делает
2) `insert(item)` вызывает `insert(item.first, item.second)`
3) `erase(key)` удаляет элемент с ключом key и возвращает `true`, если такой элемент был, иначе `false`
4) `clear` удаляет все элементы, НЕ изменяет размер корзины. ВАЖНО понимать, что это работает за `O(размер корзины)`,
   потому что нужно по ней пройтись, то есть если мы закинули в структуру 10^5 элементов, а потом 10^5 раз
   делаем `clear()`, то это будет квадрат (ифать пустую структуру в этом случае тупо, так как можно просто закинуть один
   элемент и потом сделать `clear()`)
5) `[key]` возвращает ссылку на `value`, который соответствует `key->value`. Если такой пары нет, то добавляет ее

TODO:

1) пусть `insert` возвращает итератор на добавленный элемент
2) после выполнения `(1)` нужно переделать `[key]`, чтобы оно работало с `noncopyable` объектом. Сейчас нельзя потому
   что так мы потеряем объект
3) также было бы неплохо посмотреть более тщательно на то, что возвращает `insert` и `erase` у `std::unordered_map` и
   сделать также, чтобы более поддерживаемо

### Итератор

Хранит в себе индекс корзины с элементом или размер корзины, если это `end`. А также константную ссылку на корзину (это
можно переделать как просто указатель на память этой корзины).
Можно делать `it++`, `++it`, (TODO сделать `--`). Можно разыменовывать `*`, `->...`. Сравнение на `==` и `!=` проверяет
индекс и указатель корзины.

#### еще

1) `begin()` вернет итератор на первый элемент, `end()` на конец структуры
2) `find(key)` вернет итератор на элемент с этим ключом или `end()`, если такого там нет
3) `bucket(key)` вернет индекс корзины, в которой лежит такой ключ или -1, если не лежит. (это для тестирования, TODO:
   можно и удалить)

## Тестирование

В файлике `test.cpp` лежит огромное количество тестов.
Также в `main.cpp` выводятся вызовы специальных методов (правило 5-и), чтобы убедиться что и когда вызывается.

Также в файлике `test.cpp` в самом конце лежит `TEST_CASE("super stress and benchmark")`, в котором тестируется еще и
время

### Бенчмарк

Используется пара `int->double[3]`. Сначала делает `reserve(N)`, чтобы далее не расширялась вся структура, потом
добавляются `N` рандомных элементов. Теперь `M` раз берется рандомный ключ `X`
, `if(map.erase(X)) {map.insert({X, {}});}`. Таким образом набор элементов не изменяется, а вот порядок да.

`mean` это среднее время выполнения той операции, `max` — максимальное. `hash calc counter` — количество вычислений
хеша.

`N=10^5, M=10^6`\
`std::map, mean 273 ns, max 201626 ns/iter, hash calc counter 0`\
`std::unordered_map, mean 81 ns, max 121896 ns/iter, hash calc counter 3365066`\
`hashmap, mean 41 ns, max 58290 ns/iter, hash calc counter 5547855`

`N=10^5, M=10^7`\
`std::map, mean 325 ns, max 191835 ns/iter, hash calc counter 0`\
`std::unordered_map, mean 83 ns, max 137155 ns/iter, hash calc counter 32748874`\
`hashmap, mean 40 ns, max 124857 ns/iter, hash calc counter 54582774`

`N=10^6, M=10^7`\
`std::map, mean 1014 ns, max 193892 ns/iter, hash calc counter 0`\
`std::unordered_map, mean 291 ns, max 174699 ns/iter, hash calc counter 33641320`\
`hashmap, mean 110 ns, max 313861 ns/iter, hash calc counter 55404756`

Как видно `std::map` очень сильно проигрывает. 

У `std::unordered_map` меньше вычислений хеша (в `0,6` раз по сравнению с
моим), что круче для больших объектов, у которых он долго вычисляется (`std::string?`). Это в TODO: можно дополнительно
хранить хеш в каждой корзине и вычислять его один раз.

Также видно, что `hashmap` быстрее `std::unordered_map` в 2-3 раза, а также в первом тесте `max` у `hashmap` в 2 раза меньше, но вот во втором и третьем он больше, что говорит о попадании в большой блок, по которому пришлось пройти. Может, стоит увеличить коэффициент нагрузки, чтобы такое было менее вероятно