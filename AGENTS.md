# AGENTS.md

## Scope and intent
- `apm_ext` is a PHP extension that caches `solarwinds/apm` sampling data in-process; most work is under `ext/`.
- Runtime boundary has 3 layers: Zend module/API (`ext/apm_ext.c`) -> C bridge (`ext/cache_c_wrapper.h/.cpp`) -> C++ cache (`ext/cache.h/.cpp`).
- Public API is `Solarwinds\\Cache\\{get,put,getBucketState,putBucketState}` from `ext/apm_ext.stub.php` (generated header: `ext/apm_ext_arginfo.h`).

## Core behavior you must preserve
- Two caches exist in module globals (`ext/php_apm_ext.h`): settings and bucket-state, each with separate INI limits.
- Settings key is intentionally composite in `ext/cache_c_wrapper.cpp`: `collector + hash(token) + serviceName` (token is never stored raw).
- `Solarwinds::Cache` is LRU-by-access/update (`ext/cache.cpp`): both `Get` and `Put` move entries to MRU; overflow evicts oldest.
- Fork handling is explicit in `ext/apm_ext.c`: `pthread_atfork(prefork, postfork, postfork)` frees/rebuilds caches across fork.

## Project-specific rules (Do/Don't)
- Do add/adjust tunables in `PHP_INI_BEGIN` (`ext/apm_ext.c`) and ensure they appear in `php --ri apm_ext` output.
- Do keep API error contract: invalid/oversize input returns `false`; oversize paths also log to `stderr`.
- Do keep the C/C++ boundary thin: add cache ops in `ext/cache_c_wrapper.h/.cpp`, then bind from Zend functions.
- Don't hand-edit `ext/apm_ext_arginfo.h`; update `ext/apm_ext.stub.php` and regenerate stubs.
- Don't bypass size guards (`COLLECTOR_MAX_LENGTH`, `TOKEN_MAX_LENGTH`, `SERVICE_NAME_MAX_LENGTH`, `PID_MAX_LENGTH`, and INI max lengths).

## Build/test workflow used in this repo
- Prefer root `Makefile` targets: `make build-image`, `make build`, `make test`, `make all`.
- Docker services mount `./ext` to `/usr/src/myapp` (`docker-compose.yaml`), so extension build/tests run in that directory context.
- `make build` executes `ext/build.sh` (`phpize && ./configure && make`).
- Tests are PHPT (not PHPUnit); canonical suite is `ext/tests/*.phpt`.

## Common commands (zsh)
```zsh
make all
make build-image
make build
make test
```

## Quick task playbook
- If editing Zend API or INI/config, start in `ext/apm_ext.c`; if editing function signatures, update `ext/apm_ext.stub.php` first.
- If editing cache semantics/keying, touch `ext/cache.cpp` and/or `ext/cache_c_wrapper.cpp`, then run targeted PHPTs.
- For API shape changes, do not edit `ext/apm_ext_arginfo.h` directly; regenerate from `ext/apm_ext.stub.php` before running tests.
- Run the smallest relevant tests first (`004` update, `008` LRU, `012` fork), then run broader `make test`.

## Test files to mirror when changing behavior
- Cache update semantics: `ext/tests/004.phpt`.
- LRU eviction/order semantics: `ext/tests/008.phpt`.
- Fork isolation semantics: `ext/tests/012.phpt`.
- Extension load/config smoke: `ext/tests/001.phpt` and nearby `00x.phpt` files.

## Existing AI guidance discovery
- One required glob search found only `README.md` as an existing AI-instructions source in this workspace.
