# apm-php-ext

## Overview
solarwinds/apm_ext is a PHP extension for [solarwinds/apm](https://packagist.org/packages/solarwinds/apm), to provide the sampling setting to the APM library.

## Requirements
- PHP 8+
- [libcurl](https://curl.se/libcurl/)

## Installation

The extension can be installed in the following way:

### php-extension-installer

TBD

## Verify that the extension is installed and enabled

```shell
php --ri  apm_ext
```

## Configuration

This feature can be configured via `.ini` by modifying the following entries:

- `apm_ext.collector` - Solarwinds [APM collector](https://documentation.solarwinds.com/en/success_center/observability/content/system_requirements/endpoints.htm#General). Default `apm.collector.na-01.cloud.solarwinds.com`
- `apm_ext.service_key` - Service Key

## Contributing

Contributions are welcome!

Thanks to all contributors:
<a href="https://github.com/solarwinds/apm-php-ext/graphs/contributors">
<img src="https://contributors-img.web.app/image?repo=solarwinds/apm-php-ext"/>
</a>

## License

Apache-2.0. See [LICENSE](./LICENSE) for details.
