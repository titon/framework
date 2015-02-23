# Service Providers #

Service providers provide a central location to bootstrap dependencies for an application. They allow the registering of items into a `Titon\Context\Depository` in groups as well as deferred loading so that items aren't registered until needed.

A service provider must extend the `Titon\Context\ServiceProvider\AbstractServiceProvider` abstract class or implement the `Titon\Context\ServiceProvider` interface, as well as define the `register()` method. An optional `provides` vector property can be defined, which should contain a list of fully qualified class names that the service provider provides. This allows deferred loading of the service provider and deferred initialization until a class from the service provider is needed.

## Registering ##

The `register()` method can be used to register items in the `Depository`.

```hack
use Titon\Context\ClassList;

class ExampleServiceProvider extends Titon\Context\ServiceProvider\AbstractServiceProvider {

    protected ClassList $provides = Vector {
        'Some\Namespace\Foo'
    };

    public function register() {
        $this->depository->register('foo', 'Some\Namespace\Foo');
    }
}
```

To make use of a service provider, it must be added to a `Depository`. The provider can either be passed as a string, or as an object. If passed as a string, the provider will be instantiated using the container allowing for automatic constructor injection.

```hack
$container->addServiceProvider($provider);
$container->addServiceProvider('ExampleServiceProvider');
```
