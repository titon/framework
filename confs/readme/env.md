# Environment #

Provides a simple structure for managing configurations and bootstraps for multiple environments, whether it be develop or production,
or a more robust application with multiple locale, server, or developer configuration combinations.
Environments are mapped through a `Host` class that defines the machine hostname to verify against.

```hack
use Titon\Environment\Environment;
use Titon\Environment\Host;
use Titon\Environment\Server;

$env = new Environment();
$env->addHost('dev', new Host(Server::DEV, ['dev-host', 'local-host']);
$env->addHost('prod', new Host(Server::PROD, 'prod-host');
$env->setFallback('prod');
$env->initialize();
```

When a match is found via environment vars, or the machine hostname, the matched host will be bootstrapped.
A `Bootstrapper` class can be defined to handle the bootstrapping process.

```hack
$env->addBootstrapper(new CustomBootstrapper());
```