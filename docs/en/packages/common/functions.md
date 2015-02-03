# Global Functions #

The Common package provides a few global functions that solve scenarios that do not pass Hack's strict mode.

### include_file(`string $path`): `array<Tk, Tv>` ###

Include a file defined at the given path and return the response of the include. This is primarily used for 
including PHP configuration files that return arrays. If the file does not exist, a `Titon\Common\Exception\MissingFileException` 
is thrown.

```php
$data = include_file('/path/to/config.php');
```

### render_template(`string $path`[, `array<string, Tv> $variables`]): `string` ###

Render a template defined at the given path and return the rendered response. An optional array of variables can 
be defined that will be available in the template. If the template does not exist, a `Titon\Common\Exception\MissingFileException` 
is thrown.

```php
$template = render_template('/path/to/template.tpl', ['foo' => 'bar']);
```