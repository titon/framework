# Environment Detection #

It is usually good practice to have different sets of configuration for different types of environments. 
The `Titon\Environment\Environment` class can be used for managing hosts (different environments), 
bootstrappers (configuration loading), secure variables, and the core of its functionality, 
matching and detection.

To begin, instantiate a new `Environment` using the `registry()` method, which will persist the instance 
throughout the application.

```hack
$env = Titon\Environment\Environment::registry();
```

## Mapping Hosts ##

Multiple environment configurations are mapped with a `Titon\Environment\Host` class. Each host represents a 
specific server type defined in the `Titon\Environment\Server` enumerable -- the available types are:
`DEV`, `PROD`, `QA`, `STAGING`, and `TESTING`.

A host can be mapped with a unique key using the `addHost()` method. The unique key is used for matching 
against the `APP_ENV` environment variable. An optional list of hostnames can also be passed for matching. 
[Jump to the matching patterns chapter for information on matching](#detection-patterns).

```hack
use Titon\Environment\Host;
use Titon\Environment\Server;

$env->addHost('dev', new Host(Server::DEV, ['vagrant-ubuntu-trusty-64']));
$env->addHost('prod', new Host(Server::PROD));
```

A host can be retrieved by key using the `getHost()` method. If no host is found, 
a `Titon\Environment\Exception\MissingHostException` is thrown.

```hack
$host = $env->getHost('dev');
```

To retrieve all hosts, use `getHosts()`.

```hack
$hosts = $env->getHosts(); // Titon\Environment\HostMap
```

### Fallback Host ###

Once hosts are mapped, a fallback host should be set using `setFallback()`, usually the production host. 
A fallback host is used in situations where no host can be matched successfully.

```hack
$env->setFallback('prod');
```

### Wildcard Hostnames ###

Even though hostnames are optional, they are useful in situations when environment variable matching fails.
Hostnames can be defined literally like the example above, or defined using a wildcard `*` that will match using regex.

```hack
$env->addHost('dev', new Host(Server::DEV, 'vagrant-ubuntu-*'));
```

## Matching Patterns ##

There are 2 patterns used for matching a host to the correct environment. In order of execution, 
the first pattern attempts to match a hosts unique key against the `APP_ENV` environment variable. 
If no matches are found, then the second pattern attempts to match the machines hostname (using `gethostname()`)
against the list of hostnames defined in each host.

To start the matching process, call `initialize()`.

```hack
$env->initialize();
```

If a match is found, then any defined bootstrappers will be triggered, and secure variables will be loaded. 
If no match is found, a `Titon\Environment\Exception\NoHostMatchException` is thrown.

The matched host can be accessed using `current()`.

```hack
$host = $env->current();
```

### Detection Methods ###

There are a handful of methods that are useful for detecting environment factors based on the currently matched host.

The `is()` method can detect based on a hosts unique key.

```hack
$env->is('dev');
```

The `isDevelopment()`, `isProduction()`, `isQA()`, `isStaging()`, and `isTesting()` methods can detect if
the host is of a specific `Server` type. The host objects support the same functionality.

```hack
$env->isProduction(); // Same as
$env->current()->isProduction();
```

The `isLocalhost()` method does not make use of the matched host but is used for detecting if the server is localhost. 
It will match if the HTTP host is `localhost` or if the IP is `127.0.0.1` and `::1`.

```hack
$env->isLocalhost();
```

## Secure Variables ##

Secure variables represent sensitive configuration like API keys, encryption keys, database passwords, etc, 
that *should not* be part of an application's source code, nor should it ever be committed into a VCS.
So how should we load and use this sensitive configuration if they aren't a part of the source code? 
Thankfully, this is solved through a unique environment dot file that *should* be automatically created 
through some kind of deployment process.

There are 2 possible dot files. The first being the `.env.php` file, which will load for all environments, 
and the second is the host specific `.env.{key}.php` file, which will load based on the matched host. 
Each of these dot files should return a PHP array.

```hack
return [
    'FACEBOOK_APP_ID' => 'foo',
    'S3_SECRET_KEY' => 'bar'
];
```

We then need to set a directory path to where the dot files will be stored, and initialize,
which will attempt to find and load the dot files.

```hack
$env = Titon\Environment\Environment::registry('/path/to/secure/vars');
// ...
$env->initialize();
```

Once loaded, we can access the variables using `getVariable()` and `getVariables()`, or the global `env()` function 
(which will only work if the `Environment::registry()` method is used).

```hack
$env->getVariable('FACEBOOK_APP_ID'); // foo

env('S3_SECRET_KEY'); // bar
```

<div class="notice is-info">
    Secure variables are immutable by design and are not found in <code>$_ENV</code>, <code>$_SERVER</code>, 
    or <code>getenv()</code>
</div>

## Bootstrapping ##

[Jump to the bootstrapping docs for more information.](bootstrapping.md)

