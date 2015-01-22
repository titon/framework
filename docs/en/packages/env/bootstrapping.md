# Bootstrapping #

A bootstrapper is a unique class that's triggered once an environment host is matched. 
Its primary purpose is for loading environment specific configuration but it can really be used for anything.

## Creating A Bootstrapper #

A bootstrapper should implement the `Titon\Environment\Bootstrapper` interface and implement the `bootstrap()` method. 
This method will receive the `Titon\Environment\Host` that was successfully matched.

```hack
use Titon\Environment\Host;

class ExampleBootstrapper implements Titon\Environment\Bootstrapper {
    public function bootstrap(Host $host): void {
        if ($host->isDevelopment()) {
            // ...
        } else {
            // ...
        }
    }
}
```

## Using Bootstrappers ##

To make use of a bootstrapper class, it can be set on the `Titon\Environment\Environment` class using `addBootstrapper()`. 
And once the environment has initialized, all mapped bootstrappers will be notified. That's all there is to it.

```hack
$env->addBootstrapper(new ExampleBootstrapper());
// ...
$env->initialize();
```
