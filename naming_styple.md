# C++ Naming Conventions

> One unified reference for the team. Follow this for every identifier in the codebase.

---

## Files

| Type | Convention | Example |
|---|---|---|
| Header | `PascalCase.hpp` | `HttpRequest.hpp` |
| Source | `PascalCase.cpp` | `HttpRequest.cpp` |
| Main entry | lowercase | `main.cpp` |

---

## Classes & Structs

`PascalCase` — always.

```cpp
class HttpRequest {};
class ManageClients {};
struct EpollContext {};
```

---

## Interfaces / Abstract Classes

Prefix with `I`.

```cpp
class IHandler {};
class IParser {};
```

---

## Member Variables

Prefix with `_`, then `camelCase`.

```cpp
class Client {
    int         _fd;
    std::string _readBuffer;
    size_t      _writeOffset;
};
```

---

## Local Variables

`camelCase`, no prefix.

```cpp
int         bytesRead;
std::string rawHeader;
bool        isReady;
```

---

## Constants & Macros

`ALL_CAPS_SNAKE_CASE`.

```cpp
#define MAX_CLIENTS     1024
#define BUFFER_SIZE     4096

const int TIMEOUT_SECONDS = 30;
```

---

## Enums & Enum Values

Enum name: `PascalCase`. Values: `ALL_CAPS`.

```cpp
enum ClientState {
    READING_HEADERS,
    READING_BODY,
    PROCESSING,
    RESPONDING,
    DONE
};
```

---

## Functions & Methods

`camelCase` for public and private alike.

```cpp
void        handleRequest();
std::string parseHeaders(const std::string& raw);
bool        isTimeout(int fd) const;
```

---

## Static / Class-Level Methods

Same rule — `camelCase`. No special prefix.

```cpp
static Client* create(int fd);
```

---

## Getters & Setters

Getters: `getFoo()`. Setters: `setFoo(val)`.

```cpp
int         getFd()    const;
std::string getState() const;
void        setState(ClientState s);
```

---

## Typedefs & Type Aliases

`PascalCase` with a `_t` suffix for plain structs/pod types; plain `PascalCase` for class aliases.

```cpp
typedef struct s_epoll_ctx  t_epoll_ctx;
typedef std::map<int, Client*>  ClientMap;
```

---

## Template Parameters

Single uppercase letter or short `PascalCase` word.

```cpp
template <typename T>
template <typename Key, typename Value>
```

---

## Namespaces

`lowercase_snake_case`.

```cpp
namespace http_utils {}
namespace config_parser {}
```

---

## Boolean Variables

Prefix with `is`, `has`, or `can`.

```cpp
bool isCgi;
bool hasBody;
bool canWrite;
```

---

## Quick Cheat Sheet

```
Files          →  PascalCase.hpp / PascalCase.cpp
Classes        →  PascalCase
Interfaces     →  IPascalCase
Members        →  _camelCase
Locals         →  camelCase
Constants      →  ALL_CAPS
Enums          →  PascalCase  /  ALL_CAPS values
Functions      →  camelCase()
Getters/Setters→  getFoo() / setFoo()
Typedefs       →  t_name  or  PascalCase
Booleans       →  isFoo / hasFoo / canFoo
Namespaces     →  snake_case
```
