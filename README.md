# ReGlob
Convert a glob to a regular expression, supercharged⚡️

## Features

1. Supports authentic glob syntax
1. Supercharged with regexp expressions
1. Automatic capture groups for quick grab of the info you want

## Usage

```cpp
#include "ReGlob.hpp"
```

### Capturing

ReGlob addes the ability to capture wildcard matches in glob:

```cpp
auto regexp1 = ReGlob(R"*/*.js", {.capture = true});
//

auto regexp2 = ReGlob(R"**/[a-z].js", {.bash_syntax = true, .globstars = true, .capture = true});
//
```

### Escaping

ReGlob enhanced glob by adding the ability to escape regexp control characters:

```cpp
// Without escaping
auto regexp3 = ReGlob(R"^[]{}$", {.full_match = false});
//

// With escaping
auto regexp4 = ReGlob(R"\^\[\]\{\}\$", {.full_match = false});
//
```

## License

MIT