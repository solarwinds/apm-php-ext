# apm-php-ext
![Packagist Version](https://img.shields.io/packagist/v/solarwinds/apm_ext)
![Packagist Downloads](https://img.shields.io/packagist/dm/solarwinds/apm_ext)
[![Build](https://github.com/solarwinds/apm-php-ext/actions/workflows/build.yml/badge.svg)](https://github.com/solarwinds/apm-php-ext/actions/workflows/build.yml)
![GitHub License](https://img.shields.io/github/license/solarwinds/apm-php-ext)

## Overview
solarwinds/apm_ext is an add-on PHP extension to [solarwinds/apm](https://packagist.org/packages/solarwinds/apm), used to cache the sampling settings for the APM library.

## Requirements
- PHP 8+
- [pie](https://github.com/php/pie)

## Installation

You can install the extension as follows:

### php-extension-installer

```shell
pie install solarwinds/apm_ext
```

## Configuration

This extension can be configured via `.ini` by modifying the following entries:

| Configuration                 | Default | Description                                       |
|-------------------------------|---------|---------------------------------------------------|
| `apm_ext.cache_max_entries`   | 48      | Maximum number of entries in the cache            |
| `apm_ext.settings_max_length` | 2048    | Maximum length of the settings value in the cache |

## To verify that the extension is installed and enabled:

```shell
php --ri  apm_ext
```

## Contributing

We welcome contributions!

Thanks to all contributors:
<a href="https://github.com/solarwinds/apm-php-ext/graphs/contributors">
<img src="https://contributors-img.web.app/image?repo=solarwinds/apm-php-ext"/>
</a>

## License

Apache-2.0. See [LICENSE](./LICENSE) for details.
