# Logging #

Logging is structured around the wonderful [PHP-FIG Log specification](https://github.com/php-fig/log) as the framework provides no real logging mechanism. It does however provide a very basic logging class, `Titon\Debug\Logger`, that writes to the filesystem. We suggest using [Monolog](https://github.com/Seldaek/monolog) for more advanced logging.

To enable logging within the debug layer, a logger must be set on the `Titon\Debug\Debugger` class.

```hack
Titon\Debug\Debugger::setLogger(new Titon\Debug\Logger('/temp/logs'));
```

## Log Levels ##

There are 8 logging levels that can be used to denote the severity of the log, they are in order: debug, info, notice, warning, error, critical, alert, and emergency. Each of these levels can be triggered as a method on the logger class.

```hack
$logger = new Titon\Debug\Logger('/temp/logs');
$logger->info('This message is informational.');
$logger->alert('Something is going really really wrong!');
```

An array of key-values can be passed as the 2nd argument and will be inserted into any tokens in the message.

```hack
$logger->warning('{foo} is failing!', ['foo' => 'bar']); // bar is failing!
```

All logs will be written to a file in the directory passed to the constructor, with the file name matching the log level. For example, logging an emergency will append to `/temp/logs/emergency.log`.

## Daily Rotation ##

The built-in `Logger` class implements a very rudimentary log rotation system by appending all log files with the current day timestamp. To continue with the previous example, logging an emergency will write to `/temp/logs/emergency-2014-02-26.log`.
